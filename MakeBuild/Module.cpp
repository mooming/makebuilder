// Created by mooming.go@gmail.com 2016

#include "Module.h"

#include "StringUtil.h"
#include <algorithm>
#include <fstream>
#include <iostream>


using namespace std;
using namespace OS;

namespace Builder
{
	const string& BuildTypeToString(BuildType type)
	{
		static string strings[] = {
			  "None"
			, "Ignored"
			, "HeaderOnly"
			, "Executable"
			, "StaticLibrary"
			, "SharedLibrary"
		};
		
		constexpr auto numStrings = sizeof(strings) / sizeof(strings[0]);
		const int index = static_cast<uint8_t>(type);

		if (index > numStrings || index < 0)
			return strings[0];

		return strings[index];
	}

	namespace
	{
		BuildType ParseBuildTypeStr(const std::string& str)
		{
			constexpr uint8_t start = static_cast<uint8_t>(BuildType::None);
			constexpr uint8_t end = static_cast<uint8_t>(BuildType::SharedLibrary);
			for (uint8_t it = start; it <= end; ++it)
			{
				auto buildType = static_cast<BuildType>(it);
				auto& buildTypeStr = BuildTypeToString(buildType);
				if (Util::EqualsIgnoreCase(str, buildTypeStr))
					return buildType;
			}

			return BuildType::None;
		}
	}

	Module::Module(const Directory& dir)
		: Directory(dir)
		, config(dir.path.c_str(), ".module.config")
        , buildType(BuildType::Ignored)
		, isIncludePath(false)
	{
		if (!config.IsValid())
		{
			buildType = BuildType::Ignored;
			return;
		}

		cout << "[Module] Open " << path << endl;
		
		auto name = config.GetValue("name", "");
		if (!name.empty())
		{
			moduleName = name;
			cout << "[Module] Name = " << name << endl;
		}
		else
		{
			moduleName = Util::PathToName(path);
			cout << "[Module] Name set by path = " << name << endl;
		}

		{
			auto buildTypeConfig = config.GetValue("buildType", "None");
			buildType = ParseBuildTypeStr(buildTypeConfig);

			cout << "[Module] build type of " << moduleName << " = " << buildTypeConfig << endl;
		}

		precompileDefinitions = config.GetValue("precompileDefinitions", "");

        Files files;
		for (const auto& file : FileList())
		{
			using namespace Util;
			string filename(file.GetPath());

			if (EndsWith(ToLowerCase(filename), ".c")
				|| EndsWith(ToLowerCase(filename), ".cpp"))
			{
				srcFiles.push_back(file);
			}
			else if (EndsWith(ToLowerCase(filename), ".h")
				|| EndsWith(ToLowerCase(filename), ".hpp")
				|| EndsWith(ToLowerCase(filename), ".inl"))
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

		for (auto& file : files)
		{
			if (Util::EqualsIgnoreCase(file.GetPath(), "include.txt"))
			{
				isIncludePath = true;
				break;
			}
		}

		if (buildType != BuildType::Ignored && SrcFileList().empty() && !HeaderFileList().empty())
		{
			buildType = BuildType::HeaderOnly;
		}

		BuildLists(files);
	}

	bool Module::HasSourceFileRecursive() const
	{
		if (!srcFiles.empty() || !headerFiles.empty())
			return true;
		
		for (auto& subDir : projDirs)
		{
			if (subDir.HasSourceFileRecursive())
				return true;
		}

		return false;
	}

	void Module::PrintSubModules(const std::string& header) const
	{
		cout << "[DIR]" << header << path << endl;
		for (auto& subDir : projDirs)
		{
			auto newHeader = header;
			newHeader.append(" ");
			subDir.PrintSubModules(newHeader);
		}
	}

	void Module::BuildLists(const Files& files)
	{
        dependencies.clear();
        libraries.clear();
        frameworks.clear();
        
		for (const auto& element : files)
		{
			using namespace Util;

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
		}

        Sort();
	}

	void Module::LoadList(const char* filePath, Strings& list)
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

	void Module::Sort()
	{
		auto SortVector = [](auto& v)
		{
			sort(v.begin(), v.end());
		};
		
		SortVector(dependencies);
		SortVector(libraries);
		SortVector(frameworks);
	}
}
