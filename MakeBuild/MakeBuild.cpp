// Created by mooming.go@gmail.com 2016

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
      
        cout << "== .txt Specifiers ==" << endl;
        cout << "Each directory is processed as a single module." << endl;
        cout << "Place following .txt files in a directory. It describes a characteristic of the module." << endl;
        cout << endl;
        cout << "[ignore.txt] : This directory should be ignored." << endl;
        cout << "[executable.txt] : This directory is a executable module." << endl;
        cout << "[static_library.txt] : This directory is a static library." << endl;
        cout << "[shared_library.txt] : This directory is a dynamic library." << endl;
        cout << "[include.txt] : This directory should be set as an include path of itself and children." << endl;
        cout << "[dependencies.txt] : Each line in this file defines a module dependency." << endl;
        cout << "[libraries.txt] : Each line denotes a library file to be linked." << endl;
        cout << "[definitions.txt] : compiler options" << endl;
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
