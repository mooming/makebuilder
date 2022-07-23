//
//  OSAbstractLayer.h
//  mbuild
//
//  Created by mooming on 2016. 8. 14..
//
//

#ifndef OSAbstractLayer_h
#define OSAbstractLayer_h

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
#endif // POSIX

namespace OS
{
	std::string GetFullPath(std::string path);

	bool IsDirectory(const char* path);
	std::vector<std::string> ListFilesInDirectory(const char* path);
}

#endif /* OSAbstractLayer_h */
