// Created by mooming.go@gmail.com 2016

#pragma once

#include <string>
#include "Module.h"
#include "ProjectBuilder.h"

namespace mb
{
// Generates CMakeLists.txt file for a module
class CMakeGenerator final
{
private:
	const mb::ProjectBuilder& build;
	const mb::Module& module;

public:
	CMakeGenerator(const mb::ProjectBuilder& build, const mb::Module& module);
	~CMakeGenerator() = default;

	void Generate() const;
	std::string TranslatePath(std::string path) const;
};
} // namespace mb
