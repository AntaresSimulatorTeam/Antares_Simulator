/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "../io.h"
#include "../directory.h"
#ifndef YUNI_OS_WINDOWS
# include <unistd.h>
#else
# include "../../core/system/windows.hdr.h"
# include "../../core/string/wstring.h"
# include <direct.h>
# include <stdlib.h>
# include <stdio.h>
#endif




namespace Yuni
{
namespace IO
{
namespace Directory
{
namespace Current
{

	namespace // anonymous
	{

		template<class StringT>
		static inline bool FetchAndAppendCurrentDirectory(StringT& string)
		{
			#ifdef YUNI_OS_WINDOWS
			{
				wchar_t* cwd = _wgetcwd(nullptr, 0); // length: Arbitrary value
				if (cwd != nullptr)
				{
					string.append(cwd);
					free(cwd);
				}
			}
			#else
			{
				// arbitrary - good enough for most cases
				uint more = 512 - 1;
				do
				{
					string.reserve(string.size() + more);
					char* buffer = string.data();
					// making sure that the buffer has been allocated
					if (YUNI_UNLIKELY(nullptr == buffer or string.capacity() <= string.size()))
						return false;

					buffer += string.size();
					size_t length = string.capacity() - string.size() - 1;

					char* path = ::getcwd(buffer, length);
					if (YUNI_LIKELY(path))
					{
						string.resize(string.size() + (uint)strlen(path));
						return true;
					}
					more += 2096; // already have -1
				}
				while (more < 65535);
			}
			#endif

			// just in case the inner content has been modified
			string.resize(string.size());
			return false;
		}

	} // anonymous namespace



	String Get()
	{
		String current;
		FetchAndAppendCurrentDirectory(current);
		return current;
	}


	bool Get(String& out, bool clearBefore)
	{
		if (clearBefore)
			out.clear();
		return FetchAndAppendCurrentDirectory(out);
	}


	bool Get(Clob& out, bool clearBefore)
	{
		if (clearBefore)
			out.clear();
		return FetchAndAppendCurrentDirectory(out);
	}


	bool Set(const AnyString& path)
	{
		if (path.empty())
			return false;

		String realpath;
		IO::Canonicalize(realpath, path);

		#ifdef YUNI_OS_WINDOWS
		{
			WString wstring(realpath);
			return (0 == _wchdir(wstring.c_str()));
		}
		#else
		{
			return (0 == ::chdir(realpath.c_str()));
		}
		#endif
	}





} // namespace Current
} // namespace Directory
} // namespace IO
} // namespace Yuni
