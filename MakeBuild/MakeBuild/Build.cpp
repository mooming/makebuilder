//
//  Build.cpp
//  mbuild
//
//  Created by mooming on 2016. 8. 15..
//
//

#include "Build.h"

#include "CMakeFile.h"
#include "StringUtil.h"

#include <iostream>
#include <string>

using namespace std;

namespace Builder
{

	Build::Build(const char* path) : baseDir(path), projectDirs()
	{
		TraverseDirTree(baseDir, "");
		projectDirs.shrink_to_fit();

		cout << endl;
		cout << "=== Project Directories Found ===" << endl;

		int i = 0;
		for (const ProjectDir& element : projectDirs)
		{
			cout << i++ << " : " << element.path << endl;
		}
	}

	void Build::BuildCMakeFiles()
	{
		for (const ProjectDir& element : projectDirs)
		{
			using namespace CMake;

			CMakeFile cmf(*this, element);
			cmf.Make();
		}
	}

	bool Build::TraverseDirTree(const OS::Directory& dir, string header)
	{
		bool havingProjDir = false;

		ProjectDir projDir(dir);

		if (!projDir.SrcFileList().empty() || !projDir.HeaderFileList().empty())
		{
			cout << endl;
			cout << header << "### Directory = " << projDir.path << endl;

			if (!projDir.SrcFileList().empty())
			{
				cout << header << "### Source Files" << endl;
				for (const auto& element : projDir.SrcFileList())
				{
					cout << header << element << endl;
				}
			}

			if (!projDir.HeaderFileList().empty())
			{
				cout << endl;
				cout << header << "### Header Files" << endl;
				for (const auto& element : projDir.HeaderFileList())
				{
					cout << header << element << endl;
				}
			}

			havingProjDir = true;
		}

		bool isBaseIncludeDir = false;
		for (const auto& element : dir.FileList())
		{
			if (Util::EqualsIgnoreCase(element.path, "ignore.txt"))
			{
				return false;
			}

			if (Util::EqualsIgnoreCase(element.path, "include.txt"))
			{
				isBaseIncludeDir = true;
			}
		}

		if (isBaseIncludeDir)
		{
			includeDirs.push_back(dir.path);
		}

		for (const auto& element : dir.DirList())
		{
			string newHeader = header;
			if (TraverseDirTree(element, newHeader.append("  ")))
			{
				projDir.ProjDirList().push_back(element);
				havingProjDir = true;
			}
		}

		if (havingProjDir)
		{
			projectDirs.push_back(projDir);
		}

		return havingProjDir;
	};
}
