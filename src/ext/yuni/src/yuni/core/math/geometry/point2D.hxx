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
#include "point2D.h"



namespace Yuni
{

	template<class T>
	inline Point2D<T>::Point2D() :
		x(), y()
	{}


	template<class T>
	template<class U, class V>
	inline Point2D<T>::Point2D(const U x1, const V y1) :
		x((T)x1), y((T)y1)
	{}


	template<class T>
	template<class U>
	inline Point2D<T>::Point2D(const Point2D<U>& p) :
		x((T)p.x), y((T)p.y)
	{}


	template<class T>
	inline Point2D<T>& Point2D<T>::reset()
	{
		x = y = T();
		return *this;
	}


	template<class T>
	inline void Point2D<T>::move(const T& x1, const T& y1)
	{
		x = x1;
		y = y1;
	}


	template<class T>
	template<class U>
	inline void Point2D<T>::move(const Point2D<U>& p)
	{
		x = (T) p.x;
		y = (T) p.y;
	}


	template<class T>
	inline void Point2D<T>::translate(const T& k)
	{
		x += k;
		y += k;
	}


	template<class T>
	inline void Point2D<T>::translate(const T& x1, const T& y1)
	{
		x += x1;
		y += y1;
	}


	template<class T>
	template<class U>
	inline void Point2D<T>::translate(const Point2D<U>& p)
	{
		x += (T) p.x;
		y += (T) p.y;
	}


	template<class T>
	template<class U>
	inline void Point2D<T>::mean(const Point2D<U>& p)
	{
		x = (x + p.x) / T(2);
		y = (y + p.y) / T(2);
	}


	template<class T>
	template<class U, class V>
	inline Point2D<T>& Point2D<T>::mean(const Point2D<U>& p1, const Point2D<V>& p2)
	{
		x = (p1.x + p2.x) / T(2);
		y = (p1.y + p2.y) / T(2);
		return *this;
	}


	template<class T>
	inline Point2D<T> Point2D<T>::Mean(const Point2D<T>& p1, const Point2D<T>& p2)
	{
		return Point2D<T>().mean(p1, p2);
	}


	template<class T>
	template<class U>
	bool Point2D<T>::isCloseTo(const Point2D<U>& rhs, const T& delta) const
	{
		return Math::Abs((U) x - rhs.x) < delta and Math::Abs((U) y - rhs.y) < delta;
	}


	template<class T>
	inline bool Point2D<T>::isCloseTo(const T& x1, const T& y1, const T& delta) const
	{
		return Math::Abs(x - x1) < delta and Math::Abs(y - y1) < delta;
	}


	template<class T>
	inline void Point2D<T>::operator () (const T& x1, const T& y1)
	{
		x = x1;
		y = y1;
	}


	template<class T>
	template<class U>
	inline void Point2D<T>::operator () (const Point2D<U>& p)
	{
		x = (T) p.x;
		y = (T) p.y;
	}


	template<class T>
	template<class U>
	inline Point2D<T>& Point2D<T>::operator += (const U& k)
	{
		x += (T)k;
		y += (T)k;
		return (*this);
	}


	template<class T>
	template<class U>
	inline Point2D<T>& Point2D<T>::operator += (const Point2D<U>& p)
	{
		x += (T) p.x;
		y += (T) p.y;
		return (*this);
	}


	template<class T>
	template<class U>
	inline Point2D<T>& Point2D<T>::operator *= (const U& k)
	{
		x = (T)(x * k);
		y = (T)(y * k);
		return (*this);
	}


	template<class T>
	template<class U>
	inline bool Point2D<T>::operator == (const Point2D<U>& rhs) const
	{
		return ((T) rhs.x == x) and ((T) rhs.y == y);
	}


	template<class T>
	template<class U>
	inline bool Point2D<T>::operator != (const Point2D<U>& rhs) const
	{
		return !(*this == rhs);
	}


	template<class T>
	template<class U>
	inline Point2D<T>& Point2D<T>::operator = (const Point2D<U>& p)
	{
		x = (T) p.x;
		y = (T) p.y;
		return (*this);
	}


	template<class T>
	template<class U>
	inline bool Point2D<T>::operator < (const Point2D<U>& p) const
	{
		if (not Math::Equals(x, (T)p.x))
			return x < (T)p.x;
		return y < (T)p.y;
	}


	template<class T>
	inline T& Point2D<T>::operator [] (uint i)
	{
		switch (i)
		{
			case 0:
				return x;
			case 1:
				return y;
			default:
				assert(false and "Index out of bounds !");
		}
		return x;
	}


	template<class T>
	inline const T& Point2D<T>::operator [] (uint i) const
	{
		switch (i)
		{
			case 0:
				return x;
			case 1:
				return y;
			default:
				assert(false and "Index out of bounds !");
		}
		return x;
	}


	template<class T>
	template<class StreamT>
	inline StreamT& Point2D<T>::print(StreamT& out) const
	{
		out << '(' << x << ',' << y << ')';
		return out;
	}





} // namespace Yuni



template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Point2D<T>& p)
{
	return p.print(out);
}


template<class T, class U>
inline Yuni::Point2D<T> operator + (const Yuni::Point2D<T>& lhs, const U& rhs)
{
	return Yuni::Point2D<T>(lhs) += rhs;
}


template<class T, class U>
inline Yuni::Point2D<T> operator + (const U& lhs, const Yuni::Point2D<T>& rhs)
{
	return Yuni::Point2D<T>(rhs) += lhs;
}


template<class T, class U>
inline Yuni::Point2D<T> operator * (const Yuni::Point2D<T>& lhs, const U& rhs)
{
	return Yuni::Point2D<T>(lhs) *= rhs;
}


template<class T, class U>
inline Yuni::Point2D<T> operator * (const U& lhs, const Yuni::Point2D<T>& rhs)
{
	return Yuni::Point2D<T>(rhs) *= lhs;
}
