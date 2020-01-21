#!/bin/bash

mkdir -p lib/googletest
tar -xf lib/googletest.tar.gz -C lib/googletest --strip-components 1
cd lib/googletest && mkdir -p build
UNAME=$(uname)
COMPILER=""
if [ $UNAME == "Windows" ]; then 
  COMPILER="gnu"
else
   COMPILER="c"
fi
cd build && cmake -DCMAKE_CXX_COMPILER="g++" -DCMAKE_CXX_FLAGS="-std=$COMPILER++11" ..
make
