// Created by mooming.go@gmail.com 2016

#pragma once

#include "ConfigFile.h"
#include "Module.h"

#include <string>
#include <vector>

namespace Builder
{
    class Build final
    {
    public:
        using Modules = std::vector<Module*>;
        using Paths = std::vector<std::string>;

    public:
        const ConfigFile config;
        Module baseModule;
        Modules modules;
        Paths includeDirs;

    public:
        Build(const char* path);
        ~Build() = default;

        void BuildCMakeFiles();

    private:
        bool TraverseDirectoryTree(Module& dir, const std::string& logHeader);
    };
} // namespace Builder
