cmake_minimum_required (VERSION 3.12)
project (Common)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${ CMAKE_CXX_FLAGS } -Wall -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)
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



