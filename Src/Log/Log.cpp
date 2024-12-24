// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <string>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QRegularExpression>
#include <QDateTime>
#include <QtGlobal>
#include <QMutex>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QLoggingCategory>

#ifdef HAVE_RABBITCOMMON_GUI
    #include <QDesktopServices>
    #include <QMessageBox>

    #include "DockDebugLog.h"
    #include "FileBrowser.h"
    #include "DlgEdit.h"
    extern CDockDebugLog* g_pDcokDebugLog;
#endif

#include "Log.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "CoreDump/StackTrace.h"

namespace RabbitCommon {

static QFile g_File;
static QMutex g_Mutex;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static QtMessageHandler g_originalMessageHandler = Q_NULLPTR;
#else
    static QtMsgHandler g_originalMessageHandler = Q_NULLPTR;
#endif
QRegularExpression g_reInclude;
QRegularExpression g_reExclude;

static bool g_bPrintStackTrace = false;
QList<QtMsgType> g_lstPrintStackTraceLevel{QtMsgType::QtCriticalMsg};
static CCallTrace* g_pStack = nullptr;

static Q_LOGGING_CATEGORY(log, "RabbitCommon.log")

CLog::CLog() : QObject(),
    m_szName(QCoreApplication::applicationName()),
    m_szDateFormat("yyyy-MM-dd"),
    m_nLength(0),
    m_nCount(0)
{
    if(!g_pStack) g_pStack = new CCallTrace();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    g_originalMessageHandler = qInstallMessageHandler(myMessageOutput);
#else
    g_originalMessageHandler = qInstallMsgHandler(myMessageOutput);
#endif

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    SetFilter(set.value("Log/Filter/include").toString(),
              set.value("Log/Filter/Exclude").toString());

    bool bReadOnly = false;
#if defined(Q_OS_ANDROID)
    bReadOnly = true;
#endif

    QString szConfFile = QStandardPaths::locate(
        QStandardPaths::ConfigLocation,
        QCoreApplication::applicationName() + "_logqt.ini");
    szConfFile = set.value("Log/ConfigFile", szConfFile).toString();
    if(!QFile::exists(szConfFile))
        szConfFile = QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                            "logqt.ini");
    if(!QFile::exists(szConfFile))
        szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(bReadOnly)
                     + QDir::separator() + QCoreApplication::applicationName()
                     + "_logqt.ini";
    if(!QFile::exists(szConfFile))
        szConfFile = RabbitCommon::CDir::Instance()->GetDirConfig(bReadOnly)
                     + QDir::separator() + "logqt.ini";
    LoadConfigure(szConfFile);

    qInfo(log) << "Application name:" << QCoreApplication::applicationName();
    qInfo(log) << "Application folder:" << CDir::Instance()->GetDirApplication();
    qInfo(log) << "Application install root folder:"
               << CDir::Instance()->GetDirApplicationInstallRoot();
    qInfo(log) << "Document folder:" << CDir::Instance()->GetDirUserDocument();

    bool check = false;
    check = connect(&m_Watcher, SIGNAL(fileChanged(QString)),
                    this, SLOT(slotFileChanged(QString)));
    Q_ASSERT(check);
}

CLog::~CLog()
{
    qDebug(log) << "CLog::~CLog()";
    g_Mutex.lock();
    qInstallMessageHandler(g_originalMessageHandler);
    if(g_File.isOpen()) g_File.close();
    g_Mutex.unlock();
    if(m_Timer.isActive()) m_Timer.stop();
    if(g_pStack) delete g_pStack;
}

CLog* CLog::Instance()
{
    static CLog* p = NULL;
    if(!p)
    {
        p = new CLog();
    }
    return p;
}

int CLog::LoadConfigure(const QString &szFile)
{
    int nRet = 0;

    QString szPattern = "[%{time hh:mm:ss.zzz} %{pid}|%{threadid} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}E%{endif}%{if-fatal}F%{endif}] %{category} - %{message}";
#ifdef QT_MESSAGELOGCONTEXT
    // Use qt message log context(__FILE__, __LIEN__, Q_FUNC_INFO) in release
    szPattern += " [%{file}:%{line}, %{function}]";
#elif defined(DEBUG) || defined(_DEBUG)
    szPattern += " [%{file}:%{line}, %{function}]";
#endif
    QString szFilterRules;
    quint64 nInterval = 60; // Unit: second
#if !(defined(DEBUG) || defined(_DEBUG) || ANDROID)
    szFilterRules = "*.debug = false";
#endif

    if (QFile::exists(szFile)) {
        m_szConfigureFile = szFile;
        QSettings setConfig(m_szConfigureFile, QSettings::IniFormat);
        setConfig.beginGroup("Log");
        szPattern = setConfig.value("Pattern", szPattern).toString();

        // Log file
        m_szPath = setConfig.value("Path", CDir::Instance()->GetDirLog()).toString();
        m_szName = setConfig.value("Name", m_szName).toString();
        m_szDateFormat = setConfig.value("DateFormat", "yyyy-MM-dd").toString();
        nInterval = setConfig.value("Interval", nInterval).toUInt();
        m_nCount = setConfig.value("Count", 0).toULongLong();
        QString szLength = setConfig.value("Length", 0).toString();
        if(!szLength.isEmpty()) {
            QRegularExpression e("(\\d+)([mkg]?)",
                                 QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch m;
            if(szLength.contains(e, &m) && m.hasMatch())
            {
                quint64 len = 0;
                if(m.capturedLength(1) > 0)
                    len = m.captured(1).toULong();
                if(m.capturedLength(2) > 0)
                {
                    QString u = m.captured(2).toUpper();
                    if("K" == u)
                        len <<= 10;
                    else if("M" == u)
                        len <<= 20;
                    else if("G" == u)
                        len <<= 30;
                }
                m_nLength = len;
            }
        }
        // Log file end

        g_bPrintStackTrace = setConfig.value(
                                          "PrintStackTrace",
                                          g_bPrintStackTrace).toBool();
        QStringList lstPrintStackTraceLevel;
        foreach(auto l, g_lstPrintStackTraceLevel) {
            lstPrintStackTraceLevel << QString::number(l);
        }
        lstPrintStackTraceLevel =
            setConfig.value("PrintStackTraceLevel",
                            lstPrintStackTraceLevel).toStringList();
        g_lstPrintStackTraceLevel.clear();
        foreach (auto level, lstPrintStackTraceLevel) {
            g_lstPrintStackTraceLevel << (QtMsgType)level.toInt();
        }

        setConfig.endGroup(); // Log end

        setConfig.beginGroup("Rules");
        auto keys = setConfig.childKeys();
        //qDebug(log) << "keys" << keys;
        szFilterRules.clear();
        foreach(auto k, keys) {
            QString v = setConfig.value(k).toString();
            szFilterRules += k + "=" + v + "\n";
        }
        setConfig.endGroup(); // Rules end
    }

    if(!szFilterRules.isEmpty())
        QLoggingCategory::setFilterRules(szFilterRules);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qSetMessagePattern(szPattern);
#endif

    QDir d;
    if(!d.exists(m_szPath))
    {
        if(!d.mkpath(m_szPath)) {
            qCritical(log) << "Create log directory fail." << m_szPath;
        }
    }

    g_Mutex.lock();
    if(g_File.isOpen())
        g_File.close();
    g_Mutex.unlock();
    g_File.setFileName(QString());
    slotTimeout();

    if (QFile::exists(szFile))
        qInfo(log) << "Load log configure file:" << m_szConfigureFile;
    else
        qWarning(log) << "Log configure file is not exist:" << szFile
                      << ". Use default settings.";

    qInfo(log) << "Log configure:"
               << "\n    Path:" << m_szPath
               << "\n    Name:" << m_szName
               << "\n    DateFormat:" << m_szDateFormat
               << "(Base file name: " + getBaseName() + ")"
               << "\n    Interval:" << nInterval
               << "\n    Count:" << m_nCount
               << "\n    Length:" << m_nLength
               << "\n    PrintStackTrace:" << g_bPrintStackTrace
               << "\n    PrintStackTraceLevel:" << g_lstPrintStackTraceLevel
               << "\n    szPattern:" << szPattern
               << "\n    Rules:" << szFilterRules;

    m_Watcher.addPath(m_szConfigureFile);

    if(m_Timer.isActive())
        m_Timer.stop();
    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotTimeout()));
    Q_ASSERT(check);
    m_Timer.start(nInterval * 1000);

    return nRet;
}

void CLog::slotFileChanged(const QString &szPath)
{
    qDebug(log) << "File changed:" << szPath;
    if(GetLogConfigureFile() != szPath)
        return;
    LoadConfigure(szPath);
}

QString CLog::GetLogFile()
{
    return g_File.fileName();
}

QString CLog::GetLogConfigureFile()
{
    return m_szConfigureFile;
}

QString CLog::GetLogDir()
{
    QString f = GetLogFile();
    if(f.isEmpty()) return f;
    
    QFileInfo fi(f);
    return fi.absolutePath();
}

int CLog::SetFilter(const QString &szInclude, const QString &szExclude)
{
    g_reInclude = QRegularExpression(szInclude);
    g_reExclude = QRegularExpression(szExclude);
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Log/Filter/include", szInclude);
    set.setValue("Log/Filter/Exclude", szExclude);
    return 0;
}

int CLog::GetFilter(QString &szInclude, QString &szExclude)
{
    szInclude = g_reInclude.pattern();
    szExclude = g_reExclude.pattern();
    return 0;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void CLog::myMessageOutput(QtMsgType type,
                           const QMessageLogContext &context,
                           const QString &msg)
{
    QString szMsg;
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    szMsg = qFormatLogMessage(type, context, msg);
#else
    szMsg = msg;
#endif

    if(g_reInclude.isValid() && !g_reInclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reInclude.match(szMsg);
        if(!m.hasMatch()) {
            return;
        }
    }
    if(g_reExclude.isValid() && !g_reExclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reExclude.match(szMsg);
        if(m.hasMatch()) {
            return;
        }
    }

    if(g_bPrintStackTrace && g_lstPrintStackTraceLevel.contains(type))
    {
        szMsg += "\n";
        if(g_pStack)
            szMsg += g_pStack->GetStack();
    }

#ifdef HAVE_RABBITCOMMON_GUI
    if(g_pDcokDebugLog)
        emit g_pDcokDebugLog->sigAddLog(szMsg);
#endif

    g_Mutex.lock();
    if(g_File.isOpen())
    {
        QTextStream s(&g_File);

        s << szMsg << "\r\n";
        //s.flush();
    }

    if(g_originalMessageHandler) {
        QString szRawMsg = msg;
        if(g_bPrintStackTrace && g_lstPrintStackTraceLevel.contains(type))
        {
            szRawMsg += "\n";
            if(g_pStack)
                szRawMsg += g_pStack->GetStack();
        }
        g_originalMessageHandler(type, context, szRawMsg);
    }
    g_Mutex.unlock();
}
#else
void CLog::myMessageOutput(QtMsgType type, const char* msg)
{
    QString szMsg(msg);
    
    if(g_reInclude.isValid() && !g_reInclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reInclude.match(szMsg);
        if(!m.hasMatch()) {
            return;
        }
    }
    if(g_reExclude.isValid() && !g_reExclude.pattern().isEmpty()) {
        QRegularExpressionMatch m = g_reExclude.match(szMsg);
        if(m.hasMatch()) {
            return;
        }
    }

    if(g_bPrintStackTrace && g_lstPrintStackTraceLevel.contains(type))
    {
        szMsg += "\n";
        if(g_pStack)
            szMsg += g_pStack->GetStack();
    }

#ifdef HAVE_RABBITCOMMON_GUI
    if(g_pDcokDebugLog)
        emit g_pDcokDebugLog->sigAddLog(szMsg);
#endif

    g_Mutex.lock();
    if(g_File.isOpen())
    {
        QTextStream s(&g_File);
        s << szMsg << "\r\n";
    }
    g_Mutex.unlock();

    if(g_originalMessageHandler) {
        QString szRawMsg = msg;
        if(g_bPrintStackTrace && g_lstPrintStackTraceLevel.contains(type))
        {
            szRawMsg += "\n";
            if(g_pStack)
                szRawMsg += g_pStack->GetStack();
        }
        g_originalMessageHandler(type, szRawMsg);
    }
}
#endif

void CLog::checkFileCount()
{
    if(0 == m_nCount) return;
    QString szFile;
    QDir d(m_szPath);
    d.setNameFilters(QStringList() << getBaseName() + "*");
    auto lstFiles = d.entryInfoList(QDir::Files, QDir::Time);
    if(lstFiles.size() <= m_nCount) return;

    if(lstFiles.first().lastModified() < lstFiles.back().lastModified())
        szFile = lstFiles.first().absoluteFilePath();
    else
        szFile = lstFiles.back().absoluteFilePath();

    if(szFile.isEmpty()) return;

    bool bRet = false;
    bRet = d.remove(szFile);
    if(bRet)
        qDebug(log) << "Remove file:" << szFile;
    else
        qCritical(log) << "Remove file fail:" << szFile;
}

QString CLog::getBaseName()
{
    QString szSep("_");
    return m_szName + szSep
           + QDate::currentDate().toString(m_szDateFormat) + szSep
           + QString::number(QCoreApplication::applicationPid())
        ;
}

QString CLog::getFileName()
{
    QChar fill('0');
    QString szNo = QString("%1").arg(1, 4, 10, fill);
    QString szSep("_");
    QString szName;
    QString szFile;
    QDir d(m_szPath);
    
    szName = getBaseName();
    d.setNameFilters(QStringList() << szName + "*");
    auto lstFiles = d.entryInfoList(QDir::Files, QDir::Name);
    if(lstFiles.isEmpty())
        szFile = m_szPath + QDir::separator() + szName + szSep + szNo + ".log";
    else
        szFile = lstFiles.back().absoluteFilePath();

    return szFile;
}

QString CLog::getNextFileName(const QString szFile)
{
    if(szFile.isEmpty())
        return QString();

    QFileInfo fi(szFile);
    QChar fill('0');
    QString szNo = QString("%1").arg(1, 4, 10, fill);
    QString szSep("_");
    QString szName = fi.baseName();

    auto s = szName.split(szSep);
    if(s.size() > 0)
    {
        szNo = s[s.size() - 1];
        szNo = QString("%1").arg(szNo.toInt() + 1, 4, 10, fill);
    }

    return m_szPath + QDir::separator() + getBaseName() +szSep + szNo + ".log";
}

bool CLog::checkFileLength()
{
    if(m_nLength == 0) return false;

    if(g_File.fileName().isEmpty()) return false;

    QFileInfo fi(g_File.fileName());
    if(fi.exists()) {
        if(fi.size() < m_nLength) return false;
    } else {
        return false;
    }

    return true;
}

bool CLog::checkFileName()
{
    QString szFile;
    szFile = g_File.fileName();
    QFileInfo fi(szFile);
    szFile = fi.baseName();
    
    int nPos = szFile.lastIndexOf("_");
    return szFile.left(nPos) == getBaseName();
}

void CLog::slotTimeout()
{
    QString szFile;
    if(g_File.fileName().isEmpty())
    {
        szFile = getFileName();
        g_File.setFileName(szFile);
    }

    do {
        if(checkFileLength())
            szFile = getNextFileName(g_File.fileName());
        else if(!checkFileName())
            szFile = getFileName();
        else
            if(g_File.isOpen()) break;
#ifdef Q_OS_ANDROID
        if(szFile.isEmpty()) return;
#endif
        Q_ASSERT(!szFile.isEmpty());

        g_Mutex.lock();
        if(g_File.isOpen())
            g_File.close();

        g_File.setFileName(szFile);
        bool bRet = g_File.open(QFile::WriteOnly | QFile::Append);
        g_Mutex.unlock();
        if(bRet) {
            qInfo(log) << "Log file:" << g_File.fileName();
        } else {
            qCritical(log) << "Open log file fail." << g_File.fileName();
        }
    } while(0);
    
    checkFileCount();
}

#ifdef HAVE_RABBITCOMMON_GUI

void OpenLogConfigureFile()
{
    QString f = RabbitCommon::CLog::Instance()->GetLogConfigureFile();
    if(f.isEmpty())
    {
        qCritical(log) << "Configure file is empty";
        return;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    bool bRet = false;
    if(env.value("SNAP").isEmpty() && env.value("APPDIR").isEmpty()) {
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(f));
    }
    if(bRet)
        return;

    qDebug(log) << "Open log configure file:" << f;
    CDlgEdit e(QObject::tr("Log configure file"), f,
               QObject::tr("Log configure file:") + f, false);
    if(RC_SHOW_WINDOW(&e) != QDialog::Accepted)
        return;

    QFile file(f);
    if(file.open(QFile::WriteOnly)) {
        QString szText = e.getContext();
        file.write(szText.toStdString().c_str(), szText.length());
        file.close();
        return;
    }
    qWarning(log) << "Save log configure file fail:" << file.errorString() << f;
    QFileInfo fi(f);
    QString szFile
        = QFileDialog::getSaveFileName(
            nullptr, QObject::tr("Save as..."),
            RabbitCommon::CDir::Instance()->GetDirUserConfig()
                + QDir::separator() + fi.fileName()
            );
    if(!szFile.isEmpty()) {
        QFile f(szFile);
        if(f.open(QFile::WriteOnly)) {
            QString szText = e.getContext();
            f.write(szText.toStdString().c_str(), szText.length());
            f.close();
            QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                          QSettings::IniFormat);
            set.setValue("Log/ConfigFile", szFile);
            qInfo(log) << "Save log configure file:" << szFile;
        }
    }
}

void OpenLogFile()
{
    QString f = RabbitCommon::CLog::Instance()->GetLogFile();
    if(f.isEmpty())
    {
        qCritical(log) << "Log file is empty";
        return;
    }
    bool bRet = false;
    auto env = QProcessEnvironment::systemEnvironment();
    if(env.value("SNAP").isEmpty())
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(f));
    if(!bRet) {
        //qCritical(log) << "Open log file fail:" << f;
        CDlgEdit e(QObject::tr("Log file"), f, QObject::tr("Log file:") + f);
        RC_SHOW_WINDOW(&e);
    }
}

void OpenLogFolder()
{
    QString d = RabbitCommon::CLog::Instance()->GetLogDir();
    if(d.isEmpty())
    {
        qCritical(log) << "Log folder is empty";
        return;
    }
    bool bRet = false;
    auto env = QProcessEnvironment::systemEnvironment();
    if(env.value("SNAP").isEmpty())
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(d));
    if(!bRet) {
        CFileBrowser* f = new CFileBrowser();
        f->setWindowTitle(QObject::tr("Log folder"));
        f->setRootPath(d);
#if defined(Q_OS_ANDROID)
        f->showMaximized();
#else
        f->show();
#endif
    }
}

#endif // #ifdef HAVE_RABBITCOMMON_GUI

} // namespace RabbitCommon
