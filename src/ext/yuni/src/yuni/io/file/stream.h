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
#include "../../yuni.h"
#include "../../core/static/assert.h"
#include "../../core/string.h"
#include "../../core/static/remove.h"
#include "../../core/noncopyable.h"
#include "openmode.h"
#include <stdio.h>



namespace Yuni
{
namespace IO
{
namespace File
{

	/*!
	** \brief Seek origin
	*/
	enum SeekOrigin
	{
		//! From the begining of the stream
		seekOriginBegin,
		//! From the current position in the stream
		seekOriginCurrent,
		//! From the end of the stream
		seekOriginEnd,
	};




	/*!
	** \brief A low-level implementation for reading and writing files
	**
	** The file will be automatically closed (if not already done) at the
	** destruction of the object.
	**
	** Here is a simple example for reading a file, line by line :
	** \code
	** IO::File::Stream file;
	** // opening out file
	** if (file.open("myfile.txt"))
	** {
	**		// A buffer. The given capacity will be the maximum length for a single line
	**		Clob buffer;
	**		while (file.readline(buffer))
	**		{
	**			// do something with the buffer
	**			// here we will merely dump it to the std::cout
	**			std::cout << buffer << std::endl;
	**		}
	** }
	** // the file will be implicitely closed here
	** \endcode
	** Obviously, you can also consider \p IO::File::ReadLineByLine.
	**
	** When writing a data into a file, the data will be written 'as it' if it can
	** be represented by a mere c-string. Otherwise a 'Yuni::String' will be used
	** to perform the convertion.
	** \code
	** IO::File::Stream file;
	** if (file.open("out.txt", IO::OpenMode::write | IO::OpenMode::truncate))
	** {
	**		file << "Without implicit convertion: Hello world !\n";
	**		file << "With implicit convertion   : " << 42 << '\n';
	** }
	** \endcode
	**
	** \internal This implementation is most of the time a C++ wrapper over the standard
	**   routines 'fopen', 'fclose'... The implementation is a bit different on Windows
	**   because 'fopen' only handles ansi filenames.
	*/
	class Stream final : private NonCopyable<Stream>
	{
	public:
		//! The native handle type
		typedef FILE* HandleType;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		Stream();
		/*!
		** \brief Open a file
		*/
		explicit Stream(const AnyString& filename, int mode = OpenMode::read);
		/*!
		** \brief Destructor
		**
		** The file will be closed if necessary
		*/
		~Stream();
		//@}


		//! \name Open / Close a file
		//@{
		/*!
		** \brief Open a file
		**
		** The file will be previously closed if necessary.
		**
		** \param filename Any string reprensenting an UTF8 relative or absolute filename
		** \param mode The open mode to use
		** \return True if the operation succeeded, false otherwise
		*/
		bool open(const AnyString& filename, int mode = OpenMode::read);

		/*!
		** \brief Open a file for writing
		**
		** This method is strictly equivalent to :
		** \code
		** open(filename, IO::OpenMode::write | IO::OpenMode::truncate);
		** \endcode
		** The file will be previously closed if necessary.
		**
		** \param filename Any string reprensenting an UTF8 relative or absolute filename
		** \return True if the operation succeeded, false otherwise
		*/
		bool openRW(const AnyString& filename);

		/*!
		** \brief Close the file if opened
		*/
		bool close();
		//@}


		//! \name Stream
		//@{
		/*!
		** \brief Get if a file is currently opened
		*/
		bool opened() const;

		/*!
		** \brief Get if the end-of-file has been reached
		*/
		bool eof() const;

		/*!
		** \brief Get the current value of the file position indicator
		*/
		ssize_t tell() const;

		/*!
		** \brief Set the position in the stream
		**
		** \param offset A relative offset
		** \param origin Origin of the offset
		** \return True if the operation succeeded, false otherwise
		*/
		bool seek(ssize_t offset, SeekOrigin origin = seekOriginCurrent);

		/*!
		** \brief Set the position indicator associated with the currently opened file from the beginning of the file
		**
		** \param offset A relative offset
		** \return True if the operation succeeded, False otherwise
		*/
		bool seekFromBeginning(ssize_t offset);

		/*!
		** \brief Set the position indicator associated with the currently opened file from the end of the file
		**
		** \param offset A relative offset
		** \return True if the operation succeeded, False otherwise
		*/
		bool seekFromEndOfFile(ssize_t offset);

		/*!
		** \brief Move the position indicator from the current position in the stream
		**
		** \param offset A relative offset
		** \return True if the operation succeeded, False otherwise
		*/
		bool seekMove(ssize_t offset);

		/*!
		** \brief Flush the last I/O operations
		**
		** \return True if the operation succeeded, False otherwise
		*/
		bool flush();
		//@}


		//! \name Read
		//@{
		/*!
		** \brief Read a single char
		*/
		char get();

		/*!
		** \brief Read a line from the file
		**
		** \param buffer The buffer where to write the line
		** \param maxSize The maximum allowed size for the buffer
		*/
		bool readline(char* buffer, size_t maxSize);

		/*!
		** \brief Read a line from the file
		**
		** It reads a line into the buffer pointed to by \p buffer until either a terminating
		** newline or EOF, which it replaces with ’\0’.
		** The maximum number of char read is `buffer.chunkSize`. For code robutness
		** (to prevent against misuses) this routine will reserve space according to
		** the chunk size.
		**
		** The newline, if any, is retained.
		**
		** \param buffer The buffer where to write the line
		** \param trim   True to remove any trailing linefeed
		*/
		template<class StringT> bool readline(StringT& buffer, bool trim = true);

		/*!
		** \brief Read a line from the file (with a custom chunk size)
		**
		** It reads a line into the buffer pointed to by \p buffer until either a terminating
		** newline or EOF, which it replaces with ’\0’.
		** The maximum number of char read is `CustomChunkT`. For code robutness
		** (to prevent against misuses) this routine will reserve space according to
		** the chunk size.
		**
		** The newline, if any, is retained.
		**
		** \param buffer The buffer where to write the line
		** \param trim   True to remove any trailing linefeed
		*/
		template<uint CustomChunkT, class StringT> bool readline(StringT& buffer, bool trim = true);


		/*!
		** \brief Read data into a string buffer
		**
		** \param buffer An arbitrary buffer (Yuni::String)
		** \param size The size in bytes to read
		** \return The number of bytes that have been read
		*/
		template<uint CSizeT, bool ExpT>
		uint64 read(CString<CSizeT,ExpT>&  buffer, uint64 size);

		/*!
		** \brief Read a buffer
		**
		** \param buffer A raw buffer where to store the data which will be read from the file
		** \param size Size (in bytes) of the data to read (and size of the buffer)
		** \return The number of bytes that have been read
		*/
		uint64 read(char* buffer, uint64 size);

		/*!
		** \brief Read data into a string buffer
		**
		** 'buffer.size() * sizeof(C)' bytes will be read from the stream and store
		** into the given buffer.
		** Use the method 'buffer.resize()' to change the buffer size before calling this method.
		**
		** \param buffer An arbitrary buffer
		** \return The number of bytes that have been read
		*/
		template<uint CSizeT, bool ExpT>
		uint64 chunkRead(CString<CSizeT,ExpT>&  buffer);
		//@}


		//! \name Write
		//@{
		/*!
		** \brief Write a chr to the stream
		*/
		bool put(char c);

		/*!
		** \brief Write a raw buffer
		**
		** \param buffer An arbitrary buffer
		** \param size Size of the buffer to write
		** \return The number of bytes that have been written
		*/
		uint64 write(const char* buffer, uint64 size);

		/*!
		** \brief Write any arbitrary buffer
		**
		** \param buffer An arbitrary buffer (const char*, String, CString)
		** \return The number of bytes that have been written
		*/
		template<class U> uint64 write(const U& buffer);

		//! Write an interger
		uint write(char buffer);
		//! Write an interger
		uint write(sint16  value);
		//! Write an interger
		uint write(sint32  value);
		//! Write an interger
		uint write(sint64  value);
		//! Write an interger
		uint write(uint16  value);
		//! Write an interger
		uint write(uint32  value);
		//! Write an interger
		uint write(uint64  value);
		//! Write a bool
		uint write(bool value);
		//! Write a float
		uint write(float value);
		//! Write a double
		uint write(double value);

		/*!
		** \brief Write any arbitrary buffer
		**
		** \param buffer An arbitrary buffer (const char*, String, CString)
		** \param maxsize Maximum Size of the buffer to write
		** \return The number of bytes that have been written
		*/
		template<class U> uint64 write(const U& buffer, uint64 maxsize);
		uint write(char buffer, uint64 maxsize);

		//! Write an interger
		uint write(sint16  value, uint64 maxsize);
		//! Write an interger
		uint write(sint32  value, uint64 maxsize);
		//! Write an interger
		uint write(sint64  value, uint64 maxsize);
		//! Write an interger
		uint write(uint16  value, uint64 maxsize);
		//! Write an interger
		uint write(uint32  value, uint64 maxsize);
		//! Write an interger
		uint write(uint64  value, uint64 maxsize);
		//! Write a bool
		uint write(bool value, uint64 maxsize);
		//! Write a float
		uint write(float value, uint64 maxsize);
		//! Write a double
		uint write(double value, uint64 maxsize);
		//@}


		//! \name Locking
		//@{
		/*!
		** \brief Lock the file for a shared access
		**
		** \warning The implementation is missing on Windows (#346)
		** \warning The user must call `unlock`
		*/
		bool lockShared();
		/*!
		** \brief Lock the file for an exclusive access
		**
		** \warning The implementation is missing on Windows (#346)
		** \warning The user must call `unlock`
		*/
		bool lockExclusive();

		/*!
		** \brief Unlock the file
		**
		** \warning The implementation is missing on Windows (#346)
		*/
		void unlock();
		//@}


		//! \name Truncating
		//@{
		/*!
		** \brief Truncate or extend the opened file
		**
		** The operation does not guarantee that the data can be really allocated
		** on disk (depending the platform), and some extra code is required to make
		** sure of it.
		** Using `ensureAllocation = true` is the best way for allocating big files.
		**
		** \param size The new size of the file
		** \param ensureAllocation True to make sure that the data are really allocated
		** \return True if the file has been resized
		*/
		bool truncate(uint64 size, bool ensureAllocation = false);
		//@}


		//! \name Native
		//@{
		//! Get the OS Dependant handle
		HandleType nativeHandle() const;
		//@}


		//! \name Operators
		//@{
		//! True if the stream if not opened
		bool operator ! () const;
		//! operator += (write)
		template<class U> Stream& operator += (const U& u);
		//! operator += (write)
		Stream& operator += (const char c);

		//! operator << (write)
		template<class U> Stream& operator << (const U& u);
		//! operator << (write)
		Stream& operator << (const char c);

		//! Operator >> (read)
		template<class U> Stream& operator >> (U& rhs);
		//@}


	private:
		//! A FILE pointer
		HandleType pFd;

	}; // class Stream






} // namespace File
} // namespace IO
} // namespace Yuni

#include "stream.hxx"
