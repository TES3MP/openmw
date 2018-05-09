#!/bin/sh

free -m
mkdir build
cd build

export CODE_COVERAGE=0
export RAKNET_ROOT=~/RakNet
if [ ! -z "${ANALYZE}" ]; then
    CODE_COVERAGE=1
fi

${ANALYZE}cmake .. \
    -DBUILD_OPENMW_MP=ON \
    -DBUILD_WITH_CODE_COVERAGE=${CODE_COVERAGE} \
    -DBUILD_BSATOOL=OFF \
    -DBUILD_ESMTOOL=OFF \
    -DBUILD_ESSIMPORTER=OFF \
    -DBUILD_LAUNCHER=OFF \
    -DBUILD_MWINIIMPORTER=OFF \
    -DBUILD_MYGUI_PLUGIN=OFF \
    -DBUILD_OPENCS=OFF \
    -DBUILD_WIZARD=OFF \
    -DBUILD_BROWSER=OFF \
    -DBUILD_UNITTESTS=1 \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DBINDIR=/usr/games \
    -DCMAKE_BUILD_TYPE="None" \
    -DUSE_SYSTEM_TINYXML=TRUE \
    -DRakNet_LIBRARY_RELEASE=~/RakNet/lib/libRakNetLibStatic.a \
    -DRakNet_LIBRARY_DEBUG=~/RakNet/lib/libRakNetLibStatic.a
