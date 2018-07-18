#!/bin/bash

PATH_TO_MTS=/home/chenziwe/MapTextSynthesizer
BUILD_FOLDER=build
PATH_TO_VENV=/home/chenziwe/virtualenv/test4

#start to configure cmake

mkdir -p ${PATH_TO_MTS}/${BUILD_FOLDER}
pushd ${PATH_TO_MTS}/${BUILD_FOLDER}

cmake -D CMAKE_INSTALL_PREFIX=${PATH_TO_VENV}/local \
    ${PATH_TO_MTS}

#make

make install
popd
