// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "Style.h"
#include "RabbitCommonDir.h"

#include <QStyle>
#include <QStyleFactory>
#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <QRegularExpression>
#include <QLoggingCategory>
#include <QDir>
#include <QColor>
#include <QPalette>
#include <QIcon>
#include <QStyleHints>
#include <QMessageBox>

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Style")

CStyle::CStyle(QObject *parent) : QObject(parent)
    , m_bModifyStyleSheetFile(false)
    , m_bModifyStyleName(false)
{
    qDebug(log) << Q_FUNC_INFO;

    m_szDefaultIconTheme = QIcon::themeName();
    if(m_szDefaultIconTheme.isEmpty()) {
        m_szDefaultIconTheme = "rabbit-black";
        #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        const auto scheme = QGuiApplication::styleHints()->colorScheme();
        if(Qt::ColorScheme::Dark == scheme)
            m_szDefaultIconTheme = "rabbit-white";
        #endif
        QIcon::setThemeName(m_szDefaultIconTheme);
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
    if(!QIcon::fallbackThemeName().isEmpty())
        qDebug(log) << "Old icon fallback theme:" << QIcon::fallbackThemeName();
    //Note: the fallback icon theme must set "rabbit-*"
    m_szDefaultFallbackIconTheme = "rabbit-black";
    #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    if(Qt::ColorScheme::Dark == scheme)
            m_szDefaultFallbackIconTheme = "rabbit-white";
    #endif
    QIcon::setFallbackThemeName(m_szDefaultFallbackIconTheme);
#endif
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

    bool check = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if(QApplication::instance()) {
        check = connect(QApplication::styleHints(),
                        SIGNAL(colorSchemeChanged(Qt::ColorScheme)),
                        this, SLOT(slotColorSchemeChanged(Qt::ColorScheme)));
        Q_ASSERT(check);
    }
#endif

#if defined(Q_OS_WIN)
    if(QStyleFactory::keys().contains("Fusion"))
        m_szStyleName = "Fusion";
#else
    #if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
        if(QApplication::style())
            m_szStyleName = QApplication::style()->name();
    #else
        if(!QStyleFactory::keys().isEmpty())
            m_szStyleName = QStyleFactory::keys().at(0);
    #endif
#endif
    if(m_szStyleName.isEmpty()) {
        if(!QStyleFactory::keys().isEmpty())
            m_szStyleName = QStyleFactory::keys().at(0);
    }
    qDebug(log) << "Init sytle name:" << m_szStyleName;
}

int CStyle::LoadStyle()
{
    // Load icons theme
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QIcon::setThemeSearchPaths(
        QIcon::themeSearchPaths()
        << RabbitCommon::CDir::Instance()->GetDirIcons(true));
    bool bIconTheme = set.value("Style/Icon/Theme/Enable", true).toBool();
    if(bIconTheme) {
        QString szThemeName = QIcon::themeName();
        if(szThemeName.isEmpty())
            szThemeName = m_szDefaultIconTheme;
        szThemeName = set.value("Style/Icon/Theme", szThemeName).toString();

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        Qt::ColorScheme scheme = Qt::ColorScheme::Unknown;
        if(QGuiApplication::styleHints())
            scheme = QGuiApplication::styleHints()->colorScheme();
        //qDebug(log) << "Scheme:" << scheme;
        static QRegularExpression reBlack("black|[Dd]ark");
        QRegularExpressionMatch matchBlack = reBlack.match(szThemeName);
        if (Qt::ColorScheme::Dark == scheme && matchBlack.hasMatch()) {
            QString szInfo = tr("Current system theme is dark, current theme is ") + QString("\"") + szThemeName + "\". ";
            szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.");
            szThemeName = "rabbit-white";
            szInfo += " " + tr("change to ") + szThemeName;
            qInfo(log) << szInfo;
        }
        static QRegularExpression reWhite("[Ww]hite|[L|l]ight");
        QRegularExpressionMatch matchWhite = reWhite.match(szThemeName);
        if (Qt::ColorScheme::Light == scheme && matchWhite.hasMatch()) {
            QString szInfo = tr("Current system theme is light, current theme is ") + QString("\"") + szThemeName + "\". ";
            szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.");
            szThemeName = "rabbit-black";
            szInfo += " " + tr("change to ") + szThemeName;
            qInfo(log) << szInfo;
        }
#endif // QT_VERSION

        QIcon::setThemeName(szThemeName);
        
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
        QIcon::setFallbackSearchPaths(
            QIcon::fallbackSearchPaths()
            << RabbitCommon::CDir::Instance()->GetDirIcons(true));
        QString szFallbackThemeName = QIcon::fallbackThemeName();
        if(szFallbackThemeName.isEmpty())
            szFallbackThemeName = m_szDefaultFallbackIconTheme;
        QIcon::setFallbackThemeName(set.value("Style/Icon/Theme/Fallback",
                                              szFallbackThemeName).toString());

        #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        if (Qt::ColorScheme::Dark == scheme && matchBlack.hasMatch()) {
            QString szInfo = tr("Current system theme is dark, current theme is ") + QString("\"") + szFallbackThemeName + "\". ";
            szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.");
            szFallbackThemeName = "rabbit-white";
            szInfo += " " + tr("change to ") + szFallbackThemeName;
            qInfo(log) << szInfo;
        }
        if (Qt::ColorScheme::Light == scheme && matchWhite.hasMatch()) {
            QString szInfo = tr("Current system theme is light, current theme is ") + QString("\"") + szFallbackThemeName + "\". ";
            szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.");
            szFallbackThemeName = "rabbit-black";
            szInfo += " " + tr("change to ") + szFallbackThemeName;
            qInfo(log) << szInfo;
        }
        #endif
#endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)

        qDebug(log) << Q_FUNC_INFO << "\n"
                    << "Icon theme name:" << QIcon::themeName() << "\n"
                    << "Icon theme search paths:" << QIcon::themeSearchPaths() << "\n"
                    #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                    << "Fallback theme name:" << QIcon::fallbackThemeName() << "\n"
                    #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                    #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                    << "Fallback search paths:" << QIcon::fallbackSearchPaths() << "\n"
                    #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                    ;
    }

    //qDebug(log) << "Current style:" << QApplication::style() << "QStyleFactory:" << QStyleFactory::keys();

    QString szName = set.value("Style/Name", GetStyleName()).toString();
    SetStyleName(szName);
    LoadStyle(szName);

    // Get style sheet file
    QString szFile = set.value("Style/Sheet/File", GetStyleSheetFile()).toString();
    SetStyleSheetFile(szFile);
    return LoadStyleSheet(szFile);
}

CStyle* CStyle::Instance()
{
    static CStyle* p = new CStyle();
    if(!p) p = new CStyle();
    return p;
}

int CStyle::ReLoadStyle()
{
    if(m_bModifyStyleName) {
        LoadStyle(m_szStyleName);
        m_bModifyStyleName = false;
    }
    if(m_bModifyStyleSheetFile) {
        LoadStyleSheet(m_szStyleSheetFile);
        m_bModifyStyleSheetFile = false;
    }
    return 0;
}

int CStyle::LoadStyle(const QString& szName)
{
    // qDebug(log) << "Current style:" << QApplication::style()
    //             << "QStyleFactory:" << QStyleFactory::keys();

    if(szName.isEmpty())
        return 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0)
    if(QApplication::style() && QApplication::style()->name() == szName)
        return 0;
#endif
    qInfo(log) << "Apply style:" << szName;
    QApplication::setStyle(QStyleFactory::create(szName));
    return 0;

#if defined(Q_OS_WIN)
    // 显式设置 Windows 样式，随着系统颜色改变（测试环境：Qt6.9.2; Windows10)。
    //QApplication::setStyle(QStyleFactory::create("windows"));
    
    // 显式设置 Windows11 样式，可随着系统颜色改变（测试环境：Qt6.9.2; Windows10)。
    //QApplication::setStyle(QStyleFactory::create("windows11"));
    
    // 或者使用 WindowsVista 样式（更现代）,不能随着系统颜色改变（测试环境：Qt6.9.2; Windows10)。
    //QApplication::setStyle(QStyleFactory::create("WindowsVista"));
    
    if(QStyleFactory::keys().contains("Fusion")) {
        // 使用 Fusion 样式（跨平台一致）
        QApplication::setStyle(QStyleFactory::create("Fusion"));
    }
    // 但使用 Windows 的默认调色板
    //QApplication::setPalette(QApplication::style()->standardPalette());
#endif
    return 0;
}

int CStyle::LoadStyleSheet(const QString &szFile)
{
    //m_szStyleSheetFile = szFile;
    if(szFile.isEmpty())
    {
        qApp->setStyleSheet("");
        //qApp->setPalette(QPalette(QColor(Qt::gray)));
    } else {
        QFile file(szFile);
        if(file.open(QFile::ReadOnly))
        {
            qInfo(log) << "Load style file:" << szFile;
            QString stylesheet= file.readAll();
            QString pattern("QPalette\\{background:#[0-9a-fA-F]+;\\}");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QRegularExpression re(pattern);
            QRegularExpressionMatch match = re.match(stylesheet);
            if (match.hasMatch()) {
                QString matched = match.captured(0);
                if(!matched.isEmpty())
                {
                    QRegularExpression rePalette("#[0-9a-fA-F]+");
                    match = rePalette.match(matched);
                    if(match.hasMatch())
                    {
                        QString paletteColor = match.captured(0);
                        qApp->setPalette(QPalette(QColor(paletteColor)));
                    }
                }            
            }
#else
            QRegExp rx(pattern);
            int pos = rx.indexIn(stylesheet);
            if(pos > -1)
            {
                QString szPalette = rx.cap();
                QRegExp rxPalette("#[0-9a-fA-F]+");
                int pos = rxPalette.indexIn(stylesheet);
                if(pos > -1)
                {
                    QString paletteColor =  rxPalette.cap();
                    qApp->setPalette(QPalette(QColor(paletteColor)));
                }
            }
#endif
            qApp->setStyleSheet(stylesheet);
            file.close();
        }
        else
        {
            qCritical(log) << "file open file fail:" << szFile;
        }
    }
    return 0;
}

QString CStyle::GetStyleSheet()
{
    QString szFile = m_szStyleSheetFile;   
    QString szPath = RabbitCommon::CDir::Instance()->GetDirData(true)
                     + QDir::separator()
                     + "style";
#ifdef Q_OS_LINUX
    QDir d(szPath);
    if(!d.exists())
        szPath = "/usr/share/style";
#endif
    if(!szFile.isEmpty()) {
        QFileInfo fi(szFile);
        szPath = fi.absoluteFilePath();
    }
    qDebug(log) << "Path:" << szPath;
    QWidget* pParent = dynamic_cast<QWidget*>(this->parent());
    szFile = QFileDialog::getOpenFileName(pParent, tr("Open style"),
                                          szPath,
                                          tr("Style files(*.qss *.css);; All files(*.*)"));
    return szFile;
}

QString CStyle::GetStyleSheetFile()
{
    if(!m_szStyleSheetFile.isEmpty())
        if(!QFile::exists(m_szStyleSheetFile))
            return "";
    return m_szStyleSheetFile;
}

void CStyle::SetStyleSheetFile(const QString &file)
{
    if(m_szStyleSheetFile == file)
        return;
    m_szStyleSheetFile = file;
    m_bModifyStyleSheetFile = true;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Style/Sheet/File", m_szStyleSheetFile);
}

QString CStyle::GetStyleName()
{
    return m_szStyleName;
}

void CStyle::SetStyleName(const QString &szName)
{
    if(szName == m_szStyleName)
        return;
    m_szStyleName = szName;
    m_bModifyStyleName = true;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Style/Name", m_szStyleName);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void CStyle::slotColorSchemeChanged(Qt::ColorScheme colorScheme)
{
    QString szThemeName;
    QString szColorScheme;
    QRegularExpression reBlack("[Bb]lack|[Dd]ark");
    QRegularExpressionMatch match = reBlack.match(QIcon::themeName());
    if (Qt::ColorScheme::Dark == colorScheme && match.hasMatch()) {
        
        szThemeName = "rabbit-white";
        szColorScheme = tr("Dark");
    }
    QRegularExpression reWhite("[Ww]hite|[L|l]ight");
    match = reWhite.match(QIcon::themeName());
    if (Qt::ColorScheme::Light == colorScheme && match.hasMatch()) {
        
        szThemeName = "rabbit-black";
        szColorScheme = tr("Light");
    }
    if(szThemeName.isEmpty()) return;
    QString szInfo = tr("Current system theme is") + " \"" + szColorScheme + "\", " + tr("current theme is") + QString(" \"") + QIcon::themeName() + "\". ";
    szInfo += tr("it's almost impossible to find the icon because its color matches the current system theme.");        
    szInfo += " " + tr("change to ") + "\"" + szThemeName + "\"?";
    auto ret = QMessageBox::information(nullptr, tr("Change theme"), szInfo,
                                        QMessageBox::Ok | QMessageBox::No,
                                        QMessageBox::Ok);
    if(QMessageBox::Ok == ret) {
        QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                      QSettings::IniFormat);
        QIcon::setThemeName(szThemeName);
        szThemeName = set.value("Style/Icon/Theme", szThemeName).toString();
    }
}
#endif // QT_VERSION

} //namespace RabbitCommon
