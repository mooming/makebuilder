cmake_minimum_required (VERSION 2.6)
project (MakeBuild)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${ CMAKE_CXX_FLAGS } -Wall -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)
add_definitions (-std=c++14)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/Common
 )

link_directories (${CMAKE_SOURCE_DIR}/lib)



add_executable (MakeBuild
 Build.cpp
 CMakeFile.cpp
 MakeBuild.cpp
 ProjectDir.cpp
 Build.h
 CMakeFile.h
 ProjectDir.h
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS MakeBuild DESTINATION ${CMAKE_SOURCE_DIR}/bin)


target_link_libraries (MakeBuild Common)

add_dependencies (MakeBuild Common)


