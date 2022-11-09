/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Style load
 */

#ifndef CSTYLE_H
#define CSTYLE_H

#pragma once

#include <QObject>
#include "rabbitcommon_export.h"

#include "FrmStyle/FrmStyle.h"

namespace RabbitCommon {

/**
 * @brief The CStyle class
 * @note When major version >= 2, The class is an inner class. please use CFrmStyle
 */
class RABBITCOMMON_EXPORT CStyle : public QObject
{
    Q_OBJECT
public:
    static CStyle* Instance();
    //TODO:  When major version >= 2, the function is remove.
    void SetDefaultFile(const QString &file);

    /**
     * @brief Load style from configure file
     * @return 
     */
    int LoadStyle();
    /**
     * @brief Load style from the file
     * @param szFile: style file(*.qss)
     * @return 
     */
    int LoadStyle(const QString &szFile);
    QString GetStyleFile();

public Q_SLOTS:
    /**
     * @brief Used to respond to menu events, open select style diaglog
     */
    void slotStyle();
    /**
     * @brief Set default style
     * @return 
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

} //namespace RabbitCommon

#endif // CSTYLE_H
