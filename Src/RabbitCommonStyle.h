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
    static CStyle* Instance();
    
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
    
public Q_SLOTS:
    /**
     * @brief Used to respond to menu events
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
    
};

} //namespace RabbitCommon

#endif // CSTYLE_H
