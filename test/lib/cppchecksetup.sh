#!/bin/bash


if [ "$1" = "CI" ]
then
    echo "If branch: CI = $1" # TEMP DEBUG
    command -v cppcheck > /dev/null 2>&1 || {
        echo "Error: CPPCheck is not installed on Travis"
        exit 1
    }
else 
    echo "Else branch: CI = $1" # TEMP DEBUG
    mkdir -p lib/cppcheck
    tar -xf lib/cppcheck.tar.gz -C lib/cppcheck --strip-components 1
    
    cd lib/cppcheck 
    make MATCHCOMPILER=yes FILESDIR=./ HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" 1> /dev/null
fi