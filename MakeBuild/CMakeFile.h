// Created by mooming.go@gmail.com 2016

#pragma once

#include "Build.h"
#include "Module.h"
#include <string>

namespace CMake
{
    class CMakeFile final
    {
    private:
        const Builder::Build& build;
        const Builder::Module& module;

    public:
        CMakeFile(const Builder::Build& build, const Builder::Module& module);
        ~CMakeFile() = default;

        void Make();
        std::string TranslatePath(std::string path);
    };
} // namespace CMake
