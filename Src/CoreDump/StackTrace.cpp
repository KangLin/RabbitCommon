// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QThread>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QUrl>
#include <QMimeData>
#ifdef HAVE_RABBITCOMMON_GUI
#include <QMessageBox>
#include <QPushButton>
#include <QDesktopServices>
#include <QClipboard>
#include <QApplication>
#endif

#if defined(Q_OS_WIN)
#include <process.h>
#include <Windows.h>
#include <dbghelp.h>
#if HAVE_StackWalker
#include "StackWalker.h"
#endif
#include "CoreDump/MiniDumper.h"
#elif defined(Q_OS_ANDROID)
#include <unwind.h>
#include <dlfcn.h>
#include <vector>
#else
#include <execinfo.h>
#include <cxxabi.h>
#endif

#include "RabbitCommonTools.h"
#include "Log/Log.h"
#include "StackTrace.h"

namespace RabbitCommon {

const char* g_PattenFileLine = "%p [%s] in %s:%lu";
const char* g_PattenDLL = "%p [%s] in %s";

static Q_LOGGING_CATEGORY(log, "RabbitCommon.StackTrace")

CCallTrace::CCallTrace(QObject *parent)
    : QObject{parent}
    , m_hMainThread(QThread::currentThreadId())
{}

#ifdef HAVE_RABBITCOMMON_GUI
void CCallTrace::ShowCoreDialog(QString szTitle, QString szContent, QString szDetail, QString szCoreDumpFile)
{
    if(QThread::currentThreadId() == m_hMainThread) {
        qDebug(log) << "Main thread:" << m_hMainThread << "core";
        return slotShowCoreDialog(szTitle, szContent, szDetail, szCoreDumpFile);
    }
    qDebug(log) << "Thread:" << QThread::currentThreadId() << "core."
                << "main thread:" << m_hMainThread;
    bool check = false;
    check = connect(this, SIGNAL(sigShowCoreDialog(QString,QString,QString,QString)),
                    this, SLOT(slotShowCoreDialog(QString,QString,QString,QString)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    emit sigShowCoreDialog(szTitle, szContent, szDetail, szCoreDumpFile);
}

void CCallTrace::slotShowCoreDialog(QString szTitle, QString szContent,
                                QString szDetail, QString szCoreDumpFile)
{
    qDebug(log) << "CCallTrace::slotShowCoreDialog";
    QMessageBox msg(QMessageBox::Icon::Critical, szTitle, szContent, QMessageBox::StandardButton::Close);
    QPushButton* pOpenLogFile = msg.addButton(QObject::tr("Open log file"), QMessageBox::ActionRole);
    QPushButton* pOpenCoreDumpFolder = msg.addButton(QObject::tr("Open core dump folder"), QMessageBox::ActionRole);
#ifndef QT_NO_CLIPBOARD
    QPushButton* pCopyClipboard = msg.addButton(tr("Copy to clipboard"), QMessageBox::ActionRole);
#endif
    if(!szDetail.isEmpty())
        msg.setDetailedText(szDetail);
    RC_SHOW_WINDOW(&msg);
    if(msg.clickedButton() == pOpenLogFile)
    {
        OpenLogFile();
    } else if (msg.clickedButton() == pOpenCoreDumpFolder) {
        QFileInfo info(szCoreDumpFile);
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#ifndef QT_NO_CLIPBOARD
    } else if(msg.clickedButton() == pCopyClipboard) {
        QClipboard* cb = QGuiApplication::clipboard();
        if(!cb) {
            qCritical(log) << "The application has not clipboard";
            return;
        }
        QMimeData* m = new QMimeData();
        if(!m) {
            qCritical(log) << "new QMimeData fail";
            return;
        }
        QList<QUrl> lstUrl;
        lstUrl << CLog::Instance()->GetLogFile() << szCoreDumpFile;
        m->setUrls(lstUrl);
        cb->setMimeData(m);
        qDebug(log) << "Clipboard urls" << cb->mimeData()->urls();
#endif
    }
    qDebug(log) << "CCallTrace::slotShowCoreDialog end";
}
#endif //#ifdef HAVE_RABBITCOMMON_GUI

QString CCallTrace::GetStack(uint index)
{
    QString szMsg("Stack:\n");
    QStringList szTrace = GetStack(index, 63);
    for(int i = 0; i < szTrace.length(); i++) {
        szMsg += "    " + QString::number(i + 1) + " " + szTrace[i] + "\n";
    }
    return szMsg;
}

#if defined(Q_OS_WIN)

// See: https://segmentfault.com/q/1010000042761513
#define TRACE_MAX_STACK_FRAMES 62
QStringList CCallTrace::GetStack(uint index, unsigned int max_frames)
{
    QStringList szStack;
    const int nLen = 1024;
    void *stack[TRACE_MAX_STACK_FRAMES];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD numberOfFrames = CaptureStackBackTrace(index, TRACE_MAX_STACK_FRAMES, stack, NULL);
    char buf[sizeof(SYMBOL_INFO) + (nLen - 1) * sizeof(TCHAR)];
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
    symbol->MaxNameLen = nLen;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;
    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    QScopedArrayPointer<char> bufStack(new char[nLen]);
    if(!bufStack)
    {
        qCritical(log) << "new buffer fail";
        return szStack;
    }
    for (int i = 0; i < numberOfFrames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, NULL, symbol);
        if (SymGetLineFromAddr64(process, address, &displacement, &line))
        {
            snprintf(bufStack.data(), nLen, g_PattenFileLine,
                     (LPVOID)symbol->Address, symbol->Name, line.FileName, line.LineNumber);
        }
        else
        {
            //failed to get line
            const int MaxNameLen = 256;
            QScopedArrayPointer<char> module(new char[MaxNameLen]);
            HMODULE hModule = NULL;
            lstrcpyA(module.data(), "");
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                                 | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR) address, &hModule);

            //at least print module name
            if(hModule != NULL)
                GetModuleFileNameA(hModule, module.data(), MaxNameLen);

            snprintf(bufStack.data(), nLen, g_PattenDLL,
                     (LPVOID)symbol->Address, symbol->Name, module.data());
        }
        szStack << bufStack.data();
    }
    return szStack;
}


#if HAVE_StackWalker
// See: https://github.com/JochenKalmbach/StackWalker
static void MyStrCpy(char* szDest, size_t nMaxDestSize, const char* szSrc)
{
    if (nMaxDestSize <= 0)
        return;
    strncpy_s(szDest, nMaxDestSize, szSrc, _TRUNCATE);
    // INFO: _TRUNCATE will ensure that it is null-terminated;
    // but with older compilers (<1400) it uses "strncpy" and this does not!)
    szDest[nMaxDestSize - 1] = 0;
} // MyStrCpy
class MyStackWalker : public StackWalker
{
public:
    MyStackWalker(QStringList *lstText,
                  int index,
                  int options = OptionsAll, // 'int' is by design, to combine the enum-flags
                  LPCSTR szSymPath = NULL,
                  DWORD  dwProcessId = GetCurrentProcessId(),
                  HANDLE hProcess = GetCurrentProcess())
        : StackWalker(options, szSymPath, dwProcessId, hProcess)
    {
        m_lstText = lstText;
        m_Index = index + 1;
    }
    QStringList *m_lstText;
    int m_Index;

protected:
    virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry) override;
    virtual void OnOutput(LPCSTR szText) override
    {}
};

void MyStackWalker::OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
{
    if(m_Index) {
        m_Index--;
        return;
    }

    CHAR   buffer[STACKWALK_MAX_NAMELEN];
    size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
    maxLen = _TRUNCATE;
#endif
    if ((eType != lastEntry) && (entry.offset != 0))
    {
        if (entry.name[0] == 0)
            MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, "(function-name not available)");
        if (entry.undName[0] != 0)
            MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undName);
        if (entry.undFullName[0] != 0)
            MyStrCpy(entry.name, STACKWALK_MAX_NAMELEN, entry.undFullName);
        if (entry.moduleName[0] == 0)
            MyStrCpy(entry.moduleName, STACKWALK_MAX_NAMELEN, "(module-name not available)");
        if (entry.lineFileName[0] == 0)
        {
            MyStrCpy(entry.lineFileName, STACKWALK_MAX_NAMELEN, "(filename not available)");
        }
        _snprintf_s(buffer, maxLen, "%s in [%s] (%s:%d) address: %p",
                    entry.name, entry.moduleName,
                    entry.lineFileName, entry.lineNumber,
                    (LPVOID)entry.offset);
        buffer[STACKWALK_MAX_NAMELEN - 1] = 0;
        *m_lstText << buffer;
    }
}

QStringList PrintStackTrace1(uint index, unsigned int max_frames)
{
    QStringList lstStack;
    MyStackWalker sw(&lstStack, index, StackWalker::RetrieveSymbol);
    sw.ShowCallstack();
    return lstStack;
}
#endif //#if HAVE_StackWalker

#elif defined(Q_OS_ANDROID)
// See: https://blog.csdn.net/taohongtaohuyiwei/article/details/105147933

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    std::vector<_Unwind_Word> &stack = *(std::vector<_Unwind_Word>*)arg;
    stack.push_back(_Unwind_GetIP(context));
    return _URC_NO_REASON;
}

QStringList CCallTrace::GetStack(uint index, unsigned int max_frames)
{
    QStringList lstStack;

    std::vector<_Unwind_Word> stack;
    _Unwind_Backtrace(unwindCallback, (void*)&stack);

    int nBufferSize = 1024;
    QScopedArrayPointer<char> buffer(new char[nBufferSize]);
    if(!buffer)
    {
        qCritical(log) << "new buffer fail";
        return lstStack;
    }
    for (int i = index; i < stack.size(); i++) {
        Dl_info info;
        if (!dladdr((void*)stack[i], &info)) {
            continue;
        }
        int addr = (char*)stack[i] - (char*)info.dli_fbase - 1;
        if (info.dli_sname == NULL || strlen(info.dli_sname) == 0) {
            sprintf(buffer.data(), "%p [%s]", addr, info.dli_fname);
        } else {
            sprintf(buffer.data(), "%p [%s] in %s",
                    addr, info.dli_sname, info.dli_fname);
        }
        lstStack << buffer.data();
    }

    return lstStack;
}
#else
// See: https://segmentfault.com/q/1010000042689957
//      http://skyscribe.github.io/blog/2012/11/27/linuxshang-ru-he-cong-c-plus-plus-cheng-xu-zhong-huo-qu-backtracexin-xi/
/** Print a demangled stack backtrace of the caller function to FILE* out. */
QStringList CCallTrace::GetStack(uint index, unsigned int max_frames)
{
    QStringList szMsg;

    int nBufferSize = 1024;
    QScopedArrayPointer<char> buffer(new char[nBufferSize]);
    if(!buffer)
    {
        qCritical(log) << "new buffer fail";
        return szMsg;
    }

    // storage array for stack trace address data
    void* addrlist[max_frames + 1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
        qCritical(log) << "Get backtrace is empty, possibly corrupt";
        return szMsg;
    }

    /*! ~/english resolve addresses into strings containing "filename(function+address)",
     *       this array must be free()-ed
     *
     * ~/chinese 接受 backtrace 返回的 buffer 和 size，
     *  将每一个函数地址通过符号表进行翻译，生成一个包含函数名，
     *  内部指令偏移和实际返回地址的字符串，
     *  并返回一个malloc出来的字符串数组，用户需要手工释放数组本身，
     *  但是数组的每一个指针本身则不必被用户释放
     */
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

                snprintf(buffer.data(), nBufferSize, "%p [%s] in %s",
                          begin_offset, funcname, symbollist[i]);
            } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                snprintf(buffer.data(), nBufferSize, "%p [%s] in %s",
                         begin_offset, begin_name, symbollist[i]);
            }
        }
        else
        {
            // couldn't parse the line? print the whole line.
            snprintf(buffer.data(), nBufferSize, "%s", symbollist[i]);
        }
        szMsg << buffer.data();
    }

    if(funcname)
        free(funcname);
    if(symbollist)
        free(symbollist);
    return szMsg;
}
#endif // #if defined(Q_OS_WIN)

} //namespace RabbitCommon {
