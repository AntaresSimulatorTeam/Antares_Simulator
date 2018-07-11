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
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>



#if defined(YUNI_OS_MSVC) && _MSC_VER < 1800
/* Those functions are not available on Windows... */
double rint(double nr);
#	ifdef YUNI_HAS_LONG_DOUBLE
long double rintl(long double x);
#	endif
float rintf(float x);
long int lrint(double x);
#	ifdef YUNI_HAS_LONG_DOUBLE
long int lrintl(long double x);
#	endif
long int lrintf(float x);
long long int llrint(double x);
#	ifdef YUNI_HAS_LONG_DOUBLE
long long int llrintl(long double x);
#	endif
long long int llrintf(float x);
#endif



namespace Yuni
{
namespace Math
{


	template<class U, class V>
	inline U Max(U a, V b)
	{
		return (a < b) ? b : a;
	}


	template<class U, class V, class W>
	inline U Max(U a, V b, W c)
	{
		return Max<U,W>(Max<U,V>(a, b), c);
	}


	template<class U, class V>
	inline U Min(U a, V b)
	{
		return (a < b) ? a : b;
	}



	template<class U, class V, class W>
	inline U Min(U a, V b, W c)
	{
		return Min<U, W>(Min<U,V>(a, b), c);
	}


	template<class T>
	inline void Swap(T& a, T&b)
	{
		std::swap(a, b);
	}


	template<class T>
	inline const T& MinMax(const T& expr, const T& min, const T& max)
	{
		return ((expr < min)
			? min
			: ((expr > max) ? max : expr));
	}


	template<class T, class Expr, class MinT, class MaxT>
	inline T MinMaxEx(const Expr& expr, const MinT& min, const MaxT& max)
	{
		return ((expr < min)
			? static_cast<T>(min)
			: ((expr > max)
				? static_cast<T>(max)
				: static_cast<T>(expr)));
	}



	//! Factorial(1) = 1
	template <> struct Factorial<1> final { enum { value = 1 }; };


	//! PowerInt<X,0>
	template <int X> struct PowerInt<X,0> final { enum { value = 1 }; };

	// partial specialization to end the iteration
	template<int N> struct SquareRootInt<N,N> final { enum { value = N }; };


	template <class U> inline bool Equals(U a, U b)
	{
		return (a == b);
	}


	template<> inline bool Equals<float>(float a, float b)
	{
		return ::fabsf(a - b) < YUNI_EPSILON;
	}

	template<> inline bool Equals<double>(double a, double b)
	{
		return ::fabs(a - b) < YUNI_EPSILON;
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline bool Equals<long double>(long double a, long double b)
	{
		return ::fabsl(a - b) < YUNI_EPSILON;
	}
	# endif


	template <class U> inline bool Zero(U a)
	{
		return (0 == a);
	}

	template<> inline bool Zero<float>(float a)
	{
		return ::fabsf(a) < YUNI_EPSILON;
	}

	template<> inline bool Zero<double>(double a)
	{
		return ::fabs(a) < YUNI_EPSILON;
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline bool Zero<long double>(long double a)
	{
		return ::fabsl(a) < YUNI_EPSILON;
	}
	# endif


	template<class U>
	inline U SquareRoot(U x)
	{
		return (x < YUNI_EPSILON) ? U() : static_cast<U>(::sqrt(static_cast<double>(x)));
	}

	template<> inline double SquareRoot(double x)
	{
		return (x < YUNI_EPSILON) ? 0. : ::sqrt(x);
	}

	template<> inline float SquareRoot(float x)
	{
		return (x < YUNI_EPSILON) ? 0.f : ::sqrtf(x);
	}


	template<class U>
	inline U SquareRootNoCheck(U x)
	{
		return static_cast<U>(::sqrt(static_cast<double>(x)));
	}

	template<> inline double SquareRootNoCheck(double x)
	{
		return ::sqrt(x);
	}

	template<> inline float SquareRootNoCheck(float x)
	{
		return ::sqrtf(x);
	}





	inline bool PowerOfTwo(int x)
	{
		return not (x & (x - 1)) and x;
	}


	template<class T> inline T DegreeToRadian(T x)
	{
		return x * static_cast<T>(0.017453292);
	}


	template<class T> inline T RadianToDegree(T x)
	{
		return x * static_cast<T>(57.29578122);
	}


	template<class T> inline bool NaN(const T& x)
	{
		// According to the IEEE standard, NaN values have the odd property that
		// comparisons involving them are always false
		return x != x;
	}


	template<class T> inline int Infinite(const volatile T& x)
	{
		return ((x >= DBL_MAX) ? 1 : ((x <= -DBL_MAX) ? -1 : 0));
	}


	template<class T> inline T Floor(T x)
	{
		return x;
	}

	template<> inline float Floor(float x)
	{
		return ::floorf(x);
	}

	template<> inline double Floor(double x)
	{
		return ::floor(x);
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline long double Floor<long double>(long double x)
	{
		return ::floorl(x);
	}
	# endif



	template<class T> inline T Ceil(T x)
	{
		return x;
	}

	template<> inline float Ceil(const float x)
	{
		return ::ceilf(x);
	}

	template<> inline double Ceil(const double x)
	{
		return ::ceil(x);
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline long double Ceil<long double>(const long double x)
	{
		return ::ceill(x);
	}
	# endif


	template<class T> inline T Fract(T x)
	{
		return x - Floor(x);
	}


	inline float Power(float x, float y)
	{
		return ::powf(x, y);
	}

	inline double Power(double x, double y)
	{
		return ::pow(x, y);
	}



	template<class T> inline T Round(T x, uint)
	{
		return x;
	}


	template<> inline double Round<double>(double x, uint place)
	{
		if (place)
		{
			double temp, mult;
			mult = Power(10., place);
			temp = Floor(x * mult + 0.5);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifdef YUNI_OS_MSVC
			return (x < 0.) ? (::ceil(x - 0.5)) : (::floor(x + 0.5));
			# else
			return ::round(x);
			# endif
		}
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline long double Round<long double>(long double x, uint place)
	{
		if (place)
		{
			long double temp, mult;
			mult = Power(static_cast<long double>(10.), place);
			temp = Floor(x * mult + 0.5);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifdef YUNI_OS_MSVC
			return (x < 0.) ? (::ceill(x - 0.5L)) : (::floorl(x + 0.5L));
			# else
			return ::roundl(x);
			# endif
		}
	}
	# endif

	template<> inline float Round<float>(float x, uint place)
	{
		if (place != 0)
		{
			float temp, mult;
			mult = Power(10.f, static_cast<float>(place));
			temp = Floor(x * mult + 0.5f);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifdef YUNI_OS_MSVC
			return (x < 0.) ? (::ceilf(x - 0.5f)) : (::floorf(x + 0.5f));
			# else
			return ::roundf(x);
			# endif
		}
	}


	template<class T> inline T RoundUp(T value, T multiple)
	{
		if (multiple == T())
			return value;

		T remainder = Abs(value) % multiple;
		if (remainder == T())
			return value;
		if (value < 0)
			return -(Abs(value) - remainder);
		return value + multiple - remainder;
	}



	template<class T> inline T Trunc(T x, uint)
	{
		return x;
	}

	template<> inline double Trunc<double>(double x, uint place)
	{
		if (place)
		{
			double temp, mult;
			mult = Power(10.0, place);
			temp = Floor(x * mult);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifndef YUNI_OS_MSVC
			return ::trunc(x);
			# else
			return ((x > 0.) ? ::floor(x) : ::ceil(x));
			# endif
		}
	}

	template<> inline float Trunc<float>(float x, uint place)
	{
		if (place)
		{
			float temp, mult;
			mult = Power(10.f, static_cast<float>(place));
			temp = Floor(x * mult);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifndef YUNI_OS_MSVC
			return ::truncf(x);
			# else
			return ((x > 0.) ? ::floorf(x) : ::ceilf(x));
			# endif
		}
	}


	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline long double Trunc<long double>(long double x, uint place)
	{
		if (place)
		{
			long double temp, mult;
			mult = Power(10.0, place);
			temp = Floor(x * mult);
			temp = temp / mult;
			return temp;
		}
		else
		{
			# ifndef YUNI_OS_MSVC
			return ::trunc(x);
			# else
			return ((x > 0.) ? ::floor(x) : ::ceil(x));
			# endif
		}
	}
	# endif




	template<class T, class R>
	struct YUNI_DECL RoundToInt final
	{
		typedef T Type;
		typedef R ResultType;
		static inline ResultType Value(Type x)
		{
			// Default Behavior
			return static_cast<ResultType>(Round<Type>(x));
		}
	};

	template<class T>
	struct YUNI_DECL RoundToInt<T, T> final
	{
		typedef T Type;
		typedef T ResultType;
		static inline ResultType Value(Type x)
		{
			// Same type nothing to do
			return x;
		}
	};


	template<>
	struct YUNI_DECL RoundToInt<float, double> final
	{
		typedef float Type;
		typedef double ResultType;
		static inline ResultType Value(Type x) { return static_cast<ResultType>(x); }
	};

	template<>
	struct YUNI_DECL RoundToInt<double, float> final
	{
		typedef double Type;
		typedef float ResultType;
		static inline ResultType Value(Type x) { return static_cast<ResultType>(x); }
	};



	template<>
	struct YUNI_DECL RoundToInt<float, long int> final
	{
		typedef float Type;
		typedef long int ResultType;
		static inline ResultType Value(Type x) { return ::lrintf(x); }
	};

	template<>
	struct YUNI_DECL RoundToInt<double, long int> final
	{
		typedef double Type;
		typedef long int ResultType;
		static inline ResultType Value(Type x) { return ::lrint(x); }
	};


	# ifdef YUNI_HAS_LONG_DOUBLE
	template<>
	struct YUNI_DECL RoundToInt<long double, long int> final
	{
		typedef long double Type;
		typedef long int ResultType;
		static inline ResultType Value(Type x) { return ::lrintl(x); }
	};
	# endif



	template<>
	struct YUNI_DECL RoundToInt<float, long long int> final
	{
		typedef float Type;
		typedef long long int ResultType;
		static inline ResultType Value(Type x) { return ::llrintf(x); }
	};

	template<>
	struct YUNI_DECL RoundToInt<double, long long int> final
	{
		typedef double Type;
		typedef long long int ResultType;
		static inline ResultType Value(Type x) { return ::llrint(x); }
	};

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<>
	struct YUNI_DECL RoundToInt<long double, long long int> final
	{
		typedef long double Type;
		typedef long long int ResultType;
		static inline ResultType Value(Type x) { return ::llrintl(x); }
	};
	# endif

	template<class T> inline T Abs(const T x)
	{
		return static_cast<T>(::abs(static_cast<int>(x)));
	}

	template<> inline long Abs<long>(const long x)
	{
		return ::labs(x);
	}

	template<> inline long long Abs<long long>(const long long x)
	{
	# ifdef YUNI_OS_MSVC
		return ::_abs64(x);
	# else
		return ::llabs(x);
	# endif
	}

	template<> inline double Abs<double>(const double x)
	{
		return ::fabs(x);
	}

	template<> inline float Abs<float>(const float x)
	{
		return ::fabsf(x);
	}

	# ifdef YUNI_HAS_LONG_DOUBLE
	template<> inline long double Abs<long double>(const long double x)
	{
		return ::fabsl(x);
	}
	# endif


	template<class T> inline bool IsInteger(T x)
	{
		return !(Abs(Fract(x)) > 0.0);
	}


	template<class U, class V>
	inline U GCD(U u, V v)
	{
		// Euclidean algorithm
		if (!u)
			return v;

		while (v)
		{
			if (u > v)
				u = u - v;
			else
				v = v - u;
		}
		return u;
	}




} // namespace Math
} // namespace Yuni

#if defined(YUNI_OS_MSVC) && _MSC_VER < 1800
#	include "msvc.hxx"
#endif
