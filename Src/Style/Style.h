/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Style load
 */

#ifndef CSTYLE_H
#define CSTYLE_H

#pragma once

#include <QObject>
#include <QtGlobal>

#include "FrmStyle.h"
#include <QLoggingCategory>

namespace RabbitCommon {

/*!
 * \brief The CStyle class
 * \ingroup INTERNAL_API
 */
class CStyle : public QObject
{
    Q_OBJECT
public:
    static CStyle* Instance();

    /*!
     * \brief Load style from configure file
     */
    int LoadStyle();

private:
    QString GetStyle();
    QString GetDefaultStyle();

    /*!
     * \brief Load style from the file
     * \param szFile: style file(*.qss)
     */
    int LoadStyle(const QString &szFile);
    void SetFile(const QString& file);
    QString GetStyleFile();

private:
    CStyle(QObject *parent = nullptr);

    QString m_szDefaultFile;
    QString m_szFile;

    QString m_szDefaultIconTheme;
    QString m_szDefaultFallbackIconTheme;

    friend CFrmStyle;
};

Q_DECLARE_LOGGING_CATEGORY(LoggerStyle);

} //namespace RabbitCommon

#endif // CSTYLE_H
