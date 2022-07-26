// Created by mooming.go@gmail.com 2016

#pragma once

#include <string>


namespace Builder
{
    class MakeBuildConfig final
    {
    private:
        using TString = std::string;
        TString requiredCMakeVersion;
        TString cxxStandard;
        TString compileOptions;
        TString msvcCompileOptions;
        TString precompileDefinitions;

    public:
        MakeBuildConfig(const char* basePath);
        ~MakeBuildConfig() = default;

        inline auto& GetRequiredCMakeVersion() const { return requiredCMakeVersion; }
        inline auto& GetCXXStandard() const { return cxxStandard; }
        inline auto& GetCompileOptions() const { return compileOptions; }
        inline auto& GetMSVCCompileOptions() const { return msvcCompileOptions; }
        inline auto& GetPrecompileDefinitions() const { return precompileDefinitions; }

    private:
        void ReadConfigFile(const char* fileName);
        void ParseKeyValue(const std::string& key, const std::string& value);
    };
}