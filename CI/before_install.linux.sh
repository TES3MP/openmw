#!/bin/sh

# Set compiler
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 60
sudo update-alternatives --set g++ /usr/bin/g++-6
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60
sudo update-alternatives --set gcc /usr/bin/gcc-6

sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-5.0 50
sudo update-alternatives --set clang++ /usr/bin/clang++-5.0
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-5.0 50
sudo update-alternatives --set clang /usr/bin/clang-5.0

# build libgtest & libgtest_main
sudo mkdir /usr/src/gtest/build
cd /usr/src/gtest/build
sudo cmake .. -DBUILD_SHARED_LIBS=1
sudo make -j4
sudo ln -s /usr/src/gtest/build/libgtest.so /usr/lib/libgtest.so
sudo ln -s /usr/src/gtest/build/libgtest_main.so /usr/lib/libgtest_main.so

# Build RakNet
cd ~/
git clone https://github.com/TES3MP/RakNet
cd RakNet
cmake . -DRAKNET_ENABLE_DLL=OFF -DRAKNET_ENABLE_SAMPLES=OFF -DCMAKE_BUILD_TYPE=Release
make -j3
cd ..

