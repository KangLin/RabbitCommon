#!/bin/bash

if [ -f /usr/lib/`uname -m`-linux-gnu/qt5/bin/qmake -a -z "$QT_ROOT" ]; then
    QT_ROOT=/usr/lib/`uname -m`-linux-gnu/qt5
fi

if [ -n "$1" ]; then
	QT_ROOT=$1
fi

if [ ! -f /usr/bin/qmake -a -z "$QT_ROOT" ]; then
	echo "$0 QT_ROOT"
    exit -1
fi

export QT_ROOT=$QT_ROOT
dpkg-buildpackage -us -uc -b
