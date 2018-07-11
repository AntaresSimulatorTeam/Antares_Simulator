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
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

// We need to remove strict ansi compliance under MinGW
// otherwise _wfopen and _fileno are not defined...
// And we need to do it here, before the first inclusion of <stdio.h>
#ifdef __STRICT_ANSI__
# undef __STRICT_ANSI__
#endif

#include <string.h>
#include "stream.h"
#include "../../core/string.h"
#include "../../core/string/wstring.h"

#ifdef YUNI_OS_WINDOWS
# include "../../core/system/windows.hdr.h"
# include <io.h>
# include <stdio.h>    // _fileno
#else
# include <sys/file.h> // lock
#endif

#ifndef YUNI_OS_MSVC
# include <unistd.h>   // ftruncate
#endif

#if defined(YUNI_HAS_POSIX_FALLOCATE) || defined(YUNI_OS_MAC)
# include <fcntl.h>
#endif

#ifdef YUNI_OS_WINDOWS
# define FILENO(X)  _fileno(X)
#else
# define FILENO(X)  fileno(X)
#endif



namespace Yuni
{
namespace IO
{
namespace File
{


	# ifdef YUNI_OS_WINDOWS
	namespace // anonymous
	{

		static Stream::HandleType OpenFileOnWindows(const AnyString& filename, int mode)
		{
			WString wfilenm(filename);
			if (wfilenm.empty())
				return nullptr;

			FILE* f;
			# ifdef YUNI_OS_MSVC
			{
				if (0 != _wfopen_s(&f, wfilenm.c_str(), OpenMode::ToWCString(mode)))
					return nullptr;
			}
			# else
			{
				f = _wfopen(wfilenm.c_str(), OpenMode::ToWCString(mode));
			}
			# endif
			return f;
		}

	} // anonymous namespace
	# endif




	Stream::Stream(const AnyString& filename, int mode)
		: pFd(nullptr)
	{
		open(filename, mode);
	}



	bool Stream::open(const AnyString& filename, int mode)
	{
		// Close the file if already opened
		if (pFd)
			(void)::fclose(pFd);

		# ifdef YUNI_OS_WINDOWS
		pFd = OpenFileOnWindows(filename, mode);
		# else
		// It is mandatory to open file with the flag O_CLOEXEC to avoid race
		// conditions with fork
		// fopen should used O_CLOEXEC as one of the option. However, at the current
		// state, not all operating systems do that.
		// So we have to do it by ourselves with open and fdopen.
		/*int flag = O_CLOEXEC;
		if (0 != (mode & OpenMode::read) and 0 != (mode & OpenMode::write))
			flag |= O_RDWR;
		else if (0 != (mode & OpenMode::read))
			flag |= O_RDONLY;
		else if (0 != (mode & OpenMode::write))
			flag |= O_WRONLY;

		if (0 != (mode & OpenMode::truncate))
			flag |= O_TRUNC;
		else if (0 != (mode & OpenMode::append))
			flag |= O_APPEND;

		if (0 != (mode & ~OpenMode::read))
			flag |= O_CREAT;

		int fd = ::open(filename.c_str(), flag);
		if (fd < 0) // error
			pFd = nullptr;
		else
			pFd = ::fdopen(fd, OpenMode::ToCString(mode));*/
		pFd = ::fopen(filename.c_str(), OpenMode::ToCString(mode));
		# endif

		return (NULL != pFd);
	}



	bool Stream::close()
	{
		if (pFd)
		{
			if (0 == ::fclose(pFd))
			{
				pFd = NULL;
				return true;
			}
			return false;
		}
		return true;
	}


	bool Stream::seek(ssize_t offset, SeekOrigin origin)
	{
		switch (origin)
		{
			case seekOriginBegin:   return (0 == ::fseek(pFd, (long) offset, SEEK_SET));
			case seekOriginCurrent: return (0 == ::fseek(pFd, (long) offset, SEEK_CUR));
			case seekOriginEnd:     return (0 == ::fseek(pFd, (long) offset, SEEK_END));
		}
		return false;
	}



	bool Stream::lockShared()
	{
		# ifndef YUNI_OS_WINDOWS
		return pFd ? (0 == flock(FILENO(pFd), LOCK_SH)) : false;
		# else
		// warning The implementation is missing on Windows (#346)
		assert("Stream::lock: the implementation is missing on Windows, see ticket #346");
		return false;
		# endif
	}


	bool Stream::lockExclusive()
	{
		# ifndef YUNI_OS_WINDOWS
		return pFd ? (0 == flock(FILENO(pFd), LOCK_EX)) : false;
		# else
		// warning The implementation is missing on Windows (#346)
		assert("Stream::lock: the implementation is missing on Windows, see ticket #346");
		return false;
		# endif
	}


	void Stream::unlock()
	{
		# ifndef YUNI_OS_WINDOWS
		if (pFd)
			flock(FILENO(pFd), LOCK_UN);
		# else
		// warning The implementation is missing on Windows (#346)
		assert("Stream::lock: the implementation is missing on Windows, see ticket #346");
		# endif
	}


	uint64 Stream::read(char* buffer, uint64 size)
	{
		//TODO We should not read with block too large (especially on 32bits)
		// systems. It can make the process/thread interruptible  too long
		return (uint64) ::fread(buffer, 1, (size_t)size, pFd);
	}


	uint64 Stream::write(const char* buffer, uint64 size)
	{
		//TODO We should not write with block too large (especially on 32bits)
		// systems. It can make the process/thread interruptible  too long
		return (uint64) ::fwrite(buffer, 1, (size_t)size, pFd);
	}



	# if !defined(YUNI_HAS_POSIX_FALLOCATE) and !defined(YUNI_OS_MAC)

	static bool TruncateFileDefault(Stream& file, uint64 size)
	{
		// Default implementation

		// Getting the current end of file
		if (not file.seekFromEndOfFile(0))
			return false;
		ssize_t end = (ssize_t) file.tell();

		#	ifndef YUNI_OS_MSVC
		bool result = (0 == ::ftruncate(FILENO(file.nativeHandle()), (off_t) size));
		#	else
		bool result = (0 == _chsize_s(FILENO(file.nativeHandle()), (sint64) size));
		#	endif
		if (result)
		{
			// if the file was already bigger than the new size, there is nothing to do
			if ((uint64)end >= size)
				return true;

			if (not file.seekFromBeginning(end))
				return false;

			enum
			{
				bufferSize = 1024 * 1024
			};
			size -= (uint64) end;

			if (size)
			{
				char* zero = new char[bufferSize];
				(void)::memset(zero, '\0', sizeof(char) * bufferSize);

				while (size > bufferSize)
				{
					if (bufferSize != file.write(zero, bufferSize))
					{
						delete[] zero;
						return false;
					}
					size -= bufferSize;
				}

				if (size)
				{
					if (size != file.write(zero, size))
					{
						delete[] zero;
						return false;
					}
				}
				delete[] zero;
			}
			return true;
		}
		return false;
	}

	# endif



	bool Stream::truncate(uint64 size, bool ensureAllocation)
	{
		if (pFd)
		{
			int fd = FILENO(pFd);

			if (not ensureAllocation)
			{
				# ifndef YUNI_OS_MSVC
				return (0 == ::ftruncate(fd, (off_t) size));
				# else
				return (0 == _chsize_s(fd, (sint64) size));
				# endif
			}
			else
			{
				# ifdef YUNI_HAS_POSIX_FALLOCATE
				return (0 == posix_fallocate(fd, 0, (off_t) size));
				# else

				# ifdef YUNI_OS_MAC
				// On OS X, we can use fcntl(F_PREALLOCATE) to emulate posix_fallocate
				// (but ftruncate must be called anyway)
				fstore_t store;
				memset(&store, 0, sizeof(store));
				store.fst_flags    = F_ALLOCATECONTIG;
				store.fst_posmode  = F_PEOFPOSMODE;
				store.fst_length   = (off_t) size;

				if (-1 == fcntl(fd, F_PREALLOCATE, &store))
				{
					// OK, perhaps we are too fragmented, allocate non-continuous
					store.fst_flags = F_ALLOCATEALL;
					if (-1 == fcntl(fd, F_PREALLOCATE, &store))
						return false;
				}
				return (0 == ::ftruncate(fd, (off_t) size));

				# else

				return TruncateFileDefault(*this, size);

				# endif // OS X
				# endif // POSIX_FALLOCATE
			}
		}
		return false;
	}




} // namespace File
} // namespace IO
} // namespace Yuni

