#!/bin/bash

if [ -n $1 ]; then
	Qt6_ROOT=$1
fi

mkdir -p build_android
cd build_android
${Qt6_ROOT}/bin/qt-cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DQT_HOST_PATH=${Qt6_ROOT}/../gcc_64 \
        -DQt6LinguistTools_DIR=${Qt6_ROOT}/../gcc_64/lib/cmake/Qt6LinguistTools \
        -DCMAKE_INSTALL_PREFIX=`pwd`/install \
        -DANDROID_PLATFORM=android-35 \
        -DBUILD_TEST=OFF \
        -DBUILD_APP=OFF

cmake --build . -j $(nproc)
