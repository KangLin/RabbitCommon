#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T09:59:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RabbitCommon
TEMPLATE = subdirs

lib.subdir = Src
App.depends = lib
CONFIG *= ordered
SUBDIRS = lib App

isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : android : PREFIX = /.
    else : unnix : PREFIX = /usr/local
    else : PREFIX = $$OUT_PWD/install
}

DISTFILES += Authors.md \
    Authors_zh_CN.md \
    ChangeLog.md \
    ChangeLog_zh_CN.md \
    License.md

other.files = $$DISTFILES
other.path = $$PREFIX
other.CONFIG += directory no_check_exist 
!android : INSTALLS += other

OTHER_FILES += CMakeLists.txt \
    .travis.yml \
    appveyor.yml \
    ci/* \
    tag.sh \
    README.md \
    cmake/* \
    debian/* \
    build_debpackage.sh
