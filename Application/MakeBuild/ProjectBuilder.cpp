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
      baseModule(path)
{
    TraverseDirectoryTree(baseModule, "=");
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

        CMakeGenerator generator(*this, *module);
        generator.Generate();
    }
}

// return true if the given module is valid, or it has a valid submodule.
bool ProjectBuilder::TraverseDirectoryTree(
    Module& module, const string& logHeader)
{
    module.PrintInfo(logHeader);

    if (module.GetBuildType() == EBuildType::Ignored)
        return false;

    if (module.IsIncludePath())
    {
        includeDirs.push_back(module.path);
    }

    bool isValidModule = false;
    if (module.GetBuildType() == EBuildType::ExternalLibrary)
    {
        isValidModule = true;
    }

    bool hasHeaderOrSource = !module.GetSourceFiles().empty() || !module.GetHeaderFiles().empty();
    if (hasHeaderOrSource || module.GetBuildType() == EBuildType::HeaderOnly)
    {
        isValidModule = true;
    }

    auto& submodules = module.GetSubModules();
    for (const auto& subDirectory : module.DirList())
    {
        cout << endl;
        cout << "[" << module.GetName() << "] Visit " << subDirectory.path << endl;

        Module submodule(&module, subDirectory);
        auto& ignoredSubDirs = module.GetIgnoredSubdirectories();
        auto subModuleName = submodule.GetName();

        if (submodule.GetBuildType() == EBuildType::Ignored
            || std::find(ignoredSubDirs.begin(), ignoredSubDirs.end(), subModuleName) != ignoredSubDirs.end())
        {
            cout << "[" << module.GetName() << "] SKIP: " << subModuleName << endl;
            continue;
        }

        string childLogHeader = logHeader;
        childLogHeader.append("=");

        if (TraverseDirectoryTree(submodule, childLogHeader))
        {
            submodules.push_back(submodule);
        }
    }

    // A module is valid if it has submodules.
    if (!submodules.empty())
    {
        isValidModule = true;
    }

    auto& subModules = module.GetSubModules();
    sort(subModules.begin(), subModules.end());

    cout << "[" << module.GetName() << "] Valid: " << isValidModule << endl;

    return isValidModule;
}

} // namespace mb
