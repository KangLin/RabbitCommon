#include "RabbitCommonStyle.h"
#include "RabbitCommonDir.h"

#include <QSettings>
#include <QDebug>
#include <QApplication>

namespace RabbitCommon {

CStyle::CStyle(QWidget *parent) : QObject(parent)
{}

int CStyle::LoadStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QString szFile = set.value("Sink",
                  RabbitCommon::CDir::Instance()->GetDirData(true)
                  + QDir::separator()
                  + "style" + QDir::separator()
                  + "black_green.qss").toString();
    qDebug() << "LoadStyle:" << szFile;
    return  LoadStyle(szFile);
}

int CStyle::LoadStyle(const QString &szFile)
{
    if(szFile.isEmpty())
        qApp->setStyleSheet("");
    else
    {
        QFile file(szFile);
        if(file.open(QFile::ReadOnly))
        {
            QString stylesheet= file.readAll();
            qApp->setStyleSheet(stylesheet);
            file.close();
        }
        else
        {
            qDebug() << "file open file fail:" << szFile;                       
        }
    }
    return 0;
}

int CStyle::SetDefaultStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Sink", "");
    qApp->setStyleSheet("");
    return 0;
}

void CStyle::slotStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QString szFile = set.value("Sink",
                  RabbitCommon::CDir::Instance()->GetDirData()
                  + QDir::separator()
                  + "style" + QDir::separator()
                  + "black_green.qss").toString();
    if(szFile.isEmpty())
        szFile = RabbitCommon::CDir::Instance()->GetDirData()
                + QDir::separator()
                + "style" + QDir::separator()
                + "black_green.qss";
    QWidget* pParent = dynamic_cast<QWidget*>(this->parent());
    szFile = RabbitCommon::CDir::GetOpenFileName(pParent, tr("Open sink"),
                 szFile,
                 tr("Style files(*.qss *.css);; All files(*.*)"));
    if(szFile.isEmpty()) return;
    LoadStyle(szFile);
    
    set.setValue("Sink", szFile);
}

} //namespace RabbitCommon
