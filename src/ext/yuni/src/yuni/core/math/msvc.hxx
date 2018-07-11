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


#if defined(YUNI_OS_MSVC) && _MSC_VER < 1800


/* Those functions are not available on Windows... */

inline double rint(double nr)
{
	const double f = Yuni::Math::Floor(nr);
	const double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? f : c);
}


# ifdef YUNI_HAS_LONG_DOUBLE
inline long double rintl(long double nr)
{
	const long double f = Yuni::Math::Floor(nr);
	const long double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? f : c);
}
# endif


inline float rintf(float nr)
{
	const float f = Yuni::Math::Floor(nr);
	const float c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? f : c);
}


inline long int lrint(double nr)
{
	const double f = Yuni::Math::Floor(nr);
	const double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long int)f : (long int)c);
}


# ifdef YUNI_HAS_LONG_DOUBLE
inline long int lrintl(long double nr)
{
	const long double f = Yuni::Math::Floor(nr);
	const long double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long int)f : (long int)c);
}
# endif

inline long int lrintf(float nr)
{
	const float f = Yuni::Math::Floor(nr);
	const float c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long int)f : (long int)c);
}


inline long long int llrint(double nr)
{
	const double f = Yuni::Math::Floor(nr);
	const double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long long int)f : (long long int)c);
}


# ifdef YUNI_HAS_LONG_DOUBLE
inline long long int llrintl(long double nr)
{
	const long double f = Yuni::Math::Floor(nr);
	const long double c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long long int)f : (long long int)c);
}
# endif


inline long long int llrintf(float nr)
{
	const float f = Yuni::Math::Floor(nr);
	const float c = Yuni::Math::Ceil(nr);
	return (((c -nr) >= (nr - f)) ? (long long int)f : (long long int)c);
}

#endif /* ifdef MSVC */
