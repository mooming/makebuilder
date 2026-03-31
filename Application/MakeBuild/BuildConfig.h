// Created by mooming.go@gmail.com 2016

#pragma once

#include "Common/ConfigParser.h"

#include <string>

namespace mb
{
class BuildConfig final
{
public:
    using TString = std::string;
    using TValue = ConfigParser::TValue;

private:
    ConfigParser parser;

public:
    explicit BuildConfig(const char* path);
    BuildConfig(const char* path, const char* fileName);
    ~BuildConfig() = default;

    [[nodiscard]] TValue GetValue(const TString& key) const;
    [[nodiscard]] TString GetValue(const TString& key, const TString& defaultValue) const;

    auto IsValid() const { return parser.IsValid(); }

private:
    void Load(const char* path);
};
} // namespace mb
