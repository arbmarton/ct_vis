#!/bin/bash

commit_hash=$(git rev-parse --short HEAD)
current_date=$(date +"%Y-%m-%d")
foldername="${current_date}_${commit_hash}"

./build_vs2022.sh

mkdir deploy
cd deploy
mkdir $foldername
cd $foldername
mkdir bin
cd ../..
cp -r ./out/Release/* ./deploy/$foldername/bin
cp -r ./shaders/ ./deploy/$foldername

7z a -tzip ./deploy/$foldername.zip ./deploy/$foldername
mv ./deploy/$foldername.zip ./deploy/$foldername