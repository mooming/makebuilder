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
        OS::Files objectiveCSrcFiles;
        OS::Files headerFiles;
        OS::Files otherFiles;

        Modules submodules;
        Strings dependencies;
        Strings includePaths;
        Strings libraries;
        Strings linkDirectories;
        Strings frameworks;
        Strings customCMake;

        TString precompileDefinitions;
        TString optimizeLevel;
        Strings ignoredSubdirectories;

        EBuildType buildType;
        bool isIncludePath;

    public:
        explicit Module(const OS::Directory& dir);
        Module(const Module* parent, const OS::Directory& dir);
        ~Module() override = default;

        bool operator<(const Module& rhs) const
        {
            return moduleName < rhs.moduleName;
        }

        [[nodiscard]] bool HasSourceFileRecursive() const;

        void PrintInfo(const std::string& header) const;
        void PrintSubModules(const TString& header) const;

        [[nodiscard]] auto& GetName() const { return moduleName; }
        [[nodiscard]] auto GetBuildType() const { return buildType; }
        [[nodiscard]] auto IsIncludePath() const { return isIncludePath; }

        [[nodiscard]] auto& GetSourceFiles() const { return srcFiles; }
        [[nodiscard]] auto& GetObjectiveCSourceFiles() const { return objectiveCSrcFiles; }
        [[nodiscard]] auto& GetHeaderFiles() const { return headerFiles; }
        [[nodiscard]] auto& GetSubModules() { return submodules; }
        [[nodiscard]] auto& GetSubModules() const { return submodules; }
        [[nodiscard]] auto& GetDependencies() const { return dependencies; }
        [[nodiscard]] auto& GetLibraries() const { return libraries; }
        [[nodiscard]] auto& GetLinkDirectories() const { return linkDirectories; }
        [[nodiscard]] auto& GetFrameworks() const { return frameworks; }
        [[nodiscard]] auto& GetCustomCMake() const { return customCMake; }
        [[nodiscard]] auto& GetIncludePaths() const { return includePaths; }
        [[nodiscard]] auto& GetPrecompileDefinitions() const { return precompileDefinitions; }
        [[nodiscard]] auto& GetOptimizeLevel() const { return optimizeLevel; }
        [[nodiscard]] auto& GetIgnoredSubdirectories() const { return ignoredSubdirectories; }

    private:
        void CollectFiles();
        void ParseBuildSpecifiers(const OS::Files& files);
        void Sort();
    };
} // namespace mb
