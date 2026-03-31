// Created by mooming.go@gmail.com 2016

#include "Module.h"

#include "StringUtil.h"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;
using namespace OS;

namespace mb
{
const string& BuildTypeToString(EBuildType type)
{
    constexpr auto arraySize = static_cast<size_t>(EBuildType::Max);
    // Array must match EBuildType enum order exactly.
    // Note: ExternalCMakeProject added at end (was missing before fix 2026-03-31).
    static array<string, arraySize> strings{"None", "Ignored", "HeaderOnly", "Executable",
        "StaticLibrary", "SharedLibrary", "ExternalLibraries", "ExternalCMakeProject"};

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
    if (parent != nullptr && parent->GetBuildType() == EBuildType::ExternalLibraries)
    {
        auto& files = FileList();
        for (auto& file : files)
        {
            if (Util::EqualsIgnoreCase(file.GetPath(), "CMakeLists.txt"))
            {
                buildType = EBuildType::ExternalCMakeProject;
                moduleName = path;
                break;
            }
        }

        return;
    }

    // FIX 2026-03-31: Early file categorization to support HeaderOnly auto-detection
    // Previously, files were categorized only after config was validated.
    // Now we categorize files first so we can auto-detect HeaderOnly modules
    // even when no .module.config exists.
    OS::Files allFiles = FileList();
    OS::Files srcFiles;
    OS::Files headerFiles;
    OS::Files otherFiles;

    for (const auto& file : allFiles)
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

    sort(srcFiles.begin(), srcFiles.end());
    sort(headerFiles.begin(), headerFiles.end());
    sort(otherFiles.begin(), otherFiles.end());

    bool hasValidConfig = config.IsValid();
    bool hasSourceFiles = !srcFiles.empty();
    bool hasHeaderFiles = !headerFiles.empty();
    Files files = std::move(otherFiles);

    if (!hasValidConfig && !hasSourceFiles && !hasHeaderFiles)
    {
        buildType = EBuildType::Ignored;
        return;
    }

    // FIX 2026-03-31: Auto-detect HeaderOnly modules without .module.config
    // If no config file exists but directory has header files and no source files,
    // automatically classify as HeaderOnly. This allows header-only libraries
    // to work without explicit configuration.
    if (!hasValidConfig)
    {
        buildType = EBuildType::None;
        moduleName = Util::PathToName(path);

        if (hasHeaderFiles && !hasSourceFiles)
        {
            buildType = EBuildType::HeaderOnly;
            cout << "[Module] Auto-detected type: HeaderOnly (no source files)" << endl;
        }

        this->srcFiles = std::move(srcFiles);
        this->headerFiles = std::move(headerFiles);

        BuildLists(otherFiles);
        Sort();
        return;
    }

    cout << "[Module] Open " << path << endl;

    auto name = config.GetValue("name", "");
    if (!name.empty())
    {
        moduleName = name;
        cout << "[Module] Name = " << name << endl;
    }
    else
    {
        moduleName = Util::PathToName(path);
        cout << "[Module] Name set by path = " << moduleName << endl;
    }

    {
        auto buildTypeConfig = config.GetValue("buildType", "None");
        buildType = ParseBuildTypeStr(buildTypeConfig);

        cout << "[Module] build type of [" << moduleName
             << "] = " << BuildTypeToString(buildType).c_str() << " ("
             << buildTypeConfig << ')' << endl;
    }

    precompileDefinitions = config.GetValue("precompileDefinitions", "");
    optimizeLevel = config.GetValue("optimizeLevel", "");

    if (!optimizeLevel.empty())
    {
        cout << "[Module] optimizeLevel = " << optimizeLevel << endl;
    }

    for (const auto& file : FileList())
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
            files.push_back(file);
        }
    }

    sort(srcFiles.begin(), srcFiles.end());
    sort(headerFiles.begin(), headerFiles.end());
    sort(files.begin(), files.end());

    // BUG FIX 2026-03-31: Source files must be saved to member variables
    // Previously, files were categorized into local variables (lines 184-204),
    // but were never copied to this->srcFiles and this->headerFiles.
    // This caused GetSourceFiles() to return empty for modules with .module.config.
    this->srcFiles = srcFiles;
    this->headerFiles = headerFiles;

    if (buildType == EBuildType::ExternalLibraries)
    {
        isIncludePath = true;
    }
    else
    {
        for (const auto& file : files)
        {
            if (Util::EqualsIgnoreCase(file.GetPath(), "include.txt"))
            {
                isIncludePath = true;
                break;
            }
        }

        if (buildType != EBuildType::Ignored && GetSourceFiles().empty() &&
            !GetHeaderFiles().empty())
        {
            buildType = EBuildType::HeaderOnly;
        }
    }

    BuildLists(files);
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

void Module::BuildLists(const Files& files)
{
    dependencies.clear();
    libraries.clear();
    frameworks.clear();

    for (const auto& element : files)
    {
        using namespace Util;

        if (EqualsIgnoreCase(element.GetPath(), "dependencies.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            LoadList(filePath.c_str(), dependencies);
            continue;
        }

        if (EqualsIgnoreCase(element.GetPath(), "libraries.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            LoadList(filePath.c_str(), libraries);
            continue;
        }

        if (EqualsIgnoreCase(element.GetPath(), "frameworks.txt"))
        {
            string filePath = path;
            filePath.append("/");
            filePath.append(element.GetPath());

            LoadList(filePath.c_str(), frameworks);
            continue;
        }
    }
}

void Module::LoadList(const char* filePath, Strings& list)
{
    ifstream ifs(filePath);

    if (!ifs.is_open())
    {
        cout << "Failed to load list file, " << filePath << "." << endl;
        abort();
    }

    cout << "List from " << filePath << endl;

    while (true)
    {
        string line;
        getline(ifs, line);

        if (!line.empty())
        {
            // Check for duplicates before adding to the list.
            // This prevents the same dependency from being added multiple times
            // if the list file is accidentally loaded multiple times.
            bool isDuplicate = false;
            for (const auto& existing : list)
            {
                if (existing == line)
                {
                    isDuplicate = true;
                    break;
                }
            }

            if (!isDuplicate)
            {
                cout << "Element: " << line << endl;
                list.push_back(line);
            }
        }

        if (ifs.eof())
            break;
    }
}

void Module::Sort()
{
    auto SortVector = [](auto& v) { sort(v.begin(), v.end()); };

    SortVector(dependencies);
    SortVector(libraries);
    SortVector(frameworks);
}
} // namespace mb
