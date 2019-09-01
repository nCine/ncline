# Download and unpack cpptoml at configure time
configure_file(cmake/cpptoml_download.in cpptoml-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cpptoml-download
)
if(result)
	message(STATUS "CMake step for cpptoml failed: ${result}")
	set(CPPTOML_ERROR TRUE)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cpptoml-download
)
if(result)
	message(STATUS "Build step for cpptoml failed: ${result}")
	set(CPPTOML_ERROR TRUE)
endif()

if(CPPTOML_ERROR)
	message(FATAL_ERROR "Cannot download cpptoml")
else()
	set(CPPTOML_SOURCE_DIR ${CMAKE_BINARY_DIR}/cpptoml-src)
endif()
