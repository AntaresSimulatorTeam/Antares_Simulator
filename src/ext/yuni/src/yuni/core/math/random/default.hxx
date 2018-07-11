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
#include "default.h"



namespace Yuni
{
namespace Math
{
namespace Random
{


	inline void Default::reset()
	{
		// Initializing the random generator with a standard seed
		::srand((uint) ::time(NULL));
		// Compute a random number for nothing, to avoid the first generated number
		::rand();
	}


	inline void Default::reset(uint seed)
	{
		// Initializing the random generator with the given seed
		::srand(seed);
		// Compute a random number for nothing, to avoid the first generated number
		::rand();
	}


	inline Default::Value Default::next()
	{
		return rand();
	}


	inline Default::Value Default::min()
	{
		return 0;
	}


	inline Default::Value Default::max()
	{
		return RAND_MAX;
	}


	template<class U>
	inline Default& Default::operator >> (U& u)
	{
		u = next();
		return *this;
	}





} // namespace Random
} // namespace Math
} // namespace Yuni
