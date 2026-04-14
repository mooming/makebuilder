cmake_minimum_required (VERSION 3.12)
project (OCApp)

set (CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set (CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} -O1")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set (CMAKE_CXX_STANDARD 17)

if (APPLE)
    set(PLATFORM_SOURCES
    )
else()
    set(PLATFORM_SOURCES "")
endif()

if (MSVC)
	add_compile_options (/W3 /WX)
else (MSVC)
	add_compile_options (-Wall -Werror)
endif (MSVC)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 )
link_directories (${CMAKE_SOURCE_DIR}/lib)


add_executable (OCApp
 main.cpp
${PLATFORM_SOURCES}
)

if (APPLE)
include_directories ( /System/Library/Frameworks )
endif (APPLE)

find_package (Cocoa)

if (COCOA_FOUND)
include_directories (${COCOA_INCLUDE_DIR})
target_link_libraries (OCApp ${COCOA_LIBRARIES})
endif ()

install (TARGETS OCApp DESTINATION ${CMAKE_SOURCE_DIR}/bin)


target_link_libraries (OCApp ObjCLib ObjCPPLib)

add_dependencies (OCApp ObjCLib)
add_dependencies (OCApp ObjCPPLib)


