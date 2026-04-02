// Created by mooming.go@gmail.com 2016
// TestRunner.h
// Runs test cases and updates TestResults.txt

#pragma once

#include <string>
#include <vector>

#include "OSAbstractLayer.h"

namespace mb
{

struct TestResult
{
    std::string testName;
    bool passed;
    std::string message;
};

class TestRunner
{
public:
    using TString = std::string;
    using TestResults = std::vector<TestResult>;

    static bool RunTests(const TString& testDir, const TString& mbuildPath, const TString& buildDir);
    static void WriteResults(const TString& outputPath, const TestResults& results);
    static TString GetTimestamp();

private:
    static bool RunSingleTest(const TString& testPath, const TString& mbuildPath, const TString& buildDir, TestResult& result);
    static bool ConfigureAndBuild(const TString& testPath, const TString& buildDir, std::string& output);
};

} // namespace mb
