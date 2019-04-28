QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia

CONFIG += c++11

isEmpty(BUILD_VERSION): error("Please set BUILD_VERSION")
contains(QMAKE_TARGET.arch, x86_64) {
    DEFINES += BUILD_ARCH=\"\\\"x86_64\\\"\"
} else {
    DEFINES += BUILD_ARCH=\"\\\"x86\\\"\"
}
CONFIG(debug, debug|release) : DEFINES += _DEBUG
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

!equals(BUILD_UPDATE, "OFF"){
    SOURCES += $$PWD/FrmUpdater/FrmUpdater.cpp
    INSTALL_HEADERS += $$PWD/FrmUpdater/FrmUpdater.h
    FORMS += $$PWD/FrmUpdater/FrmUpdater.ui
}
!equals(BUILD_ABOUT, "OFF"){
    SOURCES += $$PWD/DlgAbout/DlgAbout.cpp
    INSTALL_HEADERS += $$PWD/DlgAbout/DlgAbout.h
    FORMS += $$PWD/DlgAbout/DlgAbout.ui
}
SOURCES += \
    $$PWD/RabbitCommonGlobalDir.cpp \
    $$PWD/RabbitCommonTools.cpp

INSTALL_HEADERS += $$PWD/RabbitCommonTools.h \
    $$PWD/export/rabbitcommon_export.h \
    $$PWD/export/rabbitcommon_export_windows.h \
    $$PWD/export/rabbitcommon_export_linux.h

HEADERS += $$INSTALL_HEADERS \
    $$PWD/RabbitCommonGlobalDir.h

RESOURCES += \
    $$PWD/Resource/ResourceRabbitCommon.qrc

TRANSLATIONS_DIR=$$PWD
TRANSLATIONS_NAME=RabbitCommon
include($$PWD/../pri/Translations.pri)
