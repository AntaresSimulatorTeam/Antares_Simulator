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
#include "../filename-manipulation.h"
#include "../directory.h"

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <errno.h>
#ifdef YUNI_HAS_STDLIB_H
# include <stdlib.h>
#endif
#ifndef YUNI_OS_MSVC
# include <dirent.h>
# include <unistd.h>
#endif
#ifdef YUNI_OS_WINDOWS
# include "../../core/system/windows.hdr.h"
# include <shellapi.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include "../../core/string.h"
#include "../../core/string/wstring.h"
#include <stdio.h>

#include <fstream>
#define SEP Yuni::IO::Separator



namespace Yuni
{
namespace IO
{
namespace Directory
{

	# ifndef YUNI_OS_WINDOWS


	namespace // Anonymous namespace
	{

		static bool RmDirRecursiveInternal(const AnyString& path)
		{
			DIR* dp = ::opendir(path.c_str());
			if (dp)
			{
				struct dirent* ep = ::readdir(dp);
				if (ep)
				{
					CString<1024> buffer;
					do
					{
						if (ep->d_type == DT_DIR)
						{
							const char* const p = ep->d_name;
							bool sysfolder = (p[0] == '.' and (p[1] == '\0' or (p[1] == '.' and p[2] == '\0')));
							if (not sysfolder)
							{
								buffer.clear() << path << SEP << (const char*) ep->d_name;
								RmDirRecursiveInternal(buffer.c_str());
							}
						}
						else
						{
							buffer.clear() << path << SEP << (const char*) ep->d_name;
							::unlink(buffer.c_str());
						}
					}
					while (NULL != (ep = ::readdir(dp)));
				}
				(void)::closedir(dp);
			}
			return (0 == rmdir(path.c_str()));
		}

	} // anonymous namespace


	# endif


	# ifdef YUNI_OS_WINDOWS

	namespace // anonymous
	{

		static bool RecursiveDeleteWindow(const wchar_t* path, uint size)
		{
			enum { maxLen = (MAX_PATH < 4096) ? 4096 : MAX_PATH };

			if (size >= maxLen)
				return false;

			// temporary buffer for filename manipulation
			wchar_t* filename = new (std::nothrow) wchar_t[maxLen];
			if (nullptr == filename)
				return false;
			::wcsncpy_s(filename, maxLen, path, size);
			if (size + 2 < maxLen)
			{
				filename[size++] = L'\\';
				filename[size++] = L'*';
				filename[size]   = L'\0';
			}

			// temporary buffer
			WIN32_FIND_DATAW filedata;
			HANDLE handle = ::FindFirstFileW(filename, &filedata);
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					// Dots folders are meaningless (`.` and `..`)
					if (filedata.cFileName[0] == L'.')
					{
						if (not ::wcscmp(filedata.cFileName, L".") or not ::wcscmp(filedata.cFileName, L".."))
							continue;
					}

					// Overwrite the '*'
					int newSize = size - 1;
					// Prepare the new filename
					{
						int written = 0;
						for (; newSize < maxLen && filedata.cFileName[written]; ++newSize, ++written)
							filename[newSize] = filedata.cFileName[written];
						if (written <= 0 or newSize >= maxLen)
						{
							::FindClose(handle);
							delete[] filename;
							return false;
						}
						filename[newSize] = L'\0';
					}

					// Recursively delete the sub-folder
					if (0 != (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
						if (not RecursiveDeleteWindow(filename, newSize))
						{
							::FindClose(handle);
							delete[] filename;
							return false;
						}
					}
					else
					{
						// Simply delete the file
						// If the file has the read-only attribute, trying to
						// remove it first.
						if (0 != (filedata.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
							::SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);

						// Trying to delete the file
						if (not ::DeleteFileW(filename))
						{
							::FindClose(handle);
							delete[] filename;
							return false;
						}
					}
				}
				while (::FindNextFileW(handle, &filedata));

				// resource cleanup
				::FindClose(handle);
			}

			delete[] filename;

			// Remove the directory itself
			return (0 != ::RemoveDirectoryW(path));
		}

	} // anonymous

	# endif



	bool Remove(const AnyString& path)
	{
		if (path.empty())
			return true;

		# ifdef YUNI_OS_WINDOWS
		{
			String canon;
			Canonicalize(canon, path);

			WString fsource(canon, true);
			if (fsource.empty())
				return false;

			// SHFILEOPSTRUCT operation;
			// operation.hwnd = nullptr;
			// operation.wFunc = FO_DELETE;
			// operation.fFlags |= FOF_NOCONFIRMATION;
			// // Requires double '\0' termination
			// auto doubleTerminated = new wchar_t[fsource.size() + 2];
			// ::wcsncpy(doubleTerminated, fsource.c_str(), fsource.size() + 1);
			// doubleTerminated[fsource.size() + 1] = L'\0';
			// operation.pFrom = doubleTerminated;
			// return 0 == ::SHFileOperation(&operation);

			return RecursiveDeleteWindow(fsource.c_str(), fsource.size());
		}
		# else
		{
			String p(path);
			return RmDirRecursiveInternal(p);
		}
		# endif
	}





} // namespace Directory
} // namespace IO
} // namespace Yuni

