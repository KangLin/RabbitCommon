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

#if defined(Q_OS_WIN)
    #include <process.h>
    #include <Windows.h>
    #include <dbghelp.h>
#elif !defined(Q_OS_ANDROID)
    #include <execinfo.h>
    #include <cxxabi.h>
#endif

namespace RabbitCommon {

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QtMessageHandler g_originalMessageHandler = Q_NULLPTR;
#else
    QtMsgHandler g_originalMessageHandler = Q_NULLPTR;
#endif
QRegularExpression g_reInclude;
QRegularExpression g_reExclude;
bool g_bPrintStackTrace = false;
QList<QtMsgType> g_lstPrintStackTraceLevel(QtMsgType::QtCriticalMsg);

QString PrintStackTrace();
QStringList PrintStackTrace(uint index, unsigned int max_frames = 63);

static Q_LOGGING_CATEGORY(log, "RabbitCommon.log")
CLog::CLog() : QObject(),
    m_szName(QCoreApplication::applicationName()),
    m_szDateFormat("yyyy-MM-dd"),
    m_nLength(0),
    m_nCount(0)
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    
    SetFilter(set.value("Log/Filter/include").toString(),
              set.value("Log/Filter/Exclude").toString());
    
    QString szPattern = "[%{time hh:mm:ss.zzz} %{pid}|%{threadid} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}E%{endif}%{if-fatal}F%{endif}] %{category} - %{message}";
#ifdef QT_MESSAGELOGCONTEXT
    // Use qt message log context(__FILE__, __LIEN__, __FUNCTION__) in release
    szPattern += " [%{file}:%{line}, %{function}]";
#elif defined(DEBUG) || defined(_DEBUG)
    szPattern += " [%{file}:%{line}, %{function}]";
#endif
    QString szFilterRules;
    quint64 nInterval = 60; // Unit: second
#if !(defined(DEBUG) || defined(_DEBUG) || ANDROID)
    szFilterRules = "*.debug = false";
#endif
    bool bReadOnly = false;
#if defined(Q_OS_ANDROID)
    bReadOnly = true;
#endif
    m_szPath = CDir::Instance()->GetDirLog();
    QString szConfFile = QStandardPaths::locate(QStandardPaths::ConfigLocation,
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
    if (QFile::exists(szConfFile))
    {
        m_szConfigureFile = szConfFile;
        qInfo(log) << "Load log configure file:" << m_szConfigureFile;
        QSettings setConfig(m_szConfigureFile, QSettings::IniFormat);
        setConfig.beginGroup("Log");
        m_szPath = setConfig.value("Path", m_szPath).toString();
        m_szName = setConfig.value("Name", m_szName).toString();
        m_szDateFormat = setConfig.value("DateFormat", m_szDateFormat).toString();
        szPattern = setConfig.value("Pattern", szPattern).toString();
        nInterval = setConfig.value("Interval", nInterval).toUInt();
        m_nCount = setConfig.value("Count", 0).toULongLong();
        g_bPrintStackTrace = setConfig.value("PrintStackTrace", g_bPrintStackTrace).toBool();
        QStringList lstPrintStackTraceLevel;
        foreach(auto l, g_lstPrintStackTraceLevel) {
            lstPrintStackTraceLevel << QString::number(l);
        }
        lstPrintStackTraceLevel = setConfig.value("PrintStackTraceLevel", lstPrintStackTraceLevel).toStringList();
        g_lstPrintStackTraceLevel.clear();
        foreach (auto level, lstPrintStackTraceLevel) {
            g_lstPrintStackTraceLevel << (QtMsgType)level.toInt();
        }
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
        setConfig.endGroup();

        setConfig.beginGroup("Rules");
        auto keys = setConfig.childKeys();
        //qDebug(log) << "keys" << keys;
        szFilterRules.clear();
        foreach(auto k, keys) {
            QString v = setConfig.value(k).toString();
            szFilterRules += k + "=" + v + "\n";
        }
        setConfig.endGroup();
    } else {
        qWarning(log) << "Log configure file is not exist:" << szConfFile
                         << ". Use default settings.";
    }

    qDebug(log) << "Log configure:"
                << "\n    Path:" << m_szPath
                << "\n    Name:" << m_szName
                << "\n    DateFormat:" << m_szDateFormat
                << "(Base file name: " + getBaseName() + ")"
                << "\n    szPattern:" << szPattern
                << "\n    Interval:" << nInterval
                << "\n    Count:" << m_nCount
                << "\n    Length:" << m_nLength
                << "\n    PrintStackTrace:" << g_bPrintStackTrace
                << "\n    PrintStackTraceLevel:" << g_lstPrintStackTraceLevel
                << "\n    Rules:" << szFilterRules;

    if(!szFilterRules.isEmpty())
        QLoggingCategory::setFilterRules(szFilterRules);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qSetMessagePattern(szPattern);
    g_originalMessageHandler = qInstallMessageHandler(myMessageOutput);
#else
    g_originalMessageHandler = qInstallMsgHandler(myMessageOutput);
#endif

    QDir d;
    if(!d.exists(m_szPath))
    {
        if(!d.mkpath(m_szPath)) {
            //NOTE: Do not use qDebug or qCritical. Causes recursive calls, stuck in an endless loop.
            fprintf(stderr, "Create log directory fail. %s\n",
                    m_szPath.toStdString().c_str());
        }
    }

    slotTimeout();
    bool check = connect(&m_Timer, SIGNAL(timeout()),
                         this, SLOT(slotTimeout()));
    Q_ASSERT(check);
    m_Timer.start(nInterval * 1000);
}

CLog::~CLog()
{
    if(m_File.isOpen())
        m_File.close();
    if(m_Timer.isActive())
        m_Timer.stop();
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

QString CLog::GetLogFile()
{
    return m_File.fileName();
}

QString CLog::OpenLogConfigureFile()
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

    if(g_lstPrintStackTraceLevel.contains(type) && g_bPrintStackTrace)
    {
        szMsg += "\n" + PrintStackTrace();
    }

#ifdef HAVE_RABBITCOMMON_GUI
        if(g_pDcokDebugLog)
            emit g_pDcokDebugLog->sigAddLog(szMsg);
#endif

    /*
    QFile f(CLog::Instance()->GetLogFile());
    if(!f.open(QFile::WriteOnly | QFile::Append))
    {
        fprintf(stderr, "Open log file fail. %s\n",
                f.fileName().toStdString().c_str());
        return;
    }//*/

    if(CLog::Instance()->m_File.isOpen())
    {
        QTextStream s(&CLog::Instance()->m_File);
        CLog::Instance()->m_Mutex.lock();
        s << szMsg << "\r\n";
        //s.flush();
        CLog::Instance()->m_Mutex.unlock();
    }

    //f.close();

    if(g_originalMessageHandler) {
        QString szRawMsg = msg;
        if(g_lstPrintStackTraceLevel.contains(type) && g_bPrintStackTrace)
        {
            szRawMsg += "\n" + PrintStackTrace();
        }
        g_originalMessageHandler(type, context, szRawMsg);
    }
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

    if(g_lstPrintStackTraceLevel.contains(type) && g_bPrintStackTrace)
    {
        szMsg += "\n" + PrintStackTrace();
    }

#ifdef HAVE_RABBITCOMMON_GUI
    if(g_pDcokDebugLog)
        emit g_pDcokDebugLog->sigAddLog(szMsg);
#endif

    /*
    QFile f(CLog::Instance()->GetLogFile());
    if(!f.open(QFile::WriteOnly | QFile::Append))
    {
        fprintf(stderr, "Open log file fail. %s\n",
                f.fileName().toStdString().c_str());
        return;
    } //*/
    
    if(CLog::Instance()->m_File.isOpen())
    {
        QTextStream s(&CLog::Instance()->m_File);
        CLog::Instance()->m_Mutex.lock();
        s << szMsg << "\r\n";
        CLog::Instance()->m_Mutex.unlock();
    }

    //f.close();

    if(g_originalMessageHandler) {
        QString szRawMsg = msg;
        if(g_lstPrintStackTraceLevel.contains(type) && g_bPrintStackTrace)
        {
            szRawMsg += "\n" + PrintStackTrace();
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
    //NOTE: Do not use qDebug or qCritical. Causes recursive calls, stuck in an endless loop.
    if(bRet)
        printf("Remove file: %s\n", szFile.toStdString().c_str());
    else
        printf("Remove file fail: %s", szFile.toStdString().c_str());
}

QString CLog::getBaseName()
{
    QString szSep("_");
    return m_szName + szSep
           + QString::number(QCoreApplication::applicationPid()) + szSep
           + QDate::currentDate().toString(m_szDateFormat)
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

    if(m_File.fileName().isEmpty()) return false;

    QFileInfo fi(m_File.fileName());
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
    szFile = m_File.fileName();
    QFileInfo fi(szFile);
    szFile = fi.baseName();
    
    int nPos = szFile.lastIndexOf("_");
    return szFile.left(nPos) == getBaseName();
}

void CLog::slotTimeout()
{
    QString szFile;
    if(m_File.fileName().isEmpty())
    {
        szFile = getFileName();
        m_File.setFileName(szFile);
    }

    do {
        if(checkFileLength())
            szFile = getNextFileName(m_File.fileName());
        else if(!checkFileName())
            szFile = getFileName();
        else
            if(m_File.isOpen()) break;
#ifdef Q_OS_ANDROID
        if(szFile.isEmpty()) return;
#endif
        Q_ASSERT(!szFile.isEmpty());

        m_Mutex.lock();
        if(m_File.isOpen())
            m_File.close();

        m_File.setFileName(szFile);
        bool bRet = m_File.open(QFile::WriteOnly | QFile::Append);
        m_Mutex.unlock();
        if(bRet) {
            //NOTE: Do not use qDebug or qCritical. Causes recursive calls, stuck in an endless loop.
            fprintf(stderr, "Log file: %s\n",
                    m_File.fileName().toStdString().c_str());
        } else {
            //NOTE: Do not use qDebug or qCritical. Causes recursive calls, stuck in an endless loop.
            fprintf(stderr, "Open log file fail. %s\n",
                    m_File.fileName().toStdString().c_str());
        }
    } while(0);
    
    checkFileCount();
}

#ifdef HAVE_RABBITCOMMON_GUI

void OpenLogConfigureFile()
{
    QString f = RabbitCommon::CLog::Instance()->OpenLogConfigureFile();
    if(f.isEmpty())
    {
        qCritical(log) << "Configure file is empty";
        return;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    bool bRet = false;
    if(env.value("SNAP").isEmpty()) {
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
        = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save as..."),
                                       QStandardPaths::writableLocation(
                                           QStandardPaths::ConfigLocation)
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
    if(env.value("SNAP").isEmpty()) {
        bRet = QDesktopServices::openUrl(f);
    }
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
    if(env.value("SNAP").isEmpty()) {
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(d));
    }
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

#if defined(Q_OS_WIN)

// See: https://segmentfault.com/q/1010000042761513
#define TRACE_MAX_STACK_FRAMES 62
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024
QStringList PrintStackTrace(uint index, unsigned int max_frames)
{
    QStringList szStack;
    void *stack[TRACE_MAX_STACK_FRAMES];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD numberOfFrames = CaptureStackBackTrace(index, TRACE_MAX_STACK_FRAMES, stack, NULL);
    char buf[sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR)];
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
    symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    char bufStack[1024];
    for (int i = 0; i < numberOfFrames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, NULL, symbol);
        if (SymGetLineFromAddr64(process, address, &displacement, &line))
        {
            snprintf(bufStack, 1024, "%s in (%s:%lu) address: 0x%0X", symbol->Name, line.FileName, line.LineNumber, symbol->Address);
        }
        else
        {
            //printf("\tSymGetLineFromAddr64 returned error code %lu.", GetLastError());
            snprintf(bufStack, 1024, "%s address: 0x%0X", symbol->Name, symbol->Address);
        }
        szStack << bufStack;
    }
    return szStack;
}
#elif defined(Q_OS_ANDROID)
// See: https://blog.csdn.net/taohongtaohuyiwei/article/details/105147933
#include <unwind.h>
#include <dlfcn.h>
#include <vector>

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    std::vector<_Unwind_Word> &stack = *(std::vector<_Unwind_Word>*)arg;
    stack.push_back(_Unwind_GetIP(context));
    return _URC_NO_REASON;
}

QStringList PrintStackTrace(uint index, unsigned int max_frames)
{
    QStringList lstStack;
    QString dump;
    std::vector<_Unwind_Word> stack;
    _Unwind_Backtrace(unwindCallback, (void*)&stack);

    int nBufferSize = 1024;
    QScopedPointer<char> buffer(new char[nBufferSize]);
    if(!buffer)
    {
        printf("new buffer fail");
        return lstStack;
    }
    for (int i = 0; i < stack.size(); i++) {
        Dl_info info;
        if (!dladdr((void*)stack[i], &info)) {
            continue;
        }
        int addr = (char*)stack[i] - (char*)info.dli_fbase - 1;
        if (info.dli_sname == NULL || strlen(info.dli_sname) == 0) {
            sprintf(buffer.data(), "#%02x pc %08x  %s\n", i, addr, info.dli_fname);
        } else {
            sprintf(buffer.data(), "#%02x pc %08x  %s (%s+00)\n", i, addr, info.dli_fname, info.dli_sname);
        }
        dump = buffer.data();
        lstStack << dump;
    }
    return lstStack;
}
#else
// See: https://segmentfault.com/q/1010000042689957
//      http://skyscribe.github.io/blog/2012/11/27/linuxshang-ru-he-cong-c-plus-plus-cheng-xu-zhong-huo-qu-backtracexin-xi/
/** Print a demangled stack backtrace of the caller function to FILE* out. */
QStringList PrintStackTrace(uint index, unsigned int max_frames)
{
    QStringList szMsg;

    int nBufferSize = 1024;
    QScopedPointer<char> buffer(new char[nBufferSize]);
    if(!buffer)
    {
        printf("new buffer fail");
        return szMsg;
    }

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
        printf("empty, possibly corrupt");
        return szMsg;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = index; i < addrlen; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
            && begin_name < begin_offset)
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string

                snprintf(buffer.data(), nBufferSize, "%s in (%s) address: 0x%0X",
                         funcname, symbollist[i], begin_offset);
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                snprintf(buffer.data(), nBufferSize, "%s in (%s) address: 0x%0X",
                         begin_name, symbollist[i], begin_offset);
            }
            szMsg << buffer.data();
        }
        else
        {
            // couldn't parse the line? print the whole line.
            printf("  %s\n", symbollist[i]);
        }
    }

    if(funcname)
        free(funcname);
    if(symbollist)
        free(symbollist);
    return szMsg;
}
#endif // #if defined(Q_OS_WIN)

QString PrintStackTrace()
{
    QString szMsg("Stack:\n");
    QStringList szTrace = PrintStackTrace(5);
    for(int i = 0; i < szTrace.length(); i++) {
        szMsg += "    " + QString::number(i + 1) + " " + szTrace[i] + "\n";
    }
    return szMsg;
}

} // namespace RabbitCommon
