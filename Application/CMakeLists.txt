cmake_minimum_required (VERSION 3.12)
project (Application)

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

add_subdirectory (MakeBuild)
