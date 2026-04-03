// Created by mooming.go@gmail.com 2016

#include "BuildConfig.h"

#include <iostream>

namespace mb
{

BuildConfig::BuildConfig(const char* path, const char* fileName)
{
    parser.Load(path, fileName);
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