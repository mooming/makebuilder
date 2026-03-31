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
// PATH_MAX is not standardized on Windows; use MAX_PATH from Windows API.
// Fixed 2026-03-31: PATH_MAX was previously undefined on POSIX systems.
#define PATH_MAX MAX_PATH
#endif // WINDOWS

#ifdef POSIX
#include <dirent.h>
#include <climits>
// PATH_MAX may not be defined on all POSIX systems (e.g., macOS, BSD).
// Added fallback definition to ensure TrimPath() and realpath() work correctly.
// Previously caused compilation errors or buffer overflows on some platforms.
// Fixed 2026-03-31.
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
