brew install cmake
brew install xquartz

git submodule update --init

cd thirdparty/dcmtk
mkdir -p out
cd out

cmake -G"Xcode" ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug --parallel
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config Release --parallel

cd ..
cd ..
cd ..

mkdir -p out
cd out

cmake -G"Xcode" ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug --parallel
cmake --build . --config RelWithDebInfo --parallel
cmake --build . --config Release --parallel
