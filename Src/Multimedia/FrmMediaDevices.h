// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>
#include <QProgressBar>
#include <QTimer>
#include <QAudioFormat>
#include <QCamera>
#include <QIODevice>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QMediaDevices>
    #include <QAudioSink>
    #include <QAudioSource>
    #include <QMediaCaptureSession>
#else
    #include <QAudioDeviceInfo>
    #include <QCameraInfo>
    #include <QAudioInput>
    #include <QAudioOutput>
#endif

#include "rabbitcommon_export.h"

// Small helper that writes a generated sine wave to a QIODevice (audio sink)
class CSineWriter : public QObject
{
    Q_OBJECT
public:
    explicit CSineWriter(QObject *parent = nullptr);
    ~CSineWriter() override;

    void start(QIODevice *sinkDevice, const QAudioFormat &format);
    void stop();

private slots:
    void onTimer();

private:
    QTimer m_timer;
    QIODevice *m_sinkDevice = nullptr;
    double m_phase = 0.0;
    double m_phaseInc = 0.0;
    int m_sampleRate = 44100;
    int m_channels = 1;
    int m_amplitude = 16000; // for signed 16-bit
};

class RABBITCOMMON_EXPORT CFrmMediaDevices : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmMediaDevices(bool bDisableButtonApply = false, QWidget *parent = nullptr);
    ~CFrmMediaDevices() override;

    // Return value
    enum RV{
        Success = 0,
        Failure = -1
    };
    Q_ENUM(RV);

    struct CParameter {
        QByteArray m_Camera;
        QByteArray m_AudioInput;
        QByteArray m_AudioOutput;
        qreal m_VolumeInput = 0.8;
        qreal m_VolumeOutput = 0.8;
    };

    //! [Parameter commone functions]
    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameter* pParameter);
    /*!
     * \~chinese 接受参数
     * \param validity: 标志是否检查参数
     *    - true: 检查参数
     *    - false: 不检查参数
     * \return 成功返回 0 。其它值为失败。
     * \~english Accept parameters
     * \param validity
     *    - true: Check parameters
     *    - false: Not check parameters
     * \return 0 is success. otherwise is fail
     */
    int Accept();
    //! [Parameter commone functions]

public Q_SLOTS:
    virtual void slotAccept();

private slots:
    void refreshDevices();
    void onCameraTestToggled();
    void onToneTestToggled();
    void onVolumeInputChanged(int value);
    void onVolumeOutputChanged(int value);
    void readMicData(); // read from QAudioSource to compute level
    int applyCurrentSelections();

private:
    void setupUi();

    // UI
    QComboBox *m_cbAudioInput;
    QComboBox *m_cbAudioOutput;
    QComboBox *m_cbCamera;
    QSlider *m_sliderVolumeInput;
    QSlider *m_sliderVolumeOutput;
    QPushButton *m_btnTestCamera;
    QPushButton *m_btnTestTone;
    QPushButton *m_btnApply;
    bool m_bDisableButtonApply;
    QVideoWidget *m_videoWidget;
    QProgressBar *m_micLevelBar;

    // Devices / backends
    QCamera *m_camera = nullptr;
    QIODevice *m_audioSourceIO = nullptr;
    CSineWriter *m_sineWriter = nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMediaDevices m_Devices;
    QMediaCaptureSession m_captureSession;
    QSharedPointer<QAudioSink> m_audioSink;
    QSharedPointer<QAudioSource> m_audioSource;
#else
    QAudioDeviceInfo m_AudioDeviceInfo;
    QCameraInfo m_CameraInfo;
    QSharedPointer<QAudioOutput> m_audioSink;
    QSharedPointer<QAudioInput> m_audioSource;
#endif

    // formats
    QAudioFormat m_audioFormat;

    CParameter* m_pParameters;
    CParameter m_Para;
};
