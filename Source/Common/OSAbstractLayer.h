// Created by mooming.go@gmail.com 2016

#pragma once

#include <string>
#include <vector>

#ifdef WIN32
#define WINDOWS
#else // _WINDOWS
#define POSIX
#endif // _WINDOWS

#ifdef WINDOWS
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif // WINDOWS

#ifdef POSIX
#include <dirent.h>
#include <climits>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#endif // POSIX

namespace OS
{
    std::string GetFullPath(std::string path);

    bool IsDirectory(const char* path);
    std::vector<std::string> ListFilesInDirectory(const char* path);
} // namespace OS
