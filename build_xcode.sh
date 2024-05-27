brew install cmake
brew install xquartz
brew install opencv

git submodule update --init

cd thirdparty/dcmtk
mkdir -p build
cd build

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
