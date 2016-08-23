//
//  CMakeFile.h
//  mbuild
//
//  Created by mooming on 2016. 8. 15..
//
//

#ifndef CMakeFile_h
#define CMakeFile_h

#include "Build.h"
#include "ProjectDir.h"

namespace CMake
{
	class CMakeFile
	{

	private:
		const Builder::Build& build;
		const Builder::ProjectDir& dir;

	public:
		CMakeFile(const Builder::Build& build, const Builder::ProjectDir& targetDir);
		
		void Make();
		std::string TranslatePath(std::string path);
	};
}

#endif /* CMakeFile_h */
