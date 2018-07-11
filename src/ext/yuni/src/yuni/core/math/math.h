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
#include "../../yuni.h"


/*!
** \note long doube support : YUNI_HAS_LONG_DOUBLE must be defined
*/

/*!
** \brief An arbitrary (or nearly so) small positive quantity
**
** \ingroup Math
*/
#define YUNI_EPSILON        1.e-6

/*!
** \brief Pi, a mathematical constant (π)
**
** Pi is a mathematical constant whose value is the ratio of any circle's
** circumference to its diameter in Euclidean space.
** \see http://en.wikipedia.org/wiki/Pi
** \ingroup Math
*/
#define YUNI_PI             3.14159265358979323846

/*!
** \brief The reciprocal value of PI
**
** \ingroup Math
*/
#define YUNI_RECIPROCAL_PI  0.318309886183791 // = 1. / YUNI_PI

/*!
** \brief The half value of PI
**
** \ingroup Math
*/
#define YUNI_HALF_PI        1.570796326794897 // = YUNI_PI / 2.

/*!
** \brief E, A Mathematical constant
**
** The mathematical constant e is the unique real number such that the area
** above the x-axis and below the curve y=1/x for 1 ≤ x ≤ e is exactly 1.
** \see http://en.wikipedia.org/wiki/E_(mathematical_constant)
** \ingroup Math
*/
#define YUNI_NAT_E          2.71828182845904523536






namespace Yuni
{

/*!
** \brief Common mathematical operations, transformations and constants
** \ingroup Math
*/
namespace Math
{


	/*!
	** \brief Get if two values are equal
	*/
	template<class U> YUNI_DECL bool Equals(U a, U b);

	/*!
	** \brief Get if a value is equals to zero
	*/
	template<class U> YUNI_DECL bool Zero(U a);


	/*!
	** \brief Get the integer absolute value
	*/
	template<class T> YUNI_DECL T Abs(const T a);

	/*!
	** \brief Get the greater expression
	**
	** \param a The first expression
	** \param b The second expression
	** \return The expression considered as the grater
	*/
	template<class U, class V> YUNI_DECL U Max(U a, V b);

	/*!
	** \brief Maximum of three values
	**
	** \param a The first expression
	** \param b The second expression
	** \param c The third expression
	** \return The expression considered as the grater
	*/
	template<class U, class V, class W> YUNI_DECL U Max(U a, V b, W c);

	/*!
	** \brief Get the smallest expression
	**
	** \param a The first expression
	** \param b The second expression
	** \return The expression considered as the smaller
	*/
	template<class U, class V> YUNI_DECL U Min(U a, V b);

	/*!
	** \brief Minimum of three values
	**
	** \param a The first expression
	** \param b The second expression
	** \param c The third expression
	** \return The expression considered as the smaller
	*/
	template<class U, class V, class W> YUNI_DECL U Min(U a, V b, W c);


	/*!
	** \brief Ensure that an expression is contained in a range of values
	**
	** \param expr An expression
	** \param min The lower bound limit allowed for the expression `v`
	** \param max The upper bound limit allowed for the expression `v`
	** \return The expression itself, or a bound limit
	*/
	template<class T> YUNI_DECL const T& MinMax(const T& expr, const T& min, const T& max);


	/*!
	** \brief Ensure that an expression is contained in a range of values (with custom types)
	**
	** Most of the time, `MinMax` will produce the wanted result. But in some
	** cases, it may be inefficient, due to type matching and would produce strange
	** results (such as integer overflow).
	** This method is thus a variant from `MinMax` to allow the user to use custom
	** input types.
	** \code
	** // This code will give '56', instead of 255
	** std::cout << Yuni::Math::MinMax<uint8>(312, 0, 255) << std::endl;
	** // This code will give the wanted result '255' :
	** std::cout << Yuni::Math::MinMaxEx<uint8>(312, 0, 255) << std::endl;
	** \endcode
	**
	** The inconvenient of this method is that the user must always give the
	** return type.
	**
	** \param expr An expression
	** \param min The lower bound limit allowed for the expression `v`
	** \param max The upper bound limit allowed for the expression `v`
	** \return The expression itself, or a bound limit
	*/
	template<class T, class Expr, class MinT, class MaxT>
	YUNI_DECL T MinMaxEx(const Expr& expr, const MinT& min, const MaxT& max);

	/*!
	** \brief Swap two values
	**
	** \param a The first value
	** \param b the second value
	*/
	template<class T> YUNI_DECL void Swap(T& a, T&b);



	/*!
	** \brief The factorial function
	**
	** \code
	**    std::cout << Factorial<7>::value << std::endl;
	** \endcode
	*/
	template <int N>
	struct YUNI_DECL Factorial final
	{
		//! The Formula for the factorial function
		enum { value = N * Factorial<N-1>::value };
		// Fatorial<1> is located in math.hxx
	};



	/*!
	** \brief Get the value of x raised to the power y (x**y)
	**
	** \param x Any value
	** \param y The power y
	** \return x**y
	*/
	YUNI_DECL float Power(float x, float y);
	YUNI_DECL double Power(double x, double y);

	/*!
	** \brief The power function for Integer values
	**
	** \f$value = x^y\f$
	**
	** \tparam X The value
	** \tparam Y The power
	**
	** \code
	**    std::cout << Yuni::Math::Power<8, 3>::value << std::endl; // 512
	** \endcode
	*/
	template<int X, int Y>
	struct PowerInt final
	{
		//! The formula for the power function
		enum { value = X * PowerInt<X, Y-1>::value };
		// PowerInt<X,0> is located in math.hxx
	};




	/*!
	** \brief Get the square root value
	**
	** This routine is safe for zero or negative values
	*/
	template<class U> YUNI_DECL U SquareRoot(U x);

	/*!
	** \brief Get the square root value
	**
	** The standard square root function, without any check on the input
	*/
	template<class U> YUNI_DECL U SquareRootNoCheck(U x);


	/*!
	** \brief The Square root function for integer values
	**
	** \f$value = \sqrt{N}\f$
	**
	** \code
	**    std::cout << SquareRoot<16>::value << std::endl; // 4
	** \endcode
	**
	** \internal Via iteration
	*/
	template <int N, int I = 1>
	struct SquareRootInt final
	{
		//! The formula for the squere root function
		enum { value = (I*I < N) ? SquareRootInt<N, I+1>::value : I };
		// SquareRootInt<N,N> is located in math.hxx
	};


	/*!
	** \brief Get if a value is a power of 2
	**
	** \code
	** std::cout << Math::PowerOfTwo(64) << std::endl; // 1
	** \endcode
	*/
	YUNI_DECL bool PowerOfTwo(int x);



	/*!
	** \brief Convert degree to radion
	**
	** \note This method is provided for convenience only. You should
	**       see Yuni::Unit for more functionalities.
	*/
	template<class T> YUNI_DECL T DegreeToRadian(T x);


	/*!
	** \brief Convert radion to degree
	**
	** \note This method is provided for convenience only. You should
	**       see Yuni::Unit for more functionalities.
	*/
	template<class T> YUNI_DECL T RadianToDegree(T x);



	/*!
	** \brief Determine whether the argument value is a NaN
	*/
	template<class T> YUNI_DECL bool NaN(const T& x);

	/*!
	** \brief Determine whether the argument value is an infinite value
	**
	** \return  1 if x is positive infinity, and -1 if x is negative infinity, 0 otherwise
	*/
	template<class T> YUNI_DECL int Infinite(const volatile T& x);


	/*!
	** \brief Floor function
	*/
	template<class T> YUNI_DECL T Floor(T x);

	/*!
	** \brief Ceil function
	*/
	template<class T> YUNI_DECL T Ceil(T x);


	/*!
	** \brief The fractional part function
	*/
	template<class T> YUNI_DECL T Fract(T x);


	/*!
	** \brief Integral value nearest to x
	**
	** The function returns the integral value nearest to x rounding
	** half-way cases away from zero, regardless of the current rounding
	** direction.
	**
	** \param x A number
	** \param place The place of the decimal number to round
	*/
	template<class T> YUNI_DECL T Round(T x, uint place = 0);


	/*!
	** \brief truncate to integer value
	**
	** The function return the integral value nearest to but no larger in magnitude than x.
	**
	** \param x A number
	** \param place The place of the decimal number to round
	*/
	template<class T> YUNI_DECL T Trunc(T x, uint place = 0);


	/*!
	** \brief Trunc decimal places
	*/
	YUNI_DECL double Trunc(double x, uint places);

	/*!
	** \brief Integral value nearest to x
	**
	** Here is the best way to use this class :
	** \code
	** int i = RoundToInt<float,int>::Value(10.2f);
	** \endcode
	*/
	template<class T, class R> struct YUNI_DECL RoundToInt;


	/*!
	** \brief Rounding up to the nearest multiple of a number
	*/
	template<class T> T YUNI_DECL RoundUp(T value, T multiple);


	/*!
	** \brief Determine whether a value is in fact an integer or not
	**
	** \param x A number
	**
	** \code
	** IsInteger(2.000000); // true
	** IsInteger(2.000001); // false
	** \endcode
	*/
	template<class T> YUNI_DECL bool IsInteger(T x);


	/*!
	** ** \brief Compute the greatest common divisor of two integers (GCD)
	** **
	** ** \param u An integer
	** ** \param v An integer
	** ** \return gcd(u, v)
	** */
	template<class U, class V> YUNI_DECL U GCD(U u, V v);





} // namespace Math
} // namespace Yuni

#include "math.hxx"
