// Created by mooming.go@gmail.com 2016

#include "ProjectBuilder.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "CMakeLists.h"
#include "OSAbstractLayer.h"
#include "ConfigParser.h"
#include "StringUtil.h"

#ifdef POSIX
#include <unistd.h>
#endif

using namespace std;

namespace
{

void GetAllChildrenRecursive(mb::Module& module, mb::ProjectBuilder::Modules& inoutModules)
{
	inoutModules.push_back(&module);

	auto& subList = module.GetSubModules();
	for (auto& sub : subList)
	{
		GetAllChildrenRecursive(sub, inoutModules);
	}
};

} // namespace

namespace mb
{

ProjectBuilder::ProjectBuilder(const char* path)
	: config(path, ".project.config")
	, baseModule(path)
{
	TraverseDirectoryTree(baseModule, "=");
	modules.shrink_to_fit();

	cout << endl;
	cout << "=== Project Directories Found ===" << endl;

	baseModule.PrintSubModules(" ");

	GetAllChildrenRecursive(baseModule, modules);

	int i = 0;
	for (auto element : modules)
	{
		assert(element != nullptr);

		cout << i++ << " : " << element->GetPath() << endl;
	}
}

void ProjectBuilder::GenerateCMakeFiles()
{
	for (auto module : modules)
	{
		assert(module != nullptr);

		MigrateModuleSpecifiers(*module);
		module->ReloadConfigValues();

		CMakeGenerator generator(*this, *module);
		generator.Generate();
	}
}

// return true if the given module is valid, or it has a valid submodule.
bool ProjectBuilder::TraverseDirectoryTree(Module& module, const string& logHeader)
{
	module.PrintInfo(logHeader);

	if (module.GetBuildType() == EBuildType::Ignored)
		return false;

	if (module.IsIncludePath())
	{
		includeDirs.push_back(module.GetPath());
	}

	bool isValidModule = false;
	if (module.GetBuildType() == EBuildType::ExternalLibrary)
	{
		isValidModule = true;
	}

	bool hasHeaderOrSource = !module.GetSourceFiles().empty() || !module.GetObjectiveCSourceFiles().empty() ||
							 !module.GetHeaderFiles().empty();

	if (hasHeaderOrSource || module.GetBuildType() == EBuildType::HeaderOnly)
	{
		isValidModule = true;
	}

	auto& submodules = module.GetSubModules();
	for (const auto& subDirectory : module.DirList())
	{
		cout << endl;
		cout << "[" << module.GetName() << "] Visit " << subDirectory.GetPath() << endl;

		Module submodule(&module, subDirectory);
		auto& ignoredSubDirs = module.GetIgnoredSubdirectories();
		auto subModulePath = submodule.GetPath();
		const bool isIgnoredSubDir =
				std::find(ignoredSubDirs.begin(), ignoredSubDirs.end(), subModulePath) != ignoredSubDirs.end();
		if (submodule.GetBuildType() == EBuildType::Ignored || isIgnoredSubDir)
		{
			cout << "[" << module.GetName() << "] SKIP: " << subModulePath << endl;
			continue;
		}

		string childLogHeader = logHeader;
		childLogHeader.append("=");

		if (TraverseDirectoryTree(submodule, childLogHeader))
		{
			submodules.push_back(submodule);
		}
	}

	// A module is valid if it has submodules.
	if (!submodules.empty())
	{
		isValidModule = true;
	}

	auto& subModules = module.GetSubModules();
	sort(subModules.begin(), subModules.end());

	cout << "[" << module.GetName() << "] Valid: " << isValidModule << endl;

	return isValidModule;
}

void ProjectBuilder::MigrateModuleSpecifiers(Module& module)
{
	const string modulePath = module.GetPath();
	const vector<string> txtFiles = {"include.txt", "dependency.txt", "library.txt", "linkDirectory.txt",
								 "framework.txt"};

	// Read .txt files
	Module::Strings includePaths, dependencies, libraries, linkDirectories, frameworks;
	for (const auto& txtFile : txtFiles)
	{
		string filePath = modulePath;
		auto lastChar = filePath.back();
		if (lastChar != '/' && lastChar != '\\')
		{
			filePath.append("/");
		}
		filePath.append(txtFile);

		cout << "[Migrate] Reading " << txtFile << " from " << modulePath << " (module: " << module.GetName() << ")" << endl;

		ifstream ifs(filePath);
		if (!ifs.is_open())
		{
			cout << "[Migrate] File not found: " << filePath << endl;
			continue;
		}

		// Special handling for include.txt: add the module directory to include paths
		// (even if the file is empty - it's a marker), and also parse content for additional includes
		if (txtFile == "include.txt")
		{
			includePaths.push_back(modulePath);
			cout << "[Migrate] Marked include.txt found, added module directory to includes: "
				 << modulePath << endl;
		}

		string line;
		size_t lineNum = 0;
		while (getline(ifs, line))
		{
			lineNum++;
			line = Util::Trim(line);
			if (!line.empty())
			{
				if (txtFile == "include.txt")
				{
					includePaths.push_back(line);
					cout << "[Migrate] Parsed include: " << line << endl;
				}
				else if (txtFile == "dependency.txt")
				{
					dependencies.push_back(line);
					cout << "[Migrate] Parsed dependency: " << line << endl;
				}
				else if (txtFile == "library.txt")
				{
					libraries.push_back(line);
					cout << "[Migrate] Parsed library: " << line << endl;
				}
				else if (txtFile == "linkDirectory.txt")
				{
					linkDirectories.push_back(line);
					cout << "[Migrate] Parsed linkDirectory: " << line << endl;
				}
				else if (txtFile == "framework.txt")
				{
					frameworks.push_back(line);
					cout << "[Migrate] Parsed framework: " << line << endl;
				}
			}
			else
			{
				cout << "[Migrate] Skipped empty line (line " << lineNum << ") in " << filePath << endl;
			}
		}
		cout << "[Migrate] Finished reading " << txtFile << " from " << modulePath << ", found " << lineNum << " lines" << endl;
	}

	cout << "[Migrate] Summary for " << module.GetName() << ": " << includePaths.size() << " includes, "
		 << dependencies.size() << " dependencies, " << libraries.size() << " libraries, "
		 << linkDirectories.size() << " link directories, " << frameworks.size() << " frameworks" << endl;

	// Log whether migration is needed
	if (includePaths.empty() && dependencies.empty() && libraries.empty() && linkDirectories.empty() && frameworks.empty())
	{
		cout << "[Migrate] No module specifiers to migrate for " << module.GetName() << endl;
		return;
	}

	cout << "[Migrate] Starting migration for " << module.GetName() << endl;

	// Check if .module.config exists
	ConfigParser config;
	config.Load(modulePath.c_str(), ".module.config");
	bool hasExistingConfig = config.IsValid();
	ConfigParser::TKeyMap existingConfig;
	if (hasExistingConfig)
	{
		existingConfig = config.GetKeyMap();
	}

	// Read existing values for the keys we'll be merging
	ConfigParser::TKeyMap existingValues;
	for (const auto& key : {"include", "dependency", "library", "linkDirectory", "framework"})
	{
		if (hasExistingConfig)
		{
			auto value = config.GetValue(key);
			if (value.has_value())
			{
				existingValues[key] = value.value();
			}
		}
	}

	// Merge the data
	if (!includePaths.empty() || !dependencies.empty() || !libraries.empty() || !linkDirectories.empty() ||
		!frameworks.empty())
	{
		// Build the new config content
		stringstream newConfig;

		// Copy existing config (preserving name, buildType, etc.)
		if (hasExistingConfig)
		{
			for (const auto& [key, value] : existingConfig)
			{
				if (key == "include" || key == "dependency" || key == "library" || key == "linkDirectory" ||
					key == "framework")
				{
					continue;
				}
				newConfig << key << " = " << value << endl;
			}
		}

		// Add the migrated values (merging with existing values)
		if (!includePaths.empty())
		{
			// Start with existing value if any
			if (existingValues.count("include"))
			{
				newConfig << "include = " << existingValues["include"];
			}

			// Append new values
			for (size_t i = 0; i < includePaths.size(); ++i)
			{
				if (i > 0 || existingValues.count("include"))
					newConfig << ";";
				newConfig << includePaths[i];
			}
			newConfig << endl;
		}

		if (!dependencies.empty())
		{
			// Start with existing value if any
			if (existingValues.count("dependency"))
			{
				newConfig << "dependency = " << existingValues["dependency"];
			}

			// Append new values
			for (size_t i = 0; i < dependencies.size(); ++i)
			{
				if (i > 0 || existingValues.count("dependency"))
					newConfig << ";";
				newConfig << dependencies[i];
			}
			newConfig << endl;
		}

		if (!libraries.empty())
		{
			// Start with existing value if any
			if (existingValues.count("library"))
			{
				newConfig << "library = " << existingValues["library"];
			}

			// Append new values
			for (size_t i = 0; i < libraries.size(); ++i)
			{
				if (i > 0 || existingValues.count("library"))
					newConfig << ";";
				newConfig << libraries[i];
			}
			newConfig << endl;
		}

		if (!linkDirectories.empty())
		{
			// Start with existing value if any
			if (existingValues.count("linkDirectory"))
			{
				newConfig << "linkDirectory = " << existingValues["linkDirectory"];
			}

			// Append new values
			for (size_t i = 0; i < linkDirectories.size(); ++i)
			{
				if (i > 0 || existingValues.count("linkDirectory"))
					newConfig << ";";
				newConfig << linkDirectories[i];
			}
			newConfig << endl;
		}

		if (!frameworks.empty())
		{
			// Start with existing value if any
			if (existingValues.count("framework"))
			{
				newConfig << "framework = " << existingValues["framework"];
			}

			// Append new values
			for (size_t i = 0; i < frameworks.size(); ++i)
			{
				if (i > 0 || existingValues.count("framework"))
					newConfig << ";";
				newConfig << frameworks[i];
			}
			newConfig << endl;
		}

		// Write the merged config
		string configFilePath = modulePath + "/.module.config";
		ofstream configFile(configFilePath);
		if (configFile.is_open())
		{
			configFile << newConfig.str();
			configFile.close();
			cout << "[Migration] Wrote config to " << configFilePath << " for " << module.GetName() << endl;
			cout << "[Migration] Migrated " << includePaths.size() << " includes, " << dependencies.size() 
				 << " dependencies, " << libraries.size() << " libraries, " << linkDirectories.size() 
				 << " link directories, " << frameworks.size() << " frameworks" << endl;
		}
		else
		{
			cout << "[Migration] ERROR: Failed to write config to " << configFilePath << endl;
		}
	}

	// Remove the .txt files
	for (const auto& txtFile : txtFiles)
	{
		string filePath = modulePath;
		auto lastChar = filePath.back();
		if (lastChar != '/' && lastChar != '\\')
		{
			filePath.append("/");
		}
		filePath.append(txtFile);

		if (access(filePath.c_str(), F_OK) == 0)
		{
			remove(filePath.c_str());
			cout << "[Migration] Removed " << filePath << " for " << module.GetName() << endl;
		}
		else
		{
			cout << "[Migration] Did not need to remove " << filePath << " (file doesn't exist)" << endl;
		}
	}
}

} // namespace mb
