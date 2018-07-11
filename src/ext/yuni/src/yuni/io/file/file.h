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
#include "../io.h"
#include "openmode.h"
#include <string>



namespace Yuni
{
namespace IO
{

/*!
** \brief File manipulation functions
*/
namespace File
{

	// constants
	enum
	{
		//! The maximum allowed size for a file in memory (Default: 80Mo)
		sizeHardLimit = 83886080u,  // 80Mo = 80 * 1024 * 1024
	};



	/*!
	** \brief Test if a node exists and is actually a file
	**
	** To test if a node exists whatever its nature, you should use
	** IO::Exists() instead.
	**
	** \param filename The folder/filename to test
	** \return True if it exists, false otherwise
	*/
	bool Exists(const AnyString& filename);


	/*!
	** \brief Get the size (in bytes) of a file
	**
	** \param filename The filename
	** \param[out] size The size (in bytes) of the file. The value is guaranteed to be set (null) is an error has occured
	** \return True if the operation succeeded, False otherwise
	*/
	bool Size(const AnyString& filename, uint64& size);

	/*!
	** \brief Get the size (in bytes) of a file
	**
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/core/unit/data.h>
	** #include <yuni/io/file.h>
	** #include <iostream>
	**
	** using namespace Yuni;
	** int main()
	** {
	**	Unit::Data::Octet::SIBaseUnit<uint64> size = IO::File::Size("/path/to/my/file");
	**	std::cout << Unit::Data::Octet::Megaoctet<double>(size) << std::endl;
	**	return 0;
	** }
	** \endcode
	**
	** \param filename The filename
	** \return The size (in bytes) of the file. The returned value is guaranteed to be null is an error has occured
	*/
	uint64 Size(const AnyString& filename);


	//! \name Date / Time
	//@{
	/*!
	** \brief Get the unix timestamp of the last modification
	*/
	sint64 LastModificationTime(const AnyString& filename);
	//@}


	//! \name Load the content of a file
	//@{
	/*!
	** \brief Load the entire content of a file into memory
	**
	** If the file size is greater than \p hardlimit, the content will be truncated (see 'errMemoryLimit').
	** \param[out] out The content of the file
	** \param filename The filename to open
	** \param hardlimit If the size of the file exceeds this limit, it will not be loaded
	** \return errNone if successful
	*/
	IO::Error LoadFromFile(std::string& out, const AnyString& filename, uint64 hardlimit = sizeHardLimit);
	/*!
	** \brief Load the entire content of a file into memory
	**
	** If the file size is greater than \p hardlimit, the content will be truncated (see 'errMemoryLimit').
	** \param[out] out The content of the file
	** \param filename The filename to open
	** \param hardlimit If the size of the file exceeds this limit, it will not be loaded
	** \return errNone if successful
	*/
	IO::Error LoadFromFile(String& out, const AnyString& filename, uint64 hardlimit = sizeHardLimit);
	/*!
	** \brief Load the entire content of a file into memory
	**
	** If the file size is greater than \p hardlimit, the content will be truncated (see 'errMemoryLimit').
	** \param[out] out The content of the file
	** \param filename The filename to open
	** \param hardlimit If the size of the file exceeds this limit, it will not be loaded
	** \return errNone if successful
	*/
	IO::Error LoadFromFile(Clob& out, const AnyString& filename, uint64 hardlimit = sizeHardLimit);


	/*!
	** \brief Save the content of a string into a file
	**
	** \param filename The filename to create/overwrite
	** \param content The new content of the file
	** \return True if the operation succeeded, false otherwise
	*/
	template<class U>
	YUNI_DEPRECATED("use SetContent / AppendContent instead",
	bool SaveToFile(const AnyString& filename, const U& content));
	//@}


	/*!
	** \brief Copy a single file
	**
	** \param from The source file
	** \param to The target file
	** \param overwrite Overwrite the target file if already exists
	** \return errNone if the operation succeeded
	*/
	IO::Error Copy(const AnyString& from, const AnyString& to, bool overwrite = true);


	/*!
	** \brief Delete a file
	**
	** \param filename The file to delete
	*/
	IO::Error Delete(const AnyString& filename);


	/*!
	** \brief Create or erase a file
	**
	** \param filename An UTF8 filename
	** \return True if the file has been created or truncated
	*/
	bool CreateEmptyFile(const AnyString& filename);


	/*!
	** \brief Set the content of a file with an arbitrary string
	**
	** \code
	** IO::File::SetContent("/tmp/anyfile.txt", "Hello world !\n");
	** \endcode
	*/
	template<class U> bool SetContent(const AnyString& filename, const U& content);

	/*!
	** \brief Append the content of an arbitrary string to a file
	**
	** \code
	** IO::File::AppendContent("/tmp/anyfile.txt", "lorem ipsumi\n");
	** \endcode
	*/
	template<class U> bool AppendContent(const AnyString& filename, const U& content);


	/*!
	** \brief Read a file line by line
	**
	** A simple `cat` :
	** \code
	** IO::File::ReadLineByLine("/tmp/foo.txt", [&] (const String& line)
	** {
	**	std::cout << line << std::endl;
	** });
	** \endcode
	**
	** \param filename A filename
	** \param predicate A functor or a lambda function
	*/
	template<class PredicateT>
	bool ReadLineByLine(const AnyString& filename, const PredicateT& predicate);






} // namespace File
} // namespace IO
} // namespace Yuni

#include "stream.h"
#include "file.hxx"

