#!/bin/bash

echo "make clean"
make clean

echo "make..."
#make case=$1
make case=macTest

#cp libD2D_protocol_shared.so /home/lgc/share/arm64/
#cd /home/lgc/share/arm64/
#sh build
#sh run
#./mac_test >log/$1.log 2>&1

