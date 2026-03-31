// Created by mooming.go@gmail.com 2016
// TestRunner.cpp
// Runs test cases and updates TestResults.txt

#include "TestRunner.h"
#include "OSAbstractLayer.h"
#include "StringUtil.h"

#include "OSAbstractLayer.h"
#include "StringUtil.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace mb
{

bool TestRunner::RunTests(const TString& testDir, const TString& mbuildPath, const TString& buildDir)
{
    cout << "[TestRunner] Running tests from: " << testDir.c_str() << endl;

    TestResults results;
    auto testCases = OS::ListFilesInDirectory(testDir.c_str());

    cout << "[TestRunner] Found " << testCases.size() << " items in test directory" << endl;
    sort(testCases.begin(), testCases.end());

    for (const auto& testCase : testCases)
    {
        std::string fullPath = testDir + "/" + testCase;
        if (!OS::IsDirectory(fullPath.c_str()))
            continue;

        using namespace Util;
        TString testName = PathToName(testCase.c_str());
        if (testName.empty() || testName[0] == '.')
            continue;

        cout << "[TestRunner] Running test: " << testName.c_str() << endl;

        TestResult result;
        result.testName = testName;

        if (RunSingleTest(fullPath, mbuildPath, buildDir, result))
        {
            result.passed = true;
            result.message = "OK";
            cout << "[TestRunner] PASSED: " << testName.c_str() << endl;
        }
        else
        {
            result.passed = false;
            cout << "[TestRunner] FAILED: " << testName.c_str() << " - " << result.message.c_str() << endl;
        }

        results.push_back(result);
    }

    TString resultsPath = "TestResults.txt";
    WriteResults(resultsPath, results);

    int passed = 0;
    int failed = 0;
    for (const auto& r : results)
    {
        if (r.passed)
            passed++;
        else
            failed++;
    }

    cout << endl;
    cout << "=== Test Summary ===" << endl;
    cout << "Passed: " << passed << "/" << results.size() << endl;
    cout << "Failed: " << failed << "/" << results.size() << endl;
    cout << "Results written to: " << resultsPath.c_str() << endl;

    return failed == 0;
}

bool TestRunner::RunSingleTest(const TString& testPath, const TString& mbuildPath, const TString& buildDir, TestResult& result)
{
    string output;

    // First run makebuild to generate CMakeLists.txt
    TString mbuildCmd = "\"" + mbuildPath + "\" \"" + testPath + "\"";
    cout << "[TestRunner] Running mbuild: " << mbuildCmd.c_str() << endl;

    int mbuildResult = system(mbuildCmd.c_str());
    if (mbuildResult != 0)
    {
        result.message = "makebuild failed with code " + to_string(mbuildResult);
        return false;
    }

    if (!ConfigureAndBuild(testPath, buildDir, output))
    {
        result.message = "Build failed: " + output;
        return false;
    }

    result.message = "Build successful";
    return true;
}

bool TestRunner::ConfigureAndBuild(const TString& testPath, const TString& buildDir, string& output)
{
    using namespace Util;

    TString testBuildDir = testPath + "/" + buildDir;
    TString cmakeCmd = "cmake -B \"" + testBuildDir + "\" -G \"Ninja Multi-Config\" -S \"" + testPath + "\"";

    cout << "[TestRunner] Running: " << cmakeCmd.c_str() << endl;

    int cmakeResult = system(cmakeCmd.c_str());
    if (cmakeResult != 0)
    {
        ostringstream ss;
        ss << "cmake configure failed with code " << cmakeResult;
        output = ss.str();
        return false;
    }

    TString buildCmd = "cmake --build \"" + testBuildDir + "\" --config Release";

    cout << "[TestRunner] Running: " << buildCmd.c_str() << endl;

    int buildResult = system(buildCmd.c_str());
    if (buildResult != 0)
    {
        ostringstream ss;
        ss << "cmake build failed with code " << buildResult;
        output = ss.str();
        return false;
    }

    output = "OK";
    return true;
}

void TestRunner::WriteResults(const TString& outputPath, const TestResults& results)
{
    ofstream ofs(outputPath.c_str(), ofstream::out);

    if (!ofs.is_open())
    {
        cerr << "[TestRunner] Failed to open results file: " << outputPath.c_str() << endl;
        return;
    }

    ofs << "Test Results - " << GetTimestamp().c_str() << endl;
    ofs << "================================" << endl;
    ofs << endl;

    int passed = 0;
    int failed = 0;

    for (const auto& result : results)
    {
        ofs << "Test: " << result.testName.c_str() << endl;
        ofs << "Status: " << (result.passed ? "PASSED" : "FAILED") << endl;
        ofs << "Message: " << result.message.c_str() << endl;
        ofs << endl;

        if (result.passed)
            passed++;
        else
            failed++;
    }

    ofs << "================================" << endl;
    ofs << "Total: " << results.size() << endl;
    ofs << "Passed: " << passed << endl;
    ofs << "Failed: " << failed << endl;

    ofs.close();
}

TestRunner::TString TestRunner::GetTimestamp()
{
    auto now = chrono::system_clock::now();
    time_t nowTime = chrono::system_clock::to_time_t(now);

    char buffer[128];
    struct tm* timeinfo = localtime(&nowTime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return TString(buffer);
}

} // namespace mb
