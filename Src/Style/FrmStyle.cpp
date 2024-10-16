// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

// See: https://github.com/KangLin/Documents/blob/master/qt/theme.md
// - Icon Naming Specification: https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
// - Icon Theme Specification: https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

#include "FrmStyle.h"
#include "ui_FrmStyle.h"
#include "Style.h"
#include "RabbitCommonDir.h"
#include <QSettings>

CFrmStyle::CFrmStyle(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    ui(new Ui::CFrmStyle)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);

    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetStyleFile());

    ui->lbIconThemeChanged->setVisible(false);
    // Icon theme
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    bool bIconTheme = set.value("Style/Icon/Theme/Enable", true).toBool();
    ui->gpIconTheme->setChecked(bIconTheme);

    qDebug(RabbitCommon::LoggerStyle)
            << "Icon theme search paths:" << QIcon::themeSearchPaths() << "\n"
            << "Icon theme name:" << QIcon::themeName() << "\n"
           #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            << "Fallback search paths:" << QIcon::fallbackSearchPaths() << "\n"
           #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
           #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
            << "Fallback theme name:" << QIcon::fallbackThemeName() << "\n"
           #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                                         ;
    foreach(auto d, QIcon::themeSearchPaths())
    {
        QDir dir(d);
        if(!dir.exists())
        {
            qDebug(RabbitCommon::LoggerStyle)
                << "Theme folder isn't exists:" << d;
            continue;
        }

        qInfo(RabbitCommon::LoggerStyle)
            << "Theme folder:" << dir.absolutePath();
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
        if(RabbitCommon::CDir::Instance()->GetDirIcons() == d) continue;
#endif
        foreach(auto themeName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug(RabbitCommon::LoggerStyle) << "Theme path:" << dir.absolutePath()
                                              << "Theme:" << themeName;
            QFileInfo fi(dir.absolutePath() + QDir::separator()
                         + themeName + QDir::separator() + "index.theme");
            if(fi.exists())
            {
                qDebug(RabbitCommon::LoggerStyle) << "Theme:" << themeName;
                ui->cbIconTheme->addItem(themeName);
            } else {
                qCritical(RabbitCommon::LoggerStyle)
                    << "index.theme is not exists:" << fi.fileName()
                    << "Theme:" << themeName;
            }
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
            qDebug(RabbitCommon::LoggerStyle) << "fallback path:" << fallbackDir.absolutePath()
                                              << "Theme:" << themeName;
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
    qDebug(RabbitCommon::LoggerStyle) << "CFrmStyle::~CFrmStyle()";
    delete ui;
}

void CFrmStyle::on_pbOK_clicked()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    bool bIconTheme = ui->gpIconTheme->isChecked();
    set.setValue("Style/Icon/Theme/Enable", bIconTheme);
    if(bIconTheme) {
        if(!ui->cbIconTheme->currentText().isEmpty())
            QIcon::setThemeName(ui->cbIconTheme->currentText());
        set.setValue("Style/Icon/Theme", QIcon::themeName());

#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        if(!ui->cbFallbackTheme->currentText().isEmpty())
            QIcon::setFallbackThemeName(ui->cbFallbackTheme->currentText());
        set.setValue("Style/Icon/Theme/Fallback", QIcon::fallbackThemeName());
#endif
    }

    RabbitCommon::CStyle::Instance()->SetFile(ui->leStyleName->text());
    close();
}

void CFrmStyle::on_pbCancel_clicked()
{
    close();
}

void CFrmStyle::on_pbBrowse_clicked()
{
    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetStyle());
}

void CFrmStyle::on_pbDefault_clicked()
{
    ui->leStyleName->setText(RabbitCommon::CStyle::Instance()->GetDefaultStyle());

    if(ui->gpIconTheme->isChecked()) {
        ui->cbIconTheme->setCurrentText(RabbitCommon::CStyle::Instance()->m_szDefaultIconTheme);
        ui->cbFallbackTheme->setCurrentText(RabbitCommon::CStyle::Instance()->m_szDefaultFallbackIconTheme);
    }
}

void CFrmStyle::on_gpIconTheme_clicked()
{
    ui->lbIconThemeChanged->setVisible(true);
}
