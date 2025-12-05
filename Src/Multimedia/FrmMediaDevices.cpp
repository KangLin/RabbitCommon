// Author: Kang Lin <kl222@126.com>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>
#include <cmath>
#include <QtGlobal>
#include <QLoggingCategory>
#include <QtMath>
#include "FrmMediaDevices.h"

static Q_LOGGING_CATEGORY(log, "Device")

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
static QString deviceDisplayName(const QAudioDevice &d) {
    return d.description().isEmpty() ? d.id() : d.description();
}
static QString cameraDisplayName(const QCameraDevice &d) {
    return d.description().isEmpty() ? d.id() : d.description();
}
#else
static QString deviceDisplayName(const QAudioDeviceInfo &d) {
    return d.deviceName();
}
static QString cameraDisplayName(const QCameraInfo &d) {
    return d.description().isEmpty() ? d.deviceName() : d.description();
}
#endif

/* ---------------- CSineWriter ---------------- */
CSineWriter::CSineWriter(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &CSineWriter::onTimer);
}

CSineWriter::~CSineWriter()
{
    stop();
}

void CSineWriter::start(QIODevice *sinkDevice, const QAudioFormat &format)
{
    if (!sinkDevice) return;
    m_sinkDevice = sinkDevice;
    m_sinkDevice->open(QIODevice::WriteOnly);
    m_sampleRate = format.sampleRate();
    m_channels = format.channelCount();
    // 440 Hz tone
    m_phase = 0.0;
    m_phaseInc = 2.0 * M_PI * 440.0 / double(m_sampleRate);
    // timer interval chosen to periodically push data
    m_timer.start(30);
}

void CSineWriter::stop()
{
    m_timer.stop();
    if (m_sinkDevice) {
        m_sinkDevice->close();
        m_sinkDevice = nullptr;
    }
}

void CSineWriter::onTimer()
{
    if (!m_sinkDevice || !m_sinkDevice->isOpen()) return;
    int frames = (m_sampleRate / 10); // 0.1s of audio
    QByteArray buffer;
    buffer.resize(frames * m_channels * sizeof(qint16));
    qint16 *out = reinterpret_cast<qint16*>(buffer.data());
    for (int i = 0; i < frames; ++i) {
        qint16 sample = qint16(m_amplitude * std::sin(m_phase));
        m_phase += m_phaseInc;
        if (m_phase > 2.0*M_PI) m_phase -= 2.0*M_PI;
        for (int c = 0; c < m_channels; ++c) {
            *out++ = sample;
        }
    }
    m_sinkDevice->write(buffer);
}

/* ---------------- CFrmMediaDevices ---------------- */

CFrmMediaDevices::CFrmMediaDevices(bool bDisableButtonApply, QWidget *parent)
    : QWidget(parent)
    , m_bDisableButtonApply(bDisableButtonApply)
    , m_pParameters(nullptr)
{
    setWindowTitle(tr("Media Devices Settings"));
    setWindowIcon(QIcon::fromTheme("multimedia-player"));

    // Default audio format (signed 16-bit PCM)
    m_audioFormat.setSampleRate(44100);
    m_audioFormat.setChannelCount(1);
    // Use the Int16 enumerator which is available across Qt6 versions
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_audioFormat.setSampleFormat(QAudioFormat::Int16);
#else
    m_audioFormat.setSampleType(QAudioFormat::SampleType::SignedInt);
    m_audioFormat.setSampleSize(16);
    auto codec = QAudioDeviceInfo::defaultOutputDevice().supportedCodecs();
    //qDebug(log) << "Audio format codec:" << codec;
    if(!codec.isEmpty())
        m_audioFormat.setCodec(codec.at(0));
#endif

    setupUi();

    refreshDevices();

    applyCurrentSelections();
}

CFrmMediaDevices::~CFrmMediaDevices()
{
    if (m_camera) {
        m_camera->stop();
        delete m_camera;
    }
    if (m_audioSource) {
        if (m_audioSourceIO) {
            m_audioSourceIO->close();
            m_audioSourceIO = nullptr;
        }
        m_audioSource.reset();
    }
    if (m_sineWriter) {
        m_sineWriter->stop();
        delete m_sineWriter;
    }
}

int CFrmMediaDevices::SetParameter(CParameter* para)
{
    if(!para) {
        return RV::Failure;
    }
    m_pParameters = para;
    m_Para = *m_pParameters;
    refreshDevices();
    return RV::Success;
}

int CFrmMediaDevices::Accept()
{
    applyCurrentSelections();
    *m_pParameters = m_Para;
    qDebug(log) << "Camera:" << m_Para.m_Camera
                << "AudioInput:" << m_Para.m_AudioInput
                << "AudioOutput:" << m_Para.m_AudioOutput
                << m_Para.m_VolumeInput
                << m_Para.m_VolumeOutput;
    return RV::Success;
}

void CFrmMediaDevices::slotAccept()
{
    Accept();
}

void CFrmMediaDevices::setupUi()
{
    auto *mainLayout = new QHBoxLayout(this);

    // Left: device selection and controls
    auto *leftLayout = new QVBoxLayout();

    auto *grpVideo = new QGroupBox(tr("Video"));
    auto *videoLayout = new QVBoxLayout();
    m_cbCamera = new QComboBox();
    videoLayout->addWidget(new QLabel(tr("Camera")));
    videoLayout->addWidget(m_cbCamera);
    m_btnTestCamera = new QPushButton(tr("Turn on camera preview"));
    videoLayout->addWidget(m_btnTestCamera);
    grpVideo->setLayout(videoLayout);
    leftLayout->addWidget(grpVideo);

    auto *grpAudio = new QGroupBox(tr("Audio"));
    auto *audioLayout = new QVBoxLayout();
    m_cbAudioInput = new QComboBox();
    m_cbAudioOutput = new QComboBox();
    audioLayout->addWidget(new QLabel(tr("Microphone (Input):")));
    audioLayout->addWidget(m_cbAudioInput);

    m_sliderVolumeInput = new QSlider(Qt::Horizontal);
    m_sliderVolumeInput->setRange(0, 100);
    m_sliderVolumeInput->setValue(80);
    audioLayout->addWidget(new QLabel(tr("Input Volume:")));
    audioLayout->addWidget(m_sliderVolumeInput);

    audioLayout->addWidget(new QLabel(tr("Speaker (Output):")));
    audioLayout->addWidget(m_cbAudioOutput);

    m_sliderVolumeOutput = new QSlider(Qt::Horizontal);
    m_sliderVolumeOutput->setRange(0, 100);
    m_sliderVolumeOutput->setValue(80);
    audioLayout->addWidget(new QLabel(tr("Output Volume:")));
    audioLayout->addWidget(m_sliderVolumeOutput);

    m_btnTestTone = new QPushButton(tr("Play test sound"));
    audioLayout->addWidget(m_btnTestTone);

    audioLayout->addSpacing(8);
    audioLayout->addWidget(new QLabel(tr("Microphone input level:")));
    m_micLevelBar = new QProgressBar();
    m_micLevelBar->setRange(0, 100);
    m_micLevelBar->setTextVisible(false);
    audioLayout->addWidget(m_micLevelBar);

    grpAudio->setLayout(audioLayout);
    leftLayout->addWidget(grpAudio);

    leftLayout->addStretch();

    m_btnApply = new QPushButton(tr("Apply"));
    m_btnApply->setHidden(m_bDisableButtonApply);
    auto *btnRow = new QHBoxLayout();
    btnRow->addWidget(m_btnApply);
    leftLayout->addLayout(btnRow);

    mainLayout->addLayout(leftLayout, 1);

    // Right: camera preview area
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setMinimumSize(420, 320);
    auto *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel(tr("Camera preview")));
    rightLayout->addWidget(m_videoWidget, 1);
    mainLayout->addLayout(rightLayout, 2);

    // connections
    connect(m_btnApply, &QPushButton::clicked, this, &CFrmMediaDevices::applyCurrentSelections);
    connect(m_btnTestCamera, &QPushButton::clicked, this, &CFrmMediaDevices::onCameraTestToggled);
    connect(m_btnTestTone, &QPushButton::clicked, this, &CFrmMediaDevices::onToneTestToggled);
    connect(m_sliderVolumeInput, &QSlider::valueChanged, this, &CFrmMediaDevices::onVolumeInputChanged);
    connect(m_sliderVolumeOutput, &QSlider::valueChanged, this, &CFrmMediaDevices::onVolumeOutputChanged);
}

void CFrmMediaDevices::readMicData()
{
    if (!m_audioSourceIO) return;
    QByteArray data = m_audioSourceIO->readAll();
    if (data.isEmpty()) return;

    // assuming signed 16-bit little-endian PCM mono
    const qint16 *samples = reinterpret_cast<const qint16*>(data.constData());
    int sampleCount = data.size() / sizeof(qint16);
    double sum = 0.0;
    for (int i = 0; i < sampleCount; ++i) {
        double v = samples[i] / 32768.0;
        sum += v * v;
    }
    double rms = 0.0;
    if (sampleCount > 0) {
        rms = std::sqrt(sum / sampleCount);
    }
    int level = qBound(0, int(rms * 200.0 * 100.0 / 2.0), 100); // simple scaling
    m_micLevelBar->setValue(level);
}

void CFrmMediaDevices::onVolumeInputChanged(int value)
{
    if(m_audioSource)
        m_audioSource->setVolume(qreal(value) / 100.0);
}

void CFrmMediaDevices::onVolumeOutputChanged(int value)
{
    Q_UNUSED(value);
    // QAudioSink volume API (if supported) would be used here when sink exists.
    if (m_audioSink) {
        m_audioSink->setVolume(qreal(value) / 100.0);
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void CFrmMediaDevices::refreshDevices()
{
    int nIndex = -1;
    // Populate audio input devices
    m_cbAudioInput->clear();
    const auto audioInputs = QMediaDevices::audioInputs();
    for (const QAudioDevice &d : audioInputs) {
        m_cbAudioInput->addItem(deviceDisplayName(d), QVariant::fromValue(d));
    }
    if(m_Para.m_AudioInput.isEmpty())
        nIndex = m_cbAudioInput->findData(QVariant::fromValue(QMediaDevices::defaultAudioInput()));
    else {
        for(int i = 0; i < m_cbAudioInput->count(); i++) {
            auto d = m_cbAudioInput->itemData(i);
            if(d.value<QAudioDevice>().id() == m_Para.m_AudioInput) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbAudioInput->setCurrentIndex(nIndex);

    nIndex = -1;
    // Populate audio outputs
    m_cbAudioOutput->clear();
    const auto audioOutputs = QMediaDevices::audioOutputs();
    for (const QAudioDevice &d : audioOutputs) {
        m_cbAudioOutput->addItem(deviceDisplayName(d), QVariant::fromValue(d));
    }
    if(m_Para.m_AudioOutput.isEmpty())
        nIndex = m_cbAudioOutput->findData(QVariant::fromValue(QMediaDevices::defaultAudioOutput()));
    else {
        for(int i = 0; i < m_cbAudioOutput->count(); i++) {
            auto d = m_cbAudioOutput->itemData(i);
            if(d.value<QAudioDevice>().id() == m_Para.m_AudioOutput) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbAudioOutput->setCurrentIndex(nIndex);

    nIndex = -1;
    // Populate camera list
    m_cbCamera->clear();
    const auto cams = QMediaDevices::videoInputs();
    for (const QCameraDevice &d : cams) {
        m_cbCamera->addItem(cameraDisplayName(d), QVariant::fromValue(d));
    }
    if(m_Para.m_Camera.isEmpty())
        nIndex = m_cbCamera->findData(QVariant::fromValue(QMediaDevices::defaultVideoInput()));
    else {
        for(int i = 0; i < m_cbCamera->count(); i++) {
            auto d = m_cbCamera->itemData(i);
            if(d.value<QCameraDevice>().id() == m_Para.m_Camera) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbCamera->setCurrentIndex(nIndex);

    m_sliderVolumeInput->setValue(m_Para.m_VolumeInput * 100);
    m_sliderVolumeOutput->setValue(m_Para.m_VolumeOutput * 100);
}

void CFrmMediaDevices::onCameraTestToggled()
{
    m_cbCamera->setEnabled(m_camera);
    if (!m_camera) {
        // start preview
        int idx = m_cbCamera->currentIndex();
        if (idx < 0) return;
        QCameraDevice dev = m_cbCamera->currentData().value<QCameraDevice>();
        m_camera = new QCamera(dev, this);
        m_captureSession.setCamera(m_camera);
        m_captureSession.setVideoOutput(m_videoWidget);
        m_camera->start();
        m_btnTestCamera->setText(tr("Turn off camera preview"));
    } else {
        m_camera->stop();
        m_captureSession.setCamera(nullptr);
        m_captureSession.setVideoOutput(nullptr);
        delete m_camera;
        m_camera = nullptr;
        m_btnTestCamera->setText(tr("Turn on camera preview"));
        // clear video widget
        m_videoWidget->hide();
        m_videoWidget->show();
    }
}

void CFrmMediaDevices::onToneTestToggled()
{
    m_cbAudioInput->setEnabled(m_sineWriter);
    m_cbAudioOutput->setEnabled(m_sineWriter);

    if (!m_sineWriter) {
        // start tone
        int idx = m_cbAudioOutput->currentIndex();
        QAudioDevice dev;
        if (idx >= 0) dev = m_cbAudioOutput->currentData().value<QAudioDevice>();
        else dev = QMediaDevices::defaultAudioOutput();
        
        m_audioSink = QSharedPointer<QAudioSink>(new QAudioSink(dev, m_audioFormat, this));
        m_audioSink->setVolume(qreal(m_sliderVolumeOutput->value()) / 100.0);
        QIODevice *sinkDevice = m_audioSink->start();
        if(sinkDevice) {
            m_sineWriter = new CSineWriter(this);
            m_sineWriter->start(sinkDevice, m_audioFormat);
            m_btnTestTone->setText(tr("Stop test sound"));
        } else
            qCritical(log) << "Fail: Open audio output:"
                           << deviceDisplayName(dev)
                           << "Format:" << m_audioSink->format();
        // start mic capture to show levels while tone plays (optional)
        int inIdx = m_cbAudioInput->currentIndex();
        QAudioDevice inDev;
        if (inIdx >= 0) inDev = m_cbAudioInput->currentData().value<QAudioDevice>();
        else inDev = QMediaDevices::defaultAudioInput();
        m_audioSource = QSharedPointer<QAudioSource>(new QAudioSource(inDev, m_audioFormat, this));
        m_audioSource->setVolume(qreal(m_sliderVolumeInput->value()) / 100.0);
        m_audioSourceIO = m_audioSource->start();
        if (m_audioSourceIO) {
            connect(m_audioSourceIO, &QIODevice::readyRead, this, &CFrmMediaDevices::readMicData);
        }else
            qCritical(log) << "Fail: Open audio Input:"
                           << deviceDisplayName(inDev)
                           << "Format:" << m_audioSource->format();
    } else {
        // stop tone
        if (m_sineWriter) {
            m_sineWriter->stop();
            delete m_sineWriter;
            m_sineWriter = nullptr;
        }
        if (m_audioSink) {
            m_audioSink->stop();
            m_audioSink.reset();
        }
        
        if (m_audioSource) {
            if (m_audioSourceIO) {
                disconnect(m_audioSourceIO, &QIODevice::readyRead, this, &CFrmMediaDevices::readMicData);
                m_audioSourceIO->close();
                m_audioSourceIO = nullptr;
            }
            m_audioSource.reset();
        }
        
        m_btnTestTone->setText(tr("Play test sound"));
        m_micLevelBar->setValue(0);
    }
}

int CFrmMediaDevices::applyCurrentSelections()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_cbCamera->count() > 0)
        m_Para.m_Camera = m_cbCamera->currentData().value<QCameraDevice>().id();
    if(m_cbAudioInput->count() > 0) {
        m_Para.m_AudioInput = m_cbAudioInput->currentData().value<QAudioDevice>().id();
        m_Para.m_VolumeInput = qreal(m_sliderVolumeInput->value()) / 100.0;
    }
    if(m_cbAudioOutput->count() > 0) {
        m_Para.m_AudioOutput = m_cbAudioOutput->currentData().value<QAudioDevice>().id();
        m_Para.m_VolumeOutput = qreal(m_sliderVolumeOutput->value()) / 100.0;
    }
    return RV::Success;
}

#else // #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

void CFrmMediaDevices::refreshDevices()
{
    int nIndex = -1;
    // Populate audio input devices
    m_cbAudioInput->clear();
    foreach (auto d, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        //qDebug(log) << "Audio inputs:" << deviceDisplayName(d);
        m_cbAudioInput->addItem(deviceDisplayName(d), QVariant::fromValue(d));
    }
    if(m_Para.m_AudioInput.isEmpty()) {
        nIndex = m_cbAudioInput->findData(
            QVariant::fromValue(QAudioDeviceInfo::defaultInputDevice()));
    } else {
        for(int i = 0; i < m_cbAudioInput->count(); i++) {
            auto d = m_cbAudioInput->itemData(i);
            if(d.value<QAudioDeviceInfo>().deviceName().toLatin1()
                == m_Para.m_AudioInput) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbAudioInput->setCurrentIndex(nIndex);

    nIndex = -1;
    // Populate audio outputs
    m_cbAudioOutput->clear();
    foreach(auto d, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
        //qDebug(log) << "Audio output:" << deviceDisplayName(d);
        m_cbAudioOutput->addItem(deviceDisplayName(d), QVariant::fromValue(d));
    }
    if(m_Para.m_AudioOutput.isEmpty())
        nIndex = m_cbAudioOutput->findData(QVariant::fromValue(QAudioDeviceInfo::defaultOutputDevice()));
    else {
        for(int i = 0; i < m_cbAudioOutput->count(); i++) {
            auto d = m_cbAudioOutput->itemData(i);
            if(d.value<QAudioDeviceInfo>().deviceName().toLatin1()
                == m_Para.m_AudioOutput) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbAudioOutput->setCurrentIndex(nIndex);

    nIndex = -1;
    // Populate camera list
    m_cbCamera->clear();
    QList<QCameraInfo> cams = QCameraInfo::availableCameras();
    foreach(auto d, cams) {
        m_cbCamera->addItem(cameraDisplayName(d), d.deviceName());
    }
    if(m_Para.m_Camera.isEmpty())
        nIndex = m_cbCamera->findData(QCameraInfo::defaultCamera().deviceName());
    else {
        for(int i = 0; i < m_cbCamera->count(); i++) {
            auto d = m_cbCamera->itemData(i);
            if(d == m_Para.m_Camera) {
                nIndex = i;
                break;
            }
        }
    }
    if(-1 < nIndex)
        m_cbCamera->setCurrentIndex(nIndex);

    m_sliderVolumeInput->setValue(m_Para.m_VolumeInput * 100);
    m_sliderVolumeOutput->setValue(m_Para.m_VolumeOutput * 100);
}

void CFrmMediaDevices::onCameraTestToggled()
{
    m_cbCamera->setEnabled(m_camera);
    if (!m_camera) {
        // start preview
        int idx = m_cbCamera->currentIndex();
        if (idx < 0) return;
        QString dev = m_cbCamera->currentData().toString();
        m_camera = new QCamera(dev.toLatin1(), this);
        m_camera->setViewfinder(m_videoWidget);
        m_camera->start();
        m_btnTestCamera->setText(tr("Turn off camera preview"));
    } else {
        m_camera->stop();
        delete m_camera;
        m_camera = nullptr;
        m_btnTestCamera->setText(tr("Turn on camera preview"));
        // clear video widget
        m_videoWidget->hide();
        m_videoWidget->show();
    }
}

void CFrmMediaDevices::onToneTestToggled()
{
    m_cbAudioInput->setEnabled(m_sineWriter);
    m_cbAudioOutput->setEnabled(m_sineWriter);

    if (!m_sineWriter) {
        // start tone
        int idx = m_cbAudioOutput->currentIndex();
        QAudioDeviceInfo dev;
        if (idx >= 0)
            dev = m_cbAudioOutput->currentData().value<QAudioDeviceInfo>();
        else
            dev = QAudioDeviceInfo::defaultOutputDevice();
        m_audioSink = QSharedPointer<QAudioOutput>(
            new QAudioOutput(dev, m_audioFormat, this));
        m_audioSink->setVolume(qreal(m_sliderVolumeOutput->value()) / 100.0);
        QIODevice *sinkDevice = m_audioSink->start();
        if(sinkDevice) {
            m_sineWriter = new CSineWriter(this);
            m_sineWriter->start(sinkDevice, m_audioFormat);
            m_btnTestTone->setText(tr("Stop test sound"));
        } else
            qCritical(log) << "Fail: Open audio output:"
                           << deviceDisplayName(dev)
                           << "Format:" << m_audioSink->format();
        // start mic capture to show levels while tone plays (optional)
        int inIdx = m_cbAudioInput->currentIndex();
        QAudioDeviceInfo inDev;
        if (inIdx >= 0)
            inDev = m_cbAudioInput->currentData().value<QAudioDeviceInfo>();
        else
            inDev = QAudioDeviceInfo::defaultInputDevice();
        m_audioSource = QSharedPointer<QAudioInput>(
            new QAudioInput(inDev, m_audioFormat, this));
        m_audioSource->setVolume(qreal(m_sliderVolumeInput->value()) / 100.0);
        m_audioSourceIO = m_audioSource->start();
        if (m_audioSourceIO) {
            connect(m_audioSourceIO, &QIODevice::readyRead, this, &CFrmMediaDevices::readMicData);
        } else
            qCritical(log) << "Fail: Open audio Input:"
                           << deviceDisplayName(inDev)
                           << "Format:" << m_audioSource->format();
    } else {
        // stop tone
        if (m_sineWriter) {
            m_sineWriter->stop();
            delete m_sineWriter;
            m_sineWriter = nullptr;
        }
        if (m_audioSink) {
            m_audioSink->stop();
            m_audioSink.reset();
        }

        if (m_audioSource) {
            if (m_audioSourceIO) {
                disconnect(m_audioSourceIO, &QIODevice::readyRead, this, &CFrmMediaDevices::readMicData);
                m_audioSourceIO->close();
                m_audioSourceIO = nullptr;
            }
            m_audioSource.reset();
        }

        m_btnTestTone->setText(tr("Play test sound"));
        m_micLevelBar->setValue(0);
    }
}

int CFrmMediaDevices::applyCurrentSelections()
{
    if(m_cbCamera->count() > 0)
        m_Para.m_Camera = m_cbCamera->currentData().toString().toLatin1();
    if(m_cbAudioInput->count() > 0) {
        m_Para.m_AudioInput = m_cbAudioInput->currentData().value<QAudioDeviceInfo>().deviceName().toLatin1();
        m_Para.m_VolumeInput = qreal(m_sliderVolumeInput->value()) / 100.0;
    }
    if(m_cbAudioOutput->count() > 0) {
        m_Para.m_AudioOutput = m_cbAudioOutput->currentData().value<QAudioDeviceInfo>().deviceName().toLatin1();
        m_Para.m_VolumeOutput = qreal(m_sliderVolumeOutput->value()) / 100.0;
    }
    return RV::Success;
}

#endif // #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
