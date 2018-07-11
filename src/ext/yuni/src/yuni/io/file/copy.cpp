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
#include "../file.h"
#if defined(YUNI_OS_LINUX) and defined(YUNI_HAS_SYS_SENDFILE_H)
# include <sys/sendfile.h>
#endif
#include <sys/stat.h>
#ifndef YUNI_OS_WINDOWS
# include <unistd.h>
#endif


namespace Yuni
{
namespace IO
{
namespace File
{


	Error Copy(const AnyString& from, const AnyString& to, bool overwrite)
	{
		if (!overwrite and IO::Exists(to))
			return errOverwriteNotAllowed;

		// Open the source file
		Yuni::IO::File::Stream fromFile;
		if (not fromFile.open(from))
			return errNotFound;

		Yuni::IO::File::Stream toFile;
		if (not toFile.openRW(to))
			return errNotFound;

		enum { size = 8192 };

		# if defined(YUNI_OS_LINUX) and defined(YUNI_HAS_SYS_SENDFILE_H)
		int fdIN  = fileno(fromFile. nativeHandle());
		int fdOUT = fileno(toFile.   nativeHandle());

		// Trying sendfile first
		struct stat st;
		if (0 == fstat(fdIN, &st))
		{
			off_t offset = 0;
			if (sendfile(fdOUT, fdIN, &offset, (size_t) st.st_size) >= 0)
				return Yuni::IO::errNone;
		}

		// fallback to the standard copy
		char* buffer = new char[size];
		ssize_t numRead;

		while ((numRead = ::read(fdIN, buffer, size)) > 0)
		{
			if (numRead != ::write(fdOUT, buffer, (size_t) numRead))
			{
				delete[] buffer;
				return Yuni::IO::errWriteFailed;
			}
		}

		# else

		// Generic implementation
		char* buffer = new char[size];
		uint64 numRead;
		while ((numRead = fromFile.read(buffer, size)) != 0)
		{
			if (numRead != toFile.write(buffer, numRead))
			{
				delete[] buffer;
				return Yuni::IO::errWriteFailed;
			}
		}

		# endif

		delete[] buffer;
		return Yuni::IO::errNone;
	}




} // namespace File
} // namespace IO
} // namespace Yuni
