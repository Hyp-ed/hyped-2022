#!/bin/bash

cd lib && mkdir -p googletest
tar -xf googletest.tar.gz -C googletest --strip-components 1
cd googletest && mkdir -p build
UNAME=$(uname)
COMPILER=""
if [ $UNAME == "Windows" ]; then 
  COMPILER="gnu"
else
   COMPILER="c"
fi
cd build && cmake -DCMAKE_CXX_COMPILER="g++" -DCMAKE_CXX_FLAGS="-std=$COMPILER++11" ..
make
