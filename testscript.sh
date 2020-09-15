#!/bin/bash

## Check that we are in the main guac folder
## how??

if [ $(basename $PWD) != guac ]; then
    echo "Please run testscript.sh from the guac root folder."
    exit
fi

for file in ./testfiles/originals/*
do
    sudo ./bin/guac --compress $file ./testfiles/compressed/$(basename $file)
    
    echo ./testfiles/compressed/$(basename $file)
done

for file in ./testfiles/compressed/*
do
    sudo ./bin/guac --decompress $file ./testfiles/decompressed/$(basename $file)
done

for file in ./testfiles/decompressed/*
do
    diff $file ./testfiles/originals/$(basename $file)
done
