// Created by mooming.go@gmail.com 2016

#pragma once

#include "ConfigFile.h"
#include "Directory.h"
#include "File.h"
#include <cstdint>


namespace Builder
{
	enum class BuildType : uint8_t
	{
		None = 0,
		Ignored,
		HeaderOnly,
		Executable,
		StaticLibrary,
		SharedLibrary
	};

	const std::string& BuildTypeToString(BuildType type);

	class Module : public OS::Directory
	{
	public:
		using TString = std::string;
		using Modules = std::vector<Module>;
		using Strings = std::vector<TString>;

	private:
		const ConfigFile config;
		TString moduleName;

		OS::Files srcFiles;
		OS::Files headerFiles;
		Modules projDirs;
		Strings dependencies;
		Strings libraries;
		Strings frameworks;
        TString precompileDefinitions;

		BuildType buildType;
		bool isIncludePath;

	public:
		Module(const OS::Directory& dir);
		virtual ~Module() = default;

		bool HasSourceFileRecursive() const;
		void PrintSubModules(const TString& header) const;

		inline auto& GetName() const { return moduleName; }
		inline auto GetBuildType() const { return buildType; }
		inline auto IsIncludePath() const { return isIncludePath; }

		inline auto& SrcFileList() const { return srcFiles; }
		inline auto& HeaderFileList() const { return headerFiles; }
		inline auto& SubModuleList() { return projDirs; }
		inline auto& SubModuleList() const { return projDirs; }
		inline auto& DependencyList() const { return dependencies; }
		inline auto& LibraryList() const { return libraries; }
		inline auto& FrameworkList() const { return frameworks; }
		inline auto& GetPrecompileDefinitions() const { return precompileDefinitions; }

	private:
		void BuildLists(const OS::Files& files);
		void LoadList(const char* filePath, Strings& list);
		void Sort();
	};
}
