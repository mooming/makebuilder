// Created by mooming.go@gmail.com 2016

#include "MakeBuildConfig.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

namespace Builder
{
    MakeBuildConfig::MakeBuildConfig(const char* basePath)
    {
        std::string filePath(basePath);
        auto lastChar = filePath.back();
        if (lastChar == '/' || lastChar == '\\')
        {
            filePath.append("MakeBuild.Config");    
        }
        else
        {
            filePath.append("/MakeBuild.Config");
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
            ParseKeyValue(keyValue.first, keyValue.second);

			if (ifs.eof())
				break;
		}

    }

    void MakeBuildConfig::ParseKeyValue(const std::string& key, const std::string& value)
    {
        if (key.empty())
            return;

        if (value.empty())
            return;

        using namespace std;
        cout << "Config: " << key << " = " << value << endl;

        using TSingleParser = function<void(MakeBuildConfig&, const string&)>;
        static unordered_map<string, TSingleParser> parsers;
        if (parsers.empty())
        {
            parsers["requiredCMakeVersion"] = [](MakeBuildConfig& config, const string& value)
            {
                if (value.empty())
                    return;

                config.requiredCMakeVersion = value;
            };

            parsers["cxxStandard"] = [](MakeBuildConfig& config, const string& value)
            {
                if (value.empty())
                    return;

                config.cxxStandard = value;
            };
        }

        auto found = parsers.find(key);
        if (found == parsers.end())
            return;
        
        found->second(*this, value);
    }
}
