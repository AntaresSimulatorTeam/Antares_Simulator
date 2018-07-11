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
#include "../../../yuni.h"
#include "../math.h"
#include "../trigonometric.h"
#include "point3D.h"



namespace Yuni
{

	/*!
	** \brief Represents a 3D-vector, with generic homogeneous content
	*/
	template<class T = float>
	class YUNI_DECL Vector3D final
	{
	public:
		/*!
		** \brief Compute the mean between two arbitrary vectors
		**
		** \param p1 The first vector
		** \param p2 The second vector
		** \return A new instance of Vector3D
		*/
		static Vector3D Mean(const Vector3D& p1, const Vector3D& p2);

		/*!
		** \brief Compute the magnitude of the addition of two arbitrary vectors
		**
		** \code
		** Yuni::Vector3D<> a(1.,   2,4, 6.9);
		** Yuni::Vector3D<> b(4.1., 0.2, 3.1);
		**
		** // This way is faster
		** std::cout << "Magnitude : " << Yuni::Vector3D<>::Magnitude(a, b) << std::endl;
		** // than
		** std::cout << "Magnitude : " << (a + b).magnitude() << std::endl;
		** \endcode
		** \param p1 The first vector
		** \param p2 The second vector
		** \return The magnitude of the addition of the 2 vectors
		*/
		static T Magnitude(const Vector3D& p1, const Vector3D& p2);


		/*!
		** \brief Compute the dot product of two arbitrary vectors
		**
		** \param p1 The first vector
		** \param p2 The second vector
		** \return The dot product between the 2 vectors
		*/
		static T DotProduct(const Vector3D& p1, const Vector3D& p2);

		/*!
		** \brief Compute the cross product of two arbitrary vectors
		**
		** \param p1 The first vector
		** \param p2 The second vector
		*/
		static Vector3D CrossProduct(const Vector3D& p1, const Vector3D& p2);

		/*!
		** \brief Compute the cross product from three points (plane normal)
		**
		** \param p1 The first point
		** \param p2 The second point
		** \param p3 The third point
		*/
		template<class U, class V, class W>
		static Vector3D CrossProduct(const Point3D<U>& p1, const Point3D<V>& p2, const Point3D<W>& p3);


		/*!
		** \brief Calculate whether two vectors are colinear
		**
		** \param p1 The first vector
		** \param p2 The second vector
		** \param ignoreDirection True to accept colinearity with opposite directions
		*/
		static bool AreColinear(const Vector3D& p1, const Vector3D& p2, bool ignoreDirection = true);

		/*!
		** \brief Compute the angle between two arbitrary vectors
		**
		** \param p1 The first vector
		** \param p2 The second vector
		*/
		static T Angle(const Vector3D& p1, const Vector3D& p2);

		/*!
		** \brief Compute the angle between two vectors, with a specific return type
		**
		** \param out Where to store the result
		** \param p1 The first vector
		** \param p2 The second vector
		*/
		template<class R>
		void Angle(R& out, const Vector3D<T>& p1, const Vector3D<T>& p2);


	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Vector3D();

		/*!
		** \brief Constructor
		** \param x1 The default X coordinate
		** \param y1 The default Y coordinate
		** \param z1 The default Z coordinate
		*/
		template<class U, class V, class W>
		Vector3D(const U& x1, const V& y1, const W& z1 = W());

		/*!
		** \brief Constructor using two points
		**
		** \param origin Origin point of the vector
		** \param end End point of the vector
		*/
		template<class U, class V>
		Vector3D(const Point3D<U>& origin, const Point3D<V>& end);

		//! Constructor by copy
		Vector3D(const Vector3D& rhs);

		//! Constructor by copy
		template<class U> Vector3D(const Vector3D<U>& v);
		//@}


		//! \name Clear the coordinates
		//@{
		//! Clear the vector to the null vector
		Vector3D<T>& clear();
		//@}


		//! \name Translation
		//@{
		/*!
		** \brief Add the same value for all coordinates to the vector
		** \param k The value to add to all coordinates
		*/
		template<class U> void translate(const U k);
		/*!
		** \brief Translate the point with relative coordinates
		** \param x1 The value to add to the X coordinate
		** \param y1 The value to add to the Y coordinate
		** \param z1 The value to add to the Z coordinate
		*/
		template<class U, class V, class W>
		void translate(const U x1, const V y1, const W z1);
		/*!
		** \brief Translate the point with relative coordinates from another Point
		** \param p The values to add to the coordinates
		*/
		template<class U> void translate(const Vector3D<U>& p);
		//@}


		//! \name Components
		//@{
		/*!
		** \brief Get if the vector is null
		*/
		bool null() const;

		/*!
		** \brief Get if the vector is a unit vector (magnitude = 1)
		*/
		bool unit() const;

		/*!
		** \brief Compute the magnitude of the vector
		*/
		T magnitude() const;

		/*!
		** \brief Compute the square magnitude of the vector
		*/
		T squareMagnitude() const;

		/*!
		** \brief Compute the dot product with another arbitrary vector
		*/
		T dotProduct(const Vector3D& rhs) const;

		/*!
		** \brief Normalize the vector (coefficient = 1.)
		*/
		Vector3D& normalize();
		/*!
		** \brief Normalize the vector with a different coefficient
		*/
		Vector3D& normalize(const T coeff);
		//@}


		//! \name Mean
		//@{
		/*!
		** \brief Calculate the mean between two points
		**
		** The calling object is modified to store the value
		**
		** \param p Point to compute the mean with
		*/
		template<class U> Vector3D& mean(const Vector3D<U>& p);

		/*!
		** \brief Calculate the mean between two points
		**
		** The calling object is modified to store the value
		**
		** \param p1 Point to compute the mean with
		** \param p2 Second point to compute the mean with
		** \return Always *this
		*/
		template<class U, class V> Vector3D<T>& mean(const Vector3D<U>& p1, const Vector3D<V>& p2);
		//@}


		//! \name Operators
		//@{
		/*!
		** \brief Reset all coordinates
		**
		** \param x1 The new value for the X coordinate
		** \param y1 The new value for the Y coordinate
		** \param z1 The new value for the Z coordinate
		** \see move()
		*/
		template<class U, class V, class W>
		void operator () (const U x1, const V y1, const W z1);
		/*!
		** \brief Copy all coordinates from another vector
		**
		** \param v The coordinates to copy
		*/
		template<class U> void operator () (const Vector3D<U>& v);
		/*!
		** \brief Reset a vector using two points
		**
		** \param origin Start point of the vector
		** \param end End point of the vector
		*/
		template<class U, class V>
		void operator () (const Point3D<U>& origin, const Point3D<V>& end);

		/*!
		** \brief Translate the point with the same value for all coordinates
		**
		** \param k The value to add to all coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		Vector3D<T>& operator += (const T k);
		/*!
		** \brief Translate the point with relative coordinates
		**
		** \param p The values to add to coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U> Vector3D<T>& operator += (const Vector3D<U>& p);

		/*!
		** \brief Translate the point with the same value for all coordinates
		**
		** \param k The value to add to all coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		Vector3D<T>& operator -= (const T k);
		/*!
		** \brief Translate the point with relative coordinates
		**
		** \param p The values to add to coordinates
		** \return Always *this
		**
		** \see translate()
		*/
		template<class U> Vector3D<T>& operator -= (const Vector3D<U>& p);

		/*!
		** \brief Uniform scaling
		*/
		Vector3D<T>& operator *= (const T k);
		/*!
		** \brief Dot product
		*/
		template<class U> Vector3D<T>& operator *= (const Vector3D<U>& p);

		/*!
		** \brief Uniform scaling
		*/
		Vector3D<T>& operator /= (const T k);
		/*!
		** \brief Dot product
		*/
		template<class U> Vector3D<T>& operator /= (const Vector3D<U>& p);

		/*!
		** \brief Comparison operator (equal with)
		**
		** \param rhs The other point to compare with
		** \return True if the two points are equal
		*/
		template<class U> bool operator == (const Vector3D<U>& rhs) const;

		/*!
		** \brief Comparison operator (non equal with)
		**
		** \param rhs The other point to compare with
		** \return True if the two points are not equal
		*/
		template<class U> bool operator != (const Vector3D<U>& rhs) const;

		/*!
		** \brief Assign new values for all coordinates from another vector
		**
		** \param p The new coordinates
		** \return Always *this
		**
		** \see move()
		*/
		template<class U>
		Vector3D<T>& operator = (const Vector3D<U>& p);

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
					assert(false && "Index out of bounds !");
			}
			return x;
		}

		/*!
		** \brief Const Operator [] overload
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
					assert(false && "Index out of bounds !");
			}
			return x;
		}
		//@} Operators


		/*!
		** \brief Print the vector
		**
		** \param out An output stream
		** \return The output stream `out`
		*/
		std::ostream& print(std::ostream& out) const;


	public:
		//! X component
		T x;
		//! Y component
		T y;
		//! Z component
		T z;

	}; // class Vector3D


	//! Convenient typedef for Vector3D<float>
	typedef Vector3D<float>  Vector3f;


} // namespace Yuni


#include "vector3D.hxx"


//! \name Operator overload for stream printing
//@{
template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Vector3D<T>& v)
{ return v.print(out); }

template<class T>
inline const Yuni::Vector3D<T> operator + (const Yuni::Vector3D<T>& lhs, const Yuni::Vector3D<T>& rhs)
{ return Yuni::Vector3D<T>(lhs) += rhs; }

template<class T>
inline const Yuni::Vector3D<T> operator * (const Yuni::Vector3D<T>& lhs, const T& rhs)
{ return Yuni::Vector3D<T>(lhs) *= rhs; }
//@}
