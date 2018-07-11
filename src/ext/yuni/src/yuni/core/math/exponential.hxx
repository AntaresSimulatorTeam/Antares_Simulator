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

	template<class T> inline T Exp(T x)
	{
		return (T)::exp((T)x);
	}

	template<> inline float Exp(float x)
	{
		return ::expf(x);
	}

	template<> inline double Exp(double x)
	{
		return ::exp(x);
	}

	template<> inline long double Exp<long double>(long double x)
	{
		return ::expl(x);
	}





} // namespace Math
} // namespace Yuni
