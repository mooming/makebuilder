// Created by mooming.go@gmail.com 2016

#pragma once

#include "ProjectBuilder.h"

#include <string>
#include <vector>

namespace mb
{
class DependencyGraph final
{
public:
    explicit DependencyGraph(const ProjectBuilder& project);
    ~DependencyGraph() = default;

    void GenerateDotFile(const char* outputPath) const;
    void PrintToConsole() const;

private:
    const ProjectBuilder& project;

    void AddModuleNode(std::string& output, const Module& module) const;
    void AddDependencyEdges(std::string& output, const Module& module) const;
    std::string GetModuleLabel(const Module& module) const;
    std::string GetNodeStyle(EBuildType buildType) const;
};
} // namespace mb