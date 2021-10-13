#!/bin/sh
BUILD_DIR=.
SYSTEM=`uname`

if [ $SYSTEM = "Linux" ]; then
    echo "Downloading Linux x86 to ARM cross compiler"
    URL="https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz"
    ARCHIVE="$BUILD_DIR/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz"
    DIR="$BUILD_DIR/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf"
    wget -O $ARCHIVE $URL
    tar xf $ARCHIVE
elif [ $SYSTEM = "Darwin" ]; then
    echo "Downloading Darwin x86 to ARM cross compiler"
else
    printf "Unsupported kernel name %s\n" $SYSTEM 
fi