/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Style load
 */

#ifndef CSTYLE_H
#define CSTYLE_H

#pragma once

#include <QObject>
#include <QtGlobal>
#include "rabbitcommon_export.h"

#include "FrmStyle/FrmStyle.h"
#include <QLoggingCategory>

namespace RabbitCommon {

/*!
 * \brief The CStyle class
 * \note Since 2.0.0, The class is an inner class.
 *       please use CFrmStyle instead.
 */
class RABBITCOMMON_EXPORT CStyle : public QObject
{
    Q_OBJECT
public:
    static CStyle* Instance();
    //TODO:  When major version >= 2, the function is remove.
    void SetDefaultFile(const QString &file);

    /*!
     * \brief Load style from configure file
     */
    int LoadStyle();
    /*!
     * \brief Load style from the file
     * \param szFile: style file(*.qss)
     */
    int LoadStyle(const QString &szFile);
    QString GetStyleFile();

public Q_SLOTS:
    /*!
     * \brief Used to respond to menu events, open select style diaglog
     */
    void slotStyle();
    /*!
     * \brief Set default style
     */
    void slotSetDefaultStyle();

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
