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
#include "utf8char.h"



namespace Yuni
{
namespace UTF8
{

	inline Char::Char()
		: pValue()
	{}


	inline Char::Char(const Char& rhs)
		: pValue(rhs.pValue)
	{}


	inline Char::Char(uint c)
		: pValue(c)
	{}


	inline uint32 Char::value() const
	{
		return pValue;
	}


	inline void Char::reset()
	{
		pValue  = 0;
	}


	inline Char& Char::operator = (const Char& rhs)
	{
		pValue = rhs.pValue;
		return *this;
	}


	inline Char& Char::operator = (uint value)
	{
		pValue = static_cast<uint32>(value);
		return *this;
	}


	inline bool Char::operator == (uint value) const
	{
		return pValue == value;
	}


	inline bool Char::operator == (const Char& ch) const
	{
		return pValue == ch.pValue;
	}


	inline bool Char::operator == (const AnyString& str) const
	{
		return str == *this;
	}


	inline bool Char::operator != (uint value) const
	{
		return pValue != value;
	}


	inline bool Char::operator != (const Char& ch) const
	{
		return pValue != ch.pValue;
	}


	inline bool Char::operator != (const AnyString& str) const
	{
		return str != *this;
	}


	inline Char::operator char () const
	{
		return (pValue < 0x80) ? static_cast<char>(pValue) : '\0';
	}


	inline Char::operator unsigned char () const
	{
		return (pValue < 0x80) ? static_cast<uchar>(pValue) : static_cast<uchar>('\0');
	}


	inline uchar Char::Mask8Bits(const void* p)
	{
		return static_cast<uchar>(0xFF & *(static_cast<const char*>(p)));
	}

	inline uchar Char::Mask8Bits(const char p)
	{
		return static_cast<uchar>(0xFF & p);
	}


	inline uint Char::Size(const void* p)
	{
		// Char. number range  |        UTF-8 octet sequence
		//    (hexadecimal)    |              (binary)
		// --------------------+------------------------------------
		// 0000 0000-0000 007F | 0xxxxxxx
		// 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
		// 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
		// 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

		const uchar lead = Mask8Bits(p);
		if (lead < 0x80) // the leas byte is zero, ascii
			return 1;
		if ((lead >> 5) == 0x6)  // 110x xxxx
			return 2;
		if ((lead >> 4) == 0xe)  // 1110 xxxx
			return 3;
		if ((lead >> 3) == 0x1e) // 1111 0xxx
			return 4;
		return 0;
	}


	inline uint Char::size() const
	{
		if (pValue < 0x80)
			return 1;
		if (pValue < 0x800)
			return 2;
		if (pValue < 0x10000)
			return 3;
		return 4;
	}


	inline bool Char::IsASCII(uchar c)
	{
		return c < 0x80;
	}


	inline bool Char::IsTrail(uchar c)
	{
		return ((c >> 6) == 0x2);
	}


	template<class StreamT>
	void Char::write(StreamT& out) const
	{
		if (pValue < 0x80)
		{
			out.put(static_cast<char>(static_cast<uchar>(pValue)));
		}
		else
		{
			if (pValue < 0x800)
			{
				out.put(static_cast<char>(static_cast<uchar>((pValue >> 6)   | 0xc0)));
				out.put(static_cast<char>(static_cast<uchar>((pValue & 0x3f) | 0x80)));
			}
			else
			{
				if (pValue < 0x10000)
				{
					out.put(static_cast<char>(static_cast<uchar>((pValue >> 12)         | 0xe0)));
					out.put(static_cast<char>(static_cast<uchar>(((pValue >> 6) & 0x3f) | 0x80)));
					out.put(static_cast<char>(static_cast<uchar>((pValue & 0x3f)        | 0x80)));
				}
				else
				{
					// four bytes
					out.put(static_cast<char>(static_cast<uchar>((pValue >> 18)         | 0xf0)));
					out.put(static_cast<char>(static_cast<uchar>(((pValue >> 12)& 0x3f) | 0x80)));
					out.put(static_cast<char>(static_cast<uchar>(((pValue >> 6) & 0x3f) | 0x80)));
					out.put(static_cast<char>(static_cast<uchar>((pValue & 0x3f)        | 0x80)));
				}
			}
		}
	}


	inline bool Char::isAscii() const
	{
		return pValue < static_cast<uint>(asciiLimit);
	}


	inline bool Char::operator < (uint value) const
	{
		return pValue < value;
	}


	inline bool Char::operator <= (uint value) const
	{
		return pValue <= value;
	}


	inline bool Char::operator > (uint value) const
	{
		return pValue > value;
	}


	inline bool Char::operator >= (uint value) const
	{
		return pValue >= value;
	}




	inline bool Char::operator < (const Char& ch) const
	{
		return pValue < ch.pValue;
	}


	inline bool Char::operator <= (const Char& ch) const
	{
		return pValue <= ch.pValue;
	}


	inline bool Char::operator > (const Char& ch) const
	{
		return pValue > ch.pValue;
	}


	inline bool Char::operator >= (const Char& ch) const
	{
		return pValue >= ch.pValue;
	}




} // namespace UTF8
} // namespace Yuni



#ifdef YUNI_HAS_STL_HASH

namespace std
{
	template<>
	struct YUNI_DECL hash<Yuni::UTF8::Char> final
	{
		inline size_t operator() (const Yuni::UTF8::Char& ch) const
		{
			return ch.value();
		}
	};

} // namespace std

#endif // YUNI_HAS_STL_HASH
