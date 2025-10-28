#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QApplication>
#include "ExampleFileTransfer.h"
#include "ThreadPool.h"
#include "WorkerFileTransfer.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Pool.Example")
CExampleFileTransfer::CExampleFileTransfer(QObject *parent)
    : QObject{parent}
{}

void addTask(CTaskFileTransfer* task, CThreadPool& pool)
{
    qDebug(log) << "AddTask:" << task->Title();
    bool check = QObject::connect(
        task, &CTaskFileTransfer::sigError,
        [task](int nErr, const QString& szErr) {
            qDebug(log) << "Task error:" << task->Title() << nErr << szErr;
        });
    Q_ASSERT(check);
    check = QObject::connect(task, &CTaskFileTransfer::sigFinished, [task]() {
        qDebug(log) << "Task finished:" << task->Title();
    });
    Q_ASSERT(check);
    check = QObject::connect(
        task, &CTaskFileTransfer::sigProcess,
        [task](qint64 processed, qint64 total) {
            qDebug(log) << "Task process:" << task->Title() << processed << total;
        });
    Q_ASSERT(check);
    check = QObject::connect(&pool, &CThreadPool::sigRunning, [&pool, task]() {
        qDebug(log) << "Pool is running";
        CWorkerFileTransfer* pWorker = qobject_cast<CWorkerFileTransfer*>(pool.ChooseWorker());
        if(!pWorker) {
            qCritical(log) << "pWorker is not CWorkerDownload point";
            pool.Stop();
            return;
        }
        bool check = QObject::connect(task, &CTaskFileTransfer::sigFinished, [task, pWorker]() {
            pWorker->RemoveTask(task);
        });
        Q_ASSERT(check);
        pWorker->AddTask(task);
    });
    Q_ASSERT(check);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef RabbitCommon_VERSION
    app.setApplicationVersion(RabbitCommon_VERSION);
#endif
    app.setApplicationName("ExampleFile");

    RabbitCommon::CTools::Instance()->Init();
    RabbitCommon::CTools::Instance()->InstallTranslator("ExampleFile");

    // Must after install translator
    app.setApplicationDisplayName(QObject::tr("ExampleFile"));

    CThreadPool pool;
    int nRet = 0;
    nRet = pool.Start(CreateWorkerFileTransfer);
    if(nRet) {
        qDebug(log) << "Pool.Start fail:" << nRet;
        return nRet;
    }

    CTaskFileTransfer* task = new CTaskFileTransfer(QUrl("https://cctv.com"), "cctv.html");
    addTask(task, pool);

    QUrl urlFtp("ftp://192.168.11.49/RabbitCommon.conf");
    CTaskFileTransfer* ftp = new CTaskFileTransfer(urlFtp, "RabbitCommon.conf");
    ftp->SetUser("admin");
    ftp->SetPassword("qt");
    addTask(ftp, pool);

    QUrl urlSftp("sftp://192.168.0.101/home/daily/RabbitRemoteControl/vcpkg.json");
    CTaskFileTransfer* sftp = new CTaskFileTransfer(urlSftp, "RabbitCommon.conf");
    sftp->SetUser("daily");
    sftp->SetPassword("");
    //addTask(sftp, pool);

    nRet = app.exec();

    delete task;
    pool.Stop();
    return nRet;
}
