#!/bin/bash


command -v cppcheck > /dev/null 2>&1
if [ $? -eq 1 ]
then
    echo "CPPCheck not found, unpacking from tar ..."
    mkdir -p test/lib/cppcheck
    tar -xf test/lib/cppcheck.tar.gz -C test/lib/cppcheck --strip-components 1

    cd test/lib/cppcheck 
    echo "Building CPPCheck Analyzer"
	echo "This might take a while..."
    make MATCHCOMPILER=yes FILESDIR=./ HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" 1> /dev/null
fi
