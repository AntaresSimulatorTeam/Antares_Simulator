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
#include "string.h"
#include "../noncopyable.h"



namespace Yuni
{

	/*!
	** \brief Lightweight helper for manipulating wide strings
	**
	** This helper is especially usefull for Windows API
	** \warning Due to Windows API limitations, the size of a WString is limited to 2147483648 chars
	*/
	class YUNI_DECL WString final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		WString();

		//! Copy constructor
		WString(const WString&);

		#ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		WString(WString&& rhs);
		#endif

		/*!
		** \brief Constructor
		**
		** \param uncprefix True to prepend the Windows UNC `\\?\` before converting
		*/
		explicit WString(const AnyString& string, bool uncprefix = false);

		//! Destructor
		~WString();
		//@}

		/*!
		** \brief Reset with a new wide string
		*/
		void assign(const AnyString& string, bool uncprefix = false);

		/*!
		** \brief Clear the buffer
		*/
		void clear();

		/*!
		** \brief Size of the wide string
		*/
		uint size() const;

		/*!
		** \brief Get if the string is empty
		*/
		bool empty() const;

		/*!
		** \brief Get the wide string
		*/
		const wchar_t* c_str() const;

		/*!
		** \brief Get the wide string
		*/
		wchar_t* data();

		/*!
		** \brief Replace all occurences of a single char
		*/
		void replace(wchar_t from, wchar_t to);

		//! \name Operators
		//@{
		//! Copy
		WString& operator = (const WString& string);
		//! Assignment
		WString& operator = (const AnyString& string);

		#ifdef YUNI_HAS_CPP_MOVE
		//! move operator
		WString& operator = (WString&& rhs);
		#endif
		//@}


	private:
		//! Convert a C-String into a Wide String
		void prepareWString(const AnyString& string, bool uncprefix);

	private:
		//! Wide string
		wchar_t* pWString;
		//! Size of the wide string
		size_t pSize;

	}; // class WString





} // namespace Yuni

#include "wstring.hxx"
