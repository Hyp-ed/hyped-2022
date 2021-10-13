#!/bin/sh
BUILD_DIR=$(readlink -f .)
SYSTEM=`uname`

if [ $SYSTEM = "Linux" ]; then
    URL="https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz"
    ARCHIVE="$BUILD_DIR/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz"
    DIR="$BUILD_DIR/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf"
    printf "Downloading Linux x86 to ARM cross compiler to $BUILD_DIR..."
    wget -O $ARCHIVE $URL 2> /dev/null
    printf "DONE\nExtracting $ARCHIVE..."
    tar xf $ARCHIVE
    printf "DONE\n"
elif [ $SYSTEM = "Darwin" ]; then
    echo "Downloading Darwin x86 to ARM cross compiler"
else
    printf "Unsupported kernel name %s\n" $SYSTEM 
fi