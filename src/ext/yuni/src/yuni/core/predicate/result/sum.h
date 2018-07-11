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
	struct Sum final
	{
	public:
		//! Type of the result
		typedef T ResultType;

	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default Constructor
		*/
		Sum() :pValue() {}
		/*!
		** \brief Copy constructor
		*/
		Sum(const Sum& rhs) :pValue(rhs.pValue) {}
		//@}

		//! Take into account a new value
		void operator () (const T& v) { pValue += v;}

		/*!
		** \brief The Final result
		*/
		ResultType result() const {return pValue;}

		/*!
		** \brief Reset the internal counter
		*/
		void reset() {pValue = T();}

	private:
		//! The internal counter
		T pValue;

	}; // class Sum




} // namespace Result
} // namespace Yuni
