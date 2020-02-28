#!/bin/bash
set -e

while true; do
    read -p "Downloading the files will take a bit, make sure you have a fast internet connection. Continue? (yes/no) " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

echo "Downloading mac crosscompiler files..."
curl -L -O https://github.com/Hyp-ed/mac-crosscompiler/releases/download/v1.0/mac-crosscompiler.tar.gz

echo "Extracting mac crosscompiler archive..."
tar xvf mac-crosscompiler.tar.gz

echo "Finished installing mac crosscompiler!"
