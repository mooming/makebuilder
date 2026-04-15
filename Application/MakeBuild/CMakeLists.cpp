// Created by mooming.go@gmail.com 2016

#include "CMakeLists.h"

#include "StringUtil.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace mb;

#include "CMakeLists.h"

#include "StringUtil.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace mb;

namespace
{
void AddFrameworks(ostream& os, const string& projName, const vector<string>& frameworks)
{
    os << "if (APPLE)" << endl;
    os << "\tinclude_directories ( /System/Library/Frameworks )" << endl;
    os << "endif (APPLE)" << endl << endl;

    auto handleOpenGL = [](ostream& os, const string& projName)
    {
        os << "find_package (OpenGL)" << endl;
        os << "if (OPENGL_FOUND)" << endl;
        os << "\tinclude_directories (${OPENGL_INCLUDE_DIR})" << endl;
        os << "\ttarget_link_libraries (" << projName << " ${OPENGL_LIBRARIES})" << endl;
        os << "endif ()" << endl << endl;
    };

    auto handleVulkan = [](ostream& os, const string& projName)
    {
        os << "find_package(Vulkan COMPONENTS volk)" << endl;
        os << "if (VULKAN_FOUND)" << endl;
        os << "\ttarget_link_libraries(" << projName << "  PRIVATE Vulkan::Vulkan Vulkan::volk)" << endl;
        os << "endif ()" << endl << endl;
    };

    auto handleCocoa = [](ostream& os, const string& projName)
    {
        os << "find_library(COCOA_FRAMEWORK Cocoa)" << endl;
        os << "if (COCOA_FRAMEWORK)" << endl;
        os << "\ttarget_link_libraries (" << projName << " ${COCOA_FRAMEWORK})" << endl;
        os << "endif ()" << endl << endl;
    };

    using TMap = unordered_map<string, void (*)(ostream&, const string&)>;
    static const TMap frameworkHandlers = {
        {"opengl", handleOpenGL},
        {"vulkan", handleVulkan},
        { "cocoa", handleCocoa}
    };

    if (frameworks.empty())
    {
        return;
    }

    for (const auto& element : frameworks)
    {
        auto key = Util::ToLowerCase(element);

        auto it = frameworkHandlers.find(key);
        if (it == frameworkHandlers.end())
            continue;

        auto handler = it->second;
        if (handler == nullptr)
        {
            os << "message(WARNING " << "\"" << element << " is NOT supported. \")";
            continue;
        }

        handler(os, projName);
    }
}

void AddOptimizeLevel(ostream& os, const string& projName, const string& optimizeLevel)
{
    if (optimizeLevel.empty())
        return;

    os << "target_compile_options (" << projName << " PRIVATE -O" << optimizeLevel << ")" << endl;
}
} // anonymous namespace

namespace mb
{

CMakeGenerator::CMakeGenerator(const ProjectBuilder& build, const Module& module)
    : build(build),
      module(module)
{
}

void CMakeGenerator::Generate()
{
    const auto& buildConfig = build.config;
    const EBuildType buildType = module.GetBuildType();

    const char* basePath = "${CMAKE_SOURCE_DIR}";
    using namespace Util;

    string filePath(module.GetPath());
    string moduleName = module.GetName();

    filePath.append("/CMakeLists.txt");

    cout << "Creating: [" << moduleName << "] " << filePath.c_str() << "(" << BuildTypeToString(buildType) << ")"
         << endl;

    ofstream ofs(filePath.c_str(), ofstream::out);

    auto requiredCMakeVersion = buildConfig.GetValue("requiredCMakeVersion", "3.12");
    ofs << "cmake_minimum_required (VERSION " << requiredCMakeVersion << ")" << endl;
    ofs << "project (" << moduleName << ")" << endl;
    ofs << endl;

    ofs << "set (CMAKE_CONFIGURATION_TYPES \"Debug;Dev;Release\" CACHE STRING \"\" FORCE)" << endl;
    ofs << "if (NOT CMAKE_BUILD_TYPE)" << endl;
    ofs << "    set (CMAKE_BUILD_TYPE \"Release\" CACHE STRING \"Build type\" FORCE)" << endl;
    ofs << "endif ()" << endl;
    ofs << endl;
    ofs << "set (CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -g -O0\")" << endl;
    ofs << "set (CMAKE_CXX_FLAGS_DEV \"${CMAKE_CXX_FLAGS_DEBUG} -O1\")" << endl;
    ofs << "set (CMAKE_CXX_FLAGS_RELEASE \"${CMAKE_CXX_FLAGS_RELEASE} -O3\")" << endl;
    ofs << endl;

    auto cxxStandard = buildConfig.GetValue("cxxStandard", "17");
    ofs << "set (CMAKE_CXX_STANDARD " << cxxStandard << ")" << endl;
    ofs << endl;

    ofs << "if (APPLE)" << endl;
    ofs << "    set(PLATFORM_SOURCES" << endl;

    {
        auto& objCsrcFiles = module.GetObjectiveCSourceFiles();
        for (auto& objCsrcFile : objCsrcFiles)
        {
            ofs << "        " << objCsrcFile << endl;
        }
    }

    ofs << "    )" << endl;
    ofs << "else()" << endl;
    ofs << "    set(PLATFORM_SOURCES \"\")" << endl;
    ofs << "endif()" << endl;

    ofs << endl;
    ofs << "if (MSVC)" << endl;

    auto compileOptions = buildConfig.GetValue("compileOptions", "-Wall -Werror");
    auto msvcCompileOptions = buildConfig.GetValue("msvcCompileOptions", "/W3 /WX");

    if (!msvcCompileOptions.empty())
    {
        ofs << "\tadd_compile_options (";
        ofs << msvcCompileOptions;
        ofs << ")" << endl;
    }

    ofs << "else (MSVC)" << endl;

    if (!compileOptions.empty())
    {
        ofs << "\tadd_compile_options (";
        ofs << compileOptions;
        ofs << ")" << endl;
    }

    ofs << "endif (MSVC)" << endl;
    ofs << endl;

    auto precompileDefs = buildConfig.GetValue("precompileDefinitions");
    auto& moduleDefs = module.GetPrecompileDefinitions();
    if (precompileDefs.has_value() || !moduleDefs.empty())
    {
        ofs << "add_compile_definitions (";

        if (precompileDefs.has_value())
        {
            ofs << *precompileDefs << " ";
        }

        if (!moduleDefs.empty())
        {
            ofs << moduleDefs << " ";
        }

        ofs << ")" << endl;
    }

    ofs << "include_directories (";
    ofs << " " << basePath << endl;
    ofs << " " << basePath << "/include" << endl;

    for (const auto& element : build.includeDirs)
    {
        ofs << " " << TranslatePath(element) << endl;
    }

    for (const auto& incPath : module.GetIncludePaths())
    {
        ofs << " " << TranslatePath(module.GetPath() + "/" + incPath) << endl;
    }

    ofs << " )" << endl;

    if (buildType == EBuildType::Executable)
    {
        ofs << "link_directories (" << basePath << "/lib" << endl;
        auto& linkDirectories = module.GetLinkDirectories();
        for (auto& linkDirectory : linkDirectories)
        {
            ofs << "    " << TrimPath(module.GetPath() + '/' + linkDirectory) << endl;
        }

        ofs << ")" << endl;
        ofs << endl;
    }

    for (const auto& subModule : module.GetSubModules())
    {
        auto subModuleBuildType = subModule.GetBuildType();

        // Skip HeaderOnly modules in add_subdirectory
        // HeaderOnly modules are interface libraries that don't need to be
        // built separately - they're included via target_link_libraries.
        if (subModuleBuildType == EBuildType::HeaderOnly)
        {
            continue;
        }

        ofs << "add_subdirectory (" << PathToName(subModule.GetPath().c_str()) << ")" << endl;
    }

    ofs << endl;

    switch (buildType)
    {
    case EBuildType::Executable:
        ofs << "add_executable (" << moduleName.c_str() << endl;

        for (const auto& element : module.GetSourceFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        for (const auto& element : module.GetHeaderFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        ofs << "${PLATFORM_SOURCES}" << endl;

        ofs << ")" << endl << endl;

        AddFrameworks(ofs, moduleName, module.GetFrameworks());
        AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

        ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/bin)" << endl;
        break;

    case EBuildType::StaticLibrary:
        ofs << "set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY " << basePath << "/lib)" << endl;
        ofs << "add_library (" << moduleName.c_str() << " STATIC " << endl;

        for (const auto& element : module.GetSourceFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        for (const auto& element : module.GetHeaderFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        ofs << "${PLATFORM_SOURCES}" << endl;

        ofs << ")" << endl << endl;

        AddFrameworks(ofs, moduleName, module.GetFrameworks());
        AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

        ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/lib)" << endl;
        break;

    case EBuildType::SharedLibrary:
        ofs << "add_library (" << moduleName.c_str() << " SHARED " << endl;

        for (const auto& element : module.GetSourceFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        for (const auto& element : module.GetHeaderFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        ofs << "${PLATFORM_SOURCES}" << endl;

        ofs << ")" << endl << endl;

        AddFrameworks(ofs, moduleName, module.GetFrameworks());
        AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

        ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/bin)" << endl;
        break;

    case EBuildType::HeaderOnly:
        ofs << "add_library (" << moduleName.c_str() << " INTERFACE)" << endl;
        ofs << "target_sources (" << moduleName.c_str() << " INTERFACE " << endl;

        for (const auto& element : module.GetHeaderFiles())
        {
            ofs << " " << PathToName(element.GetPath().c_str()) << endl;
        }

        ofs << ")" << endl;
        break;

    case EBuildType::ExternalLibrary:
        ofs << "add_library (" << moduleName.c_str() << " INTERFACE)" << endl;
        ofs << "target_include_directories (" << moduleName.c_str() << " INTERFACE" << endl;

        for (const auto& incPath : module.GetIncludePaths())
        {
            ofs << " " << TranslatePath(module.GetPath() + "/" + incPath) << endl;
        }

        ofs << ")" << endl;
        break;

    default:
        break;
    };

    ofs << endl << endl;

    auto& dependencyList = module.GetDependencies();
    auto& libList = module.GetLibraries();
    auto& externalLibs = build.externalLibraries;

    // FIX 2026-04-03: Skip target_link_libraries for None type modules
    // Modules with None build type are just
    // path containers and shouldn't have target_link_libraries generated.
    // This prevents CMake errors like "Cannot specify link libraries for target
    // which is not built by this project."
    if (buildType == EBuildType::None)
    {
        // Don't generate target_link_libraries for placeholder modules
    }
    else if (!externalLibs.empty() || !dependencyList.empty() || !libList.empty())
    {
        bool isInterface = (buildType == EBuildType::HeaderOnly || buildType == EBuildType::ExternalLibrary);
        ofs << "target_link_libraries (" << moduleName;
        if (isInterface)
        {
            ofs << " INTERFACE";
        }

        // External libraries from library.txt in this module
        for (const auto& extLib : externalLibs)
        {
            if (extLib.empty())
                continue;

            ofs << " " << extLib;
        }

        for (const auto& dependency : dependencyList)
        {
            if (dependency.empty())
                continue;

            ofs << " " << dependency;
        }

        if (buildType != EBuildType::None)
        {
            for (const auto& library : libList)
            {
                if (library.empty())
                    continue;

                ofs << " " << library;
            }
        }

        ofs << ")" << endl << endl;

        for (const auto& dependency : dependencyList)
        {
            if (dependency.empty())
                continue;

            ofs << "add_dependencies (" << moduleName.c_str() << " " << dependency << ")" << endl;
        }
        ofs << endl;
    }

    // Add custom CMake lines
    auto& cmakeCustom = module.GetCustomCMake();
    if (!cmakeCustom.empty())
    {
        ofs << endl;

        for (const auto& element : cmakeCustom)
        {
            ofs << element << endl;
        }
    }

    ofs << endl;

    // flush and close
    ofs.close();
}

string CMakeGenerator::TranslatePath(string path)
{
    using namespace Util;
    path = TrimPath(path);

    if (StartsWithIgnoreCase(path, build.baseModule.GetPath()))
    {
        string newPath = string("${CMAKE_SOURCE_DIR}");
        newPath.append(path.substr(build.baseModule.GetPath().length()));
        return newPath;
    }

    return path;
}

} // namespace mb
