// Created by mooming.go@gmail.com 2016

#pragma once

#include "File.h"
#include <string>
#include <vector>

namespace OS
{
    class Directory
    {
        using Dirs = std::vector<Directory>;

    public:
        std::string path;

    private:
        Files fileList;
        Dirs dirList;

    public:
        Directory(const char* path);
        virtual ~Directory() = default;

        const Files& FileList() const { return fileList; }
        const Dirs& DirList() const { return dirList; }
    };

    using Dirs = std::vector<Directory>;
} // namespace OS
