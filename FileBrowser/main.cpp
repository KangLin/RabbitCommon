#include "MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLoggingCategory>
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "FileBrowser.main")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RabbitCommon::CTools::Instance()->Init();
    RabbitCommon::CTools::Instance()->InstallTranslator();
    a.setApplicationDisplayName(QObject::tr("File browser"));

    QStringList uiLanguages;
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    uiLanguages = QLocale::system().uiLanguages(QLocale::TagSeparator::Underscore);
#else
    uiLanguages = QLocale::system().uiLanguages();
#endif
    qDebug(log) << uiLanguages;
    /*
    QTranslator translator;
    for (const QString &locale : uiLanguages) {
        const QString baseName = "FileBrowser_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/
    MainWindow w;
    w.show();
    return a.exec();
}
