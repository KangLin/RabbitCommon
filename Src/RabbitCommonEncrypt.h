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

} // namespace RabbitCommon
#endif // CENCRYPT_H_KL_2021_12_10

