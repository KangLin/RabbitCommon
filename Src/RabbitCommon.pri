#-------------------------------------------------
# The file is deprecated
#-------------------------------------------------

QT *= core xml

CONFIG *= c++11 link_pkgconfig create_prl link_prl

isEmpty(RabbitCommon_VERSION): error("Please set RabbitCommon_VERSION")

#VERSION=$$replace(RabbitCommon_VERSION, v,)
#win32{
#    VERSION=$$split(VERSION, -)
#    VERSION=$$first(VERSION)
#}
win32: DEFINES *= WINDOWS
mingw: DEFINES *= MINGW
unix: DEFINES *= UNIX
android{
    DEFINES += BUILD_ARCH=\"\\\"$${ANDROID_TARGET_ARCH}\\\"\"
} else: win32 {
    contains(QMAKE_TARGET.arch, x86_64) {
        DEFINES += BUILD_ARCH=\"\\\"x86_64\\\"\"
    } else {
        DEFINES += BUILD_ARCH=\"\\\"x86\\\"\"
    }
} else {
    DEFINES += BUILD_ARCH=\"\\\"$$system(uname -p)\\\"\"
}
CONFIG(debug, debug|release): DEFINES *= _DEBUG
DEFINES += BUILD_PLATFORM=\"\\\"$${QMAKE_PLATFORM}\\\"\"

#Support windows xp
mingw: DEFINES += "_WIN32_WINNT=0x0501"
msvc {
    QMAKE_LFLAGS *= /SUBSYSTEM:WINDOWS",5.01"
    QMAKE_CXXFLAGS += "/utf-8"
}

INCLUDEPATH += $$PWD $$PWD/export

CONFIG(staticlib): CONFIG*=static
CONFIG(static): DEFINES *= RABBITCOMMON_STATIC_DEFINE
else: DEFINES *= RabbitCommon_EXPORTS

# 设置 Relase 时，qDebug 打印文件名与行号
DEFINES *= QT_MESSAGELOGCONTEXT

!equals(WITH_GUI, "OFF") {
    QT *= gui
    greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets
    DEFINES *= HAVE_RABBITCOMMON_GUI
    SOURCES += $$PWD/Style/Style.cpp \
        $$PWD/Style/FrmStyle.cpp \
        $$PWD/RabbitRecentMenu.cpp \
        $$PWD/TitleBar.cpp \
        $$PWD/DockFolderBrowser/DockFolderBrowser.cpp \
        $$PWD/Log/DockDebugLog.cpp
    INSTALL_HEADERS += \
        $$PWD/Style/FrmStyle.h \
        $$PWD/RabbitRecentMenu.h \
        $$PWD/TitleBar.h \
        $$PWD/DockFolderBrowser/DockFolderBrowser.h \
        $$PWD/Log/DockDebugLog.h
    HEADERS += $$PWD/Style/Style.h $${INSTALL_HEADERS}
    INCLUDEPATH += $$PWD/Style
    FORMS += $$PWD/Style/FrmStyle.ui \
       $$PWD/DockFolderBrowser/DockFolderBrowser.ui \
       $$PWD/Log/DockDebugLog.ui

    equals(BUILD_QUIWidget, "ON"){
        SOURCES += $$PWD/QUIWidget/QUIWidget.cpp
        INSTALL_HEADERS += $$PWD/QUIWidget/QUIWidget.h
        RESOURCES += $$PWD/QUIWidget/Resource/QUIWidget.qrc
        CONFIG(debug, debug|release):RESOURCES += $$PWD/QUIWidget/Resource/QUIWidgetQss.qrc
    }

    !equals(BUILD_UPDATE, "OFF"){
        greaterThan(QT_MAJOR_VERSION, 5): qtHaveModule(statemachine): QT *= statemachine
        QT *= network
        DEFINES *= HAVE_UPDATE
        SOURCES += $$PWD/FrmUpdater/FrmUpdater.cpp
        INSTALL_HEADERS += $$PWD/FrmUpdater/FrmUpdater.h
        INCLUDEPATH += $$PWD/FrmUpdater
        FORMS += $$PWD/FrmUpdater/FrmUpdater.ui
        BUILD_ADMINAUTHORISER = ON
    }

    !equals(BUILD_ABOUT, "OFF"){
        qtHaveModule(webenginewidgets) {
            QT *= webenginewidgets
            DEFINES *= HAVE_WebEngineWidgets
            }
        DEFINES *= HAVE_ABOUT
        SOURCES += $$PWD/DlgAbout/DlgAbout.cpp \
            $$PWD/DlgAbout/Information.cpp
        INSTALL_HEADERS += $$PWD/DlgAbout/DlgAbout.h
        INCLUDEPATH += $$PWD/DlgAbout
        HEADERS += $$PWD/DlgAbout/Information.h
        FORMS += $$PWD/DlgAbout/DlgAbout.ui \
            $$PWD/DlgAbout/Information.ui

        equals(WITH_CMARK_GFM, "ON"){
            # You need to set the environment variable: PKG_CONFIG_PATH
            packagesExist(libcmark-gfm) {
                pkg_config = $$pkgConfigExecutable()
                DEFINES *= HAVE_CMARK_GFM
                INCLUDEPATH *= $$system($$pkg_config --variable=includedir libcmark-gfm)
                LIBS *= $$system($$pkg_config --libs libcmark-gfm)
            }
        }
    }
}

!equals(BUILD_ADMINAUTHORISER, "OFF") {
    DEFINES *= HAVE_ADMINAUTHORISER
    SOURCES += $$PWD/AdminAuthoriser/adminauthoriser.cpp
    HEADERS += $$PWD/AdminAuthoriser/adminauthoriser.h \
               $$PWD/AdminAuthoriser/adminauthorization_p.h
    win32: SOURCES += $$PWD/AdminAuthoriser/adminauthorization_win.cpp
    else:mac: SOURCES += $$PWD/AdminAuthoriser/adminauthorization_mac.cpp
    else:unix:!emscripten:!android: SOURCES += $$PWD/AdminAuthoriser/adminauthorization_x11.cpp
    else: SOURCES += $$PWD/AdminAuthoriser/adminauthorization_dummy.cpp

    win32 {
        #QT += winextras
        LIBS += -lAdvapi32 -lOle32 -lShell32 -lnetapi32 -lWs2_32
    } else:mac {
        QT += macextras
        LIBS += -framework Security
    } else:unix:!android {
        QT += dbus
        LIBS += -lutil
    }
}

isEmpty(OpenSSL_DIR): warning("Please set OpenSSL_DIR")
else {
    DEFINES *= HAVE_OPENSSL
    SOURCES += EvpAES.cpp
    HEADERS += EvpAES.h
    INSTALL_HEADERS += 
    LIBS += $$OpenSSL_DIR/lib/libssl.so \
        $$OpenSSL_DIR/lib/libcrypto.so 
}

win32 {
    SOURCES += \
        $$PWD/CoreDump/MiniDumper.cpp \
        $$PWD/CoreDump/QMiniDumper.cpp
    HEADERS += $$PWD/CoreDump/MiniDumper.h
    DEFINES += _UNICODE UNICODE STRSAFE_NO_DEPRECATE
    LIBS += -lDbghelp -luser32
}

SOURCES += \
    $$PWD/Log/Log.cpp \
    $$PWD/RabbitCommonDir.cpp \
    $$PWD/RabbitCommonRegister.cpp \
    $$PWD/RabbitCommonTools.cpp \
    $$PWD/RabbitCommonEncrypt.cpp \
    $$PWD/DownloadFile.cpp

INSTALL_HEADERS += \
    $$PWD/RabbitCommonTools.h \
    $$PWD/RabbitCommonDir.h \
    $$PWD/export/rabbitcommon_export.h \
    $$PWD/export/rabbitcommon_export_windows.h \
    $$PWD/export/rabbitcommon_export_linux.h \
    $$PWD/RabbitCommonEncrypt.h \
    $$PWD/DownloadFile.h

HEADERS += $$INSTALL_HEADERS \
    $$PWD/Log/Log.h \
    $$PWD/RabbitCommonRegister.h

RESOURCES += \
    $$PWD/Resource/ResourceRabbitCommon.qrc

unix_install_script.target = unix_install_script
unix_install_script.files = $$PWD/../Install/install.sh
unix_install_script.path = $$system_path($${PREFIX}/install)
unix: INSTALLS += install_script

TRANSLATIONS_DIR=$$PWD
TRANSLATIONS_NAME=RabbitCommon
include($$PWD/../pri/Translations.pri)

android{
    !isEmpty(OpenSSL_DIR) : ANDROID_EXTRA_LIBS = $$OpenSSL_DIR/lib/libssl.so \
        $$OpenSSL_DIR/lib/libcrypto.so
}

# Install style files
style.target = style
style.files = $$PWD/Resource/style/*
CONFIG(release, debug|release): style.files += $$PWD/QUIWidget/Resource/qss/*
android: style.path = $$system_path(assets/data/style)
else: style.path = $$system_path($${PREFIX}/data/style)
style.CONFIG += directory no_check_exist
# Install QIcon theme
IconTheme.target = IconTheme
IconTheme.files = $$PWD/Resource/icons/*
android: IconTheme.path = $$system_path(assets/data/icons)
else: IconTheme.path = $$system_path($${PREFIX}/data/icons)
IconTheme.CONFIG += directory no_check_exist
INSTALLS += style IconTheme
