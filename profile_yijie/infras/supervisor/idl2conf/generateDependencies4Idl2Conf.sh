#!/bin/bash -x

export BOOST_ROOT=/home/linuxbrew/.linuxbrew/Cellar/boost/1.60.0_2

cd /tmp/
rm -rf mstch
git clone https://github.com/no1msd/mstch.git
cd mstch
git checkout 1.0.2
mkdir build
cd build
cmake .. 
make
