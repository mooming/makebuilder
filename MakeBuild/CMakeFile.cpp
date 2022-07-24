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

	CMakeFile::CMakeFile(const Build& build, const ProjectDir& targetDir)
		: requiredCmakeVersion(build.config.GetRequiredCMakeVersion())
		, cxxStandardVersion(build.config.GetCXXStandard())
		, build(build)
		, dir(targetDir)
	{
	}

	void CMakeFile::Make()
	{
		const BuildType buildType = dir.GetBuildType();

		const char* basePath = "${CMAKE_SOURCE_DIR}";
		using namespace Util;

		string filePath(dir.path);
		string projName(PathToName(dir.path));

		filePath.append("/CMakeLists.txt");

		cout << "Creating: " << filePath.c_str() << "(" << BuildTypeStr(buildType) << ")" << endl;

		ofstream ofs (filePath.c_str(), ofstream::out);
		ofs << "cmake_minimum_required (VERSION " << requiredCmakeVersion << ")" << endl;
		ofs << "project (" << projName << ")" << endl;
		ofs << endl;

		ofs << "if(CMAKE_COMPILER_IS_GNUCXX)" << endl;
		ofs << "	set(CMAKE_CXX_FLAGS \"${ CMAKE_CXX_FLAGS } -Wall -Werror\")" << endl;
		ofs << "endif(CMAKE_COMPILER_IS_GNUCXX)" << endl;

		ofs << "set (CMAKE_CXX_STANDARD " << cxxStandardVersion << ")" << endl;

        auto& definesList = dir.DefinitionsList();
        if (!definesList.empty())
        {
            ofs << "add_definitions (";
            for (auto& def : definesList)
            {
                ofs << def << " ";
            }
            ofs << ")" << endl;
        }
        ofs << endl;

		ofs << "include_directories (";
		ofs << " " << basePath << endl;
		ofs << " " << basePath << "/include" << endl;
        
		for (const auto& element : build.includeDirs)
		{
			ofs << " " << TranslatePath(element) << endl;
		}
        
		ofs << " )" << endl;
		ofs << endl;

		if (buildType != HEADER_ONLY)
		{
			ofs << "link_directories (" << basePath << "/lib)" << endl;
			ofs << endl;
		}
		
		ofs << endl;

		for (const auto& subDir : dir.ProjDirList())
		{
			if (!subDir.HasSourceFileRecursive())
				continue;

			ofs << "add_subdirectory (" << PathToName(subDir.path.c_str()) << ")" << endl;
		}

		ofs << endl;

		switch(buildType)
		{
			case EXECUTABLE:
				ofs << "add_executable (" << projName.c_str() << endl;

				for (const auto& element : dir.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : dir.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, projName, dir.FrameworkList());

				ofs << "install (TARGETS " << projName << " DESTINATION " << basePath << "/bin)" << endl;
				break;

			case STATIC_LIBRARY:
				ofs << "add_library (" << projName.c_str() << " STATIC " << endl;

				for (const auto& element : dir.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : dir.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, projName, dir.FrameworkList());

				ofs << "install (TARGETS " << projName << " DESTINATION " << basePath << "/lib)" << endl;
				break;

			case SHARED_LIBRARY:
				ofs << "add_library (" << projName.c_str() << " SHARED " << endl;

				for (const auto& element : dir.SrcFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				for (const auto& element : dir.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl << endl;

				AddFrameworks(ofs, projName, dir.FrameworkList());

				ofs << "install (TARGETS " << projName << " DESTINATION " << basePath << "/bin)" << endl;
				break;

			case HEADER_ONLY:
				ofs << "add_library (" << projName.c_str() << " INTERFACE)" << endl;
				ofs << "target_sources (" << projName.c_str() << " INTERFACE " << endl;

				for (const auto& element : dir.HeaderFileList())
				{
					ofs << " " << PathToName(element.GetPath().c_str()) << endl;
				}

				ofs << ")" << endl;
				break;

			default:
				break;
		};
        
		ofs << endl << endl;

		auto& dependencyList = dir.DependencyList();
		auto& libList = dir.LibraryList();

		if (!dependencyList.empty() || !libList.empty())
		{
			ofs << "target_link_libraries (" << projName;
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

				ofs << "add_dependencies (" << projName.c_str() << " " << dependency << ")" << endl;
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

		if (StartsWithIgnoreCase(path, build.baseDir.path))
		{
			string newPath = string("${CMAKE_SOURCE_DIR}");
			newPath.append(path.substr(build.baseDir.path.length()));
			return newPath;
		}

		return path;
	}

}
