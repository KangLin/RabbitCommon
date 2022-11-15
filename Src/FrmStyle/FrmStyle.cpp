#include "FrmStyle.h"
#include "ui_FrmStyle.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"
#include <QSettings>



CFrmStyle::CFrmStyle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFrmStyle)
{
    setAttribute(Qt::WA_QuitOnClose, false);
    ui->setupUi(this);

    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetStyleFile());
    // Icon theme
    qDebug(RabbitCommon::LoggerStyle) << "Icon theme search paths:" << QIcon::themeSearchPaths()
                   << "Icon theme name:" << QIcon::themeName()
                  #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                   << "Fallback search paths:" << QIcon::fallbackSearchPaths()
                  #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                  #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                   << "Fallback theme name:" << QIcon::fallbackThemeName()
                  #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                      ;
    foreach(auto d, QIcon::themeSearchPaths())
    {
        QDir dir(d);
        if(!dir.exists()) continue;
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
        if(RabbitCommon::CDir::Instance()->GetDirIcons() == d) continue;
#endif
        foreach(auto themeName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug(RabbitCommon::LoggerStyle) << "Path:" << dir.absolutePath()
                        << "Theme dir:" << themeName;
            QFileInfo fi(dir.absolutePath() + QDir::separator()
                         + themeName + QDir::separator() + "index.theme");
            if(!fi.exists())
                continue;
            qDebug(RabbitCommon::LoggerStyle) << "Theme:" << themeName;
            ui->cbIconTheme->addItem(themeName);
        }
    }
    if(!QIcon::themeName().isEmpty())
        ui->cbIconTheme->setCurrentText(QIcon::themeName());

#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
    QDir fallbackDir(RabbitCommon::CDir::Instance()->GetDirIcons());
    QStringList lstFallback;
    if(fallbackDir.exists())
    {
        foreach(auto themeName, fallbackDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug(RabbitCommon::LoggerStyle) << "fallbackDir:" << fallbackDir.absolutePath()
                        << "Theme dir:" << themeName;
            QFileInfo fi(fallbackDir.absolutePath() + QDir::separator()
                         + themeName + QDir::separator() + "index.theme");
            if(!fi.exists())
                continue;
            qDebug(RabbitCommon::LoggerStyle) << "fallback Theme:" << themeName;
            ui->cbFallbackTheme->addItem(themeName);
        }
    }

    if(!QIcon::fallbackThemeName().isEmpty())
    {
        ui->cbFallbackTheme->setCurrentText(QIcon::fallbackThemeName());
    }
    ui->gbFallbackTheme->setVisible(true);
#else
    ui->gbFallbackTheme->setVisible(false);
#endif
}

CFrmStyle::~CFrmStyle()
{
    delete ui;
}

void CFrmStyle::on_pbOK_clicked()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    if(!ui->cbIconTheme->currentText().isEmpty())
        QIcon::setThemeName(ui->cbIconTheme->currentText());
    set.setValue("Style/Icon/Theme", QIcon::themeName());

#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    if(!ui->cbFallbackTheme->currentText().isEmpty())
        QIcon::setFallbackThemeName(ui->cbFallbackTheme->currentText());
    set.setValue("Style/Icon/Theme/Fallback", QIcon::fallbackThemeName());
#endif

    close();
}

void CFrmStyle::on_pbCancel_clicked()
{
    close();
}

void CFrmStyle::on_pbBrower_clicked()
{
    RabbitCommon::CStyle::Instance()->slotStyle();
    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetStyleFile());
}

void CFrmStyle::on_pbDefault_clicked()
{
    RabbitCommon::CStyle::Instance()->slotSetDefaultStyle();
    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetStyleFile());
    ui->cbIconTheme->setCurrentText(RabbitCommon::CStyle::Instance()->m_szDefaultIconTheme);
    ui->cbFallbackTheme->setCurrentText(RabbitCommon::CStyle::Instance()->m_szDefaultFallbackIconTheme);
}
