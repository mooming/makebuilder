//
//  Directory.h
//  mbuild
//
//  Created by mooming on 2016. 8. 14..
//
//

#ifndef Directory_h
#define Directory_h

#include <string>
#include <vector>

#include "File.h"

namespace OS
{
	class Directory
	{
		using Dirs = std::vector<Directory>;
	public:
		const std::string path;

	private:
		Files fileList;
		Dirs dirList;

	public:
		Directory(const char* path);

		inline const Files& FileList() const { return fileList; }
		inline const Dirs& DirList() const { return dirList; }
	};

	using Dirs = std::vector<Directory>;
}

#endif /* Directory_h */
