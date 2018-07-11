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
#include <cassert>
#include "../math.h"



namespace Yuni
{

	/*!
	** \brief Represents a 3D-point
	*/
	template<class T = float>
	class YUNI_DECL Point3D /* final */
	{
	public:
		/*!
		** \brief Calculate the mean between two points
		**
		** \param p1 First point
		** \param p2 Second point
		** \return A new instance of Point3D
		*/
		template<class U, class V>
		static Point3D<T>& Mean(const Point3D<U>& p1, const Point3D<V>& p2)
		{ return Point3D<T>().mean(p1, p2); }


	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Point3D() : x(0), y(0), z(0) {}

		/*!
		** \brief Constructor
		** \param x1 The default X coordinate
		** \param y1 The default Y coordinate
		*/
		template<class U, class V>
		Point3D(const U x1, const V y1): x((T)x1), y((T)y1), z(T()) {}

		/*!
		** \brief Constructor
		** \param x1 The default X coordinate
		** \param y1 The default Y coordinate
		** \param z1 The default Z coordinate
		*/
		template<class U, class V, class W>
		Point3D(const U x1, const V y1, const W z1): x((T)x1), y((T)y1), z((T)z1) {}

		//! Constructor by copy
		template<class U>
		Point3D(const Point3D<U>& p) : x((T)p.x), y((T)p.y), z((T)p.z) {}
		//@} // Constructors


		//! Reset the point to origin
		Point3D<T>& reset() {x = y = z = T(); return *this;}

		/*!
		** \brief Move the point to new coordinates
		**
		** \param x1 The new X coordinate
		** \param y1 The new Y coordinate
		** \param z1 The new Z coordinate
		*/
		template<class U, class V, class W>
		void move(const U x1, const V y1, const W z1) { x = (T)x1; y = (T)y1; z = (T)z1; }
		/*!
		** \brief Move the point to new coordinates
		** \param p The new coordinates
		*/
		template<class U>
		void move(const Point3D<U>& p) { x = (T)p.x; y = (T)p.y; z = (T)p.z; }


		/*!
		** \brief Translate the point with the same value for all coordinates
		** \param k The value to add to all coordinates
		*/
		template<class U>
		void translate(const U k) { x += (T)k; y += (T)k; z += (T)k; }
		/*!
		** \brief Translate the point with relative coordinates
		** \param x1 The value to add to the X coordinate
		** \param y1 The value to add to the Y coordinate
		** \param z1 The value to add to the Z coordinate
		*/
		template<class U, class V, class W>
		void translate(const U x1, const V y1, const W z1) { x += (T)x1; y += (T)y1; z += (T)z1; }
		/*!
		** \brief Translate the point with relative coordinates from another Point
		** \param p The values to add to the coordinates
		*/
		template<class U>
		void translate(const Point3D<U>& p) { x += (T)p.x; y += (T)p.y; z += (T)p.z; }


		/*!
		** \brief Calculate the mean between two points
		** The calling object is modified to store the value
		** \param p Point to compute the mean with
		*/
		template<class U>
		void mean(const Point3D<U>& p)
		{
			x = (T) ((x + p.x) / 2.0f);
			y = (T) ((y + p.y) / 2.0f);
			z = (T) ((z + p.z) / 2.0f);
		}

		/*!
		** \brief Calculate the mean between two points
		**
		** The calling object is modified to store the value
		**
		** \param p1 Point to compute the mean with
		** \param p2 Second point to compute the mean with
		** \return Always *this
		*/
		template<class U, class V>
		Point3D<T>& mean(const Point3D<U>& p1, const Point3D<V>& p2)
		{
			x = (T) ((p1.x + p2.x) / 2.0f);
			y = (T) ((p1.y + p2.y) / 2.0f);
			z = (T) ((p1.z + p2.z) / 2.0f);
			return *this;
		}

		/*!
		** \brief Get if the point is close to another point
		**
		** \param rhs The other point
		** \param delta Delta value
		*/
		template<class U, class V>
		bool isCloseTo(const Point3D<U>& rhs, const V delta) const
		{ return Math::Abs(x-rhs.x) < delta and Math::Abs(y-rhs.y) < delta and Math::Abs(z-rhs.z) < delta; }

		/*!
		** \brief Get if the point is close to another point
		**
		** \param x1 The X coordinate of the other point
		** \param y1 The Y coordinate of the other point
		** \param z1 The Z coordinate of the other point
		** \param delta Delta value
		*/
		template<class U, class V, class W, class D>
		bool isCloseTo(const U x1, const V y1, const W z1, const D delta) const
		{ return Math::Abs(x-x1) < delta and Math::Abs(y-y1) < delta and Math::Abs(z-z1) < delta; }


		//! \name Operators
		//{

		/*!
		** \brief Reset all coordinates
		**
		** \param x1 The new value for the X coordinate
		** \param y1 The new value for the Y coordinate
		** \param z1 The new value for the Z coordinate
		** \see move()
		*/
		template<class U, class V, class W>
		void operator () (const U x1, const V y1, const W z1) { x = (T)x1; y = (T)y1; z = (T)z1; }
		/*!
		** \brief Copy all coordinates from another point
		**
		** \param p The coordinates to copy
		**
		** \see move()
		*/
		template<class U>
		void operator () (const Point3D<U>& p) { x = (T)p.x; y = (T)p.y; z = (T)p.z; }


		/*!
		** \brief Translate the point with the same value for all coordinates
		**
		** \param k The value to add to all coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U>
		Point3D<T>& operator += (const U& k) { x += (T)k; y += (T)k; z += (T)k; return (*this); }

		/*!
		** \brief Translate the point with relative coordinates
		**
		** \param p The values to add to coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U>
		Point3D<T>& operator += (const Point3D<U>& p) { x += (T)p.x; y += (T)p.y; z += (T)p.z; return (*this); }


		/*!
		** \brief Translate the point with the same value for all coordinates
		**
		** \param k The value to add to all coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U>
		Point3D<T>& operator *= (const U& k)
		{
			x = (T)(x * k);
			y = (T)(y * k);
			z = (T)(z * k);
			return (*this);
		}

		/*!
		** \brief Translate the point with relative coordinates
		**
		** \param p The values to add to coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U>
		Point3D<T>& operator *= (const Point3D<U>& p)
		{
			x = (T)(x * p.x);
			y = (T)(y * p.y);
			z = (T)(z * p.z);
			return (*this);
		}


		/*!
		** \brief Comparison operator (equal with)
		**
		** \param rhs The other point to compare with
		** \return True if the two points are equal
		*/
		template<class U> bool operator == (const Point3D<U>& rhs) const
		{ return Math::Equals<T>(rhs.x, x) and Math::Equals<T>(rhs.y, y) and Math::Equals<T>(rhs.z, z); }

		/*!
		** \brief Comparison operator (non equal with)
		**
		** \param rhs The other point to compare with
		** \return True if the two points are not equal
		*/
		template<class U> bool operator != (const Point3D<U>& rhs) const
		{ return !(*this == rhs); }

		/*!
		** \brief Assign new values for all coordinates from another point
		**
		** \param p The new coordinates
		** \return Always *this
		**
		** \see move()
		*/
		Point3D& operator = (const Point3D& p) { x = p.x; y = p.y; z = p.z; return (*this); }

		/*!
		** \brief Assign new values for all coordinates from another point
		**
		** \param p The new coordinates
		** \return Always *this
		**
		** \see move()
		*/
		template<class U>
		Point3D<T>& operator = (const Point3D<U>& p) { x = (T)p.x; y = (T)p.y; z = (T)p.z; return (*this); }

		/*!
		** \brief Check if a point's coordinates are < to another's
		**
		** \param p The point to compare with
		** \return true if this < p, false otherwise
		**
		** \remarks X is compared first, then if necessary Y, then Z.
		*/
		template<class U>
		bool operator < (const Point3D<U>& p) const
		{
			if (not Math::Equals(x, (T)p.x))
				return x < (T)p.x;
			if (not Math::Equals(y, (T)p.y))
				return y < (T)p.y;
			return z < (T)p.z;
		}

		/*!
		** \brief Check if a point's coordinates are <= to another's
		**
		** \param p The point to compare with
		** \return true if this <= p, false otherwise
		**
		** \remarks X is compared first, then if necessary Y, then Z.
		*/
		template<class U>
		bool operator <= (const Point3D<U>& p) const
		{
			return !(*this > p);
		}

		/*!
		** \brief Check if a point's coordinates are > to another's
		**
		** \param p The point to compare with
		** \return true if this > p, false otherwise
		**
		** \remarks X is compared first, then if necessary Y, then Z.
		*/
		template<class U>
		bool operator > (const Point3D<U>& p) const
		{
			return x > (T)p.x or y > (T)p.y or z > (T)p.z;
		}

		/*!
		** \brief Check if a point's coordinates are >= to another's
		**
		** \param p The point to compare with
		** \return true if this >= p, false otherwise
		**
		** \remarks X is compared first, then if necessary Y, then Z.
		*/
		template<class U>
		bool operator >= (const Point3D<U>& p) const
		{
			return !(*this < p);
		}

		/*!
		** \brief Operator [] overload.
		*/
		T& operator [] (const uint i)
		{
			switch (i)
			{
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				default:
					assert(false and "Index out of bounds !");
			}
			return x;
		}

		/*!
		** \brief Const Operator [] overload.
		*/
		const T& operator [] (const uint i) const
		{
			switch (i)
			{
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				default:
					assert(false and "Index out of bounds !");
			}
			return x;
		}

		//} Operators


		/*!
		** \brief Print the point
		**
		** \param[in,out] out An output stream
		** \return The output stream `out`
		*/
		template<class StreamT> StreamT& print(StreamT& out) const
		{
			out << "(" << x << "," << y << "," << z << ")";
			return out;
		}


	public:
		//! X coordinate
		T x;
		//! Y coordinate
		T y;
		//! Z coordinate
		T z;

	}; // class Point3D



} // namespace Yuni




//! \name Operator overload for stream printing
//@{
template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Point3D<T>& p)
{ return p.print(out); }

template<class T, class U>
inline Yuni::Point3D<T> operator + (const Yuni::Point3D<T>& lhs, const U& rhs)
{ return Yuni::Point3D<T>(lhs) += rhs; }

template<class T, class U>
inline Yuni::Point3D<T> operator + (const U& lhs, const Yuni::Point3D<U>& rhs)
{ return Yuni::Point3D<T>(rhs) += lhs; }

template<class T, class U>
inline Yuni::Point3D<T> operator * (const Yuni::Point3D<T>& lhs, const U& rhs)
{ return Yuni::Point3D<T>(lhs) *= rhs; }

template<class T, class U>
inline Yuni::Point3D<T> operator * (const U& lhs, const Yuni::Point3D<T>& rhs)
{ return Yuni::Point3D<T>(rhs) *= lhs; }
//@}
