cmake_minimum_required (VERSION 3.12)
project ("GraphApp")

set (CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set (CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} -O1")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set (CMAKE_CXX_STANDARD 23)

if (MSVC)
	add_compile_options (/W4 /WX)
else (MSVC)
	add_compile_options (-Wall -Werror)
endif (MSVC)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/Source
 ${CMAKE_SOURCE_DIR}/Source/Common
 ${CMAKE_SOURCE_DIR}/TestCases/06_dependency_graph/src
 ${CMAKE_SOURCE_DIR}/TestCases/07_external_libraries/TestSDK
 )
link_directories (${CMAKE_SOURCE_DIR}/lib)


add_executable ("GraphApp"
 main.cpp
 main.cpp
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS "GraphApp" DESTINATION ${CMAKE_SOURCE_DIR}/bin)


target_link_libraries ("GraphApp" GraphLib)

add_dependencies ("GraphApp" GraphLib)


