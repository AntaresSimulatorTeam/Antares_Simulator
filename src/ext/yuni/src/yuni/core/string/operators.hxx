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
#include "string.h"



namespace Yuni
{

//! \name Operator overload for stream printing
//@{
std::ostream& operator << (std::ostream& out, const AnyString& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::String& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::Clob& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString16& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString32& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString64& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString128& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString256& rhs);
std::ostream& operator << (std::ostream& out, const Yuni::ShortString512& rhs);

std::ostream& operator << (std::ostream& out, const Yuni::UTF8::Char& rhs);


template<class CharT, class TraitsT, uint SizeT, bool ExpT>
inline std::basic_istream<CharT, TraitsT>& operator >> (std::basic_istream<CharT, TraitsT>& stream, Yuni::CString<SizeT,ExpT>& string)
{
	string.clear();
	while (stream.good())
	{
		char c;
		stream.get(c);
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			break;
		string.append(c);
	}
	return stream;
}

}






template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const Yuni::CString<SizeT,ExpT>& rhs, const char* const u)
{
	return Yuni::CString<SizeT,ExpT>(rhs) += u;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const Yuni::CString<SizeT,ExpT>& rhs, const wchar_t* u)
{
	return Yuni::CString<SizeT,ExpT>(rhs) += u;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const Yuni::CString<SizeT,ExpT>& rhs, const char u)
{
	return Yuni::CString<SizeT,ExpT>(rhs) += u;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const Yuni::CString<SizeT,ExpT>& rhs, const wchar_t u)
{
	return Yuni::CString<SizeT,ExpT>(rhs) += u;
}



template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const wchar_t* u, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return Yuni::CString<SizeT,ExpT>(u) += rhs;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const char* const u, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return Yuni::CString<SizeT,ExpT>(u) += rhs;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const char u, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return Yuni::CString<SizeT,ExpT>(u) += rhs;
}

template<uint SizeT, bool ExpT>
inline Yuni::CString<SizeT,ExpT>
operator + (const wchar_t u, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return Yuni::CString<SizeT,ExpT>(u) += rhs;
}




template<uint SizeT, bool ExpT, class TraitsT, class AllocT>
inline Yuni::CString<SizeT,ExpT>
operator + (const std::basic_string<char,TraitsT,AllocT>& u, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return Yuni::CString<SizeT,ExpT>(u) += rhs;
}


template<uint SizeT, bool ExpT, class TraitsT, class AllocT>
inline Yuni::CString<SizeT,ExpT>
operator + (const Yuni::CString<SizeT,ExpT>& rhs, const std::basic_string<char,TraitsT,AllocT>& u)
{
	return Yuni::CString<SizeT,ExpT>(rhs) += u;
}


template<uint SizeT, bool ExpT>
inline bool
operator == (const char* lhs, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs == lhs;
}

template<uint SizeT, bool ExpT>
inline bool
operator != (const char* lhs, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs != lhs;
}

template<int N, uint SizeT, bool ExpT>
inline bool
operator == (const char lhs[N], const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs == lhs;
}

template<int N, uint SizeT, bool ExpT>
inline bool
operator != (const char lhs[N], const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs != lhs;
}

template<class TraitsT, class AllocT, uint SizeT, bool ExpT>
inline bool
operator == (const std::basic_string<char,TraitsT,AllocT>& lhs, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs == lhs;
}

template<class TraitsT, class AllocT, uint SizeT, bool ExpT>
inline bool
operator != (const std::basic_string<char,TraitsT,AllocT>& lhs, const Yuni::CString<SizeT,ExpT>& rhs)
{
	return rhs != lhs;
}


inline bool
operator == (const AnyString& mbs, Yuni::UTF8::Char c)
{
	return c.size() == mbs.size() && c == mbs.utf8begin().value();
}

inline bool
operator != (const AnyString& mbs, Yuni::UTF8::Char c)
{
	return c.size() != mbs.size() || c == mbs.utf8begin().value();
}

//@}







# ifdef YUNI_HAS_STL_HASH

namespace std
{
	template<uint SizeT, bool ExpT>
	struct YUNI_DECL hash<Yuni::CString<SizeT, ExpT> >
	{
		typedef Yuni::CString<SizeT, ExpT> argument_type;
		typedef std::size_t result_type;

		result_type operator() (const argument_type& string) const
		{
			return string.hash();
		}
	};

} // namespace std

# endif // YUNI_HAS_STL_HASH
