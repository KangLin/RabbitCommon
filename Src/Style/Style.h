/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Load style(theme)
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
    int ReLoadStyle();
    CStyle(QObject *parent = nullptr);

    /*!
     * \brief Open get style sheet dialog
     * \return 
     */
    QString GetStyleSheet();

    /*!
     * \brief Load style sheet from the file
     * \param szFile: style file(*.qss)
     */
    int LoadStyleSheet(const QString &szFile);
    QString GetStyleSheetFile();
    void SetStyleSheetFile(const QString& file);
    QString m_szStyleSheetFile;
    bool m_bModifyStyleSheetFile;
    
    int LoadStyle(const QString &szName);
    QString GetStyleName();
    void SetStyleName(const QString &szName);
    QString m_szStyleName;
    bool m_bModifyStyleName;

    QString m_szDefaultIconTheme;
    QString m_szDefaultFallbackIconTheme;

    friend CFrmStyle;

private Q_SLOTS:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    void slotColorSchemeChanged(Qt::ColorScheme colorScheme);
#endif
};

} //namespace RabbitCommon

#endif // CSTYLE_H
