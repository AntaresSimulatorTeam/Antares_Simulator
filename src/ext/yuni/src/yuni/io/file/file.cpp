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
#include <sys/stat.h>
#include "../file.h"
#include "../directory.h"
#include <cctype>
#include <cstring>
#include "../../core/string/wstring.h"

#ifndef YUNI_OS_WINDOWS
# include <unistd.h>
#endif

#ifndef YUNI_OS_WINDOWS
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


namespace Yuni
{
namespace IO
{
namespace File
{

	bool CreateEmptyFile(const AnyString& filename)
	{
		IO::File::Stream file(filename, OpenMode::write | OpenMode::truncate);
		return file.opened();
	}



	# ifdef YUNI_OS_WINDOWS

	bool Size(const AnyString& filename, uint64& value)
	{
		uint len = filename.size();
		if (!len)
		{
			value = 0u;
			return false;
		}

		const char* const p = filename.c_str();

		if (p[len - 1] == '\\' or p[len - 1] == '/')
			--len;

		// Driver letters
		if (len == 2 and p[1] == ':')
		{
			value = 0u;
			return true;
		}

		String  norm;
		Yuni::IO::Normalize(norm, AnyString(p, len));
		// Conversion into wchar_t
		WString wstr(norm, true);
		if (wstr.empty())
		{
			value = 0u;
			return false;
		}

		HANDLE hndl = CreateFileW(wstr.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hndl == INVALID_HANDLE_VALUE)
		{
			value = 0u;
			return false;
		}
		LARGE_INTEGER v;
		if (!GetFileSizeEx(hndl, &v))
		{
			CloseHandle(hndl);
			value = 0u;
			return false;
		}
		value = (uint64) v.QuadPart;

		CloseHandle(hndl);
		return true;
	}

	# else

	bool Size(const AnyString& filename, uint64& value)
	{
		struct stat results;
		if (not filename.empty() and stat(filename.c_str(), &results) == 0)
		{
			value = (uint64) results.st_size;
			return true;
		}
		value = 0u;
		return false;
	}

	# endif



	Yuni::IO::Error Delete(const AnyString& filename)
	{
		// DeleteFile is actually a macro and will be replaced by DeleteFileW
		// with Visual Studio. Consequently we can not use the word DeleteFile.....

		if (filename.empty())
			return Yuni::IO::errUnknown;

		# ifndef YUNI_OS_WINDOWS

		return (unlink(filename.c_str()))
			? Yuni::IO::errUnknown
			: Yuni::IO::errNone;

		# else

		const char* const p = filename.c_str();
		uint len = filename.size();

		if (p[len - 1] == '\\' or p[len - 1] == '/')
			--len;

		// Driver letters
		if (len == 2 and p[1] == ':')
			return Yuni::IO::errBadFilename;

		String norm;
		Yuni::IO::Normalize(norm, AnyString(p, len));

		// Conversion into wchar_t
		WString wstr(norm, true);
		if (wstr.empty())
			return Yuni::IO::errUnknown;
		wstr.replace('/', '\\');

		return (DeleteFileW(wstr.c_str()))
			? Yuni::IO::errNone
			: Yuni::IO::errUnknown;
		# endif
	}




	# ifdef YUNI_OS_WINDOWS

	/*
	bool GetLastWriteTime(HANDLE hFile)
	{
		FILETIME ftCreate, ftAccess, ftWrite;
		SYSTEMTIME stUTC, stLocal;

		// Retrieve the file times for the file.
		if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
			return false;

		// Convert the last-write time to local time.
		if (!FileTimeToSystemTime(&ftWrite, &stUTC))
			return false;
		if (!SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal))
			return false;

		return true;
	}
	*/

	# endif



	sint64 LastModificationTime(const AnyString& filename)
	{
		uint64 size;
		sint64 lastModified;
		IO::FetchFileStatus(filename, size, lastModified);
		return lastModified;
	}



	namespace // anonymous
	{

		template<class StringT>
		static inline
		IO::Error LoadFromFileImpl(StringT& out, const AnyString& filename, const uint64 hardlimit)
		{
			out.clear();
			Yuni::IO::File::Stream f(filename);
			if (not f.opened())
				return errNotFound;

			// retrieve the file size in bytes
			f.seekFromEndOfFile(0);
			uint64 filesize = (uint64) f.tell();

			if (filesize == 0)
			{
				# ifndef YUNI_OS_WINDOWS
				// On unix, some special files can have a size equals to zero, but
				// not being empty (like files on /proc)
				// swithing to the standard method for reading a file

				// we will use here a smaller fragment size, since those kind of files
				// are unlikely big ones
				enum { smallFragment = 512 * 1024 };
				uint64 offset = 0;
				do
				{
					out.reserve(((typename StringT::size_type) offset) + smallFragment);
					uint64 numread = f.read((char*)out.data() + offset, smallFragment);
					if (numread != smallFragment)
					{
						out.resize((typename StringT::size_type) (offset + numread));
						return errNone;
					}
					offset += smallFragment;
					if (offset >= hardlimit)
						return errMemoryLimit;
				}
				while (true);

				# endif
				return errNone;
			}

			if (filesize > hardlimit or filesize > (uint) -10)
				return errMemoryLimit;

			// resize the buffer accordingly
			out.resize((typename StringT::size_type) filesize);
			// replace the cursor within the file
			f.seekFromBeginning(0);

			// we would prefer to read the file by chunk instead (to be interruptible)
			enum { fragment = 4 * 1024 * 1024 };
			if (filesize < fragment)
			{
				uint64 numread = f.read((char*)out.data(), filesize);
				if (numread != filesize)
				{
					out.clear();
					return errReadFailed;
				}
			}
			else
			{
				uint64 offset = 0;
				while (filesize >= fragment)
				{
					uint64 numread = f.read((char*)out.data() + offset, fragment);
					if (numread != fragment)
					{
						out.resize((typename StringT::size_type) offset);
						return errReadFailed;
					}
					offset += fragment;
					filesize -= fragment;
				}
				if (filesize != 0)
				{
					uint64 numread = f.read((char*)out.data() + offset, filesize);
					if (numread != filesize)
					{
						out.resize((typename StringT::size_type) offset);
						return errReadFailed;
					}
				}
			}

			return errNone;
		}

	} // anonymous namespace



	IO::Error LoadFromFile(std::string& out, const AnyString& filename, uint64 hardlimit)
	{
		return LoadFromFileImpl(out, filename, hardlimit);
	}


	IO::Error LoadFromFile(String& out, const AnyString& filename, uint64 hardlimit)
	{
		return LoadFromFileImpl(out, filename, hardlimit);
	}


	IO::Error LoadFromFile(Clob& out, const AnyString& filename, uint64 hardlimit)
	{
		return LoadFromFileImpl(out, filename, hardlimit);
	}






} // namespace File
} // namespace IO
} // namespace Yuni

