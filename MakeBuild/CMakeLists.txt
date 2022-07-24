cmake_minimum_required (VERSION 3.1)
project (MakeBuild)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${ CMAKE_CXX_FLAGS } -Wall -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)
set (CMAKE_CXX_STANDARD 17)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/Common
 )

link_directories (${CMAKE_SOURCE_DIR}/lib)



add_executable (MakeBuild
 Build.cpp
 CMakeFile.cpp
 MakeBuild.cpp
 MakeBuildConfig.cpp
 ProjectDir.cpp
 Build.h
 CMakeFile.h
 MakeBuildConfig.h
 ProjectDir.h
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS MakeBuild DESTINATION ${CMAKE_SOURCE_DIR}/bin)


target_link_libraries (MakeBuild Common)

add_dependencies (MakeBuild Common)


