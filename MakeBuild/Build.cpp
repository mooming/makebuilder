// Created by mooming.go@gmail.com 2016

#include "Build.h"

#include "CMakeFile.h"
#include "MakeBuildConfig.h"
#include "StringUtil.h"

#include <iostream>
#include <string>


using namespace std;

namespace Builder
{
	namespace
	{
		void TravesAllChildren(ProjectDir& dir, Build::ProjDirs& projectDirs)
		{
			projectDirs.push_back(&dir);

			auto& subList = dir.ProjDirList();
			for (auto& sub : subList)
			{
				TravesAllChildren(sub, projectDirs);
			}
		};
	}

	Build::Build(const char* path)
        : config(path)
		, baseDir(path)
        , projectDirs()
	{
		TraverseDirTree(baseDir, "");
		projectDirs.shrink_to_fit();

		cout << endl;
		cout << "=== Project Directories Found ===" << endl;

		baseDir.PrintSubDirs(" ");

		TravesAllChildren(baseDir, projectDirs);

		int i = 0;
		for (auto element : projectDirs)
		{
			assert(element != nullptr);

			cout << i++ << " : " << element->path << endl;
		}
	}

	void Build::BuildCMakeFiles()
	{
		for (auto element : projectDirs)
		{
			assert(element != nullptr);

			using namespace CMake;

			CMakeFile cmf(*this, *element);
			cmf.Make();
		}
	}

	bool Build::TraverseDirTree(ProjectDir& projDir, string header)
	{
		bool havingProjDir = false;

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
		for (const auto& element : projDir.FileList())
		{
			if (Util::EqualsIgnoreCase(element.GetPath(), "ignore.txt"))
			{
				return false;
			}

			if (Util::EqualsIgnoreCase(element.GetPath(), "include.txt"))
			{
				isBaseIncludeDir = true;
			}
		}

		if (isBaseIncludeDir)
		{
			includeDirs.push_back(projDir.path);
		}

		for (const auto& element : projDir.DirList())
		{
			ProjectDir childProjDir(element); 
			string newHeader = header;
			newHeader.append("  ");

			if (TraverseDirTree(childProjDir, newHeader.append("  ")))
			{		
				auto& projList = projDir.ProjDirList();
				projList.push_back(childProjDir);
				
				havingProjDir = true;
			}
		}

		return havingProjDir;
	};
}
