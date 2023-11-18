// Window以及Ubuntu下的core dump调试方法: https://blog.csdn.net/zhoukehu_CSDN/article/details/125152019?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-125152019-blog-109705192.235^v38^pc_relevant_sort_base3&spm=1001.2101.3001.4242.1&utm_relevant_index=3
// Windows平台coredump处理: https://blog.csdn.net/mumufan05/article/details/109705192?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-109705192-blog-7306282.pc_relevant_aa&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-109705192-blog-7306282.pc_relevant_aa&utm_relevant_index=5

#include <qglobal.h>

#ifdef Q_OS_WIN
#include <Windows.h>
#include "dbghelp.h"
#include <QDir>
#include <QDateTime>
#ifdef HAVE_RABBITCOMMON_GUI
#include <QMessageBox>
#endif
#include <QTranslator>
#include <QCoreApplication>
#include "MiniDumper.h"

//#pragma comment( lib, "Dbghelp.lib" )

namespace RabbitCommon {

LONG WINAPI AppExceptionCallback(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	if (!QDir("logs").exists())
	{
		QDir().mkdir("logs");
	}

	QString dumpName = QDir::toNativeSeparators(
	    QCoreApplication::applicationDirPath() + QDir::separator()
               + qApp->applicationName() + QString("_%1_%2_%3.dmp")
		.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
		.arg(GetCurrentProcessId())
		.arg(GetCurrentThreadId())
		);

	QString msg;
	HANDLE hDumpFile = CreateFileW(dumpName.toStdWString().c_str(),
                                   GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_WRITE | FILE_SHARE_READ,
                                   0, CREATE_ALWAYS, 0, 0);
	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExpParam;
		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = ExceptionInfo;
		ExpParam.ClientPointers = TRUE;

		// 创建Dump文件
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                          hDumpFile, MiniDumpWithDataSegs,
                          ExceptionInfo ? &ExpParam : nullptr, nullptr, nullptr);
		CloseHandle(hDumpFile);
	}

	// 提示
    QString strTitle = QObject::tr("Application Error");
    QString strContent = QObject::tr("I'm Sorry, Application is Crash!") + "\n\n"
                         + QObject::tr("The current path: ") + QDir::currentPath() + "\n\n"
                         + QObject::tr("The dump file: ") + dumpName;
#ifdef HAVE_RABBITCOMMON_GUI
    QMessageBox::critical(0, strTitle, strContent, QMessageBox::Ok);
#endif

	return EXCEPTION_EXECUTE_HANDLER;
}

void EnableMiniDumper()
{
	SetUnhandledExceptionFilter(AppExceptionCallback);
}

} // namespace RabbitCommon

#endif //Q_OS_WIN
