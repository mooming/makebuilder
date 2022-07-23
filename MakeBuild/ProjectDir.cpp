// Created by mooming.go@gmail.com 2016

#include "ProjectDir.h"

#include "StringUtil.h"
#include <algorithm>
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

	ProjectDir::ProjectDir(const Directory& dir)
		: Directory(dir)
        , buildType(NONE)
	{
        Files files;
		for (const auto& file : FileList())
		{
			using namespace Util;
			string  name(file.GetPath());

			if (EndsWith(ToLowerCase(name), ".c")
				|| EndsWith(ToLowerCase(name), ".cpp"))
			{
				srcFiles.push_back(file);
			}
			else if (EndsWith(ToLowerCase(name), ".h")
				|| EndsWith(ToLowerCase(name), ".hpp")
				|| EndsWith(ToLowerCase(name), ".inl"))
			{
				headerFiles.push_back(file);
			}
            else
            {
                files.push_back(file);
            }
		}

		sort(srcFiles.begin(), srcFiles.end());
		sort(headerFiles.begin(), headerFiles.end());
		sort(files.begin(), files.end());

		SetUpBuildType(files);
	}

	void ProjectDir::SetUpBuildType(const Files& files)
	{
        definitions.empty();
        dependencies.empty();
        libraries.empty();
        frameworks.empty();
        
		for (const auto& element : files)
		{
			using namespace Util;

            if (EqualsIgnoreCase(element.GetPath(), "definitions.txt"))
            {
                string filePath = path;
                filePath.append("/");
                filePath.append(element.GetPath());

                LoadList(filePath.c_str(), definitions);
                continue;
            }

			if (EqualsIgnoreCase(element.GetPath(), "dependencies.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.GetPath());

				LoadList(filePath.c_str(), dependencies);
				continue;
			}

			if (EqualsIgnoreCase(element.GetPath(), "libraries.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.GetPath());

				LoadList(filePath.c_str(), libraries);
				continue;
			}

			if (EqualsIgnoreCase(element.GetPath(), "frameworks.txt"))
			{
				string filePath = path;
				filePath.append("/");
				filePath.append(element.GetPath());

				LoadList(filePath.c_str(), frameworks);
				continue;
			}

			if (buildType == NONE)
			{
				if (EqualsIgnoreCase(element.GetPath(), "ignore.txt"))
				{
					buildType = IGNORE;
				}

				if (EqualsIgnoreCase(element.GetPath(), "executable.txt"))
				{
					buildType = EXECUTABLE;
				}

				if (EqualsIgnoreCase(element.GetPath(), "static_library.txt"))
				{
					buildType = STATIC_LIBRARY;
				}

				if (EqualsIgnoreCase(element.GetPath(), "shared_library.txt"))
				{
					buildType = SHARED_LIBRARY;
				}
			}
		}

		if (buildType != IGNORE && SrcFileList().empty() && !HeaderFileList().empty())
		{
			buildType = HEADER_ONLY;
		}
        
        Sort();
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

	void ProjectDir::Sort()
	{
		auto SortVector = [](auto& v)
		{
			sort(v.begin(), v.end());
		};
		
		SortVector(dependencies);
		SortVector(libraries);
		SortVector(frameworks);
		SortVector(definitions);
	}
}
