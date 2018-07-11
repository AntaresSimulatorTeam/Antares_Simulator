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
#include "platform.h"
#include "../../../core/slist.h"
#include <cassert>
#include "../../io.h"
#include "../../directory.h"
#include "../info.h"
#include "../../../core/noncopyable.h"

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

	enum
	{
		wbufferMax = 6192,
	};



	class DirInfo final : private Yuni::NonCopyable<DirInfo>
	{
	public:
		DirInfo() :
			size(),
			modified(),
			isFolder(false),
			# ifndef YUNI_OS_WINDOWS
			pdir(nullptr),
			pent(nullptr)
			# else
			h(-1),
			callNext(false)
			# endif
		{}

		~DirInfo()
		{
			# ifdef YUNI_OS_WINDOWS
			if (h >= 0)
				_findclose(h);
			# else
			if (pdir) // check required to avoid segv
				closedir(pdir);
			# endif
		}

		# ifdef YUNI_OS_WINDOWS
		void open(wchar_t* wbuffer)
		# else
		void open()
		# endif
		{
			String canonPath;
			Yuni::IO::Canonicalize(canonPath, parent);
			# ifdef YUNI_OS_WINDOWS
			// Convert the filename
			wbuffer[0] = L'\\';
			wbuffer[1] = L'\\';
			wbuffer[2] = L'?';
			wbuffer[3] = L'\\';
			int n = ::MultiByteToWideChar(CP_UTF8, 0, canonPath.c_str(), canonPath.size(), wbuffer + 4, wbufferMax - 10);
			if (!n)
			{
				h = -1;
				return;
			}

			// Making sure that our string is zero-terminated
			wbuffer[n + 4] = L'\\';
			wbuffer[n + 5] = L'*';
			wbuffer[n + 6] = L'.';
			wbuffer[n + 7] = L'*';
			wbuffer[n + 8] = L'\0';

			// Opening the folder
			h = ::_wfindfirsti64(wbuffer, &data);
			callNext = false;

			# else

			pdir = ::opendir(canonPath.c_str());

			# endif

			// This variable must be reseted to avoid recursive calls at startup
			isFolder = false;
		}

		bool next(uint flags)
		{
			# ifndef YUNI_OS_WINDOWS

			if (!pdir)
			{
				size = 0;
				modified = 0;
				return false;
			}
			while ((pent = readdir(pdir)))
			{
				// Avoid `.` and `..`
				if (*(pent->d_name) == '.')
				{
					if ((pent->d_name[1] == '.' and pent->d_name[2] == '\0') or (pent->d_name[1] == '\0'))
						continue;
				}

				name = (const char* const) pent->d_name;
				filename.clear();
				filename << parent << Yuni::IO::Separator << name;
				if (stat(filename.c_str(), &s) != 0)
					continue;

				if (S_ISDIR(s.st_mode))
				{
					if (0 != (flags & Yuni::IO::Directory::Info::itFolder)
						|| (0 != (flags & Yuni::IO::Directory::Info::itRecursive)))
					{
						isFolder = true;
						size = 0;
						modified = (sint64) s.st_mtime;
						return true;
					}
				}
				else
				{
					if (0 != (flags & Yuni::IO::Directory::Info::itFile))
					{
						isFolder = false;
						size = (uint64) s.st_size;
						modified = (sint64) s.st_mtime;
						return true;
					}
				}
			}

			size = 0;
			modified = 0;
			return false;

			# else // WINDOWS

			if (h < 0)
				return false;
			do
			{
				if (callNext and 0 != _wfindnexti64(h, &data))
				{
					size = 0;
					modified = 0;
					return false;
				}
				callNext = true;

				// Avoid `.` and `..`
				if (*(data.name) == L'.')
				{
					if ((data.name[1] == L'.' and data.name[2] == L'\0') or (data.name[1] == L'\0'))
						continue;
				}

				const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, data.name, -1, NULL, 0,  NULL, NULL);
				if (sizeRequired <= 0)
					continue;
				name.reserve((uint) sizeRequired);
				WideCharToMultiByte(CP_UTF8, 0, data.name, -1, (char*)name.data(), sizeRequired,  NULL, NULL);
				name.resize(((uint) sizeRequired) - 1);

				filename.clear();
				filename << parent << '\\' << name;

				if (0 != (data.attrib & _A_SUBDIR))
				{
					if (0 != (flags & Yuni::IO::Directory::Info::itFolder)
						or (0 != (flags & Yuni::IO::Directory::Info::itRecursive)))
					{
						isFolder = true;
						size = 0;
						modified = (sint64) data.time_write;
						return true;
					}
				}
				else
				{
					if (0 != (flags & Yuni::IO::Directory::Info::itFile))
					{
						isFolder = false;
						size = (uint64) data.size;
						modified = (sint64) data.time_write;
						return true;
					}
				}

			}
			while (true);
			return false;
			# endif
		}

	public:
		//! Parent folder
		String parent;
		//! Name of the current node
		String name;
		//! The complete filename of the current node
		String filename;
		//! File size
		uint64 size;
		//! Date of the last modification
		sint64 modified;
		//! Flag to determine whether the inode is a folder or a file
		bool isFolder;


	private:
		# ifndef YUNI_OS_WINDOWS
		DIR* pdir;
		struct dirent* pent;
		struct stat s;
		# else
		struct _wfinddatai64_t data;
		intptr_t h;
		bool callNext;
		# endif

	};


	class IteratorData final
	{
	public:
		IteratorData() :
			flags()
		{}

		IteratorData(const IteratorData& rhs) :
			flags(rhs.flags)
		{
			if (not rhs.dirinfo.empty())
			{
				push(rhs.dirinfo.front().parent);
				next();
			}
		}

		void push(const AnyString& v)
		{
			dirinfo.push_front();
			dirinfo.front().parent = v;
			# ifdef YUNI_OS_WINDOWS
			dirinfo.front().open(wbuffer);
			# else
			dirinfo.front().open();
			# endif
		}

		void pop()
		{
			dirinfo.pop_front();
		}

		bool next()
		{
			// Entering the sub-folder if required
			if (dirinfo.front().isFolder and (0 != (Yuni::IO::Directory::Info::itRecursive & flags)))
			{
				// Starting a new state
				push(dirinfo.front().filename);
			}

			// Next node
			//
			// Infinite loop to not use a recursive function
			do
			{
				if (!dirinfo.front().next(flags))
				{
					// Parent folder
					pop();
					if (dirinfo.empty())
						return false;
				}
				else
				{
					// We must loop when we have to recursively find all files
					if ((0 == (Yuni::IO::Directory::Info::itFolder & flags))
						and dirinfo.front().isFolder
						and (0 != (Yuni::IO::Directory::Info::itRecursive & flags)))
					{
						// Starting a new state
						push(dirinfo.front().filename);
						continue;
					}
					return true;
				}
			}
			while (true);
			return false;
		}


	public:
		uint flags;
		LinkedList<DirInfo> dirinfo;
		# ifdef YUNI_OS_WINDOWS
		wchar_t wbuffer[wbufferMax];
		# endif

	}; // class IteratorData



	IteratorData* IteratorDataCreate(const AnyString& folder, uint flags)
	{
		if (not folder.empty())
		{
			IteratorData* data = new IteratorData();
			data->flags = flags;
			data->push(folder);
			return data;
		}
		return NULL;
	}


	IteratorData* IteratorDataCopy(const IteratorData* data)
	{
		return (data) ? (new IteratorData(*data)) : NULL;
	}

	void IteratorDataFree(const IteratorData* data)
	{
		delete data;
	}

	IteratorData* IteratorDataNext(IteratorData* data)
	{
		assert(data != NULL);
		if (data->next())
			return data;
		delete data;
		return NULL;
	}


	const String& IteratorDataFilename(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().filename;
	}


	const String& IteratorDataParentName(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().parent;
	}


	const String& IteratorDataName(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().name;
	}


	uint64 IteratorDataSize(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().size;
	}


	sint64 IteratorDataModified(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().modified;
	}


	bool IteratorDataIsFolder(const IteratorData* data)
	{
		assert(data != NULL);
		return data->dirinfo.front().isFolder;
	}


	bool IteratorDataIsFile(const IteratorData* data)
	{
		assert(data != NULL);
		return ! data->dirinfo.front().isFolder;
	}





} // namespace Directory
} // namespace IO
} // namespace Private
} // namespace Yuni

