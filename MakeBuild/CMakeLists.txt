cmake_minimum_required (VERSION 3.12)
project (makebuild)

set (CMAKE_CXX_STANDARD 17)

if (MSVC)
	add_compile_options (/W4 /WX)
else (MSVC)
	add_compile_options (-Wall -Werror)
endif (MSVC)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/Common
 )
link_directories (${CMAKE_SOURCE_DIR}/lib)


add_executable (makebuild
 Build.cpp
 CMakeFile.cpp
 ConfigFile.cpp
 MakeBuild.cpp
 Module.cpp
 Build.h
 CMakeFile.h
 ConfigFile.h
 Module.h
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

install (TARGETS makebuild DESTINATION ${CMAKE_SOURCE_DIR}/bin)


target_link_libraries (makebuild Common)

add_dependencies (makebuild Common)


