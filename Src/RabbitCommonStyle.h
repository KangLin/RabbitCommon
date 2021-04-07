#ifndef CSTYLE_H
#define CSTYLE_H

#pragma once

#include <QObject>
#include "rabbitcommon_export.h"

namespace RabbitCommon {

/**
 * @brief The CStyle class
 */
class RABBITCOMMON_EXPORT CStyle : public QObject
{
    Q_OBJECT
public:
    explicit CStyle(QWidget *parent = nullptr);
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
    /**
     * @brief Set default style
     * @return 
     */
    int SetDefaultStyle();
    
public Q_SLOTS:
    /**
     * @brief Used to respond to menu events
     */
    void slotStyle();
    void slotSetDefaultStyle();
};

} //namespace RabbitCommon

#endif // CSTYLE_H
