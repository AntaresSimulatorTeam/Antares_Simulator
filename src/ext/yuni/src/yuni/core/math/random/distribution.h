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
namespace Math
{
namespace Random
{


	/*!
	** \brief Random Number Distribution (Abstract)
	**
	** This class is an abstract class for any random number generators.
	**
	** Do not forget that you have to reset the random number generator yourself,
	** As this operation may be costly, You may want to control it. The other
	** reason is that some specific parameters may be required.
	**
	** \tparam T The type of the random numbers
	** \param Derived The real class (CRTP)
	*/
	template<typename T, class Derived>
	class YUNI_DECL ADistribution
	{
	public:
		//! Type of a single random number
		typedef T Value;

	public:
		/*!
		** \brief Get the name of the distribution
		*/
		static const char* Name();

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ADistribution();
		//! Destructor
		~ADistribution();
		//@}

		//! \name Random number Generator
		//@{
		/*!
		** \brief (Re)Iniitalize the random number generator
		*/
		void reset();

		/*!
		** \brief Generate the next random number
		*/
		const Value next();
		//@}


		//! \name Informations about the random number generator
		//@{
		//! Get the name of the distribution
		const char* name() const;

		//! The lower bound (including itself)
		const Value min() const;
		//! The Upper bound (but not including itself)
		const Value max() const;
		//@}

		//! \name Operators
		//@{
		//! \see next()
		const Value operator () ();
		//! \see next()
		template<class U> Derived& operator >> (U& u);
		//@}

	}; // class ADistribution





} // namespace Random
} // namespace Math
} // namespace Yuni

#include "distribution.hxx"
