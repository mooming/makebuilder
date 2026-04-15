// Created by mooming.go@gmail.com 2016

#include <iostream>
#include <cstring>

#include "ProjectBuilder.h"
#include "OSAbstractLayer.h"
#include "TestRunner.h"


int main(int argc, const char* argv[])
{
    using namespace std;

    if (argc < 2)
    {
        cout << "MakeBuild 1.0" << endl;
        cout << "Directory-based meta-build system" << endl;
        cout << "Automatically generates CMakeLists.txt files" << endl << endl;

        cout << "Usage: mbuild <projects_root_path> [options]" << endl << endl;

        cout << "Options:" << endl;
        cout << "  --test-run          Run test cases from Test directory" << endl << endl;

        cout << "== Project Configuration ==" << endl;
        cout << "Place .project.config in the project root directory." << endl << endl;

        cout << "  .project.config options:" << endl;
        cout << "    requiredCMakeVersion = <version>   (default: 3.12)" << endl;
        cout << "    cxxStandard = <standard>            (default: 17)" << endl;
        cout << "    compileOptions = <options>         (default: -Wall -Werror)" << endl;
        cout << "    msvcCompileOptions = <options>     (default: /W3 /WX)" << endl;
        cout << "    precompileDefinitions = <defs>     (optional)" << endl << endl;

        cout << "== Module Configuration ==" << endl;
        cout << "Place .module.config in each module directory." << endl << endl;

    cout << "  .module.config options:" << endl;
    cout << "    name = \"ModuleName\"                (module identifier)" << endl;
    cout << "    buildType = <type>                 (see below)" << endl;
    cout << "    precompileDefinitions = <defs>     (optional)" << endl;
    cout << "    optimizeLevel = <level>            (0-3, optional)" << endl;
    cout << "    ignoreSubdirectories = <dirs>      (space-separated list of subdirs to skip)" << endl << endl;

        cout << "  Build types:" << endl;
        cout << "    None           - Not a module, but may contain modules" << endl;
        cout << "    Ignored         - Skip this directory" << endl;
        cout << "    HeaderOnly     - Header files only (no compilation)" << endl;
        cout << "    Executable      - Produces executable" << endl;
        cout << "    StaticLibrary   - Static library (.a/.lib)" << endl;
        cout << "    SharedLibrary   - Shared library (.so/.dll)" << endl;
        cout << "    ExternalLibrary   - External library directory" << endl << endl;

        cout << "== Module Specifier Files ==" << endl;
        cout << "  include.txt        -  Add directory and its contents to include paths. Paths are relative to the file's directory." << endl;
        cout << "  dependency.txt     - List module dependencies (one per line)" << endl;
        cout << "  library.txt        - List required libraries (one per line)" << endl;
        cout << "  linkDirectory.txt  - List linker directories (one per line). Paths are relative to the file's directory." << endl;
        cout << "  framework.txt      - List required frameworks (OpenGL, Vulkan, Cocoa)" << endl;
        cout << "  customCMake.txt   - Its contents will be added to CMakeLists.txt for custom usage" << endl;

        return 0;
    }

    // Check for --test-run option
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--test-run") == 0)
        {
            std::string testDir = argv[1];
            std::string mbuildPath = OS::GetFullPath(argv[0]).c_str();

            size_t pos = mbuildPath.rfind("/build/");
            if (pos != std::string::npos)
            {
                mbuildPath = mbuildPath.substr(0, pos) + "/build/Application/MakeBuild/Release/makebuild";
            }

            std::string baseDir = OS::GetFullPath(argv[1]).c_str();
            std::string testPath = baseDir + "/TestCases";

            cout << "[Test] Running tests from: " << testPath.c_str() << endl;
            cout << "[Test] Using mbuild: " << mbuildPath.c_str() << endl;

            bool success = mb::TestRunner::RunTests(testPath, mbuildPath, "build");

            return success ? 0 : 1;
        }
    }

    mb::ProjectBuilder build(OS::GetFullPath(argv[1]).c_str());
    build.GenerateCMakeFiles();

    cout << endl;
    cout << "Done!" << endl << endl;

    cout << "Build Commands:" << endl;
    cout << "> cmake --fresh -B build -G \"Ninja Multi-Config\" -S ." << endl;
    cout << "> cmake --build build --config Debug" << endl;
    cout << "> cmake --build build --config Dev" << endl;
    cout << "> cmake --build build --config Release" << endl;
    cout << endl;

    return 0;
}
