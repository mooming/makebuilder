// Created by mooming.go@gmail.com 2016

#include "Module.h"

#include "StringUtil.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace OS;

namespace mb
{
const string& BuildTypeToString(EBuildType type)
{
    constexpr auto arraySize = static_cast<size_t>(EBuildType::Max);
    static array<string, arraySize> strings{"None", "Ignored", "HeaderOnly", "Executable",
        "StaticLibrary", "SharedLibrary", "ExternalLibrary"};

    const size_t index = static_cast<uint8_t>(type);
    if (index > arraySize || index < 0)
        return strings[0];

    return strings[index];
}

namespace
{

EBuildType ParseBuildTypeStr(const std::string& str)
{
    EBuildType buildType = EBuildType::None;

    constexpr auto start = static_cast<uint8_t>(EBuildType::None);
    constexpr auto end = static_cast<uint8_t>(EBuildType::Max);

    for (uint8_t it = start; it <= end; ++it)
    {
        auto itBuildType = static_cast<EBuildType>(it);
        auto& itBuildTypeStr = BuildTypeToString(itBuildType);
        if (Util::EqualsIgnoreCase(str, itBuildTypeStr))
        {
            buildType = itBuildType;
            break;
        }
    }

    // Handle invalid build-types
    if (buildType >= EBuildType::Max)
        buildType = EBuildType::None;

    return buildType;
}

void ParseList(const char* filePath, Module::Strings& list)
{
    ifstream ifs(filePath);

    if (!ifs.is_open())
    {
        cerr << "Failed to load list file, " << filePath << "." << endl;
        abort();
    }

    while (!ifs.eof())
    {
        string line;
        getline(ifs, line);
        if (line.empty())
            continue;

        if (std::find(list.begin(), list.end(), line) != list.end())
            continue;

        cout << "Element: " << line << " from " << filePath << endl;
        list.push_back(line);
    }
}

} // namespace

Module::Module(const Directory& dir)
    : Module(nullptr, dir)
{
}

Module::Module(const Module* parent, const OS::Directory& dir)
    : Directory(dir),
      config(dir.path.c_str(), ".module.config"),
      buildType(EBuildType::Ignored),
      isIncludePath(false)
{

    CollectFiles();

    bool hasValidConfig = config.IsValid();
    bool hasSourceFiles = !srcFiles.empty();
    bool hasHeaderFiles = !headerFiles.empty();

    auto name = config.GetValue("name", "");
    if (!name.empty())
    {
        moduleName = name;
    }
    else
    {
        moduleName = Util::PathToName(path);
    }

    if (!hasValidConfig || (!hasSourceFiles && !hasHeaderFiles))
    {
        buildType = EBuildType::Ignored;
        return;
    }

    {
        auto buildTypeConfig = config.GetValue("buildType", "None");
        buildType = ParseBuildTypeStr(buildTypeConfig);
    }

    precompileDefinitions = config.GetValue("precompileDefinitions", "");
    optimizeLevel = config.GetValue("optimizeLevel", "");

    {
        TString value = config.GetValue("ignoreSubdirectories", "");
        std::istringstream ss(value);

        std::string token;
        while (std::getline(ss, token, ' '))
        {
            ignoredSubdirectories.push_back(token);
        }
    }

    if (buildType == EBuildType::ExternalLibrary)
    {
        isIncludePath = true;
    }
    else
    {
        for (const auto& file : otherFiles)
        {
            if (Util::EqualsIgnoreCase(file.GetPath(), "include.txt"))
            {
                isIncludePath = true;
                break;
            }
        }
    }

    ParseBuildSpecifiers(otherFiles);
    Sort();
}

bool Module::HasSourceFileRecursive() const
{
    if (!srcFiles.empty() || !headerFiles.empty())
        return true;

    for (const auto& subDir : submodules)
    {
        if (subDir.HasSourceFileRecursive())
            return true;
    }

    return false;
}

void Module::PrintInfo(const std::string& header) const
{
    cout << endl;
    cout << header << " [ModuleInfo][" << moduleName << "] START ###"<< endl;
    cout << header << " [ModuleInfo][" << moduleName << "] Path = " << path << ", Build Type = "
       << BuildTypeToString(buildType).c_str() << endl;

    if (!precompileDefinitions.empty())
    {
        cout << header << " [ModuleInfo][" << moduleName << "] Precompile Definitions = " << precompileDefinitions << endl;
    }

    if (!optimizeLevel.empty())
    {
        cout << header << " [ModuleInfo][" << moduleName << "] optimizeLevel = " << optimizeLevel << endl;
    }

    for (auto& item : dependencies)
    {
        std::cout << header << " [ModuleInfo][" << moduleName << "] Dependency: " << item << std::endl;
    }

    for (auto& item : libraries)
    {
        std::cout << header << " [ModuleInfo][" << moduleName << "] Library: " << item << std::endl;
    }

    for (auto& item : frameworks)
    {
        std::cout << header << " [ModuleInfo][" << moduleName << "] Framework: " << item << std::endl;
    }

    for (auto& item : ignoredSubdirectories)
    {
        std::cout << header << " [ModuleInfo][" << moduleName << "] Ignored SubDir: " << item << std::endl;
    }

    if (!headerFiles.empty())
    {
        for (const auto& element : headerFiles)
        {
            cout << header << " [ModuleInfo][" << moduleName << "] " << element << endl;
        }
    }

    if (!srcFiles.empty())
    {
        for (const auto& element : srcFiles)
        {
            cout << header << " [ModuleInfo][" << moduleName << "] " << element << endl;
        }
    }

    cout << header << " [ModuleInfo][" << moduleName << "] END ###" << endl;
}

void Module::PrintSubModules(const std::string& header) const
{
    cout << "[DIR]" << header << path << endl;
    for (const auto& subDir : submodules)
    {
        auto newHeader = header;
        newHeader.append(" ");
        subDir.PrintSubModules(newHeader);
    }
}

void Module::CollectFiles()
{
    // Clear file containers
    srcFiles.clear();
    headerFiles.clear();
    otherFiles.clear();

    // Classify files
    auto files = FileList();
    for (const auto& file : files)
    {
        using namespace Util;
        const auto& filename = file.GetPath();

        if (EndsWith(ToLowerCase(filename), ".c") ||
            EndsWith(ToLowerCase(filename), ".cpp"))
        {
            srcFiles.push_back(file);
        }
        else if (EndsWith(ToLowerCase(filename), ".h") ||
            EndsWith(ToLowerCase(filename), ".hpp") ||
            EndsWith(ToLowerCase(filename), ".inl"))
        {
            headerFiles.push_back(file);
        }
        else
        {
            otherFiles.push_back(file);
        }
    }

    // Sort all the file containers
    sort(srcFiles.begin(), srcFiles.end());
    sort(headerFiles.begin(), headerFiles.end());
    sort(otherFiles.begin(), otherFiles.end());
}

void Module::ParseBuildSpecifiers(const Files& files)
{
    dependencies.clear();
    libraries.clear();
    frameworks.clear();
    includePaths.clear();

    for (const auto& element : files)
    {
        using namespace Util;

        //std::cout << "[Module][" << moduleName << "] Check: " << element.GetPath() << std::endl;

        if (EqualsIgnoreCase(element.GetPath(), "dependency.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            ParseList(filePath.c_str(), dependencies);
            continue;
        }

        if (EqualsIgnoreCase(element.GetPath(), "library.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            ParseList(filePath.c_str(), libraries);
            continue;
        }

        if (EqualsIgnoreCase(element.GetPath(), "framework.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            ParseList(filePath.c_str(), frameworks);
            continue;
        }

        if (EqualsIgnoreCase(element.GetPath(), "include.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            ParseList(filePath.c_str(), includePaths);
            continue;
        }
    }
}

void Module::Sort()
{
    //std::cout << "[Module][" << moduleName << "] SORT ###" << std::endl;

    auto SortVector = [](auto& v) { sort(v.begin(), v.end()); };

    SortVector(dependencies);
    SortVector(libraries);
    SortVector(frameworks);
    SortVector(ignoredSubdirectories);
}

} // namespace mb
