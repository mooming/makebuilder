// Created by mooming.go@gmail.com 2016

#pragma once

#include "ProjectBuilder.h"
#include "Module.h"
#include <string>

namespace mb
{
    // This class represents a CMakeLists.txt
    class CMakeLists final
    {
    private:
        const mb::ProjectBuilder& build;
        const mb::Module& module;

    public:
        CMakeLists(const mb::ProjectBuilder& build, const mb::Module& module);
        ~CMakeLists() = default;

        void Make();
        std::string TranslatePath(std::string path);
    };
} // namespace CMake
