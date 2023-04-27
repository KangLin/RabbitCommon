/** @copyright Copyright (c) Kang Lin studio, All Rights Reserved
 *  @author Kang Lin <kl222@126.com>
 *  @abstract Tools
 */

#ifndef CRabbitRecentMenu_H
#define CRabbitRecentMenu_H

#pragma once

#include <QMenu>
#include <QList>

#include "rabbitcommon_export.h"

namespace RabbitCommon {

/**
 * \~chinese 最近打开菜单
 * 
 * \~english Recently opened menu
 *
 * \~
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CRecentMenu : public QMenu
{
    Q_OBJECT
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
    Q_PROPERTY(QString format READ format WRITE setFormat)

public:
    //! Constructs a menu with parent parent.
    CRecentMenu(QWidget * parent = 0);
    //! Constructs a menu with a title and a parent.
    CRecentMenu(const QString & title, QWidget * parent = 0);
    //! Constructs a menu with a title, icon and a parent.
    CRecentMenu(const QString & title, const QIcon& icon, QWidget * parent = 0);

    //! Returns the maximum number of entries in the menu.
    int maxCount() const;

    /*! This property holds the string used to generate the item text.
     * %d is replaced by the item number
     * %s is replaced by the item text
     * The default value is "%d %s".
     */
    void setFormat(const QString &format);

    //! Returns the current format. /sa setFormat
    const QString & format() const;

    //! Default enable save state
    bool disableSaveState(bool disable);

    /*! Saves the state of the recent entries.
     * Typically this is used in conjunction with QSettings to remember entries
     * for a future session. A version number is stored as part of the data.
     * Here is an example:
     * QSettings settings;
     * settings.setValue("recentFiles", recentFilesMenu->saveState());
     */
    QByteArray saveState() const;

    /*! Restores the recent entries to the state specified.
     * Typically this is used in conjunction with QSettings to restore entries from a past session.
     * Returns false if there are errors.
     * Here is an example:
     * QSettings settings;
     * recentFilesMenu->restoreState(settings.value("recentFiles").toByteArray());
     */
    bool restoreState(const QByteArray &state);

public slots:
    //!
    void addRecentFile(const QString &fileName, const QString& title = QString());
    
    //! Removes all the menu's actions.
    void clearMenu();
    
    //! Sets the maximum number of entries int he menu.
    void setMaxCount(int);

signals:
    //! This signal is emitted when a recent file in this menu is triggered.
    void recentFileTriggered(const QString & filename);
    
private slots:
    void menuTriggered(QAction*);
    void updateRecentFileActions();
    void slotSaveState();

signals:
    void sigSaveState();

private:
    int m_maxCount;
    QString m_format;
    
    class _Content
    {
    public:
        _Content(){}
        _Content(const QString& title, const QString& file) {
            m_szTitle = title;
            m_szFile = file;
        }
        
        _Content(const _Content& c) {
            m_szTitle = c.m_szTitle;
            m_szFile = c.m_szFile;
        }
        bool operator==(const _Content& c) const {
            if(m_szFile == c.m_szFile && m_szTitle == c.m_szTitle)
                return true;
            return false;
        }
        
        QString m_szTitle;
        QString m_szFile;
    };
    
    friend QDataStream & operator<< (QDataStream& d, const _Content& image);
    friend QDataStream & operator>> (QDataStream& d, _Content& image);
    
    QList<_Content> m_OpenContent;
    
    bool m_DisableSaveState;
    bool m_bUpdate;
};

} //namespace RabbitCommon 
#endif // QRECENTFILEMENU_H
