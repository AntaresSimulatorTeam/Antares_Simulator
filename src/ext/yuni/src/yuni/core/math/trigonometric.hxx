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
#include "trigonometric.h"



namespace Yuni
{
namespace Math
{

	template<typename T> inline T Cos(T x)
	{
		return (T)::cos((T)x);
	}

	template<> inline float Cos<float>(float x)
	{
		return ::cosf(x);
	}

	template<> inline long double Cos<long double>(long double x)
	{
		return ::cosl(x);
	}


	template<typename T> inline T Sin(T x)
	{
		return (T)::sin((T)x);
	}

	template<> inline float Sin<float>(float x)
	{
		return ::sinf(x);
	}

	template<> inline long double Sin<long double>(long double x)
	{
		return ::sinl(x);
	}


	template<typename T> inline T Tan(T x)
	{
		return (T)::tan((T)x);
	}

	template<> inline float Tan<float>(float x)
	{
		return ::tanf(x);
	}

	template<> inline long double Tan<long double>(long double x)
	{
		return ::tanl(x);
	}



	template<typename T> inline T ACos(T x)
	{
		return (T)::cos((T)x);
	}

	template<> inline float ACos<float>(float x)
	{
		return ::acosf(x);
	}

	template<> inline long double ACos<long double>(long double x)
	{
		return ::acosl(x);
	}


	template<typename T> inline T ASin(T x)
	{
		return (T)::asin((T)x);
	}

	template<> inline float ASin<float>(float x)
	{
		return ::asinf(x);
	}

	template<> inline long double ASin<long double>(long double x)
	{
		return ::asinl(x);
	}


	template<typename T> inline T ATan(T x)
	{
		return (T)::atan((T)x);
	}

	template<> inline float ATan<float>(float x)
	{
		return ::atanf(x);
	}

	template<> inline long double ATan<long double>(long double x)
	{
		return ::atanl(x);
	}

	template<typename T> inline T ATan2(T y, T x)
	{
		return (T)::atan2(y, x);
	}

	template<> inline float ATan2<float>(float y, float x)
	{
		return ::atan2f(y, x);
	}

	template<> inline long double ATan2<long double>(long double y, long double x)
	{
		return ::atan2l(y, x);
	}




	template<typename T> inline T ASinH(T x)
	{
		# if defined(YUNI_OS_MSVC)
		return (T)::log((T)(x + ::sqrt(x * x + 1)));
		# else
		return (T)::asinh((T)x);
		# endif
	}

	template<> inline float ASinH<float>(float x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logf((x + ::sqrtf(x * x + 1)));
		# else
		return ::asinhf(x);
		# endif
	}

	template<> inline long double ASinH<long double>(long double x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logl((x + ::sqrtl(x * x + 1)));
		# else
		return ::asinhl(x);
		# endif
	}




	template<typename T> inline T ACosH(T x)
	{
		# if defined(YUNI_OS_MSVC)
		return (T)::log( (T)(x + ::sqrt(x * x - 1)));
		# else
		return (T)::acosh((T)x);
		# endif
	}

	template<> inline float ACosH<float>(float x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logf( (x + ::sqrtf(x * x - 1)));
		# else
		return ::acoshf(x);
		# endif
	}

	template<> inline long double ACosH<long double>(long double x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logl( (x + ::sqrtl(x * x - 1)));
		# else
		return ::acoshl(x);
		# endif
	}



	template<typename T> inline T ATanH(T x)
	{
		# if defined(YUNI_OS_MSVC)
		return (T)::log((T)((T)(1. / x + 1.) / (T)(1. / x - 1.))) / 2.;
		# else
		return (T)::atanh((T)x);
		# endif
	}


	template<> inline float ATanH<float>(float x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logf(((1.f / x + 1.f) / (1.f / x - 1.f))) / 2.f;
		# else
		return ::atanhf(x);
		# endif
	}

	template<> inline long double ATanH<long double>(long double x)
	{
		# if defined(YUNI_OS_MSVC)
		return ::logl(((1. / x + 1.) / (1. / x - 1.))) / 2.;
		# else
		return ::atanhl(x);
		# endif
	}





} // namespace Math
} // namespace Yuni
