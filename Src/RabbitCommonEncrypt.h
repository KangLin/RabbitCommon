/* Copyright Copyright (c) Kang Lin studio, All Rights Reserved
 * Author Kang Lin <kl222@126.com>
 */

#ifndef CENCRYPT_H_KL_2021_12_10
#define CENCRYPT_H_KL_2021_12_10

#pragma once

#include <string>
#include "rabbitcommon_export.h"
#include <QString>
#include <QByteArray>

namespace RabbitCommon {

/*!
 * \brief The encrypt class
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CEncrypt
{
public:
    CEncrypt(const char* pszPassword = "RabbitCommon");
    ~CEncrypt();
    
    int SetPassword(const char* pszPassword);
    /*!
     * \param pIn: 输入要加密的数据指针
     * \param inlen: 输入要加密的数据的长度
     * \param pOut: 输出加密了的数据指针.调用者使用完成后,需要用delete []*pOut
     * \param outLen: 加密了的数据长度
     */
    int Encode(const char* pIn, const int &inLen, char** pOut, int &outLen);
    int Encode(const QString& szIn, QByteArray& szOut);
    /*!
     * \param pIn: 输入要解密的数据指针
     * \param inlen: 输入要解密的数据的长度
     * \param pOut: 输出解密了的数据指针.调用者使用完成后,需要用delete []*pOut
     * \param outLen: 解密了的数据长度
     */
    int Dencode(const char* pIn, const int &inLen, char** pOut, int &outLen);
    int Dencode(const char* pIn, const int &inLen, std::string &szOut);
    int Dencode(const QByteArray& szIn, QString& szOut);
    
private:
    std::string m_szPassword;
};

/*!
 * \brief Password Generator
 * \ingroup API
 * \since 2.4.0
 */
class RABBITCOMMON_EXPORT CPasswordGenerator : public QObject{
    Q_OBJECT
public:
    CPasswordGenerator(QObject* parent = nullptr);

    void SetCharSet(bool use_lower, bool use_upper, bool use_digits, bool use_symbols);
    std::string Generate(int length = 16);
    static std::string GeneratePassword(int length = 16, bool use_lower = true,
                         bool use_upper = true, bool use_digits = true,
                         bool use_symbols = true);

private:
    static constexpr char LOWERCASE[] = "abcdefghijklmnopqrstuvwxyz";
    static constexpr char UPPERCASE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static constexpr char DIGITS[] = "0123456789";
    static constexpr char SYMBOLS[] = "!@#$%^&*()_+-=[]{}|;:,.<>?/\\";

    std::string m_CharSet;
    bool m_bLowser;
    bool m_bUpper;
    bool m_bDigits;
    bool m_bSymbols;
};

} // namespace RabbitCommon
#endif // CENCRYPT_H_KL_2021_12_10

