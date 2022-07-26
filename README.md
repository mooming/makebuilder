# makebuilder

This program is developed to help build a c++ project on various platforms.<br>
It helps to make a CMake file based on its directory structure.<br>
It working on with speical .txt file specifiers as descibed belows.<br>
We shall call a directory as a module in the remained document. <br>


# Module Specifier

* executable.txt - It describes the current module is a executable module having "main()" entry-point.
* static_library.txt - It denotes the current module should be compiled and linked as a static library.
* shared_library.txt - It denotes the current module should be compiled and linked as a shared library.
* include.txt - This directory path shall be set as an include path of its children and itself.
* ignore.txt - This directory shall be ignored and not processed as a module.


# Module Property Specifier

* dependencies.txt - Each line denotes a module dependency. 
* libraries.txt - Each line denotes a required library.
* definitions.txt - Each line passes to a compiler as a command line argument


# MakeBuild.config

MakeBuild.config file defines global properties.<br>

* requiredCMakeVersion - It defines required CMake version. (default: 3.12)
* cxxStandard - It defines C++ standard version. (default: 17)
* compileOptions - It defines options passing to the compiler(non-MSVC). (default: -Wall -Werror)
* msvcCompileOptions - Compiler options for MSVC. (default: /W4 /WX)
* precompileDefinitions - Pre-compile definitions.
