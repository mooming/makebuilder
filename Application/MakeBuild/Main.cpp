// Created by mooming.go@gmail.com 2016

#include <iostream>

#include "ProjectBuilder.h"
#include "OSAbstractLayer.h"


int main(int argc, const char* argv[])
{
    using namespace std;

    if (argc < 2)
    {
        cout << "MakeBuild 1.0.2" << endl;
        cout << "Directory-based meta-build system" << endl;
        cout << "Automatically generates CMakeLists.txt files" << endl << endl;

        cout << "Usage: mbuild <projects_root_path>" << endl << endl;

        cout << "== Project Configuration ==" << endl;
        cout << "Place .project.config in the project root directory." << endl << endl;

        cout << "  .project.config options:" << endl;
        cout << "    requiredCMakeVersion = <version>   (default: 3.12)" << endl;
        cout << "    cxxStandard = <standard>            (default: 23)" << endl;
        cout << "    compileOptions = <options>         (default: -Wall -Werror)" << endl;
        cout << "    msvcCompileOptions = <options>     (default: /W4 /WX)" << endl;
        cout << "    precompileDefinitions = <defs>     (optional)" << endl << endl;

        cout << "== Module Configuration ==" << endl;
        cout << "Place .module.config in each module directory." << endl << endl;

        cout << "  .module.config options:" << endl;
        cout << "    name = \"ModuleName\"                (module identifier)" << endl;
        cout << "    buildType = <type>                 (see below)" << endl;
        cout << "    precompileDefinitions = <defs>     (optional)" << endl << endl;

        cout << "  Build types:" << endl;
        cout << "    None           - Not a module, but may contain modules" << endl;
        cout << "    Ignored         - Skip this directory" << endl;
        cout << "    HeaderOnly     - Header files only" << endl;
        cout << "    Executable      - Produces executable" << endl;
        cout << "    StaticLibrary   - Static library (.a/.lib)" << endl;
        cout << "    SharedLibrary   - Shared library (.so/.dll)" << endl;
        cout << "    ExternalLibraries - External library directory" << endl << endl;

        cout << "== Module Specifier Files ==" << endl;
        cout << "  include.txt         - Add directory to include paths" << endl;
        cout << "  dependencies.txt    - List module dependencies (one per line)" << endl;
        cout << "  libraries.txt       - List required libraries (one per line)" << endl;
        cout << "  frameworks.txt     - List required frameworks (macOS only)" << endl;

        return 0;
    }

    mb::ProjectBuilder build(OS::GetFullPath(argv[1]).c_str());
    build.GenerateCMakeFiles();

    cout << endl;
    cout << "Done! Run 'cmake -B build -S . && cmake --build build' to build." << endl;
    cout << endl;

    return 0;
}
