// Created by mooming.go@gmail.com 2016

#include "BuildConfig.h"

#include <iostream>

namespace mb
{
BuildConfig::BuildConfig(const char* path)
{
    Load(path);
}

BuildConfig::BuildConfig(const char* path, const char* fileName)
{
    Load(path);
    parser.Load(path, fileName);
}

void BuildConfig::Load(const char* path)
{
    std::cout << "[BuildConfig] Loading from: " << path << std::endl;
    parser.Load(path);
}

BuildConfig::TValue BuildConfig::GetValue(const TString& key) const
{
    return parser.GetValue(key);
}

BuildConfig::TString BuildConfig::GetValue(
    const TString& key, const TString& defaultValue) const
{
    return parser.GetValue(key, defaultValue);
}
} // namespace mb