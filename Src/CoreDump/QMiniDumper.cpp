// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

// Window以及Ubuntu下的core dump调试方法: https://blog.csdn.net/zhoukehu_CSDN/article/details/125152019?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-125152019-blog-109705192.235^v38^pc_relevant_sort_base3&spm=1001.2101.3001.4242.1&utm_relevant_index=3
// Windows平台coredump处理: https://blog.csdn.net/mumufan05/article/details/109705192?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-109705192-blog-7306282.pc_relevant_aa&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-109705192-blog-7306282.pc_relevant_aa&utm_relevant_index=5
// Visual Studio 调试器中的转储文件: https://learn.microsoft.com/zh-cn/visualstudio/debugger/using-dump-files?view=vs-2022

#include <qglobal.h>

#ifdef Q_OS_WIN
#include <Windows.h>
#include "dbghelp.h"
#include <QDir>
#include <QDateTime>
#include <QTranslator>
#include <QCoreApplication>
#include <QLoggingCategory>

#include <QSettings>
#include "MiniDumper.h"
#include "Log/Log.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

//#pragma comment( lib, "Dbghelp.lib" )

namespace RabbitCommon {

LPTOP_LEVEL_EXCEPTION_FILTER g_UnhandledException = nullptr;

static Q_LOGGING_CATEGORY(log, "RabbitCommon.CoreDump.QMinDumper")
const int MaxNameLen = 256;
QString PrintStack(struct _EXCEPTION_POINTERS *pException) //Prints stack trace based on context record
{
    QString szStack;
    int no = 1;
    BOOL    result = false;
    HANDLE  process = NULL;
    HANDLE  thread = NULL;
    HMODULE hModule = NULL;

    STACKFRAME          stack;
    ULONG               frame = 0;
    DWORD64             displacement = 0;

    DWORD disp = 0;

    QScopedArrayPointer<char> buffer(new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]);
    QScopedArrayPointer<char> name(new char[MaxNameLen]);
    QScopedArrayPointer<char> module(new char[MaxNameLen]);
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer.data();

    szStack = "*** Exception 0x" + QString::number(pException->ExceptionRecord->ExceptionCode, 16) + " occurred ***\n\n";
    PCONTEXT ctx = pException->ContextRecord;

    // On x64, StackWalk64 modifies the context record, that could
    // cause crashes, so we create a copy to prevent it
    CONTEXT ctxCopy;
    memcpy(&ctxCopy, ctx, sizeof(CONTEXT));

    memset(&stack, 0, sizeof( STACKFRAME ));

    process                = GetCurrentProcess();
    thread                 = GetCurrentThread();
    displacement           = 0;
#if !defined(_M_AMD64)
    stack.AddrPC.Offset    = (*ctx).Eip;
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode   = AddrModeFlat;
#endif

    SymInitialize( process, NULL, TRUE ); //load symbols

    for( frame = 0; ; frame++ )
    {
        //get next call from stack
        result = StackWalk
            (
#if defined(_M_AMD64)
                IMAGE_FILE_MACHINE_AMD64
#else
                IMAGE_FILE_MACHINE_I386
#endif
                ,
                process,
                thread,
                &stack,
                &ctxCopy,
                NULL,
                SymFunctionTableAccess,
                SymGetModuleBase,
                NULL
                );

        if( !result ) break;

        //get symbol name for address
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, ( ULONG64 )stack.AddrPC.Offset, &displacement, pSymbol);

        QScopedPointer<IMAGEHLP_LINE> line(new IMAGEHLP_LINE());
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE);

        //try to get line
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, line.data()))
        {
            szStack += QString::number(no++) + " 0x" + QString::number(pSymbol->Address, 16)
                       + " [" +  QString(pSymbol->Name) + "] in "
                       + QString(line->FileName)
                       + ":" + QString::number(line->LineNumber)
                       + "\n";
        }
        else
        {
            //failed to get line
            hModule = NULL;
            lstrcpyA(module.data(), "");
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                              | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              (LPCTSTR)(stack.AddrPC.Offset), &hModule);

            //at least print module name
            if(hModule != NULL)
                GetModuleFileNameA(hModule, module.data(), MaxNameLen);

            szStack += QString::number(no++) + " 0x" + QString::number(pSymbol->Address, 16)
                       + " [" + QString(pSymbol->Name)
                       + "] in " + module.data() + "\n";
        }
    }
    qCritical(log) << szStack.toStdString().c_str();
    return szStack;
}

QString CoreDump(struct _EXCEPTION_POINTERS *pException)
{
    QString szPath = CDir::Instance()->GetDirLog() + QDir::separator() + "Core";
	if (!QDir(szPath).exists())
	{
		QDir().mkdir(szPath);
	}

    QString dumpName = QDir::toNativeSeparators(
        szPath + QDir::separator()
        + qApp->applicationName()
        + QString("_%1_%2_%3.dmp")
              .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
              .arg(GetCurrentProcessId())
              .arg(GetCurrentThreadId())
        );
    qCritical(log) << "The core dump file:" << dumpName;

	QString msg;
	HANDLE hDumpFile = CreateFileW(dumpName.toStdWString().c_str(),
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_WRITE | FILE_SHARE_READ,
                                   0, CREATE_ALWAYS, 0, 0);
	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExpParam;
		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pException;
		ExpParam.ClientPointers = TRUE;

		// 创建Dump文件
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                          hDumpFile, MiniDumpWithDataSegs,
                          pException ? &ExpParam : nullptr, nullptr, nullptr);
		CloseHandle(hDumpFile);
	} else
        qCritical(log) << "Core write file fail.";

    return dumpName;
}

LONG WINAPI AppExceptionCallback(struct _EXCEPTION_POINTERS *pException)
{
    QString szStack;
    QString dumpName;
    bool bDumpToLogFile = true;
    bool bDumpFile = true;
    QString szConfigure = CLog::Instance()->GetLogConfigureFile();
    if(!szConfigure.isEmpty()) {
        QSettings set(szConfigure, QSettings::IniFormat);
        bDumpToLogFile = set.value("CoreDump/DumpToLogFile", true).toBool();
        bDumpFile = set.value("CoreDump/DumpFile", true).toBool();
    }
    if(!(bDumpFile || bDumpToLogFile))
        return EXCEPTION_CONTINUE_SEARCH;
    if(bDumpToLogFile)
        szStack = PrintStack(pException);
    if(bDumpFile)
        dumpName = CoreDump(pException);

    // 提示
    QString szTitle = QObject::tr("Application Error");
    QString szContent
        = QObject::tr("I'm Sorry, Application is Crash!") + "\n\n"
          + QObject::tr("Current path: ") + QDir::currentPath() + "\n\n";
    if(bDumpFile)
          szContent += QObject::tr("Dump file: ") + dumpName + "\n\n";
    if(bDumpToLogFile)
        szContent += QObject::tr("Log file: ") + RabbitCommon::CLog::Instance()->GetLogFile();
#ifdef HAVE_RABBITCOMMON_GUI
    CTools::Instance()->ShowCoreDialog(szTitle, szContent, szStack, dumpName);
#endif

    LONG ret = EXCEPTION_EXECUTE_HANDLER;
    if(g_UnhandledException) {
        qCritical(log) << "Call system default exception";
        ret = g_UnhandledException(pException);
    }
    qCritical(log) << "Exception exit:" << ret;
    return ret;
}

void EnableMiniDumper()
{
	g_UnhandledException = SetUnhandledExceptionFilter(AppExceptionCallback);
}

} // namespace RabbitCommon

#endif //Q_OS_WIN
