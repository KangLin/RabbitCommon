#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T09:59:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
android: versionAtMost(QT_VERSION, 5.10.0): error("Qt must greate 5.10.0")

TARGET = RabbitCommon
TEMPLATE = subdirs

lib.subdir = Src
App.depends = lib
CONFIG *= ordered
SUBDIRS = lib App

isEmpty(PREFIX) {
    qnx : PREFIX = /tmp
    else : ios: PREFIX=/
    else : android : PREFIX = /
    else : unix : PREFIX = /opt/RabbitCommon
    else : PREFIX = $$OUT_PWD/install
}

DISTFILES += Authors.md \
    Authors_zh_CN.md \
    ChangeLog*.md \
    License.md

other.files = $$DISTFILES
android: other.path = $$PREFIX/assets
else: other.path = $$PREFIX
other.CONFIG += directory no_check_exist 
INSTALLS += other

OTHER_FILES += CMakeLists.txt \
    .travis.yml \
    appveyor.yml \
    ci/* \
    tag.sh \
    README*.md \
    cmake/* \
    debian/* \
    build_debpackage.sh \
    Install/*
