//! @copyright Copyright (c) Kang Lin studio, All Rights Reserved
//! @author Kang Lin(kl222@126.com)

#include "RabbitRecentMenu.h"

#include <QFileInfo>
#include <QSettings>

#include "RabbitCommonDir.h"

namespace RabbitCommon {

static const qint32 RecentFilesMenuMagic = 0xff;

CRecentMenu::CRecentMenu(QWidget * parent)
    : QMenu(parent)
    , m_maxCount(10)
    , m_format(QLatin1String("%d %s"))
    , m_DisableSaveState(false)
{
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    
    setMaxCount(m_maxCount);
    if(!m_DisableSaveState)
    {
        QSettings settings(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
        restoreState(settings.value("RecentOpen").toByteArray());
    }
}

CRecentMenu::CRecentMenu(const QString & title, QWidget * parent)
    : QMenu(title, parent)
    , m_maxCount(10)
    , m_format(QLatin1String("%d %s"))
{
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    
    setMaxCount(m_maxCount);
}

void CRecentMenu::addRecentFile(const QString &fileName, const QString &title)
{
    _Content content(title, fileName);
    
    m_OpenContent.removeAll(content);
    m_OpenContent.prepend(content);
    
    while (m_OpenContent.size() > maxCount())
        m_OpenContent.removeLast();
    
    updateRecentFileActions();
    
    if(!m_DisableSaveState)
    {
        QSettings settings(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
        settings.setValue("RecentOpen", saveState());
    }
}

void CRecentMenu::clearMenu()
{
    m_OpenContent.clear();
    
    updateRecentFileActions();
    
    if(!m_DisableSaveState)
    {
        QSettings settings(RabbitCommon::CDir::Instance()->GetFileUserConfigure(), QSettings::IniFormat);
        settings.setValue("RecentOpen", saveState());
    }
}

int CRecentMenu::maxCount() const
{
    return m_maxCount;
}

void CRecentMenu::setFormat(const QString &format)
{
    if (m_format == format)
        return;
    m_format = format;
    
    updateRecentFileActions();
}

const QString & CRecentMenu::format() const
{
    return m_format;
}

bool CRecentMenu::disableSaveState(bool disable)
{
    m_DisableSaveState = disable;
    return true;
}

QByteArray CRecentMenu::saveState() const
{
    int version = 0;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    
    stream << qint32(RecentFilesMenuMagic);
    stream << qint32(version);
    stream << m_OpenContent;
    
    return data;
}

bool CRecentMenu::restoreState(const QByteArray &state)
{
    int version = 0;
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    qint32 marker;
    qint32 v;
    
    stream >> marker;
    stream >> v;
    if (marker != RecentFilesMenuMagic || v != version)
        return false;
    
    stream >> m_OpenContent;
    
    updateRecentFileActions();
    
    return true;
}

void CRecentMenu::setMaxCount(int count)
{
    m_maxCount = count;
    
    updateRecentFileActions();
}

void CRecentMenu::menuTriggered(QAction* action)
{
    if (action->data().isValid())
        emit recentFileTriggered(action->data().toString());
}

void CRecentMenu::updateRecentFileActions()
{
    int numRecentFiles = qMin(m_OpenContent.size(), maxCount());
    
    clear();
    
    for (int i = 0; i < numRecentFiles; ++i) {
        QString strippedName = m_OpenContent[i].m_szTitle;
        if(strippedName.isEmpty())
            strippedName = QFileInfo(m_OpenContent[i].m_szFile).fileName();
        
        QString text = m_format;
        text.replace(QLatin1String("%d"), QString::number(i + 1));
        text.replace(QLatin1String("%s"), strippedName);
        
        QAction* recentFileAct = addAction(text);
        recentFileAct->setData(m_OpenContent[i].m_szFile);
        recentFileAct->setToolTip(tr("Recent open:") + m_OpenContent[i].m_szFile);
        recentFileAct->setStatusTip(tr("Recent open:") + m_OpenContent[i].m_szFile);
        recentFileAct->setWhatsThis(tr("Recent open:") + m_OpenContent[i].m_szFile);
    }
    addSeparator();
    addAction(tr("Clear Menu"), this, SLOT(clearMenu()));
    
    setEnabled(numRecentFiles > 0);
}

QDataStream& operator<< (QDataStream& d, const CRecentMenu::_Content& c)
{
    d << c.m_szFile << c.m_szTitle;
    return d;
}

QDataStream& operator>> (QDataStream& d, CRecentMenu::_Content& c)
{
    d >> c.m_szFile >> c.m_szTitle;
    return d;
}

} // namespace RabbitCommon 
