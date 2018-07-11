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
#include "../../../yuni.h"
#include "../../../core/string.h"
#include "../../io.h"
#include "../../directory.h"
#include "../../../core/iterator/iterator.h"
#include "platform.h"



namespace Yuni
{
namespace IO
{
namespace Directory
{

	/*!
	** \brief Convenient tool for creating, moving, and enumerating through directories and subdirectories
	**
	** Example for iterating through a directory :
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/io/directory/info.h>
	** #include <iostream>
	**
	** using namespace Yuni;
	**
	**
	** int main()
	** {
	**	IO::Directory::Info info{"/tmp"};
	**	for (IO::Directory::Info::iterator i = info.begin(); i != info.end(); ++i)
	**		std::cout << *i << std::endl;
	**
	**	return 0;
	** }
	** \endcode
	*/
	class Info final
	{
	public:
		//! \name Iterators
		//@{
		//! Options for performing an iteration
		enum OptionIterator
		{
			itNone       = 0,
			//! Include files
			itFile       = 1,
			//! Include folders
			itFolder     = 2,
			//! Perform a recursive iteration
			itRecursive  = 4,
			//! All options
			itAll        = itFile | itFolder | itRecursive,
		};

		// implementation
		# include "iterator.inc.hpp"

		// \internal We must specify the full namespace (`Yuni::...`) to avoid conflict
		//   with  Yuni::IO::Directory::IIterator.

		//! The default iterator, for iterating through all files and folders within a directory
		typedef Yuni::IIterator<Model::Iterator<itFile|itFolder>, true>  iterator;
		//! The default iterator, for iterating through all files within a directory
		typedef Yuni::IIterator<Model::Iterator<itFile>, true>    file_iterator;
		//! The default iterator, for iterating through all folders within a directory
		typedef Yuni::IIterator<Model::Iterator<itFolder>, true>  folder_iterator;
		//! The default iterator, for recursively iterating through all files and folders within a directory
		typedef Yuni::IIterator<Model::Iterator<itAll>, true>     recursive_iterator;
		//! The default iterator, for iterating through all files within a directory
		typedef Yuni::IIterator<Model::Iterator<itFile|itRecursive>, true>    recursive_file_iterator;
		//! The default iterator, for iterating through all folders within a directory
		typedef Yuni::IIterator<Model::Iterator<itFolder|itRecursive>, true>  recursive_folder_iterator;
		//! A generic null iterator
		typedef Yuni::IIterator<Model::NullIterator, true>  null_iterator;
		//@}


	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Info() {}
		/*!
		** \brief Constructor
		** \param directory A directory
		*/
		explicit Info(const AnyString& directory);
		//@}

		/*!
		** \brief Get if the directory exists
		*/
		bool exists() const;

		/*!
		** \brief Normalize the directory path
		** \see Yuni::IO::Normalize()
		*/
		void normalize();

		/*!
		** \brief Normalize the directory path
		**
		** A temporary string is required to normalize the path. This
		** additional parameter allows the user to provide its own
		** temporary buffer to be reused when performing the operation.
		** This should reduce the overhead due to useless memory allocation.
		**
		** \tparam StringT Any container of type string (String, std::string...)
		*/
		template<class StringT> void normalize(StringT& tmp);


		//! \name Common operations
		//@{
		/*!
		** \brief Create the directory recursively
		**
		** \param mode Access permissions (ignored on the MS Windows platforms)
		** \return True if the operation succeeded, false otherwise
		** \see Yuni::IO::Directory::Create()
		*/
		bool create(uint mode = 0755) const;

		/*!
		** \brief Remove the directory and all its content
		**
		** \return True if the operation succeeded False otherwise
		** \see Yuni::IO::Directory::Remove()
		*/
		bool remove() const;

		/*!
		** \brief Clean the directory (remove all its content but keep the top folder)
		*/
		bool clean() const;

		/*!
		** \brief Copy the directory and its content into another location
		**
		** \param destination The destination folder
		** \return True if the operation succeeded False otherwise
		** \see Yuni::IO::Directory::Copy()
		*/
		bool copy(const AnyString& destination) const;

		/*!
		** \brief Set as the current directory
		**
		** \return True if the operation succeeded False otherwise
		** \see Yuni::IO::Directory::Current::Set()
		*/
		bool setAsCurrentDirectory() const;
		//@}


		//! \name Iterators
		//@{
		iterator begin() const;
		null_iterator end() const;

		file_iterator file_begin() const;
		null_iterator file_end() const;

		folder_iterator folder_begin() const;
		null_iterator folder_end() const;

		recursive_iterator recursive_begin() const;
		null_iterator recursive_end() const;

		recursive_file_iterator recursive_file_begin() const;
		null_iterator recursive_file_end() const;

		recursive_folder_iterator recursive_folder_begin() const;
		null_iterator recursive_folder_end() const;
		//@}


		/*!
		** \brief Get a reference to the directory string itself (const)
		*/
		const String& directory() const;

		/*!
		** \brief Get a reference to the directory string itself
		**
		** \note The directory can directly be modified by the user without any
		**   restriction.
		**   This can be useful to reduce memory allocations.
		*/
		String& directory();


		//! \name Operators
		//@{
		//! Assignement
		Info& operator = (const Info& rhs);
		/*!
		** \brief Assignment, for any types convertible into a string
		*/
		Info& operator = (const AnyString& rhs);

		//! Comparison
		bool operator == (const Info& rhs) const;
		//! Comparison
		template<class U> bool operator == (const U& rhs) const;

		template<class U> Info& operator += (const U& rhs);
		template<class U> Info& operator << (const U& rhs);
		//@}


	private:
		//! The directory
		String pDirectory;

	}; // class Info






} // namespace Directory
} // namespace IO
} // namespace Yuni

# include "info.hxx"

