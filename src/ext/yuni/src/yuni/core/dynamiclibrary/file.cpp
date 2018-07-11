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
#include "../system/windows.hdr.h"
#include "file.h"
#ifndef YUNI_OS_WINDOWS
# include <dlfcn.h>
#else
# include "../../core/string/wstring.h"
#endif
#include <stdio.h>
#include "../../io/file.h"
#include "../../io/directory.h"


#ifdef YUNI_OS_WINDOWS
# define YUNI_DYNLIB_DLOPEN(X)      ::LoadLibraryW(X)
#else
# define YUNI_DYNLIB_DLOPEN(X,M)    ::dlopen(X,M)
#endif

#ifdef YUNI_OS_WINDOWS
# define YUNI_DYNLIB_DLCLOSE(X)     ::FreeLibrary(X)
#else
# define YUNI_DYNLIB_DLCLOSE(X)     ::dlclose(X)
#endif

#ifdef YUNI_OS_WINDOWS
# define YUNI_DYNLIB_DLSYM(X, Y)    ::GetProcAddress(X,Y)
#else
# define YUNI_DYNLIB_DLSYM(X, Y)    ::dlsym(X,Y)
#endif




namespace Yuni
{
namespace DynamicLibrary
{


	// Implementation of the static variable
	const File::Handle File::NullHandle = nullptr;


	namespace // anonymous
	{

		/*!
		** \brief Try to find a file from a single path, a filename and a prefix
		**
		** \param[out] out A temporary string, where to write the absolute filename
		** \param prefix The prefix to use for the filename
		** \return True if the filename in `s` exists and should be loaded, False otherwise
		**/
		template<class StringT>
		static inline bool FindLibraryFile(StringT& out, /*const StringT2& path,*/ const AnyString& filename, const char* prefix)
		{
			# define TEST_THEN_LOAD(EXT) \
				out.clear(); \
				/*if (!path.empty()) */ \
					/*out << path << IO::Separator; */ \
				out << prefix << filename << EXT; \
				if (IO::File::Exists(out)) \
					return true

			# ifdef YUNI_OS_DARWIN
			TEST_THEN_LOAD(".dylib");
			TEST_THEN_LOAD(".bundle");
			# endif

			# ifdef YUNI_OS_AIX
			TEST_THEN_LOAD(".a");
			# endif
			# ifdef YUNI_OS_HPUX
			TEST_THEN_LOAD(".sl");
			# endif

			# ifdef YUNI_OS_WINDOWS
			TEST_THEN_LOAD(".dll");
			# else
			TEST_THEN_LOAD(".so");
			# endif

			return false;
			# undef TEST_THEN_LOAD
		}


		/*!
		** \brief Try to find a file from a list of paths, a filename and a prefix
		**
		** \return True if the filename in `s` exists and should be loaded, False otherwise
		**/
		static inline bool FindLibrary(String& out, const AnyString& filename)
		{
			return (FindLibraryFile(out, filename, "lib") or FindLibraryFile(out, filename, ""));
		}


	} // anonymous namespace




	File::File(const AnyString& filename, Relocation relocation, Visibility visibility) :
		pHandle(NullHandle)
	{
		(void) loadFromFile(filename, relocation, visibility);
	}


	File::File(const AnyString& filename) :
		pHandle(NullHandle)
	{
		(void) loadFromFile(filename, relocationLazy, visibilityDefault);
	}


	File::File() :
		pHandle(NullHandle)
	{}


	File::~File()
	{
		if (NullHandle != pHandle)
			YUNI_DYNLIB_DLCLOSE(pHandle);
	}


	bool File::loadFromFile(const AnyString& filename, File::Relocation r, File::Visibility v)
	{
		// No filename
		if (not filename.empty())
		{
			// If the file name is absolute, there is no need for research
			if (IO::IsAbsolute(filename))
				return loadFromRawFilename(filename, r, v);

			// A temporary string, where to write the absolute filename
			String s;
			s.reserve(512);

			// Search paths
			// TODO : find a far more efficient way for doing this
			if (FindLibrary(s, filename))
				return loadFromRawFilename(s, r, v);
		}

		// Make sure the library has been unloaded
		// This unloading would have been done by `loadFromRawFilename()` if
		// something was found
		unload();
		// We have found nothing :(
		return false;
	}


	void File::unload()
	{
		if (loaded())
		{
			YUNI_DYNLIB_DLCLOSE(pHandle);
			pHandle = NullHandle;
			pFilename.clear();
			pFilename.shrink();
		}
	}






	# ifdef YUNI_OS_WINDOWS

	// Specific implementation for the Windows platform
	bool File::loadFromRawFilename(const AnyString& filename, File::Relocation, File::Visibility)
	{
		// Unload the library if already loaded
		unload();

		if (not filename.empty())
		{
			// Loading
			WString buffer(filename, true);
			if (buffer.empty())
				return false;

			pHandle = YUNI_DYNLIB_DLOPEN(buffer.c_str());
			if (NullHandle != pHandle)
			{
				pFilename = filename;
				return true;
			}
		}
		return false;
	}

	# else

	// Specific implementation for the Unix platforms
	bool File::loadFromRawFilename(const AnyString& filename, File::Relocation r, File::Visibility v)
	{
		// Unload the library if already loaded
		unload();

		if (not filename.empty())
		{
			// The mode
			int mode = ((relocationLazy == r) ? RTLD_LAZY : RTLD_NOW);
			if (visibilityDefault != v)
				mode |= ((visibilityGlobal == v) ? RTLD_GLOBAL : RTLD_LOCAL);

			// Loading
			pHandle = YUNI_DYNLIB_DLOPEN(filename.c_str(), mode);
			if (NullHandle != pHandle)
			{
				pFilename = filename;
				return true;
			}
		}
		return false;
	}

	# endif



	bool File::hasSymbol(const AnyString& name) const
	{
		return NullHandle != pHandle
			and NULL != reinterpret_cast<Symbol::Handle>(YUNI_DYNLIB_DLSYM(pHandle, name.c_str()));
	}


	Symbol File::resolve(const AnyString& name) const
	{
		return NullHandle != pHandle
			? reinterpret_cast<Symbol::Handle>(YUNI_DYNLIB_DLSYM(pHandle, name.c_str()))
			: nullptr;
	}




} // namespace DynamicLibrary
} // namespace Yuni

