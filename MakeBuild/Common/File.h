//
//  File.h
//  mbuild
//
//  Created by mooming on 2016. 8. 15..
//
//

#ifndef File_h
#define File_h

#include <ostream>
#include <string>

namespace OS
{
	class File
	{
	public:
		const std::string path;

		explicit File(const char* path) : path(path)
		{
		}

		friend std::ostream& operator << (std::ostream& os, const File& file)
		{
			os << file.path;
			return os;
		}
	};

	using Files = std::vector<File>;
}

#endif /* File_h */
