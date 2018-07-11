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
#include "iterator.h"

#ifdef YUNI_OS_WINDOWS
# include "../../../core/system/windows.hdr.h"
# include <wchar.h>
# include <io.h>
#else
# include <errno.h>
# include <dirent.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#endif


namespace Yuni
{
namespace Private
{
namespace IO
{
namespace Directory
{
namespace Iterator
{
//begin added by Papa NDIAYE on 20/09/2016
	Yuni::IO::Directory::IIterator<true>* test;
//end
	enum
	{
		pollingInterval = 6,
		wbufferSize = 4096,
	};


# ifndef YUNI_OS_WINDOWS

	Flow TraverseUnixFolder(const String& filename, Options& opts, IDetachedThread* thread, bool files)
	{
		// Opening the folder
		DIR* pdir = opendir(filename.c_str());
		if (!pdir)
			return opts.self->onError(filename);

		struct dirent *pent;
		struct stat s;
		String newName;
		String newFilename;

		// iterating trough files and folders
		while ((pent = readdir(pdir)))
		{
			# ifndef YUNI_NO_THREAD_SAFE
			// Checking from time to time if the thread should stop
			if (thread and (++opts.counter >= (uint) pollingInterval)) // arbitrary value
			{
				// reset counter
				opts.counter = 0;
				if (thread->suspend())
					return Yuni::IO::flowAbort;
			}
			# endif

			// Avoid `.` and `..`
			if (*(pent->d_name) == '.')
			{
				if ((pent->d_name[1] == '.' and pent->d_name[2] == '\0') or (pent->d_name[1] == '\0'))
					continue;
			}

			newName = (const char*) pent->d_name;
			newFilename.clear();
			newFilename << filename << Yuni::IO::Separator << newName;
			if (stat(newFilename.c_str(), &s) != 0)
			{
				if (opts.self->onAccessError(newFilename) == Yuni::IO::flowAbort)
					return Yuni::IO::flowAbort;
				continue;
			}

			if (S_ISDIR(s.st_mode))
			{
				if (!files)
				{
					// The node is a folder
					switch (opts.self->onBeginFolder(newFilename, filename, newName))
					{
						case Yuni::IO::flowContinue:
							{
								if (Yuni::IO::flowAbort == TraverseUnixFolder(newFilename, opts, thread, true))
								{
									opts.self->onEndFolder(newFilename, filename, newName);
									return Yuni::IO::flowAbort;
								}
								opts.self->onEndFolder(newFilename, filename, newName);
								break;
							}
						case Yuni::IO::flowAbort:
							return Yuni::IO::flowAbort;
						case Yuni::IO::flowSkip:
							break;
					}
				}
			}
			else
			{
				if (files)
				{
					// The node is a file
					switch (opts.self->onFile(newFilename, filename, newName, static_cast<uint64>(s.st_size)))
					{
						case Yuni::IO::flowContinue:
							break;
						case Yuni::IO::flowAbort:
							return Yuni::IO::flowAbort;
						case Yuni::IO::flowSkip:
							{
								closedir(pdir);
								return Yuni::IO::flowContinue;
							}
					}
				}
			}
		}
		closedir(pdir);

		if (files)
			return TraverseUnixFolder(filename, opts, thread, false);
		return Yuni::IO::flowContinue;
	}


# else

	Flow TraverseWindowsFolder(const String& filename, Options& opts, IDetachedThread* thread, bool files)
	{
		// Convertir the filename
		assert(opts.wbuffer != NULL);
		opts.wbuffer[0] = L'\\';
		opts.wbuffer[1] = L'\\';
		opts.wbuffer[2] = L'?';
		opts.wbuffer[3] = L'\\';
		int n = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), filename.size(), opts.wbuffer + 4, (uint) wbufferSize - 16);
		if (0 == n)
			return opts.self->onError(filename);

		// Making sure that our string is zero-terminated
		opts.wbuffer[n + 4] = L'\\';
		opts.wbuffer[n + 5] = L'*';
		opts.wbuffer[n + 6] = L'\0';
		opts.wbuffer[n + 7] = L'\0';
		opts.wbuffer[n + 8] = L'\0';

		// Opening the folder
		WIN32_FIND_DATAW data;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		hFind = FindFirstFileW(opts.wbuffer, &data);
		if (INVALID_HANDLE_VALUE == hFind)
			return Yuni::IO::flowContinue;

		String newName;
		String newFilename;
		LARGE_INTEGER filesize;

		// iterating trough files and folders
		do
		{
			# ifndef YUNI_NO_THREAD_SAFE
			// Checking from time to time if the thread should stop
			if (thread and (++opts.counter >= (uint) pollingInterval)) // arbitrary value
			{
				// reset counter
				opts.counter = 0;
				if (thread->suspend())
					return Yuni::IO::flowAbort;
			}
			# endif

			// Avoid `.` and `..`
			if (data.cFileName[0] == L'.')
			{
				if ((data.cFileName[1] == L'.' and data.cFileName[2] == L'\0') or (data.cFileName[1] == L'\0'))
					continue;
			}

			const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, data.cFileName, -1, NULL, 0,  NULL, NULL);
			if (sizeRequired <= 0)
				continue;
			newName.reserve((uint) sizeRequired);
			WideCharToMultiByte(CP_UTF8, 0, data.cFileName, -1, (char*)newName.data(), sizeRequired,  NULL, NULL);
			newName.resize(((uint) sizeRequired) - 1);

			newFilename.clear();
			newFilename << filename << '\\' << newName;

			if (0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (!files)
				{
					// The node is a folder
					switch (opts.self->onBeginFolder(newFilename, filename, newName))
					{
						case Yuni::IO::flowContinue:
						{
							if (Yuni::IO::flowAbort == TraverseWindowsFolder(newFilename, opts, thread, true))
							{
								opts.self->onEndFolder(newFilename, filename, newName);
								return Yuni::IO::flowAbort;
							}
							opts.self->onEndFolder(newFilename, filename, newName);
							break;
						}

						case Yuni::IO::flowAbort:
							return Yuni::IO::flowAbort;
						case Yuni::IO::flowSkip:
							break;
					}
				}
			}
			else
			{
				if (files)
				{
					// The node is a file
					filesize.LowPart  = data.nFileSizeLow;
					filesize.HighPart = data.nFileSizeHigh;
					switch (opts.self->onFile(newFilename, filename, newName, (uint64)(filesize.QuadPart)))
					{
						case Yuni::IO::flowContinue:
							break;
						case Yuni::IO::flowAbort:
							return Yuni::IO::flowAbort;
						case Yuni::IO::flowSkip:
							{
								FindClose(hFind);
								return Yuni::IO::flowContinue;
							}
					}
				}
			}
		}
		while (FindNextFileW(hFind, &data) != 0);
		FindClose(hFind);

		if (files)
			return TraverseWindowsFolder(filename, opts, thread, false);
		return Yuni::IO::flowContinue;
	}

# endif // ifndef YUNI_OS_WINDOWS











	void Traverse(Options& options, IDetachedThread* thread)
	{
		if (options.rootFolder.empty())
			return;

		// some reset
		# ifdef YUNI_OS_WINDOWS
		delete[] options.wbuffer; // just in case
		options.wbuffer = new wchar_t[wbufferSize];
		# endif

		// for each folder
		{
			const String::VectorPtr::const_iterator& end = options.rootFolder.end();
			for (String::VectorPtr::const_iterator i = options.rootFolder.begin(); i != end; ++i)
			{
				const String& path = *(*i);

				// This routine can only be called if the parameter is not empty
				if (path.empty() or !options.self->onStart(path))
					continue;

				// Making sure that the counter is properly initialized
				options.counter = 0;

				# ifdef YUNI_OS_WINDOWS
				const Flow result = TraverseWindowsFolder(path, options, thread, true);
				# else
				const Flow result = TraverseUnixFolder(path, options, thread, true);
				# endif

				# ifndef YUNI_NO_THREAD_SAFE
				if ((result == Yuni::IO::flowAbort) or (thread and thread->suspend()))
				# else
				if ((result == Yuni::IO::flowAbort))
				# endif
				{
					options.self->onAbort();
					return;
				}
			}
		}

		// Final events
		options.self->onTerminate();
	}





} // namespace Iterator
} // namespace Directory
} // namespace IO
} // namespace Private
} // namespace Yuni

