#!/bin/bash

echo "make clean"
make clean

echo "make..."
make case=$1

./mac_test >log/$1.log 2>&1

