// https://gitee.com/bailiyang/cdemo/tree/master/Qt/49OpenSSL/OpenSSL/Cipher
// DES加密算法中，ECB和CBC模式有什么区别 https://blog.51cto.com/u_15060462/4692785

#ifndef EVPAES_H
#define EVPAES_H

#include <QByteArray>

struct evp_cipher_ctx_st;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

struct evp_cipher_st;
typedef struct evp_cipher_st EVP_CIPHER;

/*!
 * \brief The EvpAES class
 * \note This class is intended for internal use only
 * \ingroup INTERNAL_API
 */
class EvpAES
{
public:
    EvpAES();
    ~EvpAES();

    bool ecb_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, bool enc = true);
    bool cbc_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, const QByteArray& ivec,
                     bool enc = true);
    bool cfb1_encrypt(const QByteArray& in, QByteArray& out,
                      const QByteArray &key, const QByteArray& ivec,
                      bool enc = true);
    bool cfb8_encrypt(const QByteArray& in, QByteArray& out,
                      const QByteArray &key, const QByteArray& ivec,
                      bool enc = true);
    bool cfb128_encrypt(const QByteArray& in, QByteArray& out,
                        const QByteArray &key, const QByteArray& ivec,
                        bool enc = true);
    bool ofb128_encrypt(const QByteArray& in, QByteArray& out,
                        const QByteArray &key, const QByteArray& ivec,
                        bool enc = true);
    bool ctr_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, const QByteArray& ivec,
                     bool enc = true);
    bool gcm_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, const QByteArray& ivec,
                     bool enc = true);
    bool xts_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, const QByteArray& ivec,
                     bool enc = true);
    bool ocb_encrypt(const QByteArray& in, QByteArray& out,
                     const QByteArray &key, const QByteArray& ivec,
                     bool enc = true);

private:
    bool encrypt(const QByteArray& in, QByteArray& out,
                 const QByteArray& key, const QByteArray& ivec,
                 const EVP_CIPHER *cipher, bool enc = true);

private:
    EVP_CIPHER_CTX *ctx;
};

#endif // EVPAES_H
