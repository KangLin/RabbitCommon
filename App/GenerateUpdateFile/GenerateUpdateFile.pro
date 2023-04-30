QT *= core xml
QT -= gui

CONFIG *= c++11 console
CONFIG -= app_bundle

TARGET = GenerateUpdateFile
TEMPLATE = app

isEmpty(PREFIX) : !isEmpty(INSTALL_ROOT) : PREFIX=$$INSTALL_ROOT
isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : android : PREFIX = /.
    else : PREFIX = $$OUT_PWD/../../install
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
    RabbitCommon_VERSION="v1.0.12"
}
message("RabbitCommon_VERSION:$$RabbitCommon_VERSION")
DEFINES += RabbitCommon_VERSION=\"\\\"$$quote($$RabbitCommon_VERSION)\\\"\"
CONFIG(debug, debug|release): DEFINES *= _DEBUG
CONFIG(staticlib): CONFIG*=static
CONFIG(static): DEFINES *= RABBITCOMMON_STATIC_DEFINE

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

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#Support windows xp
mingw : DEFINES += "_WIN32_WINNT=0x0501" 
msvc {
    QMAKE_LFLAGS *= /SUBSYSTEM:WINDOWS",5.01"
    QMAKE_CXXFLAGS += "/utf-8"
}

include($$PWD/../../pri/Translations.pri)

#VERSION=$$RabbitCommon_VERSION
INCLUDEPATH+=$$_PRO_FILE_PWD_/../../Src $$_PRO_FILE_PWD_/../../Src/export
isEmpty(DESTDIR): DESTDIR = $$OUT_PWD/../../bin
DEPENDPATH = $$DESTDIR
LIBS *= "-L$$DESTDIR" -lRabbitCommon

SOURCES += \
        GenerateUpdateFile.cpp \
        main.cpp

# Default rules for deployment.
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

HEADERS += \
    GenerateUpdateFile.h
