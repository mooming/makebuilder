cmake_minimum_required (VERSION 2.6)
project (Common)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${ CMAKE_CXX_FLAGS } -Wall -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)
add_definitions (-std=c++14)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/Common
 )

link_directories (${CMAKE_SOURCE_DIR}/lib)



add_library (Common STATIC 
 Directory.cpp
 OSAbstractLayer.cpp
 StringUtil.cpp
 Directory.h
 File.h
 OSAbstractLayer.h
 StringUtil.h
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS Common DESTINATION ${CMAKE_SOURCE_DIR}/lib)



