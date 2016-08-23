//
//  Build.h
//  mbuild
//
//  Created by mooming on 2016. 8. 15..
//
//

#ifndef Build_h
#define Build_h

#include "ProjectDir.h"

#include <string>
#include <vector>

namespace Builder
{
	class Build
	{
		using ProjDirs = std::vector<ProjectDir>;
		using Paths = std::vector<std::string>;

	public:
		ProjectDir baseDir;
		ProjDirs projectDirs;
		Paths includeDirs;

	public:
		Build(const char* path);
		void BuildCMakeFiles();

	private:
		bool TraverseDirTree(const OS::Directory& dir, std::string header);
	};

}
#endif /* Build_h */
