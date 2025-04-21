// Created by mooming.go@gmail.com 2016

#include <iostream>

#include "Build.h"
#include "OSAbstractLayer.h"

using namespace std;

int main(int argc, const char* argv[])
{
    if (argc < 2)
    {
        cout << "MakeBuild 1.0.1" << endl;
        cout << "Directory based meta-build system" << endl;
        cout << "Helping write CMakeLists.txt" << endl << endl;
        cout << "Usage: mbuild <projects root path>" << endl << endl;

        cout << "== .txt Specifiers ==" << endl;
        cout << "Each directory is processed as a single module." << endl;
        cout << "Place following .txt files in a directory. It describes a characteristic of the module." << endl;
        cout << "CMake projects will follow parent's build type." << endl;
        cout << "[.module.config] : It denotes the directory is a module. This file should contains these." << endl;
        cout << "name = \"Module Name\"" << endl;
        cout << "buildType = \"buildType\"";
        cout << " (Available Option: Ignored, HeaderOnly, Executable, StaticLibrary, SharedLibrary, ExternalLibs)" << endl;
        cout << " [None] This directory is not a module but it can contain valid modules. " << endl;
        cout << " [Ignored] This directory will be ignored." << endl;
        cout << " [HeaderOnly] A module consists of header files only" << endl;
        cout << " [Executable] A module which creates an executable" << endl;
        cout << " [StaticLibrary] A static library module" << endl;
        cout << " [SharedLibrary] A dynamic(or shared) library module" << endl;
        cout << " [ExternalLibraries] This module contains external libraries. It'll be added to include paths." << endl;
        cout << "precompileDefinitions = DEFINITION0, DEFINITION1, ..." << endl << endl;
        cout << "[include.txt] : This directory should be set as an include path of itself and children." << endl;
        cout << "[dependencies.txt] : Each line in this file defines a module dependency." << endl;
        cout << "[libraries.txt] : Each line denotes a library name" << endl;
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
