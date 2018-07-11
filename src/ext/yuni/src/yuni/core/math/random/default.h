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
#include <stdlib.h>
#include <time.h>
#include "distribution.h"



namespace Yuni
{
namespace Math
{
namespace Random
{

	/*!
	** \brief Default Pseudo random number generator
	**
	** This random number generator is the one provided by your Operating System.
	*/
	class YUNI_DECL Default /*final*/ : public ADistribution<int, Default>
	{
	public:
		// Name of the distribution
		static const char* Name() {return "Pseudo random numbers";}

		//! Type of a single random number
		typedef int Value;

	public:
		Default() {}
		~Default() {}

		void reset();
		void reset(uint seed);

		Value next();

		static Value min();
		static Value max();

		template<class U> Default& operator >> (U& u);
	};





} // namespace Random
} // namespace Math
} // namespace Yuni

#include "default.hxx"
