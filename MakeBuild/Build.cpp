// Created by mooming.go@gmail.com 2016

#include "Build.h"

#include "CMakeFile.h"
#include "StringUtil.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

namespace Builder
{
namespace
{
void GetAllChildrenRecursive(Module& module, Build::Modules& inoutModules)
{
    inoutModules.push_back(&module);

    auto& subList = module.SubModuleList();
    for (auto& sub : subList)
    {
        GetAllChildrenRecursive(sub, inoutModules);
    }
};
} // namespace

Build::Build(const char* path)
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

void Build::BuildCMakeFiles()
{
    for (auto module : modules)
    {
        assert(module != nullptr);

        using namespace CMake;
        const auto moduleBuildType = module->GetBuildType();
        if (moduleBuildType == EBuildType::ExternalLibraries)
        {
            cout << "[CMake] Skip creating CMakeLists.txt of Module = "
                 << module->GetName().c_str() << endl;
            continue;
        }

        CMakeFile cmf(*this, *module);
        cmf.Make();
    }
}

// return true if the given module is valid, or it has a valid submodule.
bool Build::TraverseDirectoryTree(
    Module& module, const string& logHeader)
{
    if (module.GetBuildType() == EBuildType::Ignored)
        return false;

    if (module.IsIncludePath())
    {
        includeDirs.push_back(module.path);
    }

    if (module.GetBuildType() == EBuildType::ExternalLibraries)
    {
        // External library path should be added to include paths.
        return true;
    }

    bool isValidModule = false;

    if (!module.SrcFileList().empty() || !module.HeaderFileList().empty())
    {
        cout << endl;
        cout << logHeader << "### Module = " << module.path << endl;

        if (!module.SrcFileList().empty())
        {
            cout << logHeader << "### Source Files" << endl;
            for (const auto& element : module.SrcFileList())
            {
                cout << logHeader << element << endl;
            }
        }

        if (!module.HeaderFileList().empty())
        {
            cout << endl;
            cout << logHeader << "### Header Files" << endl;
            for (const auto& element : module.HeaderFileList())
            {
                cout << logHeader << element << endl;
            }
        }

        isValidModule = true;
    }

    // Iterate all children
    for (const auto& element : module.DirList())
    {
        Module submodule(&module, element);
        string childLogHeader = logHeader;
        childLogHeader.append("  ");

        if (TraverseDirectoryTree(submodule, childLogHeader))
        {
            auto& submoduleList = module.SubModuleList();
            submoduleList.push_back(submodule);
            isValidModule = true;
        }
    }

    auto& subModules = module.SubModuleList();
    sort(subModules.begin(), subModules.end());

    return isValidModule;
};

} // namespace Builder
