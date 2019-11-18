#!/bin/bash

mkdir -p lib/cppcheck
tar -xf lib/cppcheck.tar.gz -C lib/cppcheck --strip-components 1

cd lib/cppcheck 
make MATCHCOMPILER=yes FILESDIR=./ HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function"
