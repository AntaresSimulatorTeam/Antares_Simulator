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



namespace Yuni
{
namespace UTF8
{

	enum Error
	{
		//! No error
		errNone = 0,
		//! Out of bound (offset >= size)
		errOutOfBound,
		//! Invalid lead character
		errInvalidLead,
		//! Not enough data
		errNotEnoughData,
		//! The sequence is incomplete
		errIncompleteSequence,
		errInvalidCodePoint,
	};


	class YUNI_DECL Char final
	{
	public:
		/*!
		** \brief Get the length in bytes of the UTF8 character
		**
		** This information is provided by the lead character (= the first char given by \p p)
		** \param p Address of a potential utf8 char
		** \return The size in bytes of the UTF8 char at the address \p p  (1,2,3, or 4, 0 if invalid).
		*/
		static uint Size(const void* p);

		/*!
		** \brief Extract the first uchar from a raw buffer
		** \param p A valid raw buffer
		*/
		static uchar Mask8Bits(const void* p);
		/*!
		** \brief Extract the first uchar from a raw buffer
		** \param p Any char
		*/
		static uchar Mask8Bits(const char p);

		/*!
		** \brief Is the UTF-8 a simple ascii char ?
		** \param c An unsigned char
		*/
		static bool IsASCII(uchar c);

		/*!
		** \brief Check if the two first bits are set
		** \param c An unsigned char
		*/
		static bool IsTrail(uchar c);

		enum
		{
			//! The maximum valid code point
			codePointMax = static_cast<uint32>(0x0010ffffu),
			//! Ascii maximum value
			asciiLimit = 0x80,
		};

	public:
		//! \name Constructors
		//@{
		//! Default Constructor
		Char();
		//! Copy constructor
		Char(const Char& rhs);
		//! Constructor from a mere char
		explicit Char(uint c);
		//@}


		/*!
		** \brief The size of the UTF8 character, in bytes
		*/
		uint size() const;

		uint32 value() const;

		template<class StreamT> void write(StreamT& out) const;

		void reset();

		//! Get if the character is a simple ascii
		bool isAscii() const;


		//! \name Operators
		//@{
		Char& operator = (const Char& rhs);
		Char& operator = (uint value);

		bool operator == (uint value) const;
		bool operator == (const Char& ch) const;
		bool operator == (const AnyString& str) const;

		bool operator != (uint value) const;
		bool operator != (const Char& ch) const;
		bool operator != (const AnyString& str) const;

		bool operator < (uint value) const;
		bool operator < (const Char& ch) const;

		bool operator <= (uint value) const;
		bool operator <= (const Char& ch) const;

		bool operator > (uint value) const;
		bool operator > (const Char& ch) const;

		bool operator >= (uint value) const;
		bool operator >= (const Char& ch) const;

		//! Cast into char
		operator char () const;

		//! Cast into unsigned char
		operator uchar () const;
		//@}


	private:
		//! The UTF-8 character
		uint32 pValue;
		// A friend !
		template<uint, bool> friend class Yuni::CString;
	};





} // namespace UTF8
} // namespace Yuni


bool operator == (const AnyString& multiByteStr, Yuni::UTF8::Char c);

bool operator != (const AnyString& multiByteStr, Yuni::UTF8::Char c);


#include "utf8char.hxx"
