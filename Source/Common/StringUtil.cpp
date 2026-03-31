// Created by mooming.go@gmail.com 2016

#include "StringUtil.h"

#include "OSAbstractLayer.h"
#include <algorithm>
#include <cctype>

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

    // Converts backslashes to forward slashes and removes trailing slash.
    // Previously used a fixed-size buffer (char tmp[PATH_MAX + 1]) which could
    // overflow if the path length exceeded PATH_MAX. Fixed 2026-03-31 by using
    // dynamic string operations to safely handle arbitrarily long paths.
    string TrimPath(string path)
    {
        const size_t len = path.length();
        string result;
        result.reserve(len);

        for (size_t i = 0; i < len; ++i)
        {
            char ch = path[i];
            result += (ch == '\\') ? '/' : ch;
        }

        if (len > 2 && result[len - 1] == '/' && result[len - 2] != '.')
        {
            result.pop_back();
        }

        return result;
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

} // namespace Util
