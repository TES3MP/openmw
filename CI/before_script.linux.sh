#!/bin/sh

free -m
mkdir build
cd build
export CODE_COVERAGE=1
export RAKNET_ROOT=~/RakNet
if [ "${CC}" = "clang" ]; then export CODE_COVERAGE=0; 

${ANALYZE} cmake .. \
         -DBUILD_WITH_CODE_COVERAGE=${CODE_COVERAGE} \
         -DBUILD_OPENMW_MP=ON \
         -DBUILD_OPENMW=ON \
         -DBUILD_MASTER=ON \
         -DBUILD_BROWSER=ON \
         -DBUILD_BSATOOL=OFF \
         -DBUILD_ESMTOOL=OFF \
         -DBUILD_ESSIMPORTER=OFF \
         -DBUILD_LAUNCHER=OFF \
         -DBUILD_MWINIIMPORTER=OFF \
         -DBUILD_MYGUI_PLUGIN=OFF \
         -DBUILD_OPENCS=OFF \
         -DBUILD_WIZARD=OFF \
         -DBUILD_UNITTESTS=1 \
         -DCMAKE_INSTALL_PREFIX=/usr \
         -DBINDIR=/usr/games \
         -DCMAKE_BUILD_TYPE="None" \
         -DUSE_SYSTEM_TINYXML=TRUE \

