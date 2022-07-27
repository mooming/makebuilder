// Created by mooming.go@gmail.com 2016

#include "StringUtil.h"

#include "OSAbstractLayer.h"
#include <algorithm>


using namespace std;

namespace Util
{
	string Trim(const string& str)
	{
		auto start = str.begin();
		auto end = str.end();

		while (start != end && isspace(*start))
		{
			++start;
		}
		
		do
		{
			--end;
		} while (distance(start, end) > 0 && std::isspace(*end));

		return string(start, end + 1);
    }

	string TrimPath(string path)
	{
		char tmp[PATH_MAX + 1];
		auto cStr = path.c_str();
		for (size_t i = 0; i < path.length(); ++i)
		{
			if (cStr[i] == '\\')
			{
				tmp[i] = '/';
			}
			else
			{
				tmp[i] = cStr[i];
			}
		}

		if (path.length() > 2 && tmp[path.length() - 2] != '.' && tmp[path.length() - 1] == '/')
		{
			tmp[path.length() - 1] = '\0';
		}

		tmp[path.length()] = '\0';

		return string(tmp);
	}

	string ToLowerCase(string src)
	{
		auto ToLowerChar = [](char ch) {
			constexpr char delta = 'A' - 'a';

			if ('A' <= ch && ch <= 'Z')
			{
				ch -= delta;
			}

			return ch;
		};

		transform(src.begin(), src.end(), src.begin(), ToLowerChar);

		return src;
	}

	bool EqualsIgnoreCase(std::string a, std::string b)
	{
		return ToLowerCase(a) == ToLowerCase(b);
	}

	bool StartsWith(string src, string startTerm)
	{
		if (src.length() < startTerm.length())
			return false;

		string head = src.substr(0, startTerm.length());

		return head == startTerm;
	}

	bool StartsWithIgnoreCase(string src, string startTerm)
	{
		return StartsWith(ToLowerCase(src), ToLowerCase(startTerm));
	}

	bool EndsWith(string src, string endTerm)
	{
		if (src.length() < endTerm.length())
			return false;

		const char* srcStr = src.c_str();
		string last(srcStr + (src.length() - endTerm.length()));

		return last == endTerm;
	}


	string PathToName(string path)
	{
		auto length = path.size();
		using Index = decltype(length);

		auto buffer = path.c_str();

		bool found = false;
		Index lastIndex = 0;
		for (Index i = 0; i < length; ++i)
		{
			const char ch = buffer[i];
			if (ch == '/' || ch == '\\')
			{
				lastIndex = i;
				found = true;
			}
		}

		if (!found || lastIndex >= (length - 1))
		{
			return path;
		}

		return string(buffer + lastIndex + 1);
	}

}
