// Author Kang Lin <kl222@126.com>

#include "RabbitCommonEncrypt.h"
#include "RabbitCommonLog.h"

#include <string.h>

#if defined (HAVE_OPENSSL)
#include "EvpAES.h"
#endif

namespace RabbitCommon {

#define PASSWORD_LENGTH 16

static const unsigned char Key[] =
{
    0x56, 0x9a, 0x28, 0x12, 0xfa, 0x59, 0xa1, 0x58,
    0x65, 0x7c, 0xc8, 0x92, 0x1a, 0x09, 0x10, 0x8a
};

CEncrypt::CEncrypt(const char* pszPassword)
{
    SetPassword(pszPassword);
}

CEncrypt::~CEncrypt()
{
}

int CEncrypt::SetPassword(const char* pszPassword)
{
    int nLen = PASSWORD_LENGTH;
    if(nLen > strlen(pszPassword))
        nLen = strlen(pszPassword);

    m_szPassword.resize(PASSWORD_LENGTH, 0);
    memcpy(&m_szPassword[0], Key, PASSWORD_LENGTH);
    for(int i = 0; i < nLen; i++)
    {
        m_szPassword[i] ^= pszPassword[i];
    }
        
    return 0;
}

int CEncrypt::Encode(const char* pIn, const int& inLen, char** pOut, int& outLen)
{
    int nRet = 0;
    QString szIn = QString::fromStdString(std::string(pIn, inLen));
    QByteArray szOut;
    nRet = Encode(szIn, szOut);
    if(nRet) return nRet;
    outLen = szOut.length();
    *pOut = new char[outLen];
    memcpy(*pOut, szOut.data(), outLen);
    return nRet;
}

int CEncrypt::Encode(const QString& szIn, QByteArray& szOut)
{
    int nRet = 0;
#if defined (HAVE_OPENSSL)
    EvpAES aes;
    QByteArray in = szIn.toUtf8();
    bool b = aes.cbc_encrypt(in, szOut,
                    QByteArray::fromStdString(m_szPassword),
                    QByteArray::fromRawData((const char*)Key, PASSWORD_LENGTH));
    if(b) return 0;
    else return -1;
#else
    szOut = szIn.toUtf8();
#endif
    return nRet;
}

int CEncrypt::Dencode(const QByteArray &szIn, QString &szOut)
{
    int nRet = 0;
#if defined (HAVE_OPENSSL)
    EvpAES aes;
    QByteArray out;
    bool b = aes.cbc_encrypt(szIn, out,
                    QByteArray::fromStdString(m_szPassword),
                    QByteArray::fromRawData((const char*)Key, PASSWORD_LENGTH),
                             false);
    if(b)
    {
        szOut = out;
        return 0;
    } else
        return -1;
#else
    szOut = szIn;
#endif
    return nRet;
}

int CEncrypt::Dencode(const char* pIn, const int& inLen, char** pOut, int& outLen)
{
    int nRet = 0;
    QByteArray in(pIn, inLen);
    QString out;
    nRet = Dencode(in, out);
    if(nRet) return nRet;
    outLen = out.length();
    *pOut = new char[outLen];
    memcpy(*pOut, out.data(), outLen);
    return nRet;
}

int CEncrypt::Dencode(const char* pIn, const int& inLen, std::string& szOut)
{
    char *pOut = NULL;
    int nOutLen = 0;
    int nRet = 0;
    nRet = Dencode(pIn, inLen, &pOut, nOutLen);
    if(nRet)
    {
        LOG_MODEL_ERROR("Encrypt", "Dencode error\n");
        return nRet;
    }

    if(pOut)
    {
        szOut = pOut;
        delete [] pOut;
    }
    return nRet;
}

} // namespace RabbitCommon

