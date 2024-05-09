cmake_minimum_required(VERSION 3.9)
project(ct_vis)

set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_HEADERS

)

set(THIRDPARTY_SOURCES

)

set(HEADER_FILES
	include/BasicDraw.h
	include/ImageLoader.h
	include/ImageSet.h
	include/Test.h
)

set(SOURCE_FILES
	src/BasicDraw.cpp
	src/ImageLoader.cpp
	src/main.cpp
	src/Test.cpp
)

add_executable(${PROJECT_NAME}
	           ${HEADER_FILES}
			   ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
						   ${CMAKE_CURRENT_LIST_DIR}/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/config/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/ofstd/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/oflog/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmimgle/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmdata/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/cimg/
)

target_link_libraries(${PROJECT_NAME}
					  ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/dcmimgle.lib
					  ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/dcmdata.lib
					  ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/ofstd.lib
					  ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/oflog.lib
)


# Parallel compilation and C++17
if(MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus")
target_compile_options(${PROJECT_NAME} PRIVATE /std:c++20 /MP /W4 /wd4554 /WX)
endif(MSVC)

if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/bin/$<CONFIG>/dcmimgle.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/bin/$<CONFIG>/dcmdata.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/bin/$<CONFIG>/ofstd.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/bin/$<CONFIG>/oflog.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/bin/$<CONFIG>/oficonv.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		VERBATIM
	)
endif(WIN32)


# Clang format setup
# http://mariobadr.com/using-clang-format-to-enforce-style.html
find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(NOT CLANG_FORMAT_EXE)
	message(STATUS "clang-format not found.")
else()
	message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
	set(DO_CLANG_FORMAT "${CLANG_FORMAT_EXE}" "-i -style=file")
endif()

function(prepend var prefix)
	set(listVar "")
	
	foreach(f ${ARGN})
	  list(APPEND listVar "${prefix}/${f}")
	endforeach()
	
	set(${var} "${listVar}" PARENT_SCOPE)
endfunction()

if(CLANG_FORMAT_EXE)
	prepend(FILES_TO_FORMAT ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER_FILES} ${SOURCE_FILES} ${BASE_FILES} ${VR_FILES})
	
	add_custom_target(
		CLANG_FORMAT
		COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${FILES_TO_FORMAT}
	)
endif()
