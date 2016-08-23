//
//  OSAbstractLayer.h
//  mbuild
//
//  Created by mooming on 2016. 8. 13..
//  Copyright, All rights reserved, Hansol Park
//

//
// Making CMake Config Files
//

#include <iostream>

#include "Build.h"
#include "OSAbstractLayer.h"

using namespace std;


int main(int argc, const char* argv[])
{
	
	if (argc < 2)
	{
		cout << "MakeBuild 1.0" << endl;
		cout << "Directory based meta-build system" << endl;
		cout << "Helping write CMakeLists.txt" << endl << endl;
		
		cout << "Usage: mbuild <projects root path>" << endl << endl;
		
		cout << "== Functions ==" << endl;
		cout << " 1. ignore.txt : denote the directory should be ignored" << endl;
		cout << " 2. executable.txt : executable project" << endl;
		cout << " 3. static_library.txt : static library project" << endl;
		cout << " 4. shared_library.txt : dynamic library project" << endl;
		cout << " 5. dependencies.txt : each line denotes project dependency" << endl;
		cout << " 6. libraries.txt : each line denotes a library file to link" << endl;
		
		cout << endl;
		return 0;
	}

	Builder::Build build(OS::GetFullPath(argv[1]).c_str());
	build.BuildCMakeFiles();

	cout << endl;
	cout << "Have a nice day!" << endl;
	cout << endl;

	return 0;
}


