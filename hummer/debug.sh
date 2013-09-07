#!/bin/bash
echo "compile begin"
./configure --prefix=/home/zhengyang/local
#make clean
#make
make install
#cp -f ./src/core/worker /home/zhengyang/local/bin/
#cp -f ./src/core/master /home/zhengyang/local/bin/
echo "install finish"
