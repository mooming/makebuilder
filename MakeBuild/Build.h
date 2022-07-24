// Created by mooming.go@gmail.com 2016

#pragma once

#include "MakeBuildConfig.h"
#include "ProjectDir.h"

#include <string>
#include <vector>


namespace Builder
{
	class Build final
	{
	public:
		using ProjDirs = std::vector<ProjectDir*>;
		using Paths = std::vector<std::string>;

	public:
		const MakeBuildConfig config;
		ProjectDir baseDir;
		ProjDirs projectDirs;
		Paths includeDirs;

	public:
		Build(const char* path);
		~Build() = default;

		void BuildCMakeFiles();

	private:
		bool TraverseDirTree(ProjectDir& dir, std::string header);
	};
}
