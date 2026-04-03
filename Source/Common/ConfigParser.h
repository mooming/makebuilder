// Created by mooming.go@gmail.com 2016

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mb
{
/// @brief Parses key=value configuration files
/// @details Loads and parses .config files in key=value format.
/// Supports multiple file loading and provides access to parsed values.
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
    ConfigParser() : isValid(false) {}
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
    [[nodiscard]] bool IsValid() const { return isValid; }

    /// @brief Get the raw key-value map
    [[nodiscard]] const TKeyMap& GetKeyMap() const { return keymap; }

private:
    /// @brief Parse a single config file
    /// @param filePath Path to the file to parse
    void Parse(const char* filePath);
};
} // namespace mb