#!/bin/bash
set -e

SOURCE_DIR=`pwd`

if [ -n "$1" ]; then
    VERSION=`git describe --tags`
    if [ -z "$VERSION" ]; then
        VERSION=` git rev-parse HEAD`
    fi
    
    echo "Current version: $VERSION, The version to will be set: $1"
    echo "Please check the follow list:"
    echo "    - Test is ok ?"
    echo "    - Translation is ok ?"
    echo "    - Setup file is ok ?"
    echo "    - Update_*.xml is ok ?"
    
    read -t 30 -p "Be sure to input Y, not input N: " INPUT
    if [ "$INPUT" != "Y" -a "$INPUT" != "y" ]; then
        exit 0
    fi
    git tag -a $1 -m "Release $1"
else
    echo "$0 version"
    exit -1
fi

VERSION=`git describe --tags`
if [ -z "$VERSION" ]; then
    VERSION=`git rev-parse --short HEAD`
fi

sed -i "s/^\SET(RabbitCommon_VERSION.*/\SET(RabbitCommon_VERSION \"${VERSION}\")/g" ${SOURCE_DIR}/CMakeLists.txt
sed -i "s/^\SET(RabbitCommon_VERSION.*/\SET(RabbitCommon_VERSION \"${VERSION}\")/g" ${SOURCE_DIR}/Src/CMakeLists.txt
sed -i "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/ci/build.sh
sed -i "s/^version: '.*{build}'/version: '${VERSION}.{build}'/g" ${SOURCE_DIR}/appveyor.yml
#sed -i "s/export VERSION=.*/export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/.travis.yml
sed -i "s/<VERSION>.*</<VERSION>${VERSION}</g" ${SOURCE_DIR}/Update/update.xml

sed -i "s/^\    RabbitCommon_VERSION=.*/\    RabbitCommon_VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/App/App.pro
sed -i "s/^\    RabbitCommon_VERSION=.*/\    RabbitCommon_VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/Src/Src.pro
sed -i "s/^\    RabbitCommon_VERSION=.*/\    RabbitCommon_VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/Tests/Tests.pro
sed -i "s/^\    RabbitCommon_VERSION=.*/\    RabbitCommon_VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/App/GenerateUpdateFile/GenerateUpdateFile.pro
#sed -i "s/^\  - export VERSION=.*/\  - export VERSION=\"${VERSION}\"/g" ${SOURCE_DIR}/.travis.yml

sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/msvc.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/build.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/doxygen.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/mingw.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/android.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${VERSION}/g" ${SOURCE_DIR}/.github/workflows/macos.yml

DEBIAN_VERSION=`echo ${VERSION}|cut -d "v" -f 2`
sed -i "s/^\Standards-Version:.*/\Standards-Version: \"${DEBIAN_VERSION}\"/g" ${SOURCE_DIR}/debian/control
sed -i "s/rabbitcommon (.*)/rabbitcommon (${DEBIAN_VERSION})/g" ${SOURCE_DIR}/debian/changelog
sed -i "s/Version=.*/Version=${DEBIAN_VERSION}/g" ${SOURCE_DIR}/share/org.Rabbit.RabbitCommon.desktop
sed -i "s/[0-9]\+\.[0-9]\+\.[0-9]\+/${DEBIAN_VERSION}/g" ${SOURCE_DIR}/README*.md
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/ubuntu.yml
sed -i "s/RabbitCommon_VERSION:.*/RabbitCommon_VERSION: ${DEBIAN_VERSION}/g" ${SOURCE_DIR}/.github/workflows/qmake.yml
if [ -f ${SOURCE_DIR}/vcpkg.json ]; then
    sed -i "s/\"version-string\":[0-9]\+\.[0-9]\+\.[0-9]\+\".*\"/\"version-string\":\"${DEBIAN_VERSION}\"/g" ${SOURCE_DIR}/vcpkg.json
fi

MAJOR_VERSION=`echo ${DEBIAN_VERSION}|cut -d "." -f 1`

if [ -n "$1" ]; then
    git add .
    git commit -m "Release $1"
    git tag -d $1
    git tag -a $1 -m "Release $1"
    git push
    git push origin $1
fi
