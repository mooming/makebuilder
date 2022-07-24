// Created by mooming.go@gmail.com 2016

#include "MakeBuildConfig.h"

#include <fstream>
#include <iostream>


namespace Builder
{
    MakeBuildConfig::MakeBuildConfig(const char* basePath)
    {
        std::string filePath(basePath);
        auto lastChar = filePath.back();
        if (lastChar == '/' || lastChar == '\\')
        {
            filePath.append("MakeBuildConfig.txt");    
        }
        else
        {
            filePath.append("/MakeBuildConfig.txt");
        }

        ReadConfigFile(filePath.c_str());
    }

    void MakeBuildConfig::ReadConfigFile(const char* filePath)
    {
        using namespace std;
        cout << "MakeBuildConfig: " << filePath << endl;

		ifstream ifs(filePath);

		if (!ifs.is_open())
		{
			cout << "Failed to load the file, " << filePath << "." << endl;
			abort();
		}

        while (true)
		{
			string line;
			getline(ifs, line);

            auto ParseLine = [&line]()
            {
                pair<string, string> keyValue;
                if (line.empty())
                    return keyValue;

                auto separator = line.find('=');
                if (separator == string::npos)
                    return keyValue;
                
                keyValue.first = line.substr(0, separator);
                keyValue.second = line.substr(separator + 1);
            };

            auto keyValue = ParseLine();
            ParseKeyValue(keyValue.first, keyValue.second);

			if (ifs.eof())
				break;
		}

    }

    void MakeBuildConfig::ParseKeyValue(const std::string& key, const std::string& value)
    {
        using namespace std;

        cout << "Config: " << key << " = " << value << endl;

        if (key.empty())
            return;

        if (value.empty())
            return;

        
    }
}
