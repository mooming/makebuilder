// Created by mooming.go@gmail.com 2016

#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace mb
{
    class BuildConfig final
    {
    public:
        using TString = std::string;
        using TValue = std::optional<TString>;

        bool isValid;
        std::unordered_map<TString, TString> keymap;

    public:
        explicit BuildConfig(const char* path);
        BuildConfig(const char* path, const char* fileName);
        ~BuildConfig() = default;

        [[nodiscard]] TValue GetValue(const TString& key) const;
        [[nodiscard]] TString GetValue(const TString& key, const TString& defaultValue) const;

        auto IsValid() const { return isValid; }

    private:
        void Parse(const char* fileName);
    };
} // namespace mb
