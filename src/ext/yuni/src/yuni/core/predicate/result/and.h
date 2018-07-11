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


namespace Yuni
{
namespace Result
{


	template<typename T>
	struct And final
	{
	public:
		//! Type of the result
		typedef bool ResultType;

	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default Constructor
		*/
		And()
			:pValue(true)
		{}
		And(bool defaultValue)
			:pValue(defaultValue)
		{}
		/*!
		** \brief Copy constructor
		*/
		And(const And& rhs) :pValue(rhs.pValue) {}
		//@}

		//! Take into account a new value
		void operator () (const T& v) { pValue = (pValue && v);}

		/*!
		** \brief The Final result
		*/
		ResultType result() const {return pValue;}

		/*!
		** \brief Reset the internal counter
		*/
		void reset() {pValue = true;}
		void reset(bool v) {pValue = v;}

	private:
		//! The internal counter
		bool pValue;

	}; // class And




} // namespace Result
} // namespace Yuni
