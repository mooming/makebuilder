cmake_minimum_required (VERSION 3.12)
project (MyModule)

set (CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "" FORCE)
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set (CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} -O1")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set (CMAKE_CXX_STANDARD 23)

if (APPLE)
    set(PLATFORM_SOURCES
    )
else()
    set(PLATFORM_SOURCES "")
endif()

if (MSVC)
	add_compile_options (/W4 /WX)
else (MSVC)
	add_compile_options (-Wall -Werror)
endif (MSVC)

include_directories ( ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/include
 ${CMAKE_SOURCE_DIR}/test_proj/MyModule
 ${CMAKE_SOURCE_DIR}/test_proj/MyModule/include1
 ${CMAKE_SOURCE_DIR}/test_proj/MyModule/../include
 )
add_subdirectory (src)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
add_library (MyModule STATIC 
${PLATFORM_SOURCES}
)

if (APPLE)
	include_directories ( /System/Library/Frameworks )
	set(MACOSX_BUNDLE_TRUE TRUE)
	set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif (APPLE)

find_library(COCOA_FRAMEWORK Cocoa)
if (COCOA_FRAMEWORK)
	target_link_libraries (MyModule ${COCOA_FRAMEWORK})
endif ()

install (TARGETS MyModule DESTINATION ${CMAKE_SOURCE_DIR}/lib)


target_link_libraries (MyModule -Wl,--start-group Core Math -Wl,--end-group foo bar)

add_dependencies (MyModule Core)
add_dependencies (MyModule Math)


message("Custom CMake snippet worked!")

