cmake_minimum_required(VERSION 3.9)
project(ct_vis)

set(CMAKE_CXX_STANDARD 20)
set(GLM_TEST_ENABLE OFF)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw-3.3/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm/)

if (APPLE)
    find_package(X11)
    find_package(ZLIB)
endif(APPLE)

set(THIRDPARTY_HEADERS

)

set(THIRDPARTY_SOURCES

)

set(HEADER_FILES
	include/Bank.h
	include/BasicDraw.h
	include/Enums.h
	include/Framebuffer.h
	include/ImageLoader.h
	include/ImageSet.h
	include/OpenGL.h
	include/Renderer.h
	include/Shader.h
	include/Utilities.h
	include/Viewport.h
)

set(SOURCE_FILES
	src/BasicDraw.cpp
	src/ImageLoader.cpp
	src/Framebuffer.cpp
	src/main.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Utilities.cpp
	src/Viewport.cpp
)

set(SHADER_FILES
	shaders/ct_viewport.vs
	shaders/ct_viewport.fs
	shaders/main_viewport.vs
	shaders/main_viewport.fs
)

add_executable(${PROJECT_NAME}
	           ${HEADER_FILES}
			   ${SOURCE_FILES}
			   ${SHADER_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
						   ${CMAKE_CURRENT_LIST_DIR}/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/config/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/ofstd/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/oflog/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmimgle/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmdata/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/cimg/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm/glm/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw-3.3/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/include/
)

if (APPLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
                               "/opt/X11/include"
    )
endif(APPLE)

if (APPLE)
    target_link_libraries(${PROJECT_NAME}
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/libdcmimgle.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/libdcmdata.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/libofstd.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/liboficonv.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/liboflog.a
    )
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
else()
    target_link_libraries(${PROJECT_NAME}
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/dcmimgle.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/dcmdata.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/ofstd.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/out/lib/$<CONFIG>/oflog.lib

						  ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glm/glm/$<CONFIG>/glm_static.lib
						  ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glfw-3.3/src/$<CONFIG>/glfw3.lib
					      ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glad/$<CONFIG>/glad.lib
						  opengl32.dll
    )
endif()

source_group("shaders" FILES ${SHADER_FILES})

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
