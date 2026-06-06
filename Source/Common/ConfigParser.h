// Created by mooming.go@gmail.com 2016

#pragma once

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mb
{
/// @brief Parses key=value configuration files
/// @details Loads and parses .config files in key=value format.
/// Supports multiple file loading and provides access to parsed values.
/// The key-value separator is always '='. Values can contain multiple items
/// separated by common delimiters (comma, semicolon, colon, space).
class ConfigParser final
{
public:
	using TString = std::string;
	using TValue = std::optional<TString>;
	using TKeyMap = std::unordered_map<TString, TString>;

private:
	bool isValid;
	TKeyMap keymap;

public:
	ConfigParser()
		: isValid(false)
	{}

	~ConfigParser() = default;

	/// @brief Load a config file from a full path
	/// @param filePath Full path to the config file
	void Load(const char* filePath);

	/// @brief Load a config file from a directory and filename
	/// @param dirPath Directory containing the file
	/// @param fileName Name of the config file
	void Load(const char* dirPath, const char* fileName);

	/// @brief Clear all loaded configuration data
	void Clear();

	/// @brief Get a value by key
	/// @param key The key to look up
	/// @return Optional containing the value if found
	[[nodiscard]] TValue GetValue(const TString& key) const;

	/// @brief Get a value by key with a default fallback
	/// @param key The key to look up
	/// @param defaultValue Value to return if key not found
	/// @return The value or defaultValue if not found
	[[nodiscard]] TString GetValue(const TString& key, const TString& defaultValue) const;

	/// @brief Check if any config files were loaded successfully
	[[nodiscard]] bool IsValid() const
	{
		return isValid;
	}

	/// @brief Set a value in the config map
	void SetValue(const TString& key, const TString& value);

	/// @brief Save the current config map to a file
	void Save(const char* filePath);

	/// @brief Get the raw key-value map
	[[nodiscard]] const TKeyMap& GetKeyMap() const
	{
		return keymap;
	}

	/// @brief Split a string by a delimiter into a vector of trimmed substrings
	/// @param str The string to split
	/// @param delimiter The delimiter character
	/// @return Vector of trimmed substrings
	[[nodiscard]] static std::vector<TString> Split(const TString& str, char delimiter);

	/// @brief Split a string by common delimiters (comma, semicolon, colon, space)
	/// @param str The string to split
	/// @return Vector of trimmed substrings
	[[nodiscard]] static std::vector<TString> SplitByCommonDelimiters(const TString& str);

	/// @brief Get all values for a key, split by common delimiters
	/// @param key The key to look up
	/// @return Vector of trimmed substrings (values)
	[[nodiscard]] std::vector<TString> GetValues(const TString& key) const;

	/// @brief Get the first value for a key, splitting by common delimiters
	/// @param key The key to look up
	/// @param defaultValue Value to return if key not found
	/// @return The first value or defaultValue if not found
	[[nodiscard]] TString GetFirstValue(const TString& key, const TString& defaultValue) const;

private:
	/// @brief Parse a single config file
	/// @param filePath Path to the file to parse
	void Parse(const char* filePath);

	/// @brief Parse a line using '=' as the key-value separator
	/// @param line The line to parse
	/// @return Pair of key and value (empty if invalid)
	[[nodiscard]] std::pair<TString, TString> ParseLine(const TString& line) const;
};
} // namespace mb
