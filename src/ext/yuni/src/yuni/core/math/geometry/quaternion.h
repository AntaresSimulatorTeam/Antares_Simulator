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
#include <ostream>
#include "vector3D.h"



namespace Yuni
{

	/*!
	** \brief Represents a quaternion, which is a mathematical object mainly used for camera
	*/
	template<typename T = float>
	class YUNI_DECL Quaternion final
	{
	public:
		/*!
		** \brief Magnitude of the quaternion
		**
		** N(q) = sqrt(w² + x² + y² + z²)
		*/
		static T Magnitude(const Quaternion& q);

		/*!
		** \brief Square magnitude of the quaternion
		*/
		static T SquareMagnitude(const Quaternion& q);

		//! \name Rotation
		//@{
		//! Point rotation
		template<class U>
		static Point3D<T> Rotate(const Point3D<T>& p, const Point3D<T>& origin,
			const Vector3D<U>& axis, T angle);

		//! Vector rotation
		template<class U>
		static Vector3D<T> Rotate(const Vector3D<T>& v, const Vector3D<U>& axis, T angle);
		//@}


	public:
		//! \name Constructors
		//@{
		//! Default constructor to null quaternion
		Quaternion();

		//! Decomposed constructor
		Quaternion(T newW, T x, T y, T z);

		//! Full constructor
		Quaternion(T newW, const Vector3D<T>& newV);

		//! Copy constructor
		template<class U>
		Quaternion(const Quaternion<U>& other);

		#ifdef GLM_VERSION
		//! From glm 3x3 matrix
		Quaternion(const glm::mat3& matrix): Quaternion(glm::quat(matrix))
		{}

		//! From glm 4x4 matrix
		Quaternion(const glm::mat4& matrix): Quaternion(glm::quat(matrix))
		{}

		//! From glm quaternion
		Quaternion(const glm::quat& quat):
			w(quat.w),
			v(quat.x, quat.y, quat.z)
		{}
		#endif
		//@}

		//! \name Reset the coordinates
		//@{
		//! Clear the quaternion to the null quaternion
		Quaternion<T>& clear();
		//@}

		//! \name Informations
		//@{
		//! Magnitude of the quaternion
		T magnitude() const;

		//! Is the quaternion null ?
		bool null() const;

		//! Is the quaternion unit ?
		bool unit() const;
		//@}

		//! \name Dependent quaternions
		//@{
		Quaternion<T> conjugate() const;
		//@}

		//! \name Manipulation
		//@{
		//! Normalization
		void normalize();
		//@}

		//! \name Operator overloads
		//@{
		//! Assignment
		template<class U>
		Quaternion<T>& operator = (const Quaternion<U>& other);

		#ifdef GLM_VERSION
		//! Assignment from glm 3x3 matrix
		Quaternion<T>& operator = (const glm::mat3& matrix)
		{
			return ((*this) = glm::quat(matrix));
		}

		//! Assignment from glm 4x4 matrix
		Quaternion<T>& operator = (const glm::mat4& matrix)
		{
			return ((*this) = glm::quat(matrix));
		}

		//! Assignment rom glm quaternion
		Quaternion<T>& operator = (const glm::quat& quat)
		{
			w = quat.w;
			v.x = quat.x;
			v.y = quat.y;
			v.z = quat.z;
			return *this;
		}
		#endif

		//! Addition
		template<class U>
		Quaternion<T>& operator += (const Quaternion<U>& other);

		//! Multiplication
		template<class U>
		Quaternion<T>& operator *= (const Quaternion<U>& other);
		//@}

		//! \name Printing
		//@{
		/*!
		** \brief Print the vector
		**
		** \param out An output stream
		** \return The output stream `out`
		*/
		std::ostream& print(std::ostream& out) const;
		//@}

	public:
		T w;
		Vector3D<T> v;

	}; // class Quaternion



} // namespace Yuni


//! \name Operator overload for stream printing
//@{

template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Quaternion<T>& v)
{ return v.print(out); }

template<class T>
inline const Yuni::Quaternion<T> operator + (const Yuni::Quaternion<T>& lhs, const Yuni::Quaternion<T>& rhs)
{
	Yuni::Quaternion<T> result(lhs);
	result += rhs;
	return result;
}

template<class T>
inline const Yuni::Quaternion<T> operator * (const Yuni::Quaternion<T>& lhs, const Yuni::Quaternion<T>& rhs)
{
	Yuni::Quaternion<T> result(lhs);
	result *= rhs;
	return result;
}

//@}


#include "quaternion.hxx"
