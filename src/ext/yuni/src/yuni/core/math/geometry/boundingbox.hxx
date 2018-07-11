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
#include "boundingbox.h"



namespace Yuni
{

	template<class T>
	inline BoundingBox<T>::BoundingBox()
		: pMin(), pMax(), pCenter()
	{}


	template<class T>
	inline
	BoundingBox<T>::BoundingBox(const Point3D<T>& min, const Point3D<T>& max)
		: pMin(min), pMax(max)
		, pCenter((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2)
	{}


	template<class T>
	void BoundingBox<T>::addPoint(const Point3D<T>& point)
	{
		// Update the minimum
		if (point.x < pMin.x)
			pMin.x = point.x;
		if (point.y < pMin.y)
			pMin.y = point.y;
		if (point.z < pMin.z)
			pMin.z = point.z;
		// Update the maximum
		if (point.x > pMax.x)
			pMax.x = point.x;
		if (point.y > pMax.y)
			pMax.y = point.y;
		if (point.z > pMax.z)
			pMax.z = point.z;
	}


	template<class T>
	inline void BoundingBox<T>::addTriangle(const Triangle& tri)
	{
		addPoint(tri.vertex1());
		addPoint(tri.vertex2());
		addPoint(tri.vertex3());
	}


	template<class T>
	inline bool BoundingBox<T>::contains(const Point3D<T>& point) const
	{
		if (pMin.x > point.x || pMax.x < point.x)
			return false;
		if (pMin.y > point.y || pMax.y < point.y)
			return false;
		if (pMin.z > point.z || pMax.z < point.z)
			return false;
		return true;
	}

	template<class T>
	template<class U>
	inline void BoundingBox<T>::reset(Point3D<U> newCenter)
	{
		pCenter.move(newCenter);
		pMin.move(pCenter);
		pMax.move(pCenter);
	}



} // namespace Yuni
