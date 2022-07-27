// Created by mooming.go@gmail.com 2016

#include "Build.h"

#include "CMakeFile.h"
#include "StringUtil.h"
#include <iostream>
#include <string>


using namespace std;

namespace Builder
{
	namespace
	{
		void GetAllChildrenRecursive(Module& module, Build::Modules& inoutModules)
		{
			inoutModules.push_back(&module);

			auto& subList = module.SubModuleList();
			for (auto& sub : subList)
			{
				GetAllChildrenRecursive(sub, inoutModules);
			}
		};
	}

	Build::Build(const char* path)
        : config(path, ".project.config")
		, baseModule(path)
        , modules()
	{
		TraverseDirTree(baseModule, "");
		modules.shrink_to_fit();

		cout << endl;
		cout << "=== Project Directories Found ===" << endl;

		baseModule.PrintSubModules(" ");

		GetAllChildrenRecursive(baseModule, modules);

		int i = 0;
		for (auto element : modules)
		{
			assert(element != nullptr);

			cout << i++ << " : " << element->path << endl;
		}
	}

	void Build::BuildCMakeFiles()
	{
		for (auto element : modules)
		{
			assert(element != nullptr);

			using namespace CMake;

			CMakeFile cmf(*this, *element);
			cmf.Make();
		}
	}

	bool Build::TraverseDirTree(Module& module, string header)
	{
		bool havingProjDir = false;

		if (!module.SrcFileList().empty() || !module.HeaderFileList().empty())
		{
			cout << endl;
			cout << header << "### Module = " << module.path << endl;

			if (!module.SrcFileList().empty())
			{
				cout << header << "### Source Files" << endl;
				for (const auto& element : module.SrcFileList())
				{
					cout << header << element << endl;
				}
			}

			if (!module.HeaderFileList().empty())
			{
				cout << endl;
				cout << header << "### Header Files" << endl;
				for (const auto& element : module.HeaderFileList())
				{
					cout << header << element << endl;
				}
			}

			havingProjDir = true;
		}

		if (module.GetBuildType() == BuildType::Ignored)
			return false;

		if (module.IsIncludePath())
		{
			includeDirs.push_back(module.path);
		}

		for (const auto& element : module.DirList())
		{
			Module childProjDir(element); 
			string newHeader = header;
			newHeader.append("  ");

			if (TraverseDirTree(childProjDir, newHeader.append("  ")))
			{		
				auto& projList = module.SubModuleList();
				projList.push_back(childProjDir);
				
				havingProjDir = true;
			}
		}

		return havingProjDir;
	};
}
