// Created by mooming.go@gmail.com 2016

#pragma once

#include "BuildConfig.h"
#include "Directory.h"
#include "File.h"


namespace mb
{
    enum class EBuildType : uint8_t
    {
        None = 0,
        Ignored,
        HeaderOnly,
        Executable,
        StaticLibrary,
        SharedLibrary,
        ExternalLibrary,
        Max
    };

    const std::string& BuildTypeToString(EBuildType type);


    // This class represent a build module which is also a directory
    // under the project base directory.
    class Module : public OS::Directory
    {
    public:
        using TString = std::string;
        using Modules = std::vector<Module>;
        using Strings = std::vector<TString>;

    private:
        BuildConfig config;
        TString moduleName;

        OS::Files srcFiles;
        OS::Files headerFiles;
        Modules submodules;
        Strings dependencies;
        Strings libraries;
        Strings frameworks;
        TString precompileDefinitions;
        TString optimizeLevel;

        // FIX 2026-04-03: Added includePaths to support includes.txt for ExternalLibrary
        // When ExternalLibrary module has includes.txt, those paths are collected
        Strings includePaths;

        EBuildType buildType;
        bool isIncludePath;

    public:
        Module(const OS::Directory& dir);
        Module(const Module* parent, const OS::Directory& dir);
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

        auto& GetSourceFiles() const { return srcFiles; }
        auto& GetHeaderFiles() const { return headerFiles; }
        auto& GetSubModules() { return submodules; }
        auto& GetSubModules() const { return submodules; }
        auto& GetDependencies() const { return dependencies; }
        auto& GetLibraries() const { return libraries; }
        auto& GetFrameworks() const { return frameworks; }
        
        // FIX 2026-04-03: Added GetIncludePaths() to expose includePaths
        // This allows ProjectBuilder to collect include paths from ExternalLibrary children.
        auto& GetIncludePaths() const { return includePaths; }
        auto& GetPrecompileDefinitions() const
        {
            return precompileDefinitions;
        }
        auto& GetOptimizeLevel() const { return optimizeLevel; }

    private:
        void BuildLists(const OS::Files& files);
        void LoadList(const char* filePath, Strings& list);
        void Sort();
    };
} // namespace mb
