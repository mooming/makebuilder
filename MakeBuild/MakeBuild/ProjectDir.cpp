//
//  ProjectDir.cpp
//  mbuild
//
//  Created by mooming on 2016. 8. 15..
//
//

#include "ProjectDir.h"

#include "StringUtil.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace OS;

namespace Builder
{
	string BuildTypeStr(BuildType type)
	{
		switch(type)
		{
			default:
				return "None";
			case IGNORE:
				return "Ignored";
			case HEADER_ONLY:
				return "Header Only";
			case EXECUTABLE:
				return "Executable";
			case STATIC_LIBRARY:
				return "Static Library";
			case SHARED_LIBRARY:
				return "Shared Library";
		}
	}

	ProjectDir::ProjectDir(const char* path) : ProjectDir(Directory(path))
	{
	}

	ProjectDir::ProjectDir(const Directory& dir) : Directory(dir), buildType(NONE)
	{
		for (const auto& file : FileList())
		{
			using namespace Util;
			string  name(file.path);

			if (EndsWith(ToLowerCase(name), string(".c")))
			{
				srcFiles.push_back(file);
			}
			else if (EndsWith(ToLowerCase(name), string(".cpp")))
			{
				srcFiles.push_back(file);
			}
			else if (EndsWith(ToLowerCase(name), string(".h")))
			{
				headerFiles.push_back(file);
			}
		}

		SetUpBuildType();
	}

	void ProjectDir::SetUpBuildType()
	{
		for (const auto& element : FileList())
		{
			using namespace Util;

			if (EqualsIgnoreCase(element.path, "dependencies.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.path);

				LoadList(filePath.c_str(), dependencies);
				continue;
			}

			if (EqualsIgnoreCase(element.path, "libraries.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.path);

				LoadList(filePath.c_str(), libraries);
				continue;
			}

			if (EqualsIgnoreCase(element.path, "frameworks.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.path);

				LoadList(filePath.c_str(), frameworks);
				continue;
			}

			if (buildType == NONE)
			{
				if (EqualsIgnoreCase(element.path, "ignore.txt"))
				{
					buildType = IGNORE;
				}

				if (EqualsIgnoreCase(element.path, "executable.txt"))
				{
					buildType = EXECUTABLE;
				}

				if (EqualsIgnoreCase(element.path, "static_library.txt"))
				{
					buildType = STATIC_LIBRARY;
				}

				if (EqualsIgnoreCase(element.path, "shared_library.txt"))
				{
					buildType = SHARED_LIBRARY;
				}
			}
		}

		if (buildType != IGNORE && SrcFileList().empty() && !HeaderFileList().empty())
		{
			buildType = HEADER_ONLY;
		}
	}

	void ProjectDir::LoadList(const char* filePath, Strings& list)
	{
		ifstream ifs(filePath);

		if (!ifs.is_open())
		{
			cout << "Failed to load list file, " << filePath << "." << endl;
			abort();
		}

		cout << "List from " << filePath << endl;

		while (true)
		{
			string line;
			getline(ifs, line);

			if (!line.empty())
			{
				cout << "Element: " << line << endl;
				list.push_back(line);
			}

			if (ifs.eof())
				break;
		}
	}
}
