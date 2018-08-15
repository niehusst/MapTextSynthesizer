#!/bin/bash

PATH_TO_MTS=/home/.../MapTextSynthesizer
BUILD_FOLDER=build
PATH_TO_VENV=/home/.../local

#start to configure cmake

mkdir -p ${PATH_TO_MTS}/${BUILD_FOLDER}
pushd ${PATH_TO_MTS}/${BUILD_FOLDER}

cmake -D CMAKE_INSTALL_PREFIX=${PATH_TO_VENV} \
    ${PATH_TO_MTS}

#make

make install
popd
