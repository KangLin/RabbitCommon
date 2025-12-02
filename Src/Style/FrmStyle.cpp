// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

// See: https://github.com/KangLin/Documents/blob/master/qt/theme.md
// - Icon Theme Specification: https://specifications.freedesktop.org/icon-theme-spec/latest/
// - Icon Naming Specification: https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
// - Icon Theme Specification: https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

#include <QSettings>
#include <QLoggingCategory>
#include <QPalette>
#include <QIcon>
#include <QStyleHints>
#include <QStyleFactory>
#include <QApplication>
#include <QMessageBox>
#include <QFontDialog>
#include <QFontDatabase>

#include "FrmStyle.h"
#include "ui_FrmStyle.h"
#include "Style.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Style")

CFrmStyle::CFrmStyle(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    ui(new Ui::CFrmStyle)
{
    qDebug(log) << Q_FUNC_INFO;
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);

    auto pStyle = RabbitCommon::CStyle::Instance();
    foreach(auto szName, QStyleFactory::keys()) {
        if(-1 == ui->cbStyleName->findData(szName))
            ui->cbStyleName->addItem(szName, szName);
    }
    if(!QStyleFactory::keys().isEmpty()) {
        int index = ui->cbStyleName->findData(pStyle->GetStyleName());
        if(-1 < index)
            ui->cbStyleName->setCurrentIndex(index);
    }

    ui->leStyleSheet->setText(pStyle->GetStyleSheetFile());

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    QFont font;
    if(font.fromString(set.value("Style/Font", qApp->font().toString()).toString()))
        ui->fontComboBox->setCurrentFont(font);
    
    LoadFontOrder loadOrder = (LoadFontOrder)set.value("Style/FontLoad", (int)LoadFontOrder::BeforeStyle).toInt();
    switch (loadOrder) {
    case LoadFontOrder::BeforeStyle:
        ui->rbLoadBeforeStyle->setChecked(true);
        break;
    case LoadFontOrder::AfterStyle:
        ui->rbLoadAfterStyle->setChecked(true);
        break;
    }

    ui->lbIconThemeChanged->setVisible(false);
    // Icon theme
    bool bIconTheme = set.value("Style/Icon/Theme/Enable", true).toBool();
    ui->gpIconTheme->setChecked(bIconTheme);

    qDebug(log)
        << "Icon theme name:" << QIcon::themeName() << "\n"
        << "Icon theme search paths:" << QIcon::themeSearchPaths() << "\n"
        #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        << "Fallback theme name:" << QIcon::fallbackThemeName() << "\n"
        #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        << "Fallback search paths:" << QIcon::fallbackSearchPaths() << "\n"
        #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        ;
    foreach(auto d, QIcon::themeSearchPaths())
    {
        QDir dir(d);
        if(!dir.exists())
        {
            qDebug(log)
            << "Theme folder isn't exists:" << d;
            continue;
        }

        qInfo(log) << "Theme folder:" << dir.absolutePath();
// #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
//         if(RabbitCommon::CDir::Instance()->GetDirIcons() == d) continue;
// #endif
        foreach(auto themeName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug(log) << "Theme path:" << dir.absolutePath()
                        << "Theme:" << themeName;
            QFileInfo fi(dir.absolutePath() + QDir::separator()
                         + themeName + QDir::separator() + "index.theme");
            if(fi.exists())
            {
                qDebug(log) << "Theme:" << themeName;
                ui->cbIconTheme->addItem(themeName);
            } else {
                qCritical(log) << "index.theme doesn't exists: " << fi.fileName()
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
            qDebug(log) << "fallback path:" << fallbackDir.absolutePath()
                        << "Theme:" << themeName;
            QFileInfo fi(fallbackDir.absolutePath() + QDir::separator()
                         + themeName + QDir::separator() + "index.theme");
            if(!fi.exists())
                continue;
            qDebug(log) << "fallback Theme:" << themeName;
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
    qDebug(log) << "CFrmStyle::~CFrmStyle()";
    delete ui;
}

void CFrmStyle::on_pbOK_clicked()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto colorScheme = QGuiApplication::styleHints()->colorScheme();
    QString szColorScheme;
    QRegularExpression re("[Bb]lack|[Dd]ark");
    QRegularExpressionMatch match = re.match(ui->cbIconTheme->currentText());
    if (Qt::ColorScheme::Dark == colorScheme && match.hasMatch()) {
        szColorScheme = tr("Dark");
    }
    QRegularExpression reWhite("[Ww]hite|[L|l]ight");
    match = reWhite.match(ui->cbIconTheme->currentText());
    if (Qt::ColorScheme::Light == colorScheme && match.hasMatch()) {
        szColorScheme = tr("Light");
    }
    if(!szColorScheme.isEmpty()) {
        QString szInfo = tr("Current system theme is") + " \""
                         + szColorScheme + "\", " + tr("current select theme is")
                         + QString(" \"") + ui->cbIconTheme->currentText() + "\". \n";
        szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.") + "\n"
                  + tr("Are you sure you want to modify it?");
        auto ret = QMessageBox::information(nullptr, tr("Change theme"),
                                            szInfo,
                                            QMessageBox::Ok | QMessageBox::No,
                                            QMessageBox::No);
        if(QMessageBox::No == ret)
            return;
    }
#endif // QT_VERSION

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

    auto pStyle = RabbitCommon::CStyle::Instance();
    pStyle->SetStyleSheetFile(ui->leStyleSheet->text());
    pStyle->SetStyleName(ui->cbStyleName->currentData().toString());

    set.setValue("Style/Font", ui->fontComboBox->currentFont().toString());

    if(ui->rbLoadBeforeStyle->isChecked()) {
        qApp->setFont(ui->fontComboBox->currentFont());
        pStyle->ReLoadStyle();
        set.setValue("Style/FontLoad", (int)LoadFontOrder::BeforeStyle);
    }
    if(ui->rbLoadAfterStyle->isChecked()) {
        pStyle->ReLoadStyle();
        qApp->setFont(ui->fontComboBox->currentFont());
        set.setValue("Style/FontLoad", (int)LoadFontOrder::AfterStyle);
    }

    close();
}

void CFrmStyle::on_pbCancel_clicked()
{
    close();
}

void CFrmStyle::on_pbBrowse_clicked()
{
    ui->leStyleSheet->setText(RabbitCommon::CStyle::Instance()->GetStyleSheet());
}

void CFrmStyle::on_pbDefault_clicked()
{
    ui->leStyleSheet->setText("");
    if(ui->cbStyleName->count() > 0) {
        int index = ui->cbStyleName->findData("Fusion");
        if(-1 == index)
            index = 0;
        ui->cbStyleName->setCurrentIndex(index);
    }
    if(ui->gpIconTheme->isChecked()) {
        ui->cbIconTheme->setCurrentText(
            RabbitCommon::CStyle::Instance()->m_szDefaultIconTheme);
        ui->cbFallbackTheme->setCurrentText(
            RabbitCommon::CStyle::Instance()->m_szDefaultFallbackIconTheme);
    }
    
    ui->rbLoadBeforeStyle->setChecked(true);
    ui->fontComboBox->setCurrentFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
}

void CFrmStyle::on_gpIconTheme_clicked()
{
    ui->lbIconThemeChanged->setVisible(true);
}

void CFrmStyle::on_pbSetFont_clicked()
{
    //*
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, ui->fontComboBox->currentFont(),
                        this, QString(), QFontDialog::DontUseNativeDialog);
    if(ok) {
        ui->fontComboBox->setCurrentFont(font);
    }//*/
    /*QFontDialog dlg(qApp->font(), this);
    dlg.setCurrentFont(qApp->font());
    int nRet = RC_SHOW_WINDOW(&dlg);
    if(QDialog::Accepted == nRet) {
        qApp->setFont(dlg.selectedFont());
    }//*/
}
