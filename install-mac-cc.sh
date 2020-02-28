#!/bin/bash
set -e

while true; do
    read -p $'Downloading the files will take a bit, make sure you have a decent internet connection.\nContinue?\n(yes/no) ' answer
    case $answer in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

echo "Downloading mac crosscompiler tarball..."
curl -L -O https://github.com/Hyp-ed/mac-crosscompiler/releases/download/v1.0/mac-crosscompiler.tar.gz

echo "Making new mac crosscompiler directory..."
mkdir mac-crosscompiler

echo "Extracting mac crosscompiler tarball..."
tar --strip-components 1 -C mac-crosscompiler -xpvf mac-crosscompiler.tar.gz

echo "Removing mac crosscompiler tarball..."
rm -f mac-crosscompiler.tar.gz

echo "Finished installing mac crosscompiler!"
