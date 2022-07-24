// Created by mooming.go@gmail.com 2016

#pragma once

#include "ProjectDir.h"

#include <string>
#include <vector>


namespace Builder
{
	class Build final
	{
		using ProjDirs = std::vector<ProjectDir>;
		using Paths = std::vector<std::string>;

	public:
		ProjectDir baseDir;
		ProjDirs projectDirs;
		Paths includeDirs;

	public:
		Build(const char* path);
		~Build() = default;

		void BuildCMakeFiles();

	private:
		bool TraverseDirTree(const OS::Directory& dir, std::string header);
	};
}