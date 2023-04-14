#include "EvpAES.h"
#include <openssl/evp.h>
#include <openssl/aes.h>

#define KEY_SIZE_16B            16
#define KEY_SIZE_24B            24
#define KEY_SIZE_32B            32

EvpAES::EvpAES()
{
    // 初始化CTX
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
}

EvpAES::~EvpAES()
{
    // 释放CTX
    EVP_CIPHER_CTX_cleanup(ctx);
    EVP_CIPHER_CTX_free(ctx);
}

bool EvpAES::ecb_encrypt(const QByteArray &in, QByteArray &out,
                         const QByteArray &key, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_ecb();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_ecb();
    }
    else
    {
        cipher = EVP_aes_256_ecb();
    }

    // 执行加解密
    return encrypt(in, out, key, QByteArray(), cipher, enc);
}

bool EvpAES::cbc_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_cbc();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_cbc();
    }
    else
    {
        cipher = EVP_aes_256_cbc();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::cfb1_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_cfb1();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_cfb1();
    }
    else
    {
        cipher = EVP_aes_256_cfb1();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::cfb8_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_cfb8();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_cfb8();
    }
    else
    {
        cipher = EVP_aes_256_cfb8();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::cfb128_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_cfb128();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_cfb128();
    }
    else
    {
        cipher = EVP_aes_256_cfb128();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::ofb128_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_ofb();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_ofb();
    }
    else
    {
        cipher = EVP_aes_256_ofb();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::ctr_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_ctr();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_ctr();
    }
    else
    {
        cipher = EVP_aes_256_ctr();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::gcm_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B
             || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_gcm();
    }
    else if (key.size() == KEY_SIZE_24B)
    {
        cipher = EVP_aes_192_gcm();
    }
    else
    {
        cipher = EVP_aes_256_gcm();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::xts_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、32字节)
    Q_ASSERT(key.size() == KEY_SIZE_16B || key.size() == KEY_SIZE_32B);
    Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

    // 根据key大小创建EVP_CIPHER
    const EVP_CIPHER * cipher = nullptr;
    if (key.size() == KEY_SIZE_16B)
    {
        cipher = EVP_aes_128_xts();
    }
    else
    {
        cipher = EVP_aes_256_xts();
    }

    // 执行加解密
    return encrypt(in, out, key, ivec, cipher, enc);
}

bool EvpAES::ocb_encrypt(const QByteArray &in, QByteArray &out,
                        const QByteArray &key, const QByteArray &ivec, bool enc)
{
#if OPENSSL_VERSION_NUMBER >= 0x3000000fL
    #ifndef OPENSSL_NO_OCB

        // 检查密钥合法性(只能是16、24、32字节)
        Q_ASSERT(key.size() == KEY_SIZE_16B
                 || key.size() == KEY_SIZE_24B || key.size() == KEY_SIZE_32B);
        Q_ASSERT(ivec.size() == KEY_SIZE_16B); // 初始向量为16字节

        // 根据key大小创建EVP_CIPHER
        const EVP_CIPHER * cipher = nullptr;
        if (key.size() == KEY_SIZE_16B)
        {
            cipher = EVP_aes_128_ocb();
        }
        else if (key.size() == KEY_SIZE_24B)
        {
            cipher = EVP_aes_192_ocb();
        }
        else
        {
            cipher = EVP_aes_256_ocb();
        }

        // 执行加解密
        return encrypt(in, out, key, ivec, cipher, enc);

    #endif
#endif
    return false;
}

bool EvpAES::encrypt(const QByteArray &in, QByteArray &out,
                     const QByteArray &key, const QByteArray &ivec,
                     const EVP_CIPHER *cipher, bool enc)
{
    if (enc)
    {
        // 指定加密算法及key和iv
        int ret = EVP_EncryptInit_ex(ctx, cipher, NULL,
                                     (const unsigned char*)key.data(),
                                     (const unsigned char*)ivec.data());
        if(ret != 1)
        {
            return false;
        }

        // 进行加密操作
        int mlen = 0;
        out.resize(in.size() + AES_BLOCK_SIZE);
        ret = EVP_EncryptUpdate(ctx, (unsigned char*)out.data(), &mlen,
                                (const unsigned char*)in.data(),
                                in.size());
        if(ret != 1)
        {
            return false;
        }

        // 结束加密操作
        int flen = 0;
        ret = EVP_EncryptFinal_ex(ctx, (unsigned char *)out.data() + mlen, &flen);
        if(ret != 1)
        {
            return false;
        }
        out.resize(mlen + flen);
        return true;
    }
    else
    {
        // 指定解密算法及key和iv
        int ret = EVP_DecryptInit_ex(ctx, cipher, NULL,
                                     (const unsigned char*)key.data(),
                                     (const unsigned char*)ivec.data());
        if(ret != 1)
        {
            return false;
        }

        // 进行解密操作
        int mlen = 0;
        out.resize(in.size());
        ret = EVP_DecryptUpdate(ctx, (unsigned char*)out.data(), &mlen,
                                (const unsigned char*)in.data(), in.size());
        if(ret != 1)
        {
            return false;
        }

        // 结束解密操作
        int flen = 0;
        ret = EVP_DecryptFinal_ex(ctx, (unsigned char *)out.data() + mlen, &flen);
        if(ret != 1)
        {
            return false;
        }
        out.resize(mlen + flen);
        return true;
    }
}
