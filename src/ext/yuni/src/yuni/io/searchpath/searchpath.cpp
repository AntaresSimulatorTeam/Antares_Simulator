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
#include "searchpath.h"
#include "../../core/static/types.h"
#include "../io.h"
#include "../directory/info/info.h"


namespace Yuni
{
namespace IO
{

	namespace // anonymous
	{

		static inline bool ValidateExtension(const String::Vector& extensions, const String& extension)
		{
			const String::Vector::const_iterator end = extensions.end();
			for (String::Vector::const_iterator i = extensions.begin(); i != end; ++i)
			{
				if (*i == extension)
					return true;
			}
			return false;
		}


		static inline bool ValidatePrefix(const String::Vector& prefixes, const String& text)
		{
			const String::Vector::const_iterator end = prefixes.end();
			for (String::Vector::const_iterator i = prefixes.begin(); i != end; ++i)
			{
				if (not (*i) or (text).startsWith(*i))
					return true;
			}
			return false;
		}





		template<class OutT>
		class LookupHelper final
		{
		public:
			enum
			{
				outIsRawString = Static::Type::StrictlyEqual<String, OutT>::Yes
			};

		public:
			LookupHelper(OutT& out, const AnyString& filename, const String::Vector& directories,
				const String::Vector&  extensions, const String::Vector&  prefixes) :
				out(out),
				filename(filename),
				directories(directories),
				extensions(extensions),
				prefixes(prefixes),
				pResultCount(0)
			{}

			bool operator() ()
			{
				if (directories.empty())
				{
					return iterateThroughPrefixes<false>(empty);
				}
				else
				{
					const String::Vector::const_iterator end = directories.end();
					for (String::Vector::const_iterator i = directories.begin(); i != end; ++i)
					{
						if (iterateThroughPrefixes<true>(*i))
							return true;
					}
					return outIsRawString ? false : (0 != pResultCount);
				}
			}


			template<bool HasDirectoryT>
			bool iterateThroughPrefixes(const String& directory)
			{
				if (prefixes.empty())
				{
					return iterateThroughExtensions<HasDirectoryT>(directory, empty);
				}
				else
				{
					const String::Vector::const_iterator end = prefixes.end();
					for (String::Vector::const_iterator i = prefixes.begin(); i != end; ++i)
					{
						if (iterateThroughExtensions<HasDirectoryT>(directory, *i))
							return true;
					}
					return false;
				}
			}


			template<bool HasDirectoryT>
			bool iterateThroughExtensions(const String& directory, const String& prefix)
			{
				if (extensions.empty())
				{
					return checkForFile<HasDirectoryT>(directory, prefix, empty);
				}
				else
				{
					const String::Vector::const_iterator end = extensions.end();
					for (String::Vector::const_iterator i = extensions.begin(); i != end; ++i)
					{
						if (checkForFile<HasDirectoryT>(directory, prefix, *i))
							return true;
					}
					return false;
				}
			}


			template<bool HasDirectoryT>
			bool checkForFile(const String& directory, const String& prefix, const String& extension)
			{
				pQuery.clear();
				if (HasDirectoryT)
					pQuery << directory << IO::Separator;

				pQuery << prefix << filename << extension;
				if (IO::Exists(pQuery))
				{
					out.push_back(pQuery);
					if (outIsRawString)
						return true; // found something - stop the loop
					else
						++pResultCount;
				}
				return false; // continue the loop
			}


		public:
			//! The output
			OutT& out;
			//! The filename
			AnyString filename;
			//! List of directories where to search of
			const String::Vector&  directories;
			//! List of extensions
			const String::Vector&  extensions;
			//! List of prefixes
			const String::Vector&  prefixes;
			//! Empty string
			String empty;
			//!
			unsigned int pResultCount;

		private:
			String pQuery;

		}; // class LookupHelpder



	} // anonymous namespace




	SearchPath::SearchPath() :
		pCacheLookup(false) // cache disabled by default
	{}


	SearchPath::~SearchPath()
	{
		// does nothing
	}


	bool SearchPath::find(String& out, const AnyString& filename) const
	{
		out.clear();
		if (filename.empty())
			return false;

		if (pCacheLookup)
		{
			CacheStore::const_iterator i = pCache.find((pCacheQuery = filename));
			if (i != pCache.end())
			{
				out = i->second;
				return true;
			}
		}
		LookupHelper<String> lookup(out, filename, directories, extensions, prefixes);
		return lookup();
	}


	bool SearchPath::find(String::Vector& out, const AnyString& filename) const
	{
		out.clear();
		if (filename.empty())
			return false;

		if (pCacheLookup)
		{
			CacheStore::const_iterator i = pCache.find((pCacheQuery = filename));
			if (i != pCache.end())
			{
				out.push_back(i->second);
				return true;
			}
		}
		LookupHelper<String::Vector> lookup(out, filename, directories, extensions, prefixes);
		return lookup();
	}


	bool SearchPath::find(String::List& out, const AnyString& filename) const
	{
		out.clear();
		if (filename.empty())
			return false;

		if (pCacheLookup)
		{
			CacheStore::const_iterator i = pCache.find((pCacheQuery = filename));
			if (i != pCache.end())
			{
				out.push_back(i->second);
				return true;
			}
		}
		LookupHelper<String::List> lookup(out, filename, directories, extensions, prefixes);
		return lookup();
	}



	void SearchPath::each(const Bind<void (const String&, const String&)>& callback,
		bool recursive, bool catchfolder) const
	{
		if (directories.empty())
			return;

		String extension;
		IO::Directory::Info info;

		if (catchfolder)
		{
			if (recursive)
			{
				const String::Vector::const_iterator end = directories.end();
				for (String::Vector::const_iterator i = directories.begin(); i != end; ++i)
				{
					info.directory() = *i;
					IO::Directory::Info::recursive_iterator itend = info.recursive_end();

					for (IO::Directory::Info::recursive_iterator it = info.recursive_begin(); it != itend; ++it)
					{
						// Checking for the prefix
						if (not prefixes.empty() and not ValidatePrefix(prefixes, *it))
							continue;
						// Checking for the extension
						if (not extensions.empty())
						{
							ExtractExtension(extension, *it);
							if (not ValidateExtension(extensions, extension))
								continue;
						}
						callback(*it, it.filename());
					}
				}
			}
			else
			{
				const String::Vector::const_iterator end = directories.end();
				for (String::Vector::const_iterator i = directories.begin(); i != end; ++i)
				{
					info.directory() = *i;
					IO::Directory::Info::iterator itend = info.end();

					for (IO::Directory::Info::iterator it = info.begin(); it != itend; ++it)
					{
						// Checking for the prefix
						if (not prefixes.empty() and not ValidatePrefix(prefixes, *it))
							continue;
						// Checking for the extension
						if (not extensions.empty())
						{
							ExtractExtension(extension, *it);
							if (not ValidateExtension(extensions, extension))
								continue;
						}
						callback(*it, it.filename());
					}
				}
			}
		}
		else
		{
			if (recursive)
			{
				const String::Vector::const_iterator end = directories.end();
				for (String::Vector::const_iterator i = directories.begin(); i != end; ++i)
				{
					info.directory() = *i;
					IO::Directory::Info::recursive_file_iterator itend = info.recursive_file_end();

					for (IO::Directory::Info::recursive_file_iterator it = info.recursive_file_begin(); it != itend; ++it)
					{
						// Checking for the prefix
						if (not prefixes.empty() and not ValidatePrefix(prefixes, *it))
							continue;
						// Checking for the extension
						if (not extensions.empty())
						{
							ExtractExtension(extension, *it);
							if (not ValidateExtension(extensions, extension))
								continue;
						}
						callback(*it, it.filename());
					}
				}
			}
			else
			{
				const String::Vector::const_iterator end = directories.end();
				for (String::Vector::const_iterator i = directories.begin(); i != end; ++i)
				{
					info.directory() = *i;
					IO::Directory::Info::file_iterator itend = info.file_end();

					for (IO::Directory::Info::file_iterator it = info.file_begin(); it != itend; ++it)
					{
						// Checking for the prefix
						if (not prefixes.empty() and not ValidatePrefix(prefixes, *it))
							continue;
						// Checking for the extension
						if (not extensions.empty())
						{
							ExtractExtension(extension, *it);
							if (not ValidateExtension(extensions, extension))
								continue;
						}
						callback(*it, it.filename());
					}
				}
			}
		}
	}





} // namespace IO
} // namespace Yuni
