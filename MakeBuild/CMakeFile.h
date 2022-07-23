// Created by mooming.go@gmail.com 2016

#pragma once

#include "Build.h"
#include "ProjectDir.h"
#include <string>

namespace CMake
{
	class CMakeFile final
	{
	private:
		std::string requiredCmakeVersion;
		std::string cxxStandardVersion;

		const Builder::Build& build;
		const Builder::ProjectDir& dir;

	public:
		CMakeFile(const Builder::Build& build, const Builder::ProjectDir& targetDir);
		~CMakeFile() = default;

		void Make();
		std::string TranslatePath(std::string path);
	};
}
