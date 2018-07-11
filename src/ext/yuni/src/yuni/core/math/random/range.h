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
#include "../math.h"
#include "distribution.h"



namespace Yuni
{
namespace Math
{
namespace Random
{

	/*!
	** \brief Wrapper for any random number generator to generate any numbers within a desired range
	**
	** \code
	** #include <iostream>
	** #include <yuni/core/math/random/default.h>
	** #include <yuni/core/math/random/range.h>
	**
	** using namespace Yuni;
	**
	** int main()
	** {
	**		// A table for pre-cached random numbers
	**		Math::Random::Range<Math::Random::Default, 0, 1, float>  randomRange;
	**		// Initializing the distribution
	**		randomRange.reset();
	**
	**		std::cout << "A random number : " << randomRange() << std::endl;
	**		std::cout << "Another one     : " << randomRange() << std::endl;
	**		return 0;
	** }
	** \endcode
	**
	** \tparam D The distribution
	** \tparam LoValue The lower bound of the range, including itself
	** \tparam HiValue The upper bound of the range, but not including itself
	** \tparam T The type of random numbers
	*/
	template<class D, int LoValue = 0, int HiValue = 1, typename T = float>
	class YUNI_DECL Range /*final*/
		: public ADistribution<T, Range<D, LoValue, HiValue, T> >
	{
	public:
		//! Type for a single random number
		typedef T Value;

	public:
		// Name of the distribution
		static const char* Name() {return D::Name();}

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Range();
		//! Destructor
		~Range();
		//@}

		/*!
		** \brief Reset the random number generator
		*/
		void reset();

		/*!
		** \brief Get the next random number
		*/
		const Value next();

		static const T min();
		static const T max();

	private:
		//! The distribution
		D pDistribution;

	}; // class Range





} // namespace Random
} // namespace Math
} // namespace Yuni

#include "range.hxx"
