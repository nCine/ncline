cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BINARY_DIR ${CMAKE_BINARY_DIR} DIRECTORY)

project(ncline)

set(TARGET_NAME ncline)
add_executable(${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)

include(get_version)
include(installation)
include(cpptoml)
target_include_directories(${TARGET_NAME} PRIVATE ${CPPTOML_SOURCE_DIR}/include)
include(clipp)
target_include_directories(${TARGET_NAME} PRIVATE ${CLIPP_SOURCE_DIR}/include)

include(generated_sources)
target_sources(${TARGET_NAME} PRIVATE ${GENERATED_SOURCES})
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
	src/DistMode.h
	src/DistMode.cpp
	src/Helpers.h
	src/Helpers.cpp
)
