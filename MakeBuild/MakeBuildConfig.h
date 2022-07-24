// Created by mooming.go@gmail.com 2016

#pragma once

#include <string>


namespace Builder
{
    class MakeBuildConfig final
    {
    private:
        std::string requiredCMakeVersion;
        std::string cxxStandard;

    public:
        MakeBuildConfig(const char* basePath);
        ~MakeBuildConfig() = default;

        inline auto& GetRequiredCMakeVersion() const { return requiredCMakeVersion; }
        inline auto& GetCXXStandard() const { return cxxStandard; }

    private:
        void ReadConfigFile(const char* fileName);
        void ParseKeyValue(const std::string& key, const std::string& value);
    };
}