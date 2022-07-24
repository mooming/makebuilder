# makebuilder

This program is developed to help build a c++ project on various platforms.
It helps to make a CMake file based on its directory structure.
It working on with speical .txt file specifiers as descibed belows.
We shall call a directory as a module in the remained document. 

Module Specifier

executable.txt - It describes the current module is a executable module having "main()" entry-point.
static_library.txt - It denotes the current module should be compiled and linked as a static library.
shared_library.txt - It denotes the current module should be compiled and linked as a shared library.
ignore.txt - This directory shall be ignored and not processed as a module.


Module Property Specifier

dependencies.txt - Each line denotes a module dependency. 
libraries.txt - Each line denotes a required library.
definitions.txt - Each line passes to a compiler as a command line argument
