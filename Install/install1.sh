#!/bin/bash

# Use to install appimage in linux
# $1: install: install program
#     install_autostart: install and set auto run on boot 
#     install_run: install and run program
#     install_autostart_run: install and run program and set auto run on boot 
#     start: start program
#     remove: remove program
# $2: program name

PROGRAM_NAME=$2
if [ -z "$PROGRAM_NAME" ]; then
    echo "$0 install/install_run/install_auto_run application_name"
    exit -1
fi

PROGRAM_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)

echo "pwd:$PROGRAM_DIR"
function install_autostart()
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
    ln -s ${PROGRAM_DIR}/share/applications/${PROGRAM_NAME}.desktop ~/.config/autostart/${PROGRAM_NAME}.desktop
}

function install()
{
    echo "install ${PROGRAM_NAME}-x86_64.AppImage ......"
    # Install destop
    if [ -f /usr/share/applications/${PROGRAM_NAME}.desktop ]; then
        rm /usr/share/applications/${PROGRAM_NAME}.desktop
    fi
    ln -s ${PROGRAM_DIR}/share/applications/${PROGRAM_NAME}.desktop /usr/share/applications/${PROGRAM_NAME}.desktop
    
    # Reset exec to desktop
    sed -i "s/Exec=.*//g" ${PROGRAM_DIR}/share/applications/${PROGRAM_NAME}.desktop
    echo "Exec=${PROGRAM_DIR}/${PROGRAM_NAME}-x86_64.AppImage" >> ${PROGRAM_DIR}/share/applications/${PROGRAM_NAME}.desktop
    
    # Install applications icon
    if [ -f /usr/share/pixmaps/${PROGRAM_NAME}.png ]; then
        rm /usr/share/pixmaps/${PROGRAM_NAME}.png
    fi
    if [ ! -d /usr/share/pixmaps ]; then
        mkdir -p /usr/share/pixmaps
    fi
    ln -s ${PROGRAM_DIR}/share/pixmaps/${PROGRAM_NAME}.png /usr/share/pixmaps/${PROGRAM_NAME}.png
}

function start()
{
    echo "start ${PROGRAM_NAME}-x86_64.AppImage ......"
    #id=`ps -ef |grep "${PROGRAM_NAME}-x86_64.AppImage"|grep -v grep | awk '{print $2}'`
    cd ${PROGRAM_DIR}
    ./${PROGRAM_NAME}-x86_64.AppImage
    #if [ -n "$id" ]; then
    #    kill -9 $id
    #fi
}

function stop()
{
    echo "stop ${PROGRAM_NAME}-x86_64.AppImage ......"
    id=`ps -ef |grep "${PROGRAM_NAME}-x86_64.AppImage"|grep -v grep | awk '{print $2}'`
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
    start)
        start
        ;;
    stop)
        stop
        ;;
    install_run)
        install
        start
        ;;
    install_autostart_run)
        install
        install_autostart
        start
        ;;
    install_autostart)
        install
        install_autostart
        ;;
    install|*)
        install
        ;;
esac
