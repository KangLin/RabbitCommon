#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T09:59:18
#
#-------------------------------------------------

QT     *= core gui xml
CONFIG *= c++11 link_pkgconfig link_prl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = RabbitCommonTests
TEMPLATE = app

isEmpty(PREFIX) : !isEmpty(INSTALL_ROOT) : PREFIX=$$INSTALL_ROOT
isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : android : PREFIX = /.
    else : PREFIX = $$OUT_PWD/../install
}

#Get app version use git, please set git path to environment variable PATH
isEmpty(RabbitCommon_VERSION) {
    isEmpty(GIT) : GIT=$$(GIT)
    isEmpty(GIT) : GIT=git
    isEmpty(GIT_DESCRIBE) {
        GIT_DESCRIBE = $$system(cd $$system_path($$_PRO_FILE_PWD_) && $$GIT describe --tags)
        isEmpty(RabbitCommon_VERSION) {
            RabbitCommon_VERSION = $$GIT_DESCRIBE
        }
    }
    isEmpty(RabbitCommon_VERSION) {
        RabbitCommon_VERSION = $$system(cd $$system_path($$_PRO_FILE_PWD_) && $$GIT rev-parse --short HEAD)
    }
    
    isEmpty(RabbitCommon_VERSION){
        warning("Built without git, please add RabbitCommon_VERSION to DEFINES or add git path to environment variable GIT or qmake parameter GIT")
    }
}
isEmpty(RabbitCommon_VERSION){
    RabbitCommon_VERSION="v1.0.15"
}
message("RabbitCommon_VERSION:$$RabbitCommon_VERSION")
DEFINES += RabbitCommon_VERSION=\"\\\"$$quote($$RabbitCommon_VERSION)\\\"\"
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

#Support windows xp
mingw : DEFINES += "_WIN32_WINNT=0x0501"
msvc {
    QMAKE_LFLAGS *= /SUBSYSTEM:WINDOWS",5.01"
    QMAKE_CXXFLAGS += "/utf-8"
}

include($$PWD/../pri/Translations.pri)

#VERSION=$$RabbitCommon_VERSION
INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src $$_PRO_FILE_PWD_/../Src/export
isEmpty(DESTDIR): DESTDIR = $$OUT_PWD/../bin
DEPENDPATH = $$DESTDIR
LIBS *= "-L$$DESTDIR" -lRabbitCommon

!equals(WITH_GUI, "OFF"){
    DEFINES *= HAVE_RABBITCOMMON_GUI
    INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src/FrmStyle
    INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src/DockFolderBrowser
}
!equals(BUILD_UPDATE, "OFF"){
    DEFINES *= HAVE_UPDATE
    INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src/FrmUpdater
}
!equals(BUILD_ABOUT, "OFF"){
    DEFINES *= HAVE_ABOUT
    INCLUDEPATH+=$$_PRO_FILE_PWD_/../Src/DlgAbout
}
!equals(BUILD_ADMINAUTHORISER, "OFF"){
    DEFINES *= HAVE_ADMINAUTHORISER
}
equals(BUILD_QUIWidget, "ON"){
    DEFINES *= BUILD_QUIWidget
}
SOURCES += \
        MainWindow.cpp \
        main.cpp

HEADERS += \
    MainWindow.h

FORMS += \
    MainWindow.ui

!android: target.path = $$PREFIX/bin
INSTALLS += target

win32 : equals(QMAKE_HOST.os, Windows){
    
    INSTALL_TARGET = $$system_path($${DESTDIR}/$(TARGET))

    Deployment_qtlib.target = Deployment_qtlib
    Deployment_qtlib.files = $$system_path($${DESTDIR}/) #把目录 DESTDIR 安装到下面 path 中,注意要加 / ,否则不包括目录，只复制目录中的文件
    Deployment_qtlib.path = $$system_path($${PREFIX})
    Deployment_qtlib.commands = "$$system_path($$[QT_INSTALL_BINS]/windeployqt)" \
                    --compiler-runtime \
                    --verbose 7 \
                    "$${INSTALL_TARGET}"
    INSTALLS += Deployment_qtlib
}

OTHER_FILES += CMakeLists.txt

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}


!android : unix {

    DESKTOP_FILE.target = DESKTOP_FILE
    DESKTOP_FILE.files = $$PWD/share/org.Rabbit.RabbitCommon.desktop
    DESKTOP_FILE.path = $${PREFIX}/share/applications
    DESKTOP_FILE.CONFIG += directory no_check_exist

    START_SCRIPT.target = START_SCRIPT
    START_SCRIPT.files = $$PWD/share/RabbitCommon.sh
    START_SCRIPT.path = $${PREFIX}/bin
    START_SCRIPT.CONFIG += directory no_check_exist
    
    # install icons
    icon128.target = icon128
    icon128.files = Src/Resource/image/App.png
    icon128.path = $${PREFIX}/share/pixmaps
    icon128.CONFIG = directory no_check_exist

    INSTALLS += DESKTOP_FILE START_SCRIPT icon128
}
