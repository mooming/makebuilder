// Created by mooming.go@gmail.com 2016

#pragma once

#include "Directory.h"
#include "File.h"


namespace Builder
{
	enum BuildType
	{
		NONE,
		IGNORE,
		HEADER_ONLY,
		EXECUTABLE,
		STATIC_LIBRARY,
		SHARED_LIBRARY
	};

	std::string BuildTypeStr(BuildType type);

	class ProjectDir : public OS::Directory
	{
	public:
		using ProjDirs = std::vector<ProjectDir>;
		using Strings = std::vector<std::string>;

	private:
		OS::Files srcFiles;
		OS::Files headerFiles;
		ProjDirs projDirs;
		Strings dependencies;
		Strings libraries;
		Strings frameworks;
        Strings definitions;

		BuildType buildType;

	public:
		ProjectDir(const OS::Directory& dir);
		virtual ~ProjectDir() = default;

		inline const BuildType GetBuildType() const { return buildType; }

		inline const OS::Files& SrcFileList() const { return srcFiles; }
		inline const OS::Files& HeaderFileList() const { return headerFiles; }

		inline ProjDirs& ProjDirList() { return projDirs; }
		inline const ProjDirs& ProjDirList() const { return projDirs; }

        inline const Strings& DefinitionsList() const { return definitions; }
		inline const Strings& DependencyList() const { return dependencies; }
		inline const Strings& LibraryList() const { return libraries; }
		inline const Strings& FrameworkList() const { return frameworks; }

	private:
		void SetUpBuildType(const OS::Files& files);
		void LoadList(const char* filePath, Strings& list);
		void Sort();
	};
}
