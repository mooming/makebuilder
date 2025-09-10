// Created by mooming.go@gmail.com 2016

#include "ProjectBuilder.h"

#include "CMakeLists.h"
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

    auto& subList = module.SubModuleList();
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

void ProjectBuilder::BuildCMakeFiles()
{
    for (auto module : modules)
    {
        assert(module != nullptr);

        using namespace mb;
        const auto moduleBuildType = module->GetBuildType();
        if (moduleBuildType == EBuildType::ExternalCMakePorject)
        {
            cout << "[CMake] Skip creating CMakeLists.txt of Module = " << module->GetName().c_str() << endl;
            continue;
        }

        CMakeLists cmf(*this, *module);
        cmf.Make();
    }
}

// return true if the given module is valid, or it has a valid submodule.
bool ProjectBuilder::TraverseDirectoryTree(
    Module& module, const string& logHeader)
{
    if (module.GetBuildType() == EBuildType::Ignored)
        return false;

    if (module.GetBuildType() == EBuildType::ExternalCMakePorject)
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
    for (const auto& subDirectory : module.DirList())
    {
        Module submodule(&module, subDirectory);
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
}

} // namespace Builder
