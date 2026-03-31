cmake_minimum_required (VERSION 3.12)
project (Common)

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
 )
link_directories (${CMAKE_SOURCE_DIR}/lib)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
add_library (Common STATIC 
 ConfigParser.cpp
 ConfigParser.cpp
 Directory.cpp
 Directory.cpp
 OSAbstractLayer.cpp
 OSAbstractLayer.cpp
 StringUtil.cpp
 StringUtil.cpp
 ConfigParser.h
 ConfigParser.h
 Directory.h
 Directory.h
 File.h
 File.h
 OSAbstractLayer.h
 OSAbstractLayer.h
 StringUtil.h
 StringUtil.h
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS Common DESTINATION ${CMAKE_SOURCE_DIR}/lib)



