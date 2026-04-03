// Created by mooming.go@gmail.com 2016

#include "DependencyGraph.h"

#include "StringUtil.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace mb
{
DependencyGraph::DependencyGraph(const ProjectBuilder& project)
    : project(project)
{
}

void DependencyGraph::GenerateDotFile(const char* outputPath) const
{
    string output;
    output += "digraph MakeBuilder {\n";
    output += "  rankdir=LR;\n";
    output += "  node [shape=box, style=rounded];\n";
    output += "  edge [arrowhead=vee];\n\n";

    for (const auto& module : project.modules)
    {
        AddModuleNode(output, *module);
    }

    for (const auto& module : project.modules)
    {
        AddDependencyEdges(output, *module);
    }

    output += "}\n";

    ofstream ofs(outputPath);
    if (!ofs.is_open())
    {
        cerr << "[DependencyGraph] Failed to create: " << outputPath << endl;
        return;
    }

    ofs << output;
    cout << "[DependencyGraph] Generated: " << outputPath << endl;
    cout << "  Use 'dot -Tpng graph.dot -o graph.png' to convert to PNG" << endl;
}

void DependencyGraph::PrintToConsole() const
{
    cout << endl;
    cout << "=== Dependency Graph ===" << endl;

    for (const auto& module : project.modules)
    {
        cout << "  " << module->GetName() << " ["
             << BuildTypeToString(module->GetBuildType()) << "]" << endl;

        const auto& deps = module->GetDependencies();
        if (!deps.empty())
        {
            cout << "    -> depends on: ";
            for (size_t i = 0; i < deps.size(); ++i)
            {
                cout << deps[i];
                if (i < deps.size() - 1)
                    cout << ", ";
            }
            cout << endl;
        }
    }
}

void DependencyGraph::AddModuleNode(string& output, const Module& module) const
{
    const auto& name = module.GetName();
    auto style = GetNodeStyle(module.GetBuildType());

    string cleanName = name;
    if (!cleanName.empty() && cleanName.front() == '"')
        cleanName.erase(0, 1);
    if (!cleanName.empty() && cleanName.back() == '"')
        cleanName.pop_back();

    output += "  \"";
    output += cleanName;
    output += "\" [label=\"";
    output += cleanName;
    output += "\\n(";
    output += BuildTypeToString(module.GetBuildType());
    output += ")\", ";
    output += style;
    output += "];\n";
}

void DependencyGraph::AddDependencyEdges(
    string& output, const Module& module) const
{
    const auto& deps = module.GetDependencies();
    const auto& fromName = module.GetName();
    string cleanFrom = fromName;
    if (!cleanFrom.empty() && cleanFrom.front() == '"')
        cleanFrom.erase(0, 1);
    if (!cleanFrom.empty() && cleanFrom.back() == '"')
        cleanFrom.pop_back();

    for (const auto& dep : deps)
    {
        string cleanDep = dep;
        if (!cleanDep.empty() && cleanDep.front() == '"')
            cleanDep.erase(0, 1);
        if (!cleanDep.empty() && cleanDep.back() == '"')
            cleanDep.pop_back();

        output += "  \"";
        output += cleanFrom;
        output += "\" -> \"";
        output += cleanDep;
        output += "\";\n";
    }
}

string DependencyGraph::GetModuleLabel(const Module& module) const
{
    ostringstream ss;
    ss << module.GetName() << "\\n(" << BuildTypeToString(module.GetBuildType())
       << ")";
    return ss.str();
}

string DependencyGraph::GetNodeStyle(EBuildType buildType) const
{
    using namespace Util;

    switch (buildType)
    {
    case EBuildType::Executable:
        return "style=filled, fillcolor=lightgreen";
    case EBuildType::StaticLibrary:
        return "style=filled, fillcolor=lightblue";
    case EBuildType::SharedLibrary:
        return "style=filled, fillcolor=lightskyblue";
    case EBuildType::HeaderOnly:
        return "style=filled, fillcolor=lightyellow";
    case EBuildType::ExternalLibrary:
        return "style=filled, fillcolor=lightgray";
    default:
        return "style=filled, fillcolor=white";
    }
}
} // namespace mb