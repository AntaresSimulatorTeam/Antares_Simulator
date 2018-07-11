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
#include <iosfwd> // STL

#ifdef None
# undef None // From X11/X.h
#endif



namespace Yuni
{

	//! Type for a default behavior / policy
	struct /*YUNI_DECL*/ Default {};

	//! Absence Option
	struct /*YUNI_DECL*/ None {};


	// Forward declaration for the base class String
	template<uint ChunkSizeT = 128, bool ExpandableT = true>
	class /*YUNI_DECL*/ CString;


	//! Standard string implementation
	typedef /*YUNI_DECL*/ CString<> String;

	//! Character Large Object
	typedef /*YUNI_DECL*/ CString<4096>  Clob;

	//! Convenient alias for short string 16 bytes
	typedef /*YUNI_DECL*/  CString<16,false>  ShortString16;
	//! Convenient alias for short string 32 bytes
	typedef /*YUNI_DECL*/  CString<32,false>  ShortString32;
	//! Convenient alias for short string 64 bytes
	typedef /*YUNI_DECL*/  CString<64,false>  ShortString64;
	//! Convenient alias for short string 128 bytes
	typedef /*YUNI_DECL*/  CString<128,false>  ShortString128;
	//! Convenient alias for short string 256 bytes
	typedef /*YUNI_DECL*/  CString<256,false>  ShortString256;
	//! Convenient alias for short string 512 bytes
	typedef /*YUNI_DECL*/  CString<512,false>  ShortString512;


} // namespace Yuni


//! Convenient typedef for cross namespace lookup
typedef Yuni::String  YString;

/*!
** \brief String adapters
**
** This is a convenient typedef for declaring a string adapter.
** A string adapter allow you to perform all read-only operations
** provided by a string to an arbitrary raw buffer, without copying it.
** This may be extremly useful to reduce memory consumption and to reduce
** some useless memory allocation.
**
** \code
** AnyString s;
** s.adapt("Here is a raw C-string");
** std::cout << "length     : " << s.size() << std::endl;
** std::cout << "find 'raw' : " << s.find("raw") << std::endl;
** \endcode
**
** Using a sub-string as it were a real string :
** \code
** String s = "abcdefghijklmnopqrst";
** AnyString adapter (s.begin() + 2, s.begin() + 9);
** std::cout << adapter << " (size: " << adapter.size() << ")" << std::endl;
** \endcode
*/
typedef ::Yuni::CString<0, true>  AnyString;


