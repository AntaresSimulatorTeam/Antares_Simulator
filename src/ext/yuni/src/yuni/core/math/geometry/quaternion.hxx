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
#include "../math.h"



namespace Yuni
{

	template<class T>
	inline T Quaternion<T>::Magnitude(const Quaternion& q)
	{
		return Math::SquareRoot(q.w * q.w + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z);
	}


	template<class T>
	inline T Quaternion<T>::SquareMagnitude(const Quaternion& q)
	{
		return q.w * q.w + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z;
	}


	template<class T>
	template<class U>
	inline Point3D<T> Quaternion<T>::Rotate(const Point3D<T>& p, const Point3D<T>& origin,
		const Vector3D<U>& axis, T angle)
	{
		// The view quaternion is [0, v]
		Quaternion<T> view(0, p.x - origin.x, p.y - origin.y, p.z - origin.z);
		if (!view.unit())
			view.normalize();
		Vector3D<U> axis2(axis);
		if (!axis2.unit())
			axis2.normalize();
		T sinA = Math::Sin(angle / (T)2);
		Quaternion<T> rot(Math::Cos(angle / (T)2), axis2.x * sinA, axis2.y * sinA, axis2.z * sinA);
		if (!rot.unit())
			rot.normalize();
		Vector3D<T> result = ((rot * view) * rot.conjugate()).v;
		return Point3D<T>(result.x + origin.x, result.y + origin.y, result.z + origin.z);
	}


	template<class T>
	template<class U>
	inline Vector3D<T> Quaternion<T>::Rotate(const Vector3D<T>& v, const Vector3D<U>& axis, T angle)
	{
		// The view quaternion is [0, v]
		Quaternion<T> view(0, v);
		if (!view.unit())
			view.normalize();
		Vector3D<U> axis2(axis);
		if (!axis2.unit())
			axis2.normalize();
		T sinA = Math::Sin(angle / (T)2);
		Quaternion<T> rot(Math::Cos(angle / (T)2), axis2.x * sinA, axis2.y * sinA, axis2.z * sinA);
		if (!rot.unit())
			rot.normalize();
		return ((rot * view) * rot.conjugate()).v;
	}



	template<class T>
	inline Quaternion<T>::Quaternion():
		w(1),
		v()
	{}


	template<class T>
	inline Quaternion<T>::Quaternion(T newW, T x, T y, T z):
		w(newW),
		v(x, y, z)
	{}


	template<class T>
	inline Quaternion<T>::Quaternion(T newW, const Vector3D<T>& newV):
		w(newW),
		v(newV)
	{}


	template<class T>
	template<class U>
	inline Quaternion<T>::Quaternion(const Quaternion<U>& other):
		w((T)other.w),
		v(other.v)
	{}


	template<class T>
	inline Quaternion<T>& Quaternion<T>::clear()
	{
		w = T(1);
		v.clear();
		return *this;
	}


	template<class T>
	inline T Quaternion<T>::magnitude() const
	{
		return Magnitude(*this);
	}


	template<class T>
	inline bool Quaternion<T>::null() const
	{
		return Math::Zero(w) && v.null();
	}


	template<class T>
	inline bool Quaternion<T>::unit() const
	{
		return Math::Zero(SquareMagnitude(*this) - T(1));
	}


	template<class T>
	inline Quaternion<T> Quaternion<T>::conjugate() const
	{
		return Quaternion<T>(w, -v.x, -v.y, -v.z);
	}


	template<class T>
	inline void Quaternion<T>::normalize()
	{
		T mag = magnitude();
		w /= mag;
		v.x /= mag;
		v.y /= mag;
		v.z /= mag;
	}


	template<class T>
	template<class U>
	inline Quaternion<T>& Quaternion<T>::operator = (const Quaternion<U>& other)
	{
		w = static_cast<T>(other.w);
		v = other.v;
	}


	template<class T>
	template<class U>
	inline Quaternion<T>& Quaternion<T>::operator += (const Quaternion<U>& other)
	{
		w += static_cast<T>(other.w);
		v.x += static_cast<T>(other.v.x);
		v.y += static_cast<T>(other.v.y);
		v.z += static_cast<T>(other.v.z);
		return *this;
	}


	template<class T>
	template<class U>
	inline Quaternion<T>& Quaternion<T>::operator *= (const Quaternion<U>& other)
	{
		T newW = other.w * w - other.v.x * v.x - other.v.y * v.y - other.v.z * v.z;
		T newX = other.v.x * w + other.w * v.x - other.v.z * v.y + other.v.y * v.z;
		T newY = other.v.y * w + other.v.z * v.x + other.w * v.y - other.v.x * v.z;
		T newZ = other.v.z * w - other.v.y * v.x + other.v.x * v.y + other.w * v.z;
		w = newW;
		v.x = newX;
		v.y = newY;
		v.z = newZ;
		return *this;
	}


	template<class T>
	inline std::ostream& Quaternion<T>::print(std::ostream& out) const
	{
		out << '[' << w << ", (" << v.x << ',' << v.y << ',' << v.z << ")]";
		return out;
	}



} // namespace Yuni
