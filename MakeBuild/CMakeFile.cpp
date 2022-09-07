// Created by mooming.go@gmail.com 2016

#include "CMakeFile.h"

#include "StringUtil.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;
using namespace Builder;

namespace
{
	void AddFrameworks(ostream& os, string projName, const vector<string>& frameworks)
	{
		os << "if (APPLE)" << endl;
		os << "include_directories ( /System/Library/Frameworks )" << endl;
		os << "endif (APPLE)" << endl << endl;

		if (!frameworks.empty())
		{
			for (const auto& element : frameworks)
			{
				if (Util::EqualsIgnoreCase(element, "opengl"))
				{
					os << "find_package (OpenGL)" << endl << endl;

					os << "if (OPENGL_FOUND)" << endl;
					os << "include_directories (${OPENGL_INCLUDE_DIR})" << endl;
					os << "target_link_libraries (" << projName << " ${OPENGL_gl_LIBRARY})" << endl;
					os << "endif (OPENGL_FOUND)" << endl << endl;

					os << "if (OPENGL_GLU_FOUND)" << endl;
					os << "target_link_libraries (" << projName << " ${OPENGL_glu_LIBRARY})" << endl;
					os << "endif (OPENGL_GLU_FOUND)" << endl << endl;
				}
				else
				{
					string libName = element;
					libName.append("_LIBRARY");

					os << "find_library(" << libName << " " << element << ")" << endl << endl;
					os << "if (" << libName << ")" << endl;
					os << "target_link_libraries (" << projName << " ${" << libName << "})" << endl;
					os << "endif (" << libName << ")" << endl << endl;
				}
			}
		}
	}
}

namespace CMake
{

	CMakeFile::CMakeFile(const Build& build, const Module& module)
		: build(build)
		, module(module)
	{
	}

	void CMakeFile::Make()
	{
		const auto& buildConfig = build.config;
		const BuildType buildType = module.GetBuildType();

		const char* basePath = "${CMAKE_SOURCE_DIR}";
		using namespace Util;

		string filePath(module.path);
		string moduleName = module.GetName();

		filePath.append("/CMakeLists.txt");

		cout << "Creating: [" << moduleName << "] " << filePath.c_str() << "(" << BuildTypeToString(buildType) << ")" << endl;

		ofstream ofs (filePath.c_str(), ofstream::out);

		auto requiredCMakeVersion = buildConfig.GetValue("requiredCMakeVersion", "3.12");
		ofs << "cmake_minimum_required (VERSION "
			<< requiredCMakeVersion << ")" << endl;
		ofs << "project (" << moduleName << ")" << endl;
		ofs << endl;

		auto cxxStandard = buildConfig.GetValue("cxxStandard", "17");
		ofs << "set (CMAKE_CXX_STANDARD " << cxxStandard << ")" << endl;
		ofs << endl;
		ofs << "if (MSVC)" << endl;
		
		auto compileOptions = buildConfig.GetValue("compileOptions", "-Wall -Werror");
		auto msvcCompileOptions = buildConfig.GetValue("msvcCompileOptions", "/W3 /WX");
		
		if (!msvcCompileOptions.empty())
		{
			ofs << "\tadd_compile_options (";
			ofs << msvcCompileOptions;
			ofs << ")" << endl;
		}

		ofs << "else (MSVC)" << endl;

		if (!compileOptions.empty())
		{
			ofs << "\tadd_compile_options (";
			ofs << compileOptions;
			ofs << ")" << endl;	
		}

		ofs << "endif (MSVC)" << endl;
		ofs << endl;
		
		auto precompileDefs = buildConfig.GetValue("precompileDefinitions");
        auto& moduleDefs = module.GetPrecompileDefinitions();
        if (precompileDefs.has_value() || !moduleDefs.empty())
        {
            ofs << "add_compile_definitions (";

			if (precompileDefs.has_value())
			{
				ofs << *precompileDefs << " ";
			}

			if (!moduleDefs.empty())
			{
				ofs << moduleDefs << " ";
			}

            ofs << ")" << endl;
        }
        
		ofs << "include_directories (";
		ofs << " " << basePath << endl;
		ofs << " " << basePath << "/include" << endl;
        
		for (const auto& element : build.includeDirs)
		{
			ofs << " " << TranslatePath(element) << endl;
		}
        
		ofs << " )" << endl;

		if (buildType != BuildType::HeaderOnly)
		{
			ofs << "link_directories (" << basePath << "/lib)" << endl;
			ofs << endl;
		}
		
		for (const auto& subDir : module.SubModuleList())
		{
			if (!subDir.HasSourceFileRecursive())
				continue;

			ofs << "add_subdirectory (" << PathToName(subDir.path.c_str()) << ")" << endl;
		}

		ofs << endl;

		switch(buildType)
		{
			case BuildType::Executable:
				ofs << "add_executable (" << moduleName.c_str() << endl;

				for (const auto& element : module.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : module.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, moduleName, module.FrameworkList());

				ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/bin)" << endl;
				break;

			case BuildType::StaticLibrary:
				ofs << "add_library (" << moduleName.c_str() << " STATIC " << endl;

				for (const auto& element : module.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : module.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, moduleName, module.FrameworkList());

				ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/lib)" << endl;
				break;

			case BuildType::SharedLibrary:
				ofs << "add_library (" << moduleName.c_str() << " SHARED " << endl;

				for (const auto& element : module.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : module.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, moduleName, module.FrameworkList());

				ofs << "install (TARGETS " << moduleName << " DESTINATION " << basePath << "/bin)" << endl;
				break;

			case BuildType::HeaderOnly:
				ofs << "add_library (" << moduleName.c_str() << " INTERFACE)" << endl;
				ofs << "target_sources (" << moduleName.c_str() << " INTERFACE " << endl;

				for (const auto& element : module.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl;
				break;

			default:
				break;
		};
        
		ofs << endl << endl;

		auto& dependencyList = module.DependencyList();
		auto& libList = module.LibraryList();

		if (!dependencyList.empty() || !libList.empty())
		{
			ofs << "target_link_libraries (" << moduleName;
			for (const auto& dependency : dependencyList)
			{
				if (dependency.empty())
					continue;

				ofs << " " << dependency;
			}

			for (const auto& library : libList)
			{
				if (library.empty())
					continue;

				ofs << " " << library;
			}
			ofs << ")" << endl << endl;

			for (const auto& dependency : dependencyList)
			{
				if (dependency.empty())
					continue;

				ofs << "add_dependencies (" << moduleName.c_str() << " " << dependency << ")" << endl;
			}
			ofs << endl;
		}

		ofs << endl;
		ofs.close();
	}

	string CMakeFile::TranslatePath(string path)
	{
		using namespace Util;
		path = TrimPath(path);

		if (StartsWithIgnoreCase(path, build.baseModule.path))
		{
			string newPath = string("${CMAKE_SOURCE_DIR}");
			newPath.append(path.substr(build.baseModule.path.length()));
			return newPath;
		}

		return path;
	}

}
