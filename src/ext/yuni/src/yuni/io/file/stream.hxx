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
#pragma once
#include "stream.h"
#include "../../core/string.h"



namespace Yuni
{
namespace IO
{
namespace File
{

	inline Stream::Stream()
		: pFd(nullptr)
	{
		// Do nothing
	}


	inline Stream::~Stream()
	{
		// The check is mandatory to avoid SegV on some platform (Darwin for example)
		if (pFd)
			(void)::fclose(pFd);
	}


	inline bool Stream::openRW(const AnyString& filename)
	{
		// note: 'this' added this to avoid name resolution conflicts
		return this->open(filename, IO::OpenMode::write | IO::OpenMode::truncate);
	}


	inline bool Stream::opened() const
	{
		return (NULL != pFd);
	}


	inline bool Stream::eof() const
	{
		# ifdef YUNI_OS_MSVC
		return ::feof(pFd) ? true : false;
		# else
		return ::feof(pFd);
		# endif
	}

	inline ssize_t Stream::tell() const
	{
		return (ssize_t)::ftell(pFd);
	}



	inline bool Stream::seekFromBeginning(ssize_t offset)
	{
		return (0 == ::fseek(pFd, (long)offset, SEEK_SET));
	}


	inline bool Stream::seekFromEndOfFile(ssize_t offset)
	{
		return (0 == ::fseek(pFd, (long)offset, SEEK_END));
	}


	inline bool Stream::seekMove(ssize_t offset)
	{
		return (0 == ::fseek(pFd, (long)offset, SEEK_CUR));
	}


	inline bool Stream::flush()
	{
		return (0 == ::fflush(pFd));
	}


	inline char Stream::get()
	{
		return (char)::fgetc(pFd);
	}


	inline bool Stream::readline(char* buffer, size_t maxSize)
	{
		return (NULL != fgets(buffer, (int)maxSize, pFd));
	}


	template<class StringT>
	inline bool
	Stream::readline(StringT& buffer, bool trim)
	{
		return readline<StringT::chunkSize, StringT>(buffer, trim);
	}


	template<uint CustomChunkT, class StringT>
	bool
	Stream::readline(StringT& buffer, bool trim)
	{
		// The buffer must be reserved to its full capacity just before
		// Assuming we have a mere Yuni::String, the internal may be null.
		buffer.reserve(CustomChunkT);
		// Read data from the file
		if ((NULL != ::fgets((char*)buffer.data(), static_cast<int>(CustomChunkT), pFd)))
		{
			// We may have read less than expected. So we have to resize the string
			// to perform maintenance (about the internal size and the final zero)
			buffer.resize(static_cast<uint>(::strlen(buffer.c_str())));
			if (trim)
				buffer.trimRight("\r\n");
			return true;
		}
		buffer.clear();
		return false;
	}



	inline bool Stream::put(char c)
	{
		return (EOF != ::fputc((int) c, pFd));
	}


	inline uint Stream::write(bool value)
	{
		return value
			? (uint) ::fwrite("true",  4, 1, pFd)
			: (uint) ::fwrite("false", 5, 1, pFd);
	}

	inline uint Stream::write(bool value, uint64 maxsize)
	{
		return value
			? (uint) ::fwrite("true",  (4 < maxsize) ? 4 : static_cast<size_t>(maxsize), 1, pFd)
			: (uint) ::fwrite("false", (5 < maxsize) ? 5 : static_cast<size_t>(maxsize), 1, pFd);
	}

	inline uint Stream::write(char buffer)
	{
		return (EOF != ::fputc((int) buffer, pFd)) ? 1 : 0;
	}

	inline uint Stream::write(float value)
	{
		ShortString64 string(value);
		return static_cast<uint>(::fwrite(string.c_str(), 1, string.size(), pFd));
	}

	inline uint Stream::write(float value, uint64 maxsize)
	{
		ShortString64 string(value);
		return static_cast<uint>(::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd));
	}

	inline uint Stream::write(double value)
	{
		ShortString128 string(value);
		return static_cast<uint>(::fwrite(string.c_str(), 1, string.size(), pFd));
	}

	inline uint Stream::write(double value, uint64 maxsize)
	{
		ShortString128 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(sint16 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(sint16 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(sint32 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(sint32 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(sint64 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(sint64 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(uint16 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(uint16 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(uint32 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(uint32 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}

	inline uint Stream::write(uint64 value)
	{
		ShortString32 string(value);
		return (uint) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}

	inline uint Stream::write(uint64 value, uint64 maxsize)
	{
		ShortString32 string(value);
		return static_cast<uint>(::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd));
	}

	template<class U>
	inline uint64 Stream::write(const U& buffer)
	{
		String string(buffer);
		return (uint64) ::fwrite(string.c_str(), 1, string.size(), pFd);
	}


	inline uint Stream::write(char buffer, uint64 maxsize)
	{
		return (maxsize != 0)
			? ((EOF != ::fputc(static_cast<int>(buffer), pFd)) ? 1 : 0)
			: 0;
	}

	template<class U>
	inline uint64 Stream::write(const U& buffer, uint64 maxsize)
	{
		String string(buffer);
		return (uint64) ::fwrite(string.c_str(), 1, string.size() > maxsize ? static_cast<size_t>(maxsize) : string.size(), pFd);
	}


	inline bool Stream::operator ! () const
	{
		return (NULL == pFd);
	}


	inline Stream& Stream::operator += (const char c)
	{
		(void)::fputc(static_cast<int>(c), pFd);
		return *this;
	}


	inline Stream& Stream::operator << (const char c)
	{
		(void)::fputc(static_cast<int>(c), pFd);
		return *this;
	}


	template<class U>
	inline Stream& Stream::operator += (const U& u)
	{
		write(u);
		return *this;
	}


	template<class U>
	inline Stream& Stream::operator << (const U& u)
	{
		write(u);
		return *this;
	}


	template<uint CSizeT, bool ExpT>
	inline uint64
	Stream::read(CString<CSizeT, ExpT>& buffer, uint64 size)
	{
		assert(pFd and "File not opened");
		assert(size <= static_cast<uint64>(2 * 1024) * 1024u * 1024u);
		if (0 == size)
			return 0;

		// special case for static strings
		if (not buffer.expandable and size > buffer.chunkSize)
			size = buffer.chunkSize;

		// Resizing the buffer
		buffer.resize(static_cast<uint>(size));

		// Assert to prevent SegV
		assert(buffer.capacity() != 0 and "When reading a file, the buffer must have reserved some space");

		typedef CString<CSizeT, ExpT> StringType;
		typedef typename StringType::Char C;
		// Reading the file
		size_t result = ::fread(const_cast<char*>(buffer.data()), 1, static_cast<size_t>(sizeof(C) * size), pFd);
		// Setting the good size, because we may have read less than asked
		if (result < static_cast<size_t>(buffer.size()))
			buffer.truncate(static_cast<uint>(result));
		// Making sure that the buffer is zero-terminated if required
		if (buffer.zeroTerminated)
			*(reinterpret_cast<C*>(buffer.data() + buffer.size())) = C();
		return result;
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline uint64
	Stream::chunkRead(CString<ChunkSizeT, ExpandableT>& buffer)
	{
		// Resizing the buffer
		buffer.reserve(buffer.chunkSize);
		// Assert to prevent SegV
		assert(buffer.capacity() != 0 and "When reading a file, the buffer must have reserved some space");

		typedef CString<ChunkSizeT, ExpandableT> StringType;
		typedef typename StringType::Char C;
		// Reading the file
		const uint64 result = ::fread(buffer.data(), 1, sizeof(C) * buffer.chunkSize, pFd);
		// Setting the good size, because we may have read less than asked
		if (result < buffer.size())
			buffer.truncate(static_cast<typename StringType::Size>(result));
		// Making sure that the buffer is zero-terminated if required
		if (buffer.zeroTerminated)
			*((C*)(buffer.data() + buffer.size())) = C();
		return result;
	}



	template<class U>
	inline Stream& Stream::operator >> (U&  rhs)
	{
		(void)read(rhs, rhs.size());
		return *this;
	}


	inline Stream::HandleType Stream::nativeHandle() const
	{
		return pFd;
	}





} // namespace File
} // namespace IO
} // namespace Yuni
