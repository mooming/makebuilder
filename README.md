# makebuilder

<p>
We have a few options to create a build scrip for our C/C++ projects. Make utility helps us manage C/C++ projects.
However, creating Makefile files is not a trivial job. It also requires some knowledge for the utility. CMake is
also good multiplatform utility supporting most of platforms and toolchains. But, it's also non-trivial to create
CMakeLists.txt for all the modules in a project.
</p>

<p>
Most of my C/C++ projects are already well-organized with their directory structures. I had to recreate them for IDE
or build utilities additionally. I've created this tool to get rid of that jobs by using directory structure. This
tool will create a CMake project based on directory structure of projects with a few setting files; .config and .txt.
</p>

Details are provided below.<br>
We'll denote a directory as a module in the remained document. <br>

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
