// Created by mooming.go@gmail.com 2016

#pragma once

#include "ConfigFile.h"
#include "Directory.h"
#include "File.h"
#include <cstdint>

namespace Builder
{
    enum class EBuildType : uint8_t
    {
        None = 0,
        Ignored,
        HeaderOnly,
        Executable,
        StaticLibrary,
        SharedLibrary,
        ExternalLibraries,
        Max
    };

    const std::string& BuildTypeToString(EBuildType type);

    class Module : public OS::Directory
    {
    public:
        using TString = std::string;
        using Modules = std::vector<Module>;
        using Strings = std::vector<TString>;

    private:
        ConfigFile config;
        TString moduleName;

        OS::Files srcFiles;
        OS::Files headerFiles;
        Module* parentModule;
        Modules submodules;
        Strings dependencies;
        Strings libraries;
        Strings frameworks;
        TString precompileDefinitions;

        EBuildType buildType;
        bool isIncludePath;

    public:
        Module(const OS::Directory& dir);
        Module(Module* parent, const OS::Directory& dir);
        virtual ~Module() = default;

        bool operator<(const Module& rhs) const
        {
            return moduleName < rhs.moduleName;
        }

        bool HasSourceFileRecursive() const;
        void PrintSubModules(const TString& header) const;

        auto& GetName() const { return moduleName; }
        auto GetBuildType() const { return buildType; }
        auto IsIncludePath() const { return isIncludePath; }

        auto& SrcFileList() const { return srcFiles; }
        auto& HeaderFileList() const { return headerFiles; }
        auto& SubModuleList() { return submodules; }
        auto& SubModuleList() const { return submodules; }
        auto& DependencyList() const { return dependencies; }
        auto& LibraryList() const { return libraries; }
        auto& FrameworkList() const { return frameworks; }
        auto& GetPrecompileDefinitions() const
        {
            return precompileDefinitions;
        }

    private:
        void BuildCMakeModule();
        void BuildLists(const OS::Files& files);
        void LoadList(const char* filePath, Strings& list);
        void Sort();
    };
} // namespace Builder
