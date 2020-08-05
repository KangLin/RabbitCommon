#!/bin/bash 
#下载工具  

#
# DOWNLOAD_QT:
#     APT: Use apt-get install qt
#     TRUE: install qt from download.qt.io/official_releases/qt
#     FALSE: Use apt-get install qt from https://launchpad.net/~beineri
#

set -e
SOURCE_DIR="`pwd`"
echo $SOURCE_DIR
TOOLS_DIR=${SOURCE_DIR}/Tools
PACKAGE_DIR=${SOURCE_DIR}/Package
ThirdLibs_DIR=${TOOLS_DIR}/ThirdLibs

if [ ! -d "${TOOLS_DIR}" ]; then
    mkdir ${TOOLS_DIR}
fi
if [ ! -d "$PACKAGE_DIR" ]; then
    mkdir -p $PACKAGE_DIR
fi

function function_install_yasm()
{
    #安装 yasm
    mkdir -p ${TOOLS_DIR}/src
    cd ${TOOLS_DIR}/src
    wget -c -nv http://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz 
    tar xzf yasm-1.3.0.tar.gz
    cd yasm-1.3.0/
    ./configure > /dev/null && sudo make install -j2 > /dev/null
    cd ${SOURCE_DIR}
}

function function_common()
{
    cd ${TOOLS_DIR}
    #下载最新cmake程序
#    if [ "cmake" = "${QMAKE}" ]; then
#        if [ ! -d "`pwd`/cmake" ]; then
#            wget -nv --no-check-certificate http://www.cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
#            tar xzf cmake-3.6.1-Linux-x86_64.tar.gz
#            mv cmake-3.6.1-Linux-x86_64 cmake
#        fi
#    fi
    
    # Download third libraries
    if [ -n "$DOWNLOAD_THIRDLIBS_URL" ]; then
        if [ ! -d ${ThirdLibs_DIR} ]; then
            mkdir -p ${ThirdLibs_DIR}
        fi
        cd ${ThirdLibs_DIR}
        ThirdLibs_File=third_libs.tar.gz
        wget -c -nv --no-check-certificate $DOWNLOAD_THIRDLIBS_URL -O $ThirdLibs_File
        tar xzf $ThirdLibs_File
    fi
    
    # Qt qt安装参见：https://github.com/benlau/qtci  
    cd ${TOOLS_DIR}
    if [ "$DOWNLOAD_QT" = "TRUE" ]; then
        QT_DIR=${TOOLS_DIR}/Qt/${QT_VERSION}
        cd ${PACKAGE_DIR}
        if [ ! -d "${QT_DIR}" ]; then
            if [ "${QT_VERSION}" = "5.6.3" ]; then
                if [ ! -f qt-opensource-linux-x64-android-${QT_VERSION}.run ]; then
                    wget -c --no-check-certificate -nv http://download.qt.io/official_releases/qt/${QT_VERSION_DIR}/${QT_VERSION}/qt-opensource-linux-x64-android-${QT_VERSION}.run
                fi
                bash ${SOURCE_DIR}/ci/qt-installer.sh qt-opensource-linux-x64-android-${QT_VERSION}.run ${QT_DIR}
                #rm qt-opensource-linux-x64-android-${QT_VERSION}.run
            else
                if [ ! -f qt-opensource-linux-x64-${QT_VERSION}.run ]; then
                    wget -c --no-check-certificate -nv http://download.qt.io/official_releases/qt/${QT_VERSION_DIR}/${QT_VERSION}/qt-opensource-linux-x64-${QT_VERSION}.run
                fi
                bash ${SOURCE_DIR}/ci/qt-installer.sh qt-opensource-linux-x64-${QT_VERSION}.run ${QT_DIR}
                #rm qt-opensource-linux-x64-${QT_VERSION}.run
            fi
        fi
    fi
    cd ${SOURCE_DIR}
}

function install_android()
{
    cd ${TOOLS_DIR}
    if [ ! -d "`pwd`/android-sdk" ]; then
        cd ${PACKAGE_DIR}
        ANDROID_STUDIO_VERSION=191.5900203
        if [ ! -f android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz ]; then
            wget -c -nv https://dl.google.com/dl/android/studio/ide-zips/3.5.1.0/android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz
        fi
        cp android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz ${TOOLS_DIR}/.
        cd ${TOOLS_DIR}
        tar xzf android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz
        export JAVA_HOME=`pwd`/android-studio/jre
        export PATH=${JAVA_HOME}/bin:$PATH
        wget -c -nv https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip
        mkdir android-sdk
        cd android-sdk
        cp ../sdk-tools-linux-4333796.zip .
        unzip -q sdk-tools-linux-4333796.zip
        echo "Install sdk and ndk ......"
        if [ -n "${ANDROID_PLATFORM}" ]; then
            PLATFORMS="platforms;${ANDROID_PLATFORM}"
        else
            PLATFORMS="platforms"
        fi
        if [ -z "${BUILD_TOOS_VERSION}" ]; then
            BUILD_TOOS_VERSION="28.0.3"
        fi
        (sleep 5 ; num=0 ; while [ $num -le 5 ] ; do sleep 1 ; num=$(($num+1)) ; printf 'y\r\n' ; done ) \
        | ./tools/bin/sdkmanager "platform-tools" "build-tools;${BUILD_TOOS_VERSION}" "${PLATFORMS}" "ndk-bundle"
        if [ ! -d ${TOOLS_DIR}/android-ndk ]; then
            ln -s ${TOOLS_DIR}/android-sdk/ndk-bundle ${TOOLS_DIR}/android-ndk
        fi
    fi
}

function function_android()
{
    cd ${TOOLS_DIR}
    
    sudo apt-get update -y -qq
    #sudo apt-get install -qq -y openjdk-11-jdk
    # install oracle jdk
    #sudo add-apt-repository ppa:linuxuprising/java -y
    #sudo apt update
    #(sleep 5 ; while true ; do sleep 1 ; printf '\r\n' ; done ) | sudo apt install oracle-java11-installer -qq -y
    
    #sudo apt install oracle-java11-set-default -qq -y
    #sudo apt-get install ant -qq -y
    sudo apt-get install libicu-dev -qq -y
    sudo apt-get install -qq -y libxkbcommon-x11-dev libglu1-mesa-dev

    function_common
    install_android

    cd ${SOURCE_DIR}
}

function function_unix()
{
    #汇编工具yasm
    #function_install_yasm

    if [ "$DOWNLOAD_QT" != "TRUE" -a "$DOWNLOAD_QT" != "APT" ]; then
        #See: https://launchpad.net/~beineri
        sudo add-apt-repository ppa:beineri/opt-qt-${QT_VERSION}-`lsb_release -c|awk '{print $2}'` -y
    fi

    sudo apt-get update -y -qq
    sudo apt-get install debhelper fakeroot -y -qq
    sudo apt-get install -y -qq libglu1-mesa-dev \
        libxkbcommon-x11-dev

    if [ "$DOWNLOAD_QT" = "APT" ]; then
        sudo apt-get install -y -qq qttools5-dev qttools5-dev-tools \
            qtbase5-dev qtbase5-dev-tools
        sudo ln -s /usr/lib/`uname -m`-linux-gnu/cmake /usr/lib/`uname -m`-linux-gnu/qt5/cmake 
    elif [ "$DOWNLOAD_QT" != "TRUE" ]; then
        sudo apt-get install -y -qq qt${QT_VERSION_DIR}base \
            qt${QT_VERSION_DIR}tools
        sed -i "s/export QT_VERSION=/export QT_VERSION=${QT_VERSION}/g" ${SOURCE_DIR}/debian/preinst
        sed -i "s/qt59/qt${QT_VERSION_DIR}/g" ${SOURCE_DIR}/debian/postinst
    fi
    function_common

    cd ${SOURCE_DIR}
}

function function_mingw()
{
    #汇编工具yasm
    #function_install_yasm
    cd ${SOURCE_DIR}
    if [ "true" == "$RABBIT_BUILD_THIRDLIBRARY" ]; then
        export RABBIT_BUILD_CROSS_HOST=i686-w64-mingw32 #i586-mingw32msvc
    fi

    function_common
    cd ${SOURCE_DIR}
}

case ${BUILD_TARGERT} in
    android)
        function_android
        ;;
    unix)
        function_unix
        ;;
    windows_mingw)
        function_mingw
        ;;
    *)
    echo "There aren't ${BUILD_TARGERT}"
        ;;
esac

cd ${SOURCE_DIR}
