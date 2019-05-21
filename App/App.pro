#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T09:59:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RabbitCommonApp
TEMPLATE = app

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
    BUILD_VERSION="v0.0.1-22-g3111129"
}
message("BUILD_VERSION:$$BUILD_VERSION")
DEFINES += BUILD_VERSION=\"\\\"$$quote($$BUILD_VERSION)\\\"\"
CONFIG(debug, debug|release): DEFINES *= _DEBUG
CONFIG(staticlib): CONFIG*=static
CONFIG(static): DEFINES *= RABBITCOMMON_STATIC_DEFINE

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#Support windows xp
mingw : DEFINES += "_WIN32_WINNT=0x0501" 
msvc {
    QMAKE_LFLAGS *= /SUBSYSTEM:WINDOWS",5.01"
    QMAKE_CXXFLAGS += "/utf-8"
}

#VERSION=$$BUILD_VERSION
INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src $$_PRO_FILE_PWD_/../Src/export
!android: DESTDIR = $$OUT_PWD/../bin
DEPENDPATH = $$DESTDIR

isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : android : PREFIX = /.
    else : unnix : PREFIX = /usr/local
    else : PREFIX = $$OUT_PWD/../install
}

!equals(BUILD_UPDATE, "OFF"){
    DEFINES *= HAVE_UPDATE
}
!equals(BUILD_ABOUT, "OFF"){
    DEFINES *= HAVE_ABOUT
}
SOURCES += \
        main.cpp

HEADERS +=

FORMS +=

android {
    LIBS *= "-L$$OUT_PWD/../Src"
} else {
    LIBS *= "-L$$DESTDIR"
}
LIBS *= -lRabbitCommon

!android: target.path = $$PREFIX/bin
INSTALLS += target

win32 : equals(QMAKE_HOST.os, Windows){
    
    INSTALL_TARGET = $$system_path($${DESTDIR}/$(TARGET))

    Deployment_qtlib.target = Deployment_qtlib
    Deployment_qtlib.files = $$system_path($${DESTDIR}/)
    Deployment_qtlib.path = $$system_path($${PREFIX})
    Deployment_qtlib.commands = "$$system_path($$[QT_INSTALL_BINS]/windeployqt)" \
                    --compiler-runtime \
                    --verbose 7 \
                    "$${INSTALL_TARGET}"
    INSTALLS += Deployment_qtlib
}

OTHER_FILES += CMakeLists.txt
