// ConfigParserTest.cpp - Test the enhanced ConfigParser with value splitting
#include <cassert>
#include <iostream>
#include <vector>
#include "ConfigParser.h"

using namespace mb;

void TestDefaultValue()
{
	std::cout << "\n=== Test 1: Default Value (no delimiter) ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("name", "MyApp");

	auto result = parser.GetValue("name");
	assert(result == "MyApp");

	auto values = parser.GetValues("name");
	assert(values.size() == 1);
	assert(values[0] == "MyApp");

	std::cout << "✓ Default value test passed" << std::endl;
}

void TestCommaSeparatedValues()
{
	std::cout << "\n=== Test 2: Comma Separated Values ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("dependencies", "lib1,lib2,lib3,lib4");

	auto result = parser.GetValue("dependencies");
	assert(result == "lib1,lib2,lib3,lib4");

	auto values = parser.GetValues("dependencies");
	assert(values.size() == 4);
	assert(values[0] == "lib1");
	assert(values[1] == "lib2");
	assert(values[2] == "lib3");
	assert(values[3] == "lib4");

	std::cout << "✓ Comma separated values test passed" << std::endl;
}

void TestSemicolonSeparatedValues()
{
	std::cout << "\n=== Test 3: Semicolon Separated Values ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("includes", "src/main.cpp;src/util.cpp;src/helper.cpp");

	auto result = parser.GetValue("includes");
	assert(result == "src/main.cpp;src/util.cpp;src/helper.cpp");

	auto values = parser.GetValues("includes");
	assert(values.size() == 3);
	assert(values[0] == "src/main.cpp");
	assert(values[1] == "src/util.cpp");
	assert(values[2] == "src/helper.cpp");

	std::cout << "✓ Semicolon separated values test passed" << std::endl;
}

void TestColonSeparatedValues()
{
	std::cout << "\n=== Test 4: Colon Separated Values ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("settings", "debug: true; verbose: false");

	auto result = parser.GetValue("settings");
	assert(result == "debug: true; verbose: false");

	auto values = parser.GetValues("settings");
	assert(values.size() == 2);
	assert(values[0] == "debug: true");
	assert(values[1] == "verbose: false");

	std::cout << "✓ Colon separated values test passed" << std::endl;
}

void TestSpaceSeparatedValues()
{
	std::cout << "\n=== Test 5: Space Separated Values ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("flags", "-O2 -Wall -Werror");

	auto result = parser.GetValue("flags");
	assert(result == "-O2 -Wall -Werror");

	auto values = parser.GetValues("flags");
	assert(values.size() == 3);
	assert(values[0] == "-O2");
	assert(values[1] == "-Wall");
	assert(values[2] == "-Werror");

	std::cout << "✓ Space separated values test passed" << std::endl;
}

void TestMixedDelimiters()
{
	std::cout << "\n=== Test 6: Mixed Delimiters ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("items", "item1,item2; item3: item4 item5");

	auto result = parser.GetValue("items");
	assert(result == "item1,item2; item3: item4 item5");

	auto values = parser.GetValues("items");
	assert(values.size() == 5);
	assert(values[0] == "item1");
	assert(values[1] == "item2");
	assert(values[2] == "item3");
	assert(values[3] == "item4");
	assert(values[4] == "item5");

	std::cout << "✓ Mixed delimiters test passed" << std::endl;
}

void TestLoadFromFileWithVariousDelimiters()
{
	std::cout << "\n=== Test 7: Load from file with various delimiters ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("name", "MyApp");
	parser.SetValue("version", "1.0");
	parser.SetValue("author", "Test Author");
	parser.SetValue("libs", "lib1,lib2,lib3");
	parser.SetValue("includes", "src/main.cpp;src/util.cpp");

	parser.Save("test_config.txt");

	// Reload
	ConfigParser parser2;
	parser2.Load("test_config.txt");

	assert(parser2.IsValid());
	assert(parser2.GetValue("name") == "MyApp");
	assert(parser2.GetValue("version") == "1.0");
	assert(parser2.GetValue("author") == "Test Author");
	assert(parser2.GetValue("libs") == "lib1,lib2,lib3");
	assert(parser2.GetValue("includes") == "src/main.cpp;src/util.cpp");

	// Test splitting loaded values
	auto libs = parser2.GetValues("libs");
	assert(libs.size() == 3);
	assert(libs[0] == "lib1");
	assert(libs[1] == "lib2");
	assert(libs[2] == "lib3");

	auto includes = parser2.GetValues("includes");
	assert(includes.size() == 2);
	assert(includes[0] == "src/main.cpp");
	assert(includes[1] == "src/util.cpp");

	std::cout << "✓ File loading test passed" << std::endl;
}

void TestGetValueWithDefault()
{
	std::cout << "\n=== Test 8: Get Value with Default ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("key1", "value1");

	auto result = parser.GetValue("nonexistent", "default");
	assert(result == "default");

	result = parser.GetValue("key1", "default");
	assert(result == "value1");

	std::cout << "✓ Get value with default test passed" << std::endl;
}

void TestSplitFunction()
{
	std::cout << "\n=== Test 9: Split function with various delimiters ===" << std::endl;

	// Test comma split
	auto result = ConfigParser::Split("a,b,c,d", ',');
	assert(result.size() == 4);
	assert(result[0] == "a");
	assert(result[1] == "b");
	assert(result[2] == "c");
	assert(result[3] == "d");

	// Test semicolon split
	result = ConfigParser::Split("a;b;c;d", ';');
	assert(result.size() == 4);
	assert(result[0] == "a");
	assert(result[1] == "b");
	assert(result[2] == "c");
	assert(result[3] == "d");

	// Test colon split
	result = ConfigParser::Split("a:b:c:d", ':');
	assert(result.size() == 4);
	assert(result[0] == "a");
	assert(result[1] == "b");
	assert(result[2] == "c");
	assert(result[3] == "d");

	// Test space split
	result = ConfigParser::Split("a b c d", ' ');
	assert(result.size() == 4);
	assert(result[0] == "a");
	assert(result[1] == "b");
	assert(result[2] == "c");
	assert(result[3] == "d");

	// Test common delimiters split
	result = ConfigParser::SplitByCommonDelimiters("a,b;c:d e,f");
	assert(result.size() == 6);
	assert(result[0] == "a");
	assert(result[1] == "b");
	assert(result[2] == "c");
	assert(result[3] == "d");
	assert(result[4] == "e");
	assert(result[5] == "f");

	std::cout << "✓ Split function tests passed" << std::endl;
}

void TestClearAndReset()
{
	std::cout << "\n=== Test 10: Clear and reset ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("key1", "value1");
	parser.SetValue("key2", "value2");

	assert(parser.GetValue("key1") == "value1");
	assert(parser.GetValue("key2") == "value2");

	parser.Clear();

	assert(!parser.GetValue("key1").has_value());
	assert(!parser.GetValue("key2").has_value());
	assert(!parser.IsValid());

	std::cout << "✓ Clear and reset test passed" << std::endl;
}

void TestGetValuesWithEmptyValue()
{
	std::cout << "\n=== Test 11: Get values with empty value ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("key", "");

	auto result = parser.GetValue("key");
	assert(!result.has_value());

	auto values = parser.GetValues("key");
	assert(values.empty());

	std::cout << "✓ Get values with empty value test passed" << std::endl;
}

void TestGetValuesWithSingleItem()
{
	std::cout << "\n=== Test 12: Get values with single item ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("key", "single_value");

	auto result = parser.GetValue("key");
	assert(result == "single_value");

	auto values = parser.GetValues("key");
	assert(values.size() == 1);
	assert(values[0] == "single_value");

	std::cout << "✓ Get values with single item test passed" << std::endl;
}

void TestKeyWithEqualSignInValue()
{
	std::cout << "\n=== Test 13: Key with equal sign in value ===" << std::endl;

	ConfigParser parser;
	parser.SetValue("path", "/path=/to=something");

	auto result = parser.GetValue("path");
	assert(result == "/path=/to=something");

	auto values = parser.GetValues("path");
	assert(values.size() == 1);
	assert(values[0] == "/path=/to=something");

	std::cout << "✓ Key with equal sign in value test passed" << std::endl;
}

int main()
{
	std::cout << "ConfigParser Enhancement Test Suite" << std::endl;
	std::cout << "====================================" << std::endl;

	try
	{
		TestDefaultValue();
		TestCommaSeparatedValues();
		TestSemicolonSeparatedValues();
		TestColonSeparatedValues();
		TestSpaceSeparatedValues();
		TestMixedDelimiters();
		TestLoadFromFileWithVariousDelimiters();
		TestGetValueWithDefault();
		TestSplitFunction();
		TestClearAndReset();
		TestGetValuesWithEmptyValue();
		TestGetValuesWithSingleItem();
		TestKeyWithEqualSignInValue();

		std::cout << "\n====================================" << std::endl;
		std::cout << "All tests passed! ✓" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "\nTest failed with exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
