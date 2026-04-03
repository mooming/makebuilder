// Created by mooming.go@gmail.com 2016

#include "ConfigParser.h"

#include "StringUtil.h"

#include <fstream>
#include <iostream>

namespace mb
{
void ConfigParser::Load(const char* filePath)
{
    Parse(filePath);
}

void ConfigParser::Load(const char* dirPath, const char* fileName)
{
    TString filePath(dirPath);

    auto lastChar = filePath.back();
    if (lastChar != '/' && lastChar != '\\')
    {
        filePath.append("/");
    }
    filePath.append(fileName);

    Parse(filePath.c_str());
}

void ConfigParser::Clear()
{
    keymap.clear();
    isValid = false;
}

ConfigParser::TValue ConfigParser::GetValue(const TString& key) const
{
    auto found = keymap.find(key);
    if (found == keymap.end())
        return {};

    return (*found).second;
}

ConfigParser::TString ConfigParser::GetValue(
    const TString& key, const TString& defaultValue) const
{
    auto found = keymap.find(key);
    if (found == keymap.end())
        return defaultValue;

    return (*found).second;
}

void ConfigParser::Parse(const char* filePath)
{
    using namespace std;

    ifstream ifs(filePath);
    if (!ifs.is_open())
    {
        cout << "[ConfigParser] Not Found: " << filePath << endl;
        return;
    }

    cout << "[ConfigParser] Parse: " << filePath << endl;

    string line;
    while (getline(ifs, line))
    {
        using TKeyValue = pair<string, string>;
        auto ParseLine = [&line]() -> TKeyValue {
            TKeyValue keyValue;
            if (line.empty())
                return keyValue;

            auto separator = line.find('=');
            if (separator == string::npos)
                return keyValue;

            auto key = line.substr(0, separator);
            auto value = line.substr(separator + 1);
            keyValue.first = Util::Trim(key);
            keyValue.second = Util::Trim(value);

            return keyValue;
        };

        auto keyValue = ParseLine();
        if (keyValue.first.empty() || keyValue.second.empty())
            continue;

        keymap[keyValue.first] = keyValue.second;
    }

    isValid = true;
}
} // namespace mb