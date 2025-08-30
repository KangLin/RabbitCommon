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

private Q_SLOTS:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    void slotColorSchemeChanged(Qt::ColorScheme colorScheme);
#endif
private:
    CStyle(QObject *parent = nullptr);

    QString m_szDefaultFile;
    QString m_szFile;

    QString m_szDefaultIconTheme;
    QString m_szDefaultFallbackIconTheme;

    friend CFrmStyle;
};

} //namespace RabbitCommon

#endif // CSTYLE_H
