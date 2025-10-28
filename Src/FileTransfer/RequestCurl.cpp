#include <QLoggingCategory>
#include <QUrl>
#include "RequestCurl.h"
#include "WorkerFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "FileTransfer.Curl.Req")
CRequestCurl::CRequestCurl(QObject *parent)
    : QObject{parent}
    , m_pTask(nullptr)
    , m_hEasy(nullptr)
    , m_Transferred(0)
    , m_Total(0)
{}

CURL* CRequestCurl::GetHandle()
{
    return m_hEasy;
}

CTaskFileTransfer* CRequestCurl::GetTask()
{
    return m_pTask;
}

int CRequestCurl::Init(CTaskFileTransfer* pTask)
{
    qDebug(log) << Q_FUNC_INFO << this;
    if(!pTask) return -1;
    if(pTask->GetFile().isEmpty()) return -1;

    bool check = connect(this, &CRequestCurl::sigError, pTask, &CTaskFileTransfer::sigError);
    Q_ASSERT(check);
    check = connect(this, &CRequestCurl::sigFinished, pTask, &CTaskFileTransfer::sigFinished);
    Q_ASSERT(check);
    check = connect(this, &CRequestCurl::sigProcess, pTask, &CTaskFileTransfer::sigProcess);
    Q_ASSERT(check);
    check = connect(this, &CRequestCurl::sigSetTransferred, pTask, &CTaskFileTransfer::slotSetTransferred);
    Q_ASSERT(check);
    check = connect(this, &CRequestCurl::sigSetFileSize, pTask, &CTaskFileTransfer::slotSetFileSize);
    Q_ASSERT(check);

    m_pTask = pTask;
    m_hEasy = curl_easy_init();
    if (!m_hEasy) {
        qCritical(log) << "curl_easy_init failed";
        return -1;
    }

    do {
        /* ask libcurl to show us the verbose output */
        curl_easy_setopt(m_hEasy, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(m_hEasy, CURLOPT_DEBUGFUNCTION, cbDebug);
        curl_easy_setopt(m_hEasy, CURLOPT_DEBUGDATA, this);

        // Basic curl options
        curl_easy_setopt(m_hEasy, CURLOPT_PRIVATE, this);
        curl_easy_setopt(m_hEasy, CURLOPT_URL, pTask->GetUrl().toString().toUtf8().constData());
        auto &user = pTask->GetUser();
        auto &pass = pTask->GetPassword();
        if (!user.isEmpty()) {
            QByteArray up = (user + ":" + pass).toUtf8();
            curl_easy_setopt(m_hEasy, CURLOPT_USERPWD, up.constData());
        }

        // Set file
        m_File.setFileName(pTask->GetFile());
        if (pTask->IsDownload()) {
            QIODevice::OpenMode mode = QIODevice::WriteOnly;
            // Offset to resume transfer from
            if(pTask->GetTransferred() > 0) {
                curl_easy_setopt(m_hEasy, CURLOPT_RESUME_FROM_LARGE, pTask->GetTransferred());
                mode = QIODevice::Append;
            }
            if (!m_File.open(mode)) {
                QString err = QString("Cannot open file for download %1: %2").arg(
                    m_File.fileName(), m_File.errorString());
                qCritical(log) << err;
                break;
            }
            //curl_easy_setopt(m_hEasy, CURLOPT_HEADERFUNCTION, headerCallback);
            curl_easy_setopt(m_hEasy, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(m_hEasy, CURLOPT_WRITEDATA, this);
        } else {
            if (!m_File.open(QIODevice::ReadOnly)) {
                QString err = QString("Cannot open file for upload %1: %2").arg(
                    m_File.fileName(), m_File.errorString());
                qCritical(log) << err;
                break;
            }
            curl_easy_setopt(m_hEasy, CURLOPT_READFUNCTION, readCallback);
            curl_easy_setopt(m_hEasy, CURLOPT_READDATA, this);
            curl_easy_setopt(m_hEasy, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(m_hEasy, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(m_File.size()));
        }

        // Process options
#if LIBCURL_VERSION_NUM >= 0x072000
        curl_easy_setopt(m_hEasy, CURLOPT_XFERINFOFUNCTION, xferInfoCallback);
        curl_easy_setopt(m_hEasy, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(m_hEasy, CURLOPT_NOPROGRESS, 0L);
#else
        curl_easy_setopt(m_Handle, CURLOPT_PROGRESSFUNCTION, xferInfoCallback);
        curl_easy_setopt(m_Handle, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(m_Handle, CURLOPT_NOPROGRESS, 0L);
#endif

        // Follow HTTP 3xx redirects
        curl_easy_setopt(m_hEasy, CURLOPT_FOLLOWLOCATION, pTask->IsRedirect() ? 1L : 0L);

        // per-task timeouts
        if (pTask->GetConnectTimeout() > 0) {
            curl_easy_setopt(m_hEasy, CURLOPT_CONNECTTIMEOUT, pTask->GetConnectTimeout());
        }
        if (pTask->GetTimeout() > 0) {
            curl_easy_setopt(m_hEasy, CURLOPT_TIMEOUT, pTask->GetTimeout());
        }

        // TLS/FTPS options
        curl_easy_setopt(m_hEasy, CURLOPT_SSL_VERIFYPEER, pTask->GetVerifyPeer() ? 1L : 0L);
        curl_easy_setopt(m_hEasy, CURLOPT_SSL_VERIFYHOST, pTask->GetVerifyHost() ? 2L : 0L);
        if (!pTask->GetCaInfoPath().isEmpty()) {
            curl_easy_setopt(m_hEasy, CURLOPT_CAINFO, pTask->GetCaInfoPath().toUtf8().constData());
        }
        if (!pTask->GetSslCertPath().isEmpty()) {
            curl_easy_setopt(m_hEasy, CURLOPT_SSLCERT, pTask->GetSslCertPath().toUtf8().constData());
        }
        if (!pTask->GetSslKeyPath().isEmpty()) {
            curl_easy_setopt(m_hEasy, CURLOPT_SSLKEY, pTask->GetSslKeyPath().toUtf8().constData());
        }

        CTaskFileTransfer::ProxyType proxyType = pTask->GetProxyType();
        if(CTaskFileTransfer::ProxyType::No != proxyType) {
            // Proxy
            if(!pTask->GetProxyHost().isEmpty()) {
                QByteArray proxy = (pTask->GetProxyHost() + ":" + QString::number(pTask->GetProxyPort())).toUtf8();
                curl_easy_setopt(m_hEasy, CURLOPT_PROXY, proxy.constData());
            }
            if(!pTask->GetProxyUser().isEmpty()) {
                QByteArray up = (pTask->GetProxyUser() + ":" + pTask->GetProxyPassword()).toUtf8();
                curl_easy_setopt(m_hEasy, CURLOPT_PROXYUSERPWD, up.constData());
            }
            long type = 0;
            switch(proxyType) {
            case CTaskFileTransfer::ProxyType::Socks4:
                type = CURLPROXY_SOCKS4;
                break;
            case CTaskFileTransfer::ProxyType::Socks4A:
                type = CURLPROXY_SOCKS4A;
                break;
            case CTaskFileTransfer::ProxyType::Socks5:
                type = CURLPROXY_SOCKS5;
                break;
            case CTaskFileTransfer::ProxyType::Socks5HostName:
                type = CURLPROXY_SOCKS5_HOSTNAME;
                break;
            case CTaskFileTransfer::ProxyType::Http:
                type = CURLPROXY_HTTP;
                break;
            case CTaskFileTransfer::ProxyType::Http1_0:
                type = CURLPROXY_HTTP_1_0;
                break;
            case CTaskFileTransfer::ProxyType::Https:
                type = CURLPROXY_HTTPS;
                break;
            case CTaskFileTransfer::ProxyType::Https2:
                type = CURLPROXY_HTTPS2;
                break;
            default:
                break;
            }
            curl_easy_setopt(m_hEasy, CURLOPT_PROXYTYPE, type);
        }

        return 0;
    } while(0);

    Clean();

    return -1;
}

int CRequestCurl::Clean()
{
    qDebug(log) << Q_FUNC_INFO << this;
    int nRet = 0;

    if(m_File.isOpen())
        m_File.close();

    if(m_hEasy) {
        curl_easy_cleanup(m_hEasy);
        m_hEasy = nullptr;
    }
    return nRet;
}

int CRequestCurl::cbDebug(CURL *handle, curl_infotype type, char *data, size_t size, void *clientp)
{
    CRequestCurl *self = static_cast<CRequestCurl*>(clientp);
    if (!self) return 0;
    return self->OnDebug(type, data, size);
}

static void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size)
{
    QString szDump;
    size_t i;
    size_t c;
    unsigned int width = 0x10;
    const int length = 128;
    char buf[length];
    snprintf(buf, length, "%s, %10.10ld bytes (0x%8.8lx)",
            text, (long)size, (long)size);
    qDebug(log) << buf;

    for(i = 0; i < size; i += width) {
        snprintf(buf, length, "%4.4lx: ", (long)i);
        szDump += buf;

        /* show hex to the left */
        for(c = 0; c < width; c++) {
            if(i + c < size) {
                snprintf(buf, length, "%02x ", ptr[i + c]);
                szDump += buf;
            }
            else
                szDump += "   ";
        }

        /* show data on the right */
        for(c = 0; (c < width) && (i + c < size); c++) {
            char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            szDump += x;
        }

        szDump += '\n';
    }

    qDebug(log, "%s", szDump.toStdString().c_str());
}

int CRequestCurl::OnDebug(curl_infotype type, char *data, size_t size)
{
    const char *text;

    switch(type) {
    case CURLINFO_TEXT:
        fputs("== Info: ", stderr);
        fwrite(data, size, 1, stderr);
    default: /* in case a new one is introduced to shock us */
        return 0;

    case CURLINFO_HEADER_OUT:
        text = "=> Send header";
        break;
    case CURLINFO_DATA_OUT:
        text = "=> Send data";
        break;
    case CURLINFO_SSL_DATA_OUT:
        text = "=> Send SSL data";
        break;
    case CURLINFO_HEADER_IN:
        text = "<= Recv header";
        break;
    case CURLINFO_DATA_IN:
        text = "<= Recv data";
        break;
    case CURLINFO_SSL_DATA_IN:
        text = "<= Recv SSL data";
        break;
    }

    dump(text, stderr, (unsigned char *)data, size);
    return 0;
}

size_t CRequestCurl::headerCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    CRequestCurl *self = static_cast<CRequestCurl*>(userdata);
    if (!self) return 0;
    return self->OnHeaderCallback(buffer, size, nitems);
}

size_t CRequestCurl::OnHeaderCallback(char *buffer, size_t size, size_t nitems)
{
    size_t total_size = size * nitems;
    // 查找 Content-Length 头部
    // QString szHead(buffer);
    // if(strncasecmp(buffer, "Content-Length:", 15) == 0) {
    //     char *value = buffer + 15;
    //     while(*value == ' ') value++; // 跳过空格
    //     long content_length = atol(value);
    //     qDebug(log) << "从头部获取的文件大小:" << content_length << " 字节";
    // }

    return total_size;
}

size_t CRequestCurl::writeCallback(char *buffer, size_t size, size_t nmemb, void *userdata)
{
    CRequestCurl *self = static_cast<CRequestCurl*>(userdata);
    if (!self) return 0;
    return self->OnWriteCallback(buffer, size, nmemb);
}

size_t CRequestCurl::OnWriteCallback(char *buffer, size_t size, size_t nmemb)
{
    qDebug(log) << Q_FUNC_INFO;
    qint64 toWrite = qint64(size) * qint64(nmemb);
    qint64 written = m_File.write(buffer, toWrite);
    if (written > 0) {
        m_Transferred += written;
        emit sigSetTransferred(m_Transferred);
    }
    return static_cast<size_t>(written);
}

size_t CRequestCurl::readCallback(char *buffer, size_t size, size_t nmemb, void *userdata)
{
    CRequestCurl *self = static_cast<CRequestCurl*>(userdata);
    if (!self) return 0;
    return self->OnReadCallback(buffer, size, nmemb);
}

size_t CRequestCurl::OnReadCallback(char *buffer, size_t size, size_t nmemb)
{
    qint64 maxlen = qint64(size) * qint64(nmemb);
    qint64 r = m_File.read(buffer, maxlen);
    if (r > 0) {
        m_Transferred += r;
        emit sigSetTransferred(m_Transferred);
    }
    return static_cast<size_t>(r < 0 ? 0 : r);
}

int CRequestCurl::xferInfoCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    CRequestCurl *self = static_cast<CRequestCurl*>(clientp);
    if (!self) return 0;
    return self->OnXferInfoCallback(dltotal, dlnow, ultotal, ulnow);
}

int CRequestCurl::OnXferInfoCallback(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    qint64 total = 0, now = 0;
    if (m_pTask->IsDownload()) {
        total = static_cast<qint64>(dltotal);
        now = static_cast<qint64>(dlnow);
    } else {
        total = static_cast<qint64>(ultotal);
        now = static_cast<qint64>(ulnow);
    }

    m_Transferred = now;
    m_Total = total;

    if (m_pTask && (now || total)) {
        //qDebug(log) << Q_FUNC_INFO << now << total;
        emit sigProcess(now, total);
    }
    return 0;
}

int CRequestCurl::ShowInfo()
{
    if(!m_hEasy) return -1;
    CURLcode rc = CURLE_OK;
    char *done_url = nullptr;
    rc = curl_easy_getinfo(m_hEasy, CURLINFO_EFFECTIVE_URL, &done_url);
    if(CURLE_OK == rc)
        qDebug(log) << "Done url:" << done_url;
    else
        qDebug(log) << "curl_easy_getinfo CURLINFO_EFFECTIVE_URL fail:" << curl_easy_strerror(rc);
    char *redirect_url = nullptr;
    rc = curl_easy_getinfo(m_hEasy, CURLINFO_REDIRECT_URL, &redirect_url);
    if(CURLE_OK == rc)
        qDebug(log) << "Http redirect url:" << redirect_url;
    else
        qDebug(log) << "curl_easy_getinfo CURLINFO_REDIRECT_URL fail:" << curl_easy_strerror(rc);
    double nDownloadLength = 0;
    rc = curl_easy_getinfo(m_hEasy, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &nDownloadLength);
    if(CURLE_OK == rc)
        qDebug(log) << "Download length:" << nDownloadLength;
    else
        qDebug(log) << "curl_easy_getinfo CURLINFO_CONTENT_LENGTH_DOWNLOAD fail:" << curl_easy_strerror(rc);
    return 0;
}
