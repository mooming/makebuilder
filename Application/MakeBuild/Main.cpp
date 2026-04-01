// Created by mooming.go@gmail.com 2016

#include <iostream>
#include <cstring>

#include "ProjectBuilder.h"
#include "DependencyGraph.h"
#include "OSAbstractLayer.h"
#include "TestRunner.h"


int main(int argc, const char* argv[])
{
    using namespace std;

    constexpr const char* version = "1.0.3";

    if (argc < 2)
    {
        cout << "MakeBuild v" << version << endl;
        cout << "Directory-based meta-build system" << endl;
        cout << "Automatically generates CMakeLists.txt files" << endl << endl;

        cout << "Usage: mbuild <projects_root_path> <options>" << endl;
        cout << "> mbuild <projects_root_path>" << endl;
        cout << "> mbuild <projects_root_path> --graph <output file name>" << endl;
        cout << "> mbuild <projects_root_path> --test-run" << endl;
        cout << endl;

        cout << "Options:" << endl;
        cout << "  --graph <output.dot>  Generate DOT dependency graph" << endl << endl;
        cout << "  --test-run          Run test cases from TestCases directory (DEBUG)" << endl;

        cout << "== New Features (v" << version << ") ==" << endl;
        cout << "* Dependency Graph: Generate visual dependency graphs" << endl;
        cout << "* Header-Only Auto-Detect: Directories with headers but no .module.config" << endl;
        cout << "  are automatically detected as HeaderOnly modules" << endl << endl;

        cout << "== Project Configuration ==" << endl;
        cout << "Place .project.config in the project root directory." << endl << endl;

        cout << "  .project.config options:" << endl;
        cout << "    requiredCMakeVersion = <version>   (default: 3.12)" << endl;
        cout << "    cxxStandard = <standard>            (default: 17)" << endl;
        cout << "    compileOptions = <options>         (default: -Wall -Werror)" << endl;
        cout << "    msvcCompileOptions = <options>     (default: /W3 /WX)" << endl;
        cout << "    precompileDefinitions = <defs>     (optional)" << endl << endl;

        cout << "== Module Configuration ==" << endl;
        cout << "Place .module.config in each module directory." << endl;
        cout << "NOTE: Directories without .module.config containing only header files" << endl;
        cout << "      are automatically detected as HeaderOnly modules." << endl << endl;

        cout << "  .module.config options:" << endl;
        cout << "    name = \"ModuleName\"                (module identifier)" << endl;
        cout << "    buildType = <type>                 (see below)" << endl;
        cout << "    precompileDefinitions = <defs>     (optional)" << endl;
        cout << "    ignoreSubdirectories = <dirs>      (comma-separated dirs to skip)" << endl << endl;

        cout << "  Build types:" << endl;
        cout << "    None           - Not a module, but may contain modules" << endl;
        cout << "    Ignored         - Skip this directory" << endl;
        cout << "    HeaderOnly     - Header files only (auto-detected without .module.config)" << endl;
        cout << "    Executable      - Produces executable" << endl;
        cout << "    StaticLibrary   - Static library (.a/.lib)" << endl;
        cout << "    SharedLibrary   - Shared library (.so/.dll)" << endl;
        cout << "    ExternalLibraries - External library directory" << endl << endl;

        cout << "== Module Specifier Files ==" << endl;
        cout << "  include.txt         - Add directory to include paths" << endl;
        cout << "  dependencies.txt    - List module dependencies (one per line)" << endl;
        cout << "  libraries.txt       - List required libraries (one per line)" << endl;
        cout << "  frameworks.txt     - List required frameworks (macOS only)" << endl;
        cout << endl;

        return 0;
    }

    // Check for --test-run option
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--test-run") == 0)
        {
            std::string testDir = argv[1];
            std::string baseDir = OS::GetFullPath(argv[1]);
            std::string testPath = baseDir + "/TestCases";

            cout << "[Test] Running tests from: " << testPath.c_str() << endl;

            bool success = mb::TestRunner::RunTests(testPath, "build");

            return success ? 0 : 1;
        }
    }

    // Check for --graph option
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--graph") == 0)
        {
            if (i + 1 >= argc)
            {
                cerr << "Error: --graph requires an output file path" << endl;
                return 1;
            }

            const char* outputPath = argv[i + 1];

            mb::ProjectBuilder build(OS::GetFullPath(argv[1]).c_str());
            build.GenerateCMakeFiles();

            mb::DependencyGraph graph(build);
            graph.PrintToConsole();
            graph.GenerateDotFile(outputPath);

            return 0;
        }
    }

    mb::ProjectBuilder build(OS::GetFullPath(argv[1]).c_str());
    build.GenerateCMakeFiles();

    cout << endl;
    cout << "Done! Run 'cmake -B build -G \"Ninja Multi-Config\" -S . && cmake --build build --config Debug' to build Debug." << endl;
    cout << "       'cmake --build build --config Dev' for Dev (optimized with debug info)" << endl;
    cout << "       'cmake --build build --config Release' for Release (full optimization)" << endl;
    cout << endl;

    return 0;
}
