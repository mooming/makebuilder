// Created by mooming.go@gmail.com 2016

#include "ProjectBuilder.h"

#include "CMakeLists.h"
#include "../Common/StringUtil.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

namespace
{

void GetAllChildrenRecursive(mb::Module& module, mb::ProjectBuilder::Modules& inoutModules)
{
    inoutModules.push_back(&module);

    auto& subList = module.GetSubModules();
    for (auto& sub : subList)
    {
        GetAllChildrenRecursive(sub, inoutModules);
    }
};

} // namespace

namespace mb
{

ProjectBuilder::ProjectBuilder(const char* path)
    : config(path, ".project.config"),
      baseModule(path),
      modules()
{
    TraverseDirectoryTree(baseModule, "");
    modules.shrink_to_fit();

    cout << endl;
    cout << "=== Project Directories Found ===" << endl;

    baseModule.PrintSubModules(" ");

    GetAllChildrenRecursive(baseModule, modules);

    int i = 0;
    for (auto element : modules)
    {
        assert(element != nullptr);

        cout << i++ << " : " << element->path << endl;
    }
}

void ProjectBuilder::GenerateCMakeFiles()
{
    for (auto module : modules)
    {
        assert(module != nullptr);

        using namespace mb;
        const auto moduleBuildType = module->GetBuildType();
        if (moduleBuildType == EBuildType::ExternalCMakeProject)
        {
            cout << "[CMake] Skip creating CMakeLists.txt of Module = " << module->GetName().c_str() << endl;
            continue;
        }

        CMakeGenerator generator(*this, *module);
        generator.Generate();
    }
}

// return true if the given module is valid, or it has a valid submodule.
bool ProjectBuilder::TraverseDirectoryTree(
    Module& module, const string& logHeader)
{
    if (module.GetBuildType() == EBuildType::Ignored)
        return false;

    // Fixed 2026-03-31: Previously had misleading indentation (extra 4 spaces).
    // With -Werror -Wmisleading-indentation, this would fail to compile.
    if (module.GetBuildType() == EBuildType::ExternalCMakeProject)
        return true;

    if (module.IsIncludePath())
    {
        includeDirs.push_back(module.path);
    }

    bool isValidModule = false;

    if (module.GetBuildType() == EBuildType::ExternalLibraries)
    {
        isValidModule = true;
    }

    // Always traverse children to find nested modules (including HeaderOnly)
    // regardless of whether this module has source files

    // FIX 2026-03-31: Support HeaderOnly modules in traversal
    // Previously, modules were only considered valid if they had source or header files.
    // Now we also consider HeaderOnly modules as valid, allowing them to be processed
    // and have their submodules discovered correctly.
    bool hasHeaderOrSource = !module.GetSourceFiles().empty() || !module.GetHeaderFiles().empty();
    
    if (hasHeaderOrSource || module.GetBuildType() == EBuildType::HeaderOnly)
    {
        cout << endl;
        cout << logHeader << "### Module = " << module.path << endl;

        if (!module.GetSourceFiles().empty())
        {
            cout << logHeader << "### Source Files" << endl;
            for (const auto& element : module.GetSourceFiles())
            {
                cout << logHeader << element << endl;
            }
        }

        if (!module.GetHeaderFiles().empty())
        {
            cout << endl;
            cout << logHeader << "### Header Files" << endl;
            for (const auto& element : module.GetHeaderFiles())
            {
                cout << logHeader << element << endl;
            }
        }

        isValidModule = true;
    }

    // For HeaderOnly modules that may not have source files but are explicitly marked
    if (module.GetBuildType() == EBuildType::HeaderOnly)
    {
        isValidModule = true;
    }

    // Always check for child modules - even if this module has no source files,
    // it may contain valid submodules (e.g., HeaderOnly directories)
    bool hasValidSubmodules = false;
    for (const auto& subDirectory : module.DirList())
    {
        // Check if this subdirectory should be ignored based on module config
        auto subDirName = Util::PathToName(subDirectory.path.c_str());
        if (module.ShouldIgnoreSubdirectory(subDirName))
        {
            cout << logHeader << "[Ignore] " << subDirectory.path << " (ignored by parent config)" << endl;
            continue;
        }

        Module submodule(&module, subDirectory);
        string childLogHeader = logHeader;
        childLogHeader.append("  ");

        if (TraverseDirectoryTree(submodule, childLogHeader))
        {
            auto& submoduleList = module.GetSubModules();
            submoduleList.push_back(submodule);
            hasValidSubmodules = true;
        }
    }

    if (hasValidSubmodules)
    {
        isValidModule = true;
    }

    auto& subModules = module.GetSubModules();
    sort(subModules.begin(), subModules.end());

    return isValidModule;
}

} // namespace mb
