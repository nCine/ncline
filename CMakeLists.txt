cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BINARY_DIR ${CMAKE_BINARY_DIR} DIRECTORY)

project(ncline)

set(TARGET_NAME ncline)
add_executable(${TARGET_NAME})

include(get_version)
#include(toml11)
#target_include_directories(${TARGET_NAME} PRIVATE ${TOML11_SOURCE_DIR})
include(cpptoml)
target_include_directories(${TARGET_NAME} PRIVATE ${CPPTOML_SOURCE_DIR}/include)
include(clipp)
target_include_directories(${TARGET_NAME} PRIVATE ${CLIPP_SOURCE_DIR}/include)

include(generated_sources)
target_sources(${TARGET_NAME} PRIVATE ${GENERATED_SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE ${PACKAGE_INCLUDE_DIRS})
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(${TARGET_NAME} PRIVATE ${GENERATED_INCLUDE_DIR})
endif()

if(MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE /MP)
endif()

target_sources(${TARGET_NAME} PRIVATE
	src/main.cpp
	src/Settings.h
	src/Settings.cpp
	src/Configuration.h
	src/Configuration.cpp
	src/Process.h
	src/Process.cpp
	src/FileSystem.h
	src/FileSystem.cpp
	src/GitCommand.h
	src/GitCommand.cpp
	src/CMakeCommand.h
	src/CMakeCommand.cpp
	src/DownloadMode.h
	src/DownloadMode.cpp
	src/ConfMode.h
	src/ConfMode.cpp
	src/BuildMode.h
	src/BuildMode.cpp
)
