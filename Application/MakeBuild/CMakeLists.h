// Created by mooming.go@gmail.com 2016

#pragma once

#include "ProjectBuilder.h"
#include "Module.h"
#include <string>

namespace mb
{
    // Generates CMakeLists.txt file for a module
    class CMakeGenerator final
    {
    private:
        const mb::ProjectBuilder& build;
        const mb::Module& module;

    public:
        CMakeGenerator(const mb::ProjectBuilder& build, const mb::Module& module);
        ~CMakeGenerator() = default;

        void Generate();
        std::string TranslatePath(std::string path);
    };
} // namespace mb
