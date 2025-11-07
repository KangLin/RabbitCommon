#ifndef REQUESTCURL_H
#define REQUESTCURL_H

#include <QObject>
#include <QFile>
#include <curl/curl.h>

class CTaskFileTransfer;

/*!
 * \brief libcurl request
 * \note It is running in worker thread
 *
 * Since 2.4.0
 */
class CRequestCurl : public QObject
{
    Q_OBJECT
public:
    explicit CRequestCurl(QObject *parent = nullptr);

    int Init(CTaskFileTransfer* pTask);
    int Clean();
    void SetResponseCode(long response_code);
    CURL* GetHandle();
    CTaskFileTransfer* GetTask();
    int ShowInfo();

Q_SIGNALS:
    void sigProcess(qint64 nProcessed, qint64 nTotal);
    void sigFinished();
    void sigError(int nErr, const QString& szError);
    void sigSetTransferred(const qint64 &transferred);    
    void sigSetFileSize(const qint64 &fileSize);

private:
    static int cbDebug(CURL *handle, curl_infotype type, char *data, size_t size, void *clientp);
    static size_t headerCallback(char *buffer, size_t size, size_t nitems, void *userdata);
    static size_t writeCallback(char *buffer, size_t size, size_t nmemb, void *userdata);
    static size_t readCallback(char *buffer, size_t size, size_t nmemb, void *userdata);
    static int xferInfoCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    int OnDebug(curl_infotype type, char *data, size_t size);
    size_t OnHeaderCallback(char *buffer, size_t size, size_t nitems);
    size_t OnWriteCallback(char *buffer, size_t size, size_t nmemb);
    size_t OnReadCallback(char *buffer, size_t size, size_t nmemb);
    int OnXferInfoCallback(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    static const char* HttpStatusText(long status_code);

private:
    CTaskFileTransfer* m_pTask;
    CURL* m_hEasy;
    QFile m_File;
    qint64 m_Transferred;
    qint64 m_Total;
};

#endif // REQUESTCURL_H
