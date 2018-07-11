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
#include "distribution.h"



namespace Yuni
{
namespace Math
{
namespace Random
{


	template<typename T, class Derived>
	inline const char* ADistribution<T,Derived>::Name()
	{
		return Derived::Name();
	}


	template<typename T, class Derived>
	ADistribution<T,Derived>::ADistribution()
	{}


	template<typename T, class Derived>
	ADistribution<T,Derived>::~ADistribution()
	{}


	template<typename T, class Derived>
	inline void ADistribution<T,Derived>::reset()
	{
		static_cast<Derived*>(this)->reset();
	}


	template<typename T, class Derived>
	inline const char* ADistribution<T,Derived>::name() const
	{
		return Derived::Name();
	}


	template<typename T, class Derived>
	inline const typename ADistribution<T,Derived>::Value
	ADistribution<T,Derived>::next()
	{
		return static_cast<Derived*>(this)->next();
	}


	template<typename T, class Derived>
	inline const typename ADistribution<T,Derived>::Value
	ADistribution<T,Derived>::operator () ()
	{
		return static_cast<Derived*>(this)->next();
	}


	template<typename T, class Derived>
	template<class U>
	Derived&  ADistribution<T,Derived>::operator >> (U& u)
	{
		u = static_cast<Derived*>(this)->next();
		return static_cast<Derived*>(this);
	}


	template<typename T, class Derived>
	inline const typename ADistribution<T,Derived>::Value
	ADistribution<T,Derived>::min() const
	{
		return static_cast<Derived*>(this)->min();
	}


	template<typename T, class Derived>
	inline const typename ADistribution<T,Derived>::Value
	ADistribution<T,Derived>::max() const
	{
		return static_cast<Derived*>(this)->max();
	}




} // namespace Random
} // namespace Math
} // namespace Yuni
