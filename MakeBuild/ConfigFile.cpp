// Created by mooming.go@gmail.com 2016

#include "ConfigFile.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

namespace Builder
{
    ConfigFile::ConfigFile(const char* path)
    {
        Parse(path);
    }

    ConfigFile::ConfigFile(const char* path, const char* fileName)
    {
        TString filePath(path);

        auto lastChar = filePath.back();
        if (lastChar != '/' && lastChar != '\\')
        {
            filePath.append("/");
        }
        filePath.append(fileName);
        
        Parse(filePath.c_str());
    }

    ConfigFile::TValue ConfigFile::GetValue(const TString& key) const
    {
        auto found = keymap.find(key);
        if (found == keymap.end())
            return TValue();
        
        return found->second;
    }

    ConfigFile::TString ConfigFile::GetValue(const TString& key, const TString& defaultValue) const
    {
        auto found = keymap.find(key);
        if (found == keymap.end())
            return defaultValue;
        
        return found->second;
    }

    void ConfigFile::Parse(const char* filePath)
    {
        using namespace std;
        cout << "MakeBuildConfig: " << filePath << endl;

		ifstream ifs(filePath);

		if (!ifs.is_open())
		{
			cout << "Failed to load the file, " << filePath << "." << endl;
			abort();
		}

        while (!ifs.eof())
		{
			string line;
			getline(ifs, line);

            using TKeyValue = pair<string, string>;
            auto ParseLine = [&line]() -> TKeyValue
            {
                TKeyValue keyValue;
                if (line.empty())
                    return keyValue;

                auto separator = line.find('=');
                if (separator == string::npos)
                    return keyValue;
                
                keyValue.first = line.substr(0, separator);
                keyValue.second = line.substr(separator + 1);
                return keyValue;
            };

            auto keyValue = ParseLine();
            if (keyValue.first.empty() || keyValue.second.empty())
                continue;
            
            keymap[keyValue.first] = keyValue.second;
		}
    }
}
