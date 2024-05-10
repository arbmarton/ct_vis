#!/bin/bash

git submodule update --init

cd thirdparty/dcmtk
mkdir -p out
cd out

cmake -G"Visual Studio 17 2022" -DBUILD_SHARED_LIBS=ON ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug --parallel
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config Release --parallel

cd ../../..

mkdir -p out
cd out

cmake -G"Visual Studio 17 2022" ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug --parallel
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config Release --parallel
