// Author Kang Lin <kl222@126.com>

#include "RabbitCommonEncrypt.h"
#include "RabbitCommonLog.h"

#include <string.h>

#if defined (BUILD_OPENSSL)
#include "openssl/aes.h"
#endif

namespace RabbitCommon {

#if defined (BUILD_OPENSSL)
    #define PASSWORD_LENGTH AES_BLOCK_SIZE
#else
    #define PASSWORD_LENGTH 16
#endif

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

    m_szPassword.resize(PASSWORD_LENGTH);
    memcpy(&m_szPassword[0], Key, PASSWORD_LENGTH);
    for(int i = 0; i < nLen; i++)
    {
        m_szPassword[i] ^= Key[i];
    }
        
    return 0;
}

int CEncrypt::Encode(const char* pIn, const int& inLen, char** pOut, int& outLen)
{
    int nRet = 0;
#if defined (BUILD_OPENSSL)
    AES_KEY k;
    nRet = AES_set_encrypt_key((const unsigned char*)m_szPassword.c_str(), m_szPassword.size() * 8, &k);
    if(nRet < 0)
    {
        LOG_MODEL_ERROR("Encrypt", "Unable to set encryption key in AES:nRet=%d\n", nRet);
        return nRet;
    }
    
    char *pI = (char*)pIn;
    char *pO = NULL;
    int nBlock = inLen / PASSWORD_LENGTH;
    int nFill = PASSWORD_LENGTH - inLen % PASSWORD_LENGTH;
    if(nFill)
        outLen = nBlock + 1;
    else
        outLen = nBlock;
    outLen *= PASSWORD_LENGTH;
    pO = new char[outLen];
    if(!pO)
    {
        LOG_MODEL_ERROR("Encrypt", "Hasn't buffer\n");
        return -1;
    }
    
    *pOut = pO;
    
    for(int i = 0; i < nBlock; i++)
    {
        AES_encrypt((const unsigned char*)pI, (unsigned char*)pO, &k);
        pI += PASSWORD_LENGTH;
        pO += PASSWORD_LENGTH;
    }
    
    //最后一块
    if(nFill)
    {
        char buf[PASSWORD_LENGTH];
        memset(buf, 0, PASSWORD_LENGTH);
        memcpy(buf, pI, PASSWORD_LENGTH);
        AES_encrypt((const unsigned char*)buf, (unsigned char*)pO, &k);
    }
#else
    memmove(*pOut, pIn, outLen);
#endif
    
    return nRet;
}

int CEncrypt::Encode(const QString& szIn, QByteArray& szOut)
{
    int nRet = 0;
    QByteArray in = szIn.toUtf8();
    int len = 0;
    char *pOut = NULL;
    nRet = Encode(in.data(), in.length(), &pOut, len);
    if(!nRet)
        szOut.setRawData(pOut, len);
    return nRet;
}

int CEncrypt::Dencode(const QByteArray &szIn, QString &szOut)
{
    int nRet = 0;
    char* pOut = NULL;
    int len = 0;
    std::string out;
    nRet = Dencode(szIn.data(), szIn.length(), out);
    if(!nRet)
        szOut = out.c_str();
    return nRet;
}

int CEncrypt::Dencode(const char* pIn, const int& inLen, char** pOut, int& outLen)
{
    int nRet = 0;
#if defined (BUILD_OPENSSL)
    AES_KEY k;
    nRet = AES_set_decrypt_key((const unsigned char*)m_szPassword.c_str(), m_szPassword.size() * 8, &k);
    if(nRet < 0)
    {
        LOG_MODEL_ERROR("Encrypt", "Unable to set dencryption key in AES:nRet=%d\n", nRet);
        return nRet;
    }
    
    char *pI = (char*)pIn;
    char *pO = NULL;
    outLen = inLen;
    pO = new char[outLen];
    if(!pO)
    {
        LOG_MODEL_ERROR("Encrypt", "Hasn't buffer\n");
        return -1;
    }
    
    memset(pO, 0, outLen);
    *pOut = pO;
    int nBlock = inLen / PASSWORD_LENGTH;
    for(int i = 0; i < nBlock; i++)
    {
        AES_decrypt((const unsigned char*)pI, (unsigned char*)pO, &k);
        pI += PASSWORD_LENGTH;
        pO += PASSWORD_LENGTH;
    }
#else
    memmove(*pOut, pIn, outLen);
#endif
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

