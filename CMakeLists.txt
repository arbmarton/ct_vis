cmake_minimum_required(VERSION 3.9)
project(ct_vis)

set(CMAKE_CXX_STANDARD 20)
set(GLM_TEST_ENABLE OFF)
if (MSVC)
    set_source_files_properties(thirdparty/imgui/backends/imgui_impl_opengl3.cpp PROPERTIES COMPILE_OPTIONS "/wd4701")
    set_source_files_properties("$ENV{OpenCV_DIR}/../../include/opencv2/core/types.hpp" PROPERTIES COMPILE_OPTIONS "/wd4127")
endif()

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-volatile")
    set(DCMTK_ENABLE_BUILTIN_DICTIONARY ON)
endif(APPLE)

if (APPLE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad_macos/)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/)
endif(APPLE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw-3.3/)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm/)

if (APPLE)
    find_package(X11)
    find_package(ZLIB)
    find_package(OpenGL REQUIRED)
    find_package(OpenCV REQUIRED)
endif(APPLE)

set(THIRDPARTY_HEADERS
	thirdparty/imgui/imconfig.h
	thirdparty/imgui/imgui.h
	thirdparty/imgui/imgui_internal.h
	thirdparty/imgui/imstb_rectpack.h
	thirdparty/imgui/imstb_textedit.h
	thirdparty/imgui/imstb_truetype.h
	thirdparty/imgui/backends/imgui_impl_glfw.h
	thirdparty/imgui/backends/imgui_impl_opengl3.h
)

set(THIRDPARTY_SOURCES
	thirdparty/imgui/imgui.cpp
	thirdparty/imgui/imgui_demo.cpp
	thirdparty/imgui/imgui_draw.cpp
	thirdparty/imgui/imgui_tables.cpp
	thirdparty/imgui/imgui_widgets.cpp
	thirdparty/imgui/backends/imgui_impl_glfw.cpp
	thirdparty/imgui/backends/imgui_impl_opengl3.cpp
)

set(HEADER_FILES
	include/Bank.h
	include/BasicDraw.h
	include/DataTransform.h
	include/Enums.h
	include/Framebuffer.h
	include/Globals.h
	include/ImageLoader.h
	include/ImageSet.h
	include/OpenGL.h
	include/OpenGLMutex.h
	include/Renderer.h
	include/Shader.h
	include/Utilities.h
	include/Viewport.h
)

set(SOURCE_FILES
	src/BasicDraw.cpp
	src/DataTransform.cpp
	src/ImageLoader.cpp
	src/ImageSet.cpp
	src/Framebuffer.cpp
	src/main.cpp
	src/OpenGLMutex.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Utilities.cpp
	src/Viewport.cpp
)

set(SHADER_FILES
	shaders/ct_viewport.vs
	shaders/ct_viewport.fs
	shaders/ct_viewport_postprocess.vs
	shaders/ct_viewport_postprocess.fs
	shaders/main_viewport.vs
	shaders/main_viewport.fs
)

add_executable(${PROJECT_NAME}
		       ${THIRDPARTY_HEADERS}
			   ${THIRDPARTY_SOURCES}
	           ${HEADER_FILES}
			   ${SOURCE_FILES}
			   ${SHADER_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
						   ${CMAKE_CURRENT_LIST_DIR}/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/config/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/ofstd/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/oflog/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmimgle/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/dcmdata/include/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/cimg/

						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm/glm/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw-3.3/include/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui/
						   ${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui/backends

						   $ENV{OpenCV_DIR}/../../include/
)

if (APPLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
                               "/opt/X11/include"
                                ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad_macos/include/
                                ${OpenCV_INCLUDE_DIRS}
    )
else()
    target_include_directories(${PROJECT_NAME} PUBLIC
                               ${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/include/
    )
endif(APPLE)

if (APPLE)
    target_link_libraries(${PROJECT_NAME}
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/libdcmimgle.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/libdcmdata.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/libofstd.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/liboficonv.a
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/liboflog.a
                          
                          ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glm/glm/$<CONFIG>/libglm_static.a
                          ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glfw-3.3/src/$<CONFIG>/libglfw3.a
                          ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glad_macos/$<CONFIG>/libglad.a
    )
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
else()
    target_link_libraries(${PROJECT_NAME}
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/dcmimgle.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/dcmdata.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/ofstd.lib
                          ${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/lib/$<CONFIG>/oflog.lib

						  ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glm/glm/$<CONFIG>/glm_static.lib
						  ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glfw-3.3/src/$<CONFIG>/glfw3.lib
					      ${CMAKE_CURRENT_LIST_DIR}/out/thirdparty/glad/$<CONFIG>/glad.lib
						  opengl32.dll

						  $ENV{OpenCV_DIR}/lib/opencv_world490$<$<CONFIG:Debug>:d>.lib
    )
endif()

source_group("shaders" FILES ${SHADER_FILES})
source_group("thirdparty" FILES ${THIRDPARTY_HEADERS})
source_group("thirdparty sources" FILES ${THIRDPARTY_SOURCES})

# Parallel compilation and C++17
if(MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus")
target_compile_options(${PROJECT_NAME} PRIVATE /std:c++20 /MP /W4 /wd4554 /WX)
endif(MSVC)

if(WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/bin/$<CONFIG>/dcmimgle.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/bin/$<CONFIG>/dcmdata.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/bin/$<CONFIG>/ofstd.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/bin/$<CONFIG>/oflog.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/thirdparty/dcmtk/build/bin/$<CONFIG>/oficonv.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "$ENV{OpenCV_DIR}/bin/opencv_world490$<$<CONFIG:Debug>:d>.dll" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
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
