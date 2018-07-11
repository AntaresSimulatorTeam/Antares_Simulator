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
#include "../../core/string.h"
#include "../../core/bind.h"
#include <map>
#ifdef SearchPath
#	undef SearchPath // On Windows, SearchPath is a macro...
#endif


namespace Yuni
{
namespace IO
{

	/*!
	** \brief Find files or folders from one or several search paths
	**
	** \code
	** IO::SearchPath searchpath;
	** searchpath.directories.push_back(".");
	** searchpath.extensions.push_back(".dylib");
	** searchpath.extensions.push_back(".a");
	** searchpath.prefixes.push_back("");
	**
	** const String& location = searchpath.find("mylib");
	** if (!location)
	**	std::cerr << "impossible to find something\n";
	** else
	**	std::cout << "found: " << location << std::endl;
	**
	** // finding all matches
	** String::Vector list;
	** if (search.find(list, "mylib"))
	**	std::cerr << "impossible to find something\n";
	** else
	**	std::cout << "found: " << list << std::endl;
	** \endcode
	**
	** \note The cache lookup is disabled by default
	*/
	class YUNI_DECL SearchPath final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		SearchPath();
		//! Destructor
		~SearchPath();
		//@}


		//! \name Lookup
		//@{
		/*!
		** \brief Try to find the first filename/folder in the search paths which mathes the criteria
		** \return An absolute filename, or empty if not found
		*/
		bool find(String& out, const AnyString& filename) const;

		/*!
		** \brief Find all matching filenames/folders in the search paths which math the criteria
		*/
		bool find(String::Vector& out, const AnyString& filename) const;

		/*!
		** \brief Find all matching filenames/folders in the search paths which math the criteria
		*/
		bool find(String::List& out, const AnyString& filename) const;

		/*!
		** \brief Iterate through all files and folders
		**
		** \code
		** void print(const String& name, const String& filename)
		** {
		**	std::cout << name << " -> " << filename << std::endl;
		** }
		**
		** int main()
		** {
		**	IO::SearchPath searchpath;
		**	searchpath.directories.push_back("/lib");
		**	searchpath.extensions.push_back(".so");
		**	searchpath.each(&print);
		** \endcode
		*/
		void each(const Bind<void (const String&, const String&)>& callback,
			bool recursive = false, bool catchfolder = false) const;
		//@}


		//! \name Cache
		//@{
		//! Get if the cache is used
		bool cacheLookup() const;
		//! Set if the cache should be used
		void cacheLookup(bool enabled);

		/*!
		** \brief Clear the cache
		*/
		void cacheClear();
		//@}


	public:
		//! List of directories where to search of
		String::Vector  directories;
		//! List of extensions
		String::Vector  extensions;
		//! List of prefixes
		String::Vector  prefixes;


	private:
		typedef std::map<String, String> CacheStore;
		//! Should we use the cache ?
		bool pCacheLookup;
		//! Cache store
		CacheStore pCache;
		//! Temporary string used for cache lookup
		// (to reduce memory allocation / deallocation)
		mutable String pCacheQuery;

	}; // class SearchPath






} // namespace IO
} // namespace Yuni

#include "searchpath.hxx"

