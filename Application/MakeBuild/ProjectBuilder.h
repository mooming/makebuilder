// Created by mooming.go@gmail.com 2016

#pragma once

#include "BuildConfig.h"
#include "Module.h"
#include <string>
#include <vector>


namespace mb
{

// This class will create a CMake project with the given root path
class ProjectBuilder final
{
public:
    using Modules = std::vector<Module*>;
    using Paths = std::vector<std::string>;

public:
    const BuildConfig config;
    Module baseModule;
    Modules modules;
    Paths includeDirs;
    
    // FIX 2026-04-03: Added externalLibraries to collect libraries from ExternalLibraries children
    // When traversing an ExternalLibraries module, we collect library names from
    // libraries.txt files in subdirectories. These are then used to generate
    // target_link_libraries() in the parent ExternalLibraries CMakeLists.txt.
    Module::Strings externalLibraries;

public:
    explicit ProjectBuilder(const char* rootPath);
    ~ProjectBuilder() = default;

    void GenerateCMakeFiles();

private:
    bool TraverseDirectoryTree(Module& dir, const std::string& logHeader);
};
} // namespace mb
