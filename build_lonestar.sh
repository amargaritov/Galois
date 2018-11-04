#!/bin/sh -e

# GCC-6 installation
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
sudo apt-get update
sudo apt-get install gcc-6 g++-6 -y
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6 && gcc -v

sudo apt-get install -y cmake3 make libnuma-dev

if [ -d ./build ]; then
    echo "./build exists!"
    exit
fi

mkdir ./build
cd ./build
cmake ..
cd lonestar/pagerank
make -j
cd ../bfs
make -j


