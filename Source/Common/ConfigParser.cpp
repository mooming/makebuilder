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

ConfigParser::TString ConfigParser::GetValue(const TString& key, const TString& defaultValue) const
{
	auto found = keymap.find(key);
	if (found == keymap.end())
		return defaultValue;

	return (*found).second;
}

void ConfigParser::SetValue(const TString& key, const TString& value)
{
	keymap[key] = value;
	isValid = true;
}

void ConfigParser::Save(const char* filePath)
{
	std::ofstream ofs(filePath);
	if (!ofs.is_open())
	{
		std::cerr << "[ConfigParser] Failed to save config file, " << filePath << "." << std::endl;
		return;
	}

	for (const auto& [key, value] : keymap)
	{
		ofs << key << "=" << value << "\n";
	}
}

std::pair<ConfigParser::TString, ConfigParser::TString> ConfigParser::ParseLine(const TString& line) const
{
	TString key;
	TString value;

	if (line.empty())
		return {key, value};

	auto separatorPos = line.find('=');
	if (separatorPos == TString::npos)
		return {key, value};

	key = line.substr(0, separatorPos);
	value = line.substr(separatorPos + 1);
	key = Util::Trim(key);
	value = Util::Trim(value);

	return {key, value};
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

	TString line;
	while (getline(ifs, line))
	{
		auto keyValue = ParseLine(line);
		if (keyValue.first.empty() || keyValue.second.empty())
			continue;

		keymap[keyValue.first] = keyValue.second;
	}

	isValid = true;
}

std::vector<ConfigParser::TString> ConfigParser::Split(const TString& str, char delimiter)
{
	std::vector<TString> result;
	TString token;

	for (char ch : str)
	{
		if (ch == delimiter)
		{
			if (!token.empty())
			{
				result.push_back(Util::Trim(token));
				token.clear();
			}
		}
		else
		{
			token += ch;
		}
	}

	if (!token.empty())
	{
		result.push_back(Util::Trim(token));
	}

	return result;
}

std::vector<ConfigParser::TString> ConfigParser::SplitByCommonDelimiters(const TString& str)
{
	std::vector<TString> result;
	TString token;

	for (char ch : str)
	{
		if (ch == ',' || ch == ';' || ch == ':' || ch == ' ')
		{
			if (!token.empty())
			{
				result.push_back(Util::Trim(token));
				token.clear();
			}
		}
		else
		{
			token += ch;
		}
	}

	if (!token.empty())
	{
		result.push_back(Util::Trim(token));
	}

	return result;
}

std::vector<ConfigParser::TString> ConfigParser::GetValues(const TString& key) const
{
	TKeyMap::const_iterator found = keymap.find(key);
	if (found == keymap.end())
		return {};

	return SplitByCommonDelimiters(found->second);
}

ConfigParser::TString ConfigParser::GetFirstValue(const TString& key, const TString& defaultValue) const
{
	TKeyMap::const_iterator found = keymap.find(key);
	if (found == keymap.end())
		return defaultValue;

	TString trimmed = Util::Trim(found->second);
	if (trimmed.empty())
		return defaultValue;

	auto values = SplitByCommonDelimiters(trimmed);
	if (values.empty())
		return defaultValue;

	return values[0];
}
} // namespace mb
