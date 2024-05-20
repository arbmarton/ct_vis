#!/bin/bash

commit_hash=$(git rev-parse --short HEAD)

./build_vs2022.sh

mkdir deploy
cd deploy
mkdir $commit_hash
cd $commit_hash
mkdir bin
cd ../..
cp -r ./out/Release/* ./deploy/$commit_hash/bin
cp -r ./shaders/ ./deploy/$commit_hash

7z a -tzip ./deploy/$commit_hash.zip ./deploy/$commit_hash
mv ./deploy/$commit_hash.zip ./deploy/$commit_hash