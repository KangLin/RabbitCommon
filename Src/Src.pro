TARGET = RabbitCommon
TEMPLATE = lib

isEmpty(DESTDIR): DESTDIR = $$OUT_PWD/../bin
message("RabbitCommon:DESTDIR:$$DESTDIR")

CONFIG(staticlib): CONFIG*=static

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
    RabbitCommon_VERSION="v1.0.11"
}
message("RabbitCommon RabbitCommon_VERSION:$$RabbitCommon_VERSION")
DEFINES += RabbitCommon_VERSION=\"\\\"$$quote($$RabbitCommon_VERSION)\\\"\"

isEmpty(PREFIX) : !isEmpty(INSTALL_ROOT) : PREFIX=$$INSTALL_ROOT
isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : android : PREFIX = /.
    else : PREFIX = $$OUT_PWD/../install
}

include(RabbitCommon.pri)

# Default rules for deployment.
unix: !android: target.path = $${PREFIX}/lib
else: target.path = $${PREFIX}/bin
INSTALLS += target

header_files.target = header_files
header_files.files = $$INSTALL_HEADERS
header_files.path = $$system_path($${PREFIX}/include/RabbitCommon)
!android: INSTALLS += header_files

style_files.target = style_files
style_files.files = $$PWD/Resource/style/*
android: style_files.path = $$system_path($${PREFIX}/assets/data/style)
else: style_files.path = $$system_path($${PREFIX}/data/style)
INSTALLS += style_files

OTHER_FILES += $$PWD/CMakeLists.txt \
    $$PWD/../Install/* \
    $$PWD/Resource/style/*

!CONFIG(static): win32 {
    #message("Install qt library in win32")
    INSTALL_TARGET = $$system_path($${DESTDIR}/$(TARGET))

    Deployment_qtlib.target = Deployment_qtlib
    #Deployment_qtlib.files = $$system_path($${DESTDIR}/)  #把目录 DESTDIR 安装到下面 path 中,注意要加 / ,否则不包括目录，只复制目录中的文件
    Deployment_qtlib.path = $$system_path($${PREFIX})
    Deployment_qtlib.commands = "$$system_path($$[QT_INSTALL_BINS]/windeployqt)" \
                    --compiler-runtime \
                    --verbose 7 \
                    "$${INSTALL_TARGET}"
    INSTALLS += Deployment_qtlib
}
