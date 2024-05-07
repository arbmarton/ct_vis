mkdir -p out
cd out

cmake -G"Visual Studio 17 2022" ${COMMON_CMAKE_CONFIG_PARAMS} ../
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --build . --config Release