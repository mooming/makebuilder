// Created by mooming.go@gmail.com 2016

#include "CMakeLists.h"

#include "StringUtil.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace mb;

namespace
{
    void AddFrameworks(
        ostream& os, string projName, const vector<string>& frameworks)
    {
        os << "if (APPLE)" << endl;
        os << "include_directories ( /System/Library/Frameworks )" << endl;
        os << "endif (APPLE)" << endl << endl;

        if (!frameworks.empty())
        {
            for (const auto& element : frameworks)
            {
                if (Util::EqualsIgnoreCase(element, "opengl"))
                {
                    os << "find_package (OpenGL)" << endl << endl;

                    os << "if (OPENGL_FOUND)" << endl;
                    os << "include_directories (${OPENGL_INCLUDE_DIR})" << endl;
                    os << "target_link_libraries (" << projName
                       << " ${OPENGL_LIBRARIES})" << endl;
                    os << "endif ()" << endl << endl;
                }
                else if (Util::EqualsIgnoreCase(element, "cocoa"))
                {
                    os << "find_package (Cocoa)" << endl << endl;

                    os << "if (COCOA_FOUND)" << endl;
                    os << "include_directories (${COCOA_INCLUDE_DIR})" << endl;
                    os << "target_link_libraries (" << projName
                       << " ${COCOA_LIBRARIES})" << endl;
                    os << "endif ()" << endl << endl;
                }
                else
                {
                    os << "find_package (" << element << ")" << endl << endl;

                    os << "if (" << element << "_FOUND)" << endl;
                    os << "target_link_libraries (" << projName << " "
                       << element << ")" << endl;
                    os << "endif ()" << endl << endl;
                }
            }
        }
    }

    void AddOptimizeLevel(
        ostream& os, const string& projName, const string& optimizeLevel)
    {
        if (optimizeLevel.empty())
            return;

        os << "target_compile_options (" << projName << " PRIVATE -O"
           << optimizeLevel << ")" << endl;
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

        string filePath(module.path);
        string moduleName = module.GetName();

        filePath.append("/CMakeLists.txt");

        cout << "Creating: [" << moduleName << "] " << filePath.c_str() << "("
             << BuildTypeToString(buildType) << ")" << endl;

        ofstream ofs(filePath.c_str(), ofstream::out);

        auto requiredCMakeVersion =
            buildConfig.GetValue("requiredCMakeVersion", "3.12");
        ofs << "cmake_minimum_required (VERSION " << requiredCMakeVersion << ")"
            << endl;
        ofs << "project (" << moduleName << ")" << endl;
        ofs << endl;

        ofs << "set (CMAKE_CONFIGURATION_TYPES \"Debug;Dev;Release\" CACHE STRING \"\" FORCE)"
            << endl;
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
        ofs << "if (MSVC)" << endl;

        auto compileOptions =
            buildConfig.GetValue("compileOptions", "-Wall -Werror");
        auto msvcCompileOptions =
            buildConfig.GetValue("msvcCompileOptions", "/W3 /WX");

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

        ofs << " )" << endl;

        if (buildType != EBuildType::HeaderOnly)
        {
            ofs << "link_directories (" << basePath << "/lib)" << endl;
            ofs << endl;
        }

        for (const auto& subModule : module.GetSubModules())
        {
            auto subModuleBuildType = subModule.GetBuildType();
            if (!subModule.HasSourceFileRecursive() && subModuleBuildType != EBuildType::ExternalLibraries
                && subModuleBuildType != EBuildType::ExternalCMakeProject)
            {
                continue;
            }

            ofs << "add_subdirectory (" << PathToName(subModule.path.c_str()) << ")" << endl;
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

            ofs << ")" << endl << endl;

            AddFrameworks(ofs, moduleName, module.GetFrameworks());
            AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

            ofs << "install (TARGETS " << moduleName << " DESTINATION "
                << basePath << "/bin)" << endl;
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

            ofs << ")" << endl << endl;

            AddFrameworks(ofs, moduleName, module.GetFrameworks());
            AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

            ofs << "install (TARGETS " << moduleName << " DESTINATION "
                << basePath << "/lib)" << endl;
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

            ofs << ")" << endl << endl;

            AddFrameworks(ofs, moduleName, module.GetFrameworks());
            AddOptimizeLevel(ofs, moduleName, module.GetOptimizeLevel());

            ofs << "install (TARGETS " << moduleName << " DESTINATION "
                << basePath << "/bin)" << endl;
            break;

        case EBuildType::HeaderOnly:
            ofs << "add_library (" << moduleName.c_str() << " INTERFACE)"
                << endl;
            ofs << "target_sources (" << moduleName.c_str() << " INTERFACE "
                << endl;

            for (const auto& element : module.GetHeaderFiles())
            {
                ofs << " " << PathToName(element.GetPath().c_str()) << endl;
            }

            ofs << ")" << endl;
            break;

        default:
            break;
        };

        ofs << endl << endl;

        auto& dependencyList = module.GetDependencies();
        auto& libList = module.GetLibraries();

        if (!dependencyList.empty() || !libList.empty())
        {
            ofs << "target_link_libraries (" << moduleName;

            for (const auto& dependency : dependencyList)
            {
                if (dependency.empty())
                    continue;

                ofs << " " << dependency;
            }

            for (const auto& library : libList)
            {
                if (library.empty())
                    continue;

                ofs << " " << library;
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

        ofs << endl;
        ofs.close();
    }

    string CMakeGenerator::TranslatePath(string path)
    {
        using namespace Util;
        path = TrimPath(path);

        if (StartsWithIgnoreCase(path, build.baseModule.path))
        {
            string newPath = string("${CMAKE_SOURCE_DIR}");
            newPath.append(path.substr(build.baseModule.path.length()));
            return newPath;
        }

        return path;
    }

} // namespace mb
