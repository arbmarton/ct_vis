#!/bin/bash

commit_hash=$(git rev-parse --short HEAD)
current_date=$(date +"%Y-%m-%d")
foldername="${current_date}_${commit_hash}"

./build_xcode.sh

mkdir deploy
cd deploy
mkdir $foldername
cd $foldername
mkdir bin
mkdir shaders
mkdir bin/executable
cd ../..
cp -r ./out/Release/* ./deploy/$foldername/bin
cp -r ./shaders/* ./deploy/$foldername/shaders
mv ./deploy/$foldername/bin/ct_vis ./deploy/$foldername/bin/executable

zip ./deploy/$foldername.zip ./deploy/$foldername
mv ./deploy/$foldername.zip ./deploy/$foldername