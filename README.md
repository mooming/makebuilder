# makebuilder

This program is developed to help build a c++ project on various platforms.<br>
It helps to make a CMake file based on its directory structure.<br>
It works with own .config and .txt files. They are project/module specifiers as descibed belows.<br>
We're going to denote a directory as a module in the remained document. <br>
<hr>

# Config Files

* .project.config - It should be located the most root directory of the project. It defines project settings.
* .module.config -  It should be located for each included directory. A directory not having this shall be ignored.
<hr>

# .project.config

.project.config file defines global properties.<br>

* requiredCMakeVersion - It defines required CMake version. (default: 3.12)
* cxxStandard - It defines C++ standard version. (default: 17)
* compileOptions - It defines options passing to the compiler(non-MSVC). (default: -Wall -Werror)
* msvcCompileOptions - Compiler options for MSVC. (default: /W4 /WX)
* precompileDefinitions - Pre-compile definitions.
<hr>

# .module.config

.module.config file defines local properties.<br>
* name - It defines the name of the module.
* buildType - It defines the type of the module.
* precompileDefinitions - It defines module-specific precompile definitions.
<hr>

# Build Type

* Ignored - This module shall be ignored and not be parsed.
* HeaderOnly - This module has header files only.
* Executable - This module is an executable and has a source file include main() function. An executable file shall be generated.
* StaticLibrary - This module is a static library.
* SharedLibrary - This module is a shared library(dyanmic library).
<hr>

# Module Specifier

* include.txt - This directory path shall be set as an include path of its children and itself.
<hr>

# Module Property Specifier

* dependencies.txt - Each line denotes a module dependency. 
* libraries.txt - Each line denotes a required library.
<hr>
