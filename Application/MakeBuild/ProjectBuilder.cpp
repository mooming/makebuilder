// Created by mooming.go@gmail.com 2016

#include "ProjectBuilder.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "CMakeLists.h"
#include "OSAbstractLayer.h"
#include "ConfigParser.h"
#include "StringUtil.h"


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

	auto basePath = Util::TrimPath(path);
	// Migrate deprecated .txt specifiers
	for (auto module : modules)
	{
		MigrateModuleSpecifiers(basePath, *module);
	}
}

void ProjectBuilder::GenerateCMakeFiles()
{
	// Phase 1: Migrate all module specifiers first to populate global includeDirs
	for (auto module : modules)
	{
		assert(module != nullptr);
		module->ReloadConfigValues();
	}

	// Phase 2: Generate CMakeLists.txt files now that all global paths are known
	for (auto module : modules)
	{
		assert(module != nullptr);

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
				ranges::find(ignoredSubDirs, subModulePath) != ignoredSubDirs.end();
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

void ProjectBuilder::MigrateModuleSpecifiers(const string& basePath, Module& module)
{
	const string modulePath = Util::TrimPath(OS::GetFullPath(module.GetPath()));
	string relativePath = modulePath.starts_with(basePath)
	? modulePath.substr(basePath.size()) : modulePath;

	if (relativePath.starts_with('/'))
	{
		relativePath = relativePath.substr(1, relativePath.size() - 1);
	}

	const vector<string> txtFiles = {
		"include.txt",
		"dependency.txt",
		"library.txt",
		"linkDirectory.txt",
		"framework.txt"};

	Module::Strings includePaths;
	Module::Strings dependencies;
	Module::Strings libraries;
	Module::Strings linkDirectories;
	Module::Strings frameworks;

	// Groups related migration data to eliminate duplication
	struct MigrationTarget
	{
		string key;
		string txtFile;
		Module::Strings* list;
	};

	const vector<MigrationTarget> targets = {
		{"include", "include.txt", &includePaths},
		{"dependency", "dependency.txt", &dependencies},
		{"library", "library.txt", &libraries},
		{"linkDirectory", "linkDirectory.txt", &linkDirectories},
		{"framework", "framework.txt", &frameworks}
	};

	// Unified loop: process all file types in one pass
	for (const auto& target : targets)
	{
		// Build absolute path to .txt file
		string filePath = modulePath;

		auto lastChar = filePath.back();
		if (lastChar != '/' && lastChar != '\\')
		{
			filePath.append("/");
		}

		filePath.append(target.txtFile);

		cout << "[Migrate] Reading " << target.txtFile << " from " << modulePath
			<< " (module: " << module.GetName() << ")" << endl;

		ifstream ifs(filePath);
		if (!ifs.is_open())
		{
			cout << "[Migrate] File not found: " << filePath << endl;
			continue;
		}

		// If 'include.txt' exists
		const bool isInclude = target.key == "include";
		if (isInclude)
		{
			includePaths.emplace_back(relativePath);
			includeDirs.push_back(relativePath);
			cout << "[Migrate] Marked include.txt found, added module directory to includes: " << relativePath	 << endl;
		}

		string line;
		size_t lineNum = 0;

		while (getline(ifs, line))
		{
			lineNum++;
			line = Util::Trim(line);
			if (isInclude)
			{
				string temp;
				swap(temp, line);
				line = modulePath;
				line.append("/");
				line.append(temp);
				line = OS::GetFullPath(line);

				if (line.starts_with(basePath))
				{
					line = line.substr(basePath.size());
				}

				if (line.starts_with('/') && line.size() > 1)
				{
					line = line.substr(1, line.size() - 1);
				}
			}

			if (!line.empty())
			{
				target.list->push_back(line);

				if (target.key == "include")
					cout << "[Migrate] Parsed include: " << line << endl;
				else if (target.key == "dependency")
					cout << "[Migrate] Parsed dependency: " << line << endl;
				else if (target.key == "library")
					cout << "[Migrate] Parsed library: " << line << endl;
				else if (target.key == "linkDirectory")
					cout << "[Migrate] Parsed linkDirectory: " << line << endl;
				else if (target.key == "framework")
					cout << "[Migrate] Parsed framework: " << line << endl;
			}
			else
			{
				cout << "[Migrate] Skipped empty line (line " << lineNum << ") in " << filePath << endl;
			}
		}

		cout << "[Migrate] Finished reading " << target.txtFile << " from " << modulePath << ", found " << lineNum << " lines" << endl;
	}

	cout << "[Migrate] Summary for " << module.GetName() << ": " << includePaths.size() << " includes, "
		 << dependencies.size() << " dependencies, " << libraries.size() << " libraries, "
		 << linkDirectories.size() << " link directories, " << frameworks.size() << " frameworks" << endl;

	if (includePaths.empty() && dependencies.empty() && libraries.empty() && linkDirectories.empty() && frameworks.empty())
	{
		cout << "[Migrate] No module specifiers to migrate for " << module.GetName() << endl;
		return;
	}

	cout << "[Migrate] Starting migration for " << module.GetName() << endl;

	// Check if .module.config exists
	ConfigParser configParser;
	configParser.Load(modulePath.c_str(), ".module.config");

	bool hasExistingConfig = configParser.IsValid();
	ConfigParser::TKeyMap existingConfig;
	if (hasExistingConfig)
	{
		existingConfig = configParser.GetKeyMap();
	}

	stringstream newConfig;

	if (hasExistingConfig)
	{
		for (const auto& [key, value] : existingConfig)
		{
			if (key != "include" && key != "dependency" && key != "library" && key != "linkDirectory" && key != "framework")
			{
				newConfig << key << " = " << value << endl;
			}
		}
	}

	// Duplicate detection for merged config values
	for (const auto& target : targets)
	{
		vector<string> finalValues;

		if (hasExistingConfig)
		{
			auto val = config.GetValue(target.key);
			if (val.has_value())
			{
				stringstream ss(val.value());
				string item;
				while (getline(ss, item, ';'))
				{
					item = Util::Trim(item);
					if (!item.empty() && ranges::find(finalValues, item) == finalValues.end())
					{
						finalValues.push_back(item);
					}
				}
			}
		}

		// Append from .txt file, avoiding duplicates
		for (const auto& newItem : *(target.list))
		{
			if (ranges::find(finalValues, newItem) == finalValues.end())
			{
				finalValues.push_back(newItem);
			}
		}

		if (!finalValues.empty())
		{
			newConfig << target.key << " = ";
			for (size_t i = 0; i < finalValues.size(); ++i)
			{
				newConfig << finalValues[i] << (i < finalValues.size() - 1 ? ";" : "");
			}
			newConfig << endl;
		}
	}

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

	module.ReloadConfigFile();

	// Cleanup: remove processed .txt files
	for (const auto& target : targets)
	{
		string filePath = modulePath;
		if (filePath.back() != '/' && filePath.back() != '\\')
			filePath.append("/");

		filePath.append(target.txtFile);

		if (std::filesystem::exists(filePath.c_str()))
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
