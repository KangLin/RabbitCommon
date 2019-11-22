#!/bin/bash

# Use to install appimage in linux
# $1: install: install program
#     install_run: install and run program
#     install_auto_run: install and run program and set auto run on boot 
#     remove
# $2: program name

PROGRAM_NAME=$2
if [ -z "$PROGRAM_NAME" ]; then
    echo "$0 install/install_run/install_auto_run application_name"
    exit -1
fi

function install_auto_run()
{
    echo "install auto run when boot ......"
    # Install auto run on boot
    if [ ! -d ~/.config/autostart ]; then
        mkdir -p ~/.config/autostart
        chmod a+wr ~/.config/autostart
    fi
    if [ -f ~/.config/autostart/${PROGRAM_NAME}.desktop ]; then
        rm ~/.config/autostart/${PROGRAM_NAME}.desktop
    fi
    ln -s `pwd`/share/applications/${PROGRAM_NAME}.desktop ~/.config/autostart/${PROGRAM_NAME}.desktop
}

function install()
{
    echo "install ${PROGRAM_NAME}-x86_64.AppImage ......"
    # Install destop
    if [ -f /usr/share/applications/${PROGRAM_NAME}.desktop ]; then
        rm /usr/share/applications/${PROGRAM_NAME}.desktop
    fi
    ln -s `pwd`/share/applications/${PROGRAM_NAME}.desktop /usr/share/applications/${PROGRAM_NAME}.desktop
    
    # Reset exec to desktop
    sed -i "s/Exec=.*//g" `pwd`/share/applications/${PROGRAM_NAME}.desktop
    echo "Exec=`pwd`/${PROGRAM_NAME}-x86_64.AppImage" >> `pwd`/share/applications/${PROGRAM_NAME}.desktop
    
    # Install applications icon
    if [ -f /usr/share/pixmaps/${PROGRAM_NAME}.png ]; then
        rm /usr/share/pixmaps/${PROGRAM_NAME}.png
    fi
    if [ ! -d /usr/share/pixmaps ]; then
        mkdir -p /usr/share/pixmaps
    fi
    ln -s `pwd`/share/pixmaps/${PROGRAM_NAME}.png /usr/share/pixmaps/${PROGRAM_NAME}.png
}

function run()
{
    echo "run ${PROGRAM_NAME}-x86_64.AppImage ......"
    id=`ps -ef |grep ${PROGRAM_NAME}-x86_64.AppImage|grep -v grep | awk '{print $2}'`
    ./${PROGRAM_NAME}-x86_64.AppImage
    if [ -n "$id" ]; then
        kill -9 $id
    fi
}

case "$1" in
    remove)
        echo "remove ......"
        rm -f /usr/share/applications/${PROGRAM_NAME}.desktop
        rm -f ~/.config/autostart/${PROGRAM_NAME}.desktop
        rm -f /usr/share/pixmaps/${PROGRAM_NAME}.png
        ;;
    install-run)
        install
        run
        ;;
    install_auto_run)
        install
        install_auto_run
        run
        ;;
    install|*)
        install
        ;;
esac
