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
#include "../yuni.h"
#include "string.h"



namespace Yuni
{

	/*!
	** \brief 3-state boolean
	*/
	class Tribool final
	{
	public:
		//! \name Constructors
		//@{
		//! Default constructor (indeterminate by default)
		Tribool();
		//! default constructor, with a default when indeterminate
		Tribool(bool value, bool defvalue = false);
		//! default constructor to indeterminate
		Tribool(const NullPtr&, bool defvalue = false);
		//! copy constructor
		Tribool(const Tribool&);
		//@}


		/*!
		** \brief Set to indeterminate
		*/
		void clear();

		/*!
		** \brief Get if the value is indeterminate
		*/
		bool indeterminate() const;

		//! Get the default value, for bool conversion when indeterminate
		bool defaultValue() const;
		//! Set the default value, for bool conversion when indeterminate
		void defaultValue(bool defvalue);

		/*!
		** \brief Convert to a bool value, using the default value when indeterminate
		*/
		bool toBool() const;

		/*!
		** \brief Print
		*/
		template<class StreamT> void print(StreamT&) const;


		//! \name Operators
		//@{
		//! Assign from a bool value
		Tribool& operator = (bool value);
		//! Reset to 'indeterminate'
		Tribool& operator = (const NullPtr&);
		//! Copy from another tribool
		Tribool& operator = (const Tribool&);

		//! Get if equal to a bool value (see toBool())
		bool operator == (bool value) const;
		//! Get if indeterminate
		bool operator == (const NullPtr&) const;
		//! Get if strictly equal to another tribool
		bool operator == (const Tribool&) const;

		//! Get if equal to a bool value (see toBool())
		bool operator != (bool value) const;
		//! Get if indeterminate
		bool operator != (const NullPtr&) const;
		//! Get if strictly equal to another tribool
		bool operator != (const Tribool&) const;

		//! Get the bool representation of this tribool, see toBool()
		operator bool () const;
		//@}


	private:
		//! internal value (0: false, 1: true, -1: indeterminate)
		// The second value is the default one
		union TriboolValue
		{
			yint8 flags[2];
			yuint16 u16;
		} pValue;

	}; // class Tribool





} // namespace Yuni

#include "tribool.hxx"
