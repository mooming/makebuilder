//
//  StringUtil.h
//  mbuild
//
//  Created by mooming on 2016. 8. 14..
//
//

#ifndef StringUtil_h
#define StringUtil_h

#include <string>

namespace Util
{
	std::string TrimPath(std::string path);
	std::string ToLowerCase(std::string src);

	bool EqualsIgnoreCase(std::string a, std::string b);
	bool StartsWith(std::string src, std::string startTerm);
	bool StartsWithIgnoreCase(std::string src, std::string startTerm);
	bool EndsWith(std::string src, std::string endTerm);

	std::string PathToName(std::string path);
}

#endif /* StringUtil_h */
