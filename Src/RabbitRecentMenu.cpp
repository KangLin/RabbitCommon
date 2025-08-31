// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitRecentMenu.h"

#include <QFileInfo>
#include <QSettings>

#include "RabbitCommonDir.h"

namespace RabbitCommon {

static const qint32 RecentFilesMenuMagic = 0xff;

CRecentMenu::CRecentMenu(QWidget * parent)
    : QMenu(parent),
    m_maxCount(10),
    m_format(QLatin1String("%d %s")),
    m_DisableSaveState(false),
    m_bUpdate(true)
{
    bool check = connect(this, SIGNAL(triggered(QAction*)),
                         this, SLOT(menuTriggered(QAction*)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(aboutToShow()),
                    this, SLOT(updateRecentFileActions()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSaveState()), this, SLOT(slotSaveState()));
    Q_ASSERT(check);

    setMaxCount(m_maxCount);
    if(!m_DisableSaveState)
    {
        QSettings settings(
                    RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                    QSettings::IniFormat);
        restoreState(settings.value("RecentOpen").toByteArray());
    }
}

CRecentMenu::CRecentMenu(const QString & title, QWidget * parent)
    : CRecentMenu(parent)
{
    setTitle(title);
}

CRecentMenu::CRecentMenu(const QString& title, const QIcon& icon, QWidget * parent)
    : CRecentMenu(title, parent)
{
    setIcon(icon);
}

void CRecentMenu::addRecentFile(const QString &fileName, const QString &title)
{
    _Content content(title, fileName);

    m_OpenContent.removeAll(content);
    m_OpenContent.prepend(content);

    while (m_OpenContent.size() > maxCount())
        m_OpenContent.removeLast();

    m_bUpdate = true;

    emit sigSaveState();
}

void CRecentMenu::clearMenu()
{
    m_OpenContent.clear();
    m_bUpdate = true;
    emit sigSaveState();
}

void CRecentMenu::setMaxCount(int count)
{
    if(m_maxCount == count) return;
    m_maxCount = count;
    m_bUpdate = true;
    emit sigSaveState();
}

int CRecentMenu::maxCount() const
{
    return m_maxCount;
}

void CRecentMenu::setFormat(const QString &format)
{
    if (m_format == format) return;
    m_format = format;
    m_bUpdate = true;
    emit sigSaveState();
}

const QString& CRecentMenu::format() const
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
    //TODO: modify version when modify save data
    int version = 1;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(RecentFilesMenuMagic);
    stream << qint32(version);
    stream << m_format;
    stream << m_maxCount;
    stream << m_OpenContent;
    
    return data;
}

bool CRecentMenu::restoreState(const QByteArray &state)
{
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    qint32 magic;
    qint32 version = 1; //TODO: modify version when modify save data

    stream >> magic;
    stream >> version;
    if (magic != RecentFilesMenuMagic)
        return false;

    //TODO: modify version when modify save data
    if(version >= 1) {
        stream >> m_format;
        stream >> m_maxCount;
    }

    stream >> m_OpenContent;

    m_bUpdate = true;

    return true;
}

void CRecentMenu::menuTriggered(QAction* action)
{
    if (action->data().isValid())
        emit recentFileTriggered(action->data().toString());
}

void CRecentMenu::updateRecentFileActions()
{
    if(!m_bUpdate) return;

    int numRecentFiles = qMin(m_OpenContent.size(), maxCount());

    clear();

    for (int i = 0; i < numRecentFiles; ++i) {
        QString szdName = m_OpenContent[i].m_szTitle;
        QString szFile = m_OpenContent[i].m_szFile;
        if(szdName.isEmpty())
            szdName = QFileInfo(szFile).fileName();
        
        QString text = m_format;
        text.replace(QLatin1String("%d"), QString::number(i + 1));
        text.replace(QLatin1String("%s"), szdName);
        
        QAction* recentFileAct = addAction(text);
        QFileInfo fi(szFile);
        recentFileAct->setEnabled(fi.exists());
        recentFileAct->setData(szFile);
        QString szMsg;
        if(fi.exists())
            szMsg = tr("Recent open: ") + szFile;
        else
            szMsg = tr("The file doesn't exists: ") + szFile;
        recentFileAct->setToolTip(szMsg);
        recentFileAct->setStatusTip(szMsg);
        recentFileAct->setWhatsThis(szMsg);
    }
    addSeparator();
    addAction(tr("Clear Menu"), this, SLOT(clearMenu()));
    
    setEnabled(numRecentFiles > 0);
}

void CRecentMenu::slotSaveState()
{
    if(m_DisableSaveState)
        return;

    QSettings settings(
                RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                QSettings::IniFormat);
    settings.setValue("RecentOpen", saveState());
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
