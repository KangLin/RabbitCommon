QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia

CONFIG += c++11

#Get app version use git, please set git path to environment variable PATH
isEmpty(BUILD_VERSION) {
    isEmpty(GIT) : GIT=$$(GIT)
    isEmpty(GIT) : GIT=git
    isEmpty(GIT_DESCRIBE) {
        GIT_DESCRIBE = $$system(cd $$system_path($$_PRO_FILE_PWD_) && $$GIT describe --tags)
        isEmpty(BUILD_VERSION) {
            BUILD_VERSION = $$GIT_DESCRIBE
        }
    }
    isEmpty(BUILD_VERSION) {
        BUILD_VERSION = $$system(cd $$system_path($$_PRO_FILE_PWD_) && $$GIT rev-parse --short HEAD)
    }
    
    isEmpty(BUILD_VERSION){
        warning("Built without git, please add BUILD_VERSION to DEFINES or add git path to environment variable GIT or qmake parameter GIT")
    }
}
isEmpty(BUILD_VERSION){
    BUILD_VERSION="v0.0.1"
}
message("BUILD_VERSION:$$BUILD_VERSION")
DEFINES += BUILD_VERSION=\"\\\"$$quote($$BUILD_VERSION)\\\"\"

contains(QMAKE_TARGET.arch, x86_64) {
    DEFINES += BUILD_ARCH=\"\\\"x86_64\\\"\"
} else {
    DEFINES += BUILD_ARCH=\"\\\"x86\\\"\"
}
CONFIG(debug, debug|release) : DEFINES += _DEBUG
DEFINES += BUILD_PLATFORM=\"\\\"$${QMAKE_PLATFORM}\\\"\"

#Support windows xp
mingw : DEFINES += "_WIN32_WINNT=0x0501" 
msvc {
    QMAKE_LFLAGS *= /SUBSYSTEM:WINDOWS",5.01"
    QMAKE_CXXFLAGS += "/utf-8"
}

INCLUDEPATH = $$PWD

DEFINES += RabbitCommon_EXPORTS

SOURCES += $$PWD/FrmUpdater/FrmUpdater.cpp \
    $$PWD/DlgAbout/DlgAbout.cpp \
    $$PWD/GlobalDir.cpp

INSTALL_HEADERS += $$PWD/FrmUpdater/FrmUpdater.h \
    $$PWD/DlgAbout/DlgAbout.h \
    $$PWD/rabbitcommon_export.h \
    $$PWD/rabbitcommon_export_windows.h \
    $$PWD/rabbitcommon_export_linux.h

HEADERS += $$INSTALL_HEADERS \
    $$PWD/GlobalDir.h 

FORMS += $$PWD/FrmUpdater/FrmUpdater.ui \
    $$PWD/DlgAbout/DlgAbout.ui

RESOURCES += \
    $$PWD/Resource/Resource.qrc

TRANSLATIONS_DIR=$$PWD
TRANSLATIONS_NAME=RabbitCommon
include($$PWD/../pri/Translations.pri)
