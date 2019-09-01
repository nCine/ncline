# Download and unpack clipp at configure time
configure_file(cmake/clipp_download.in clipp-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/clipp-download
)
if(result)
	message(STATUS "CMake step for clipp failed: ${result}")
	set(CLIPP_ERROR TRUE)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/clipp-download
)
if(result)
	message(STATUS "Build step for clipp failed: ${result}")
	set(CLIPP_ERROR TRUE)
endif()

if(CLIPP_ERROR)
	message(FATAL_ERROR "Cannot download clipp")
else()
	set(CLIPP_SOURCE_DIR ${CMAKE_BINARY_DIR}/clipp-src)
endif()
