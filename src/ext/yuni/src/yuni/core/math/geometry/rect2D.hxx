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
#include "rect2D.h"


namespace Yuni
{

	template<class T>
	inline Rect2D<T>::Rect2D()
		:x1(), y1(), x2(), y2()
	{}


	template<class T>
	template<class U, class V>
	inline Rect2D<T>::Rect2D(const Point2D<U>& a, const Point2D<V>& b)
	{
		reset(a, b);
	}


	template<class T>
	template<class U, class V, class W, class X>
	inline Rect2D<T>::Rect2D(const U ax1, const V ay1, const W ax2, const X ay2)
		:x1((T)ax1), y1((T)ay1), x2((T)ax2), y2((T)ay2)
	{}


	template<class T>
	template<class U, class V>
	inline Rect2D<T>::Rect2D(const Rect2D<U>& a, const Rect2D<V>& b)
	{
		reset(a, b);
	}



	template<class T>
	inline Rect2D<T>& Rect2D<T>::reset()
	{
		// Reset all values to zero
		x1 = y1 = x2 = y2 = T();
		return *this;
	}


	template<class T>
	template<class U, class V>
	Rect2D<T>& Rect2D<T>::reset(const Point2D<U>& a, const Point2D<V>& b)
	{
		x1 = Math::Min( (T) a.x, (T) b.x);
		y1 = Math::Min( (T) a.y, (T) b.y);
		x2 = Math::Max( (T) a.x, (T) b.x);
		y2 = Math::Max( (T) a.y, (T) b.y);
		return *this;
	}


	template<class T>
	template<class U, class V, class W, class X>
	Rect2D<T>& Rect2D<T>::reset(const U a, const V b, const W c, const X d)
	{
		x1 = Math::Min( (T) a, (T) c);
		y1 = Math::Min( (T) b, (T) d);
		x2 = Math::Max( (T) a, (T) c);
		y2 = Math::Max( (T) b, (T) d);
		return *this;
	}


	template<class T>
	template<class U, class V>
	Rect2D<T>& Rect2D<T>::reset(const Rect2D<U>& a, const Rect2D<V>& b)
	{
		x1 = Math::Min( (T) Math::Min(a.x1, a.x2), (T) Math::Min(b.x1, b.y1) );
		y1 = Math::Min( (T) Math::Min(a.y1, a.y2), (T) Math::Min(b.y1, b.y1) );
		x2 = Math::Max( (T) Math::Max(a.x1, a.x2), (T) Math::Max(b.x1, b.y1) );
		y2 = Math::Max( (T) Math::Max(a.y1, a.y2), (T) Math::Max(b.y1, b.y1) );
		return *this;
	}


	template<class T>
	inline T Rect2D<T>::size() const
	{
		return Math::Abs(x2 - x1) * Math::Abs(y2 - y1);
	}


	template<class T>
	inline T Rect2D<T>::width() const
	{
		return Math::Abs(x2 - x1);
	}


	template<class T>
	inline T Rect2D<T>::height() const
	{
		return Math::Abs(y2 - y1);
	}


	template<class T>
	inline typename Rect2D<T>::PointType Rect2D<T>::topLeft() const
	{
		return Point2DType(x1, y1);
	}


	template<class T>
	inline typename Rect2D<T>::PointType Rect2D<T>::bottomRight() const
	{
		return Point2DType(x2, y2);
	}


	template<class T>
	void Rect2D<T>::normalize()
	{
		if (x2 < x1)
			Math::Swap(x1, x2);
		if (y2 < y1)
			Math::Swap(y1, y2);
	}


	template<class T>
	inline std::ostream& Rect2D<T>::print(std::ostream& out) const
	{
		return (out << x1 << "," << y1 << ":" << x2 << "," << y2);
	}


	template<class T>
	template<class U>
	inline void Rect2D<T>::move(const U ax1, const U ay1)
	{
		x2 = (T)ax1 + (x2 - x1);
		y2 = (T)ay1 + (y2 - y1);
		x1 = (T)ax1;
		y1 = (T)ay1;
	}


	template<class T>
	template<class U>
	inline void Rect2D<T>::move(const Point2D<U>& p)
	{
		x2 = (T)p.x + (x2 - x1);
		y2 = (T)p.y + (y2 - y1);
		x1 = (T)p.x;
		y1 = (T)p.y;
	}


	template<class T>
	template<class U>
	inline void Rect2D<T>::operator () (const Point2D<U>& p)
	{
		move(p);
	}


	template<class T>
	template<class U, class V>
	inline void Rect2D<T>::operator () (const U& ax1, const V& ay1)
	{
		move(ax1, ay1);
	}


	template<class T>
	template<class U, class V>
	inline void Rect2D<T>::operator () (const Point2D<U>& a, const Point2D<V>& b)
	{
		reset(a, b);
	}


	template<class T>
	template<class U, class V>
	inline void Rect2D<T>::operator () (const Rect2D<U>& a, const Rect2D<V>& b)
	{
		reset(a, b);
	}


	template<class T>
	template<class U>
	inline bool Rect2D<T>::contains(const Point2D<U>& p) const
	{
		return p.x >= x1 && p.y >= y1 && p.x <= x2 && p.y <= y2;
	}


	template<class T>
	template<class U, class V>
	inline bool Rect2D<T>::contains(const U nx, const V ny) const
	{
		return nx >= x1 && ny >= y1 && nx <= x2 && ny <= y2;
	}


	template<class T>
	template<class U>
	inline bool Rect2D<T>::contains(const Rect2D<U>& r) const
	{
		return (contains(r.x1, r.y1) && contains(r.x2, r.y2));
	}


	template<class T>
	template<class U>
	inline bool Rect2D<T>::collidedWith(const Rect2D<U>& r) const
	{
		return (contains(r.x1, r.y1) || contains(r.x2, r.y2));
	}


	template<typename T>
	template<class U>
	inline Rect2D<T>& Rect2D<T>::operator = (const Point2D<U>& p)
	{
		move(p);
		return *this;
	}


	template<typename T>
	template<class U>
	inline Rect2D<T>& Rect2D<T>::operator = (const Rect2D<U>& p)
	{
		x1 = (T)p.x1;
		y1 = (T)p.y1;
		x2 = (T)p.x2;
		y2 = (T)p.y2;
		return *this;
	}




} // namespace Yuni
