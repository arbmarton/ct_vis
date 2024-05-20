#!/bin/bash

commit_hash=$(git rev-parse --short HEAD)

./build_xcode.sh

mkdir deploy
cd deploy
mkdir $commit_hash
cd $commit_hash
mkdir bin
mkdir shaders
mkdir bin/executable
cd ../..
cp -r ./out/Release/* ./deploy/$commit_hash/bin
cp -r ./shaders/* ./deploy/$commit_hash/shaders
mv ./deploy/$commit_hash/bin/ct_vis ./deploy/$commit_hash/bin/executable

zip ./deploy/$commit_hash.zip ./deploy/$commit_hash
mv ./deploy/$commit_hash.zip ./deploy/$commit_hash