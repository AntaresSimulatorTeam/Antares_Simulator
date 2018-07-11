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
#include "point3D.h"
#include "triangle.h"



namespace Yuni
{

	/*!
	** \brief A bounding box in 3D that grows with what is added inside
	** \ingroup Gfx
	*/
	template <class T = float>
	class YUNI_DECL BoundingBox final
	{
	public:
		//! \name Constructors and destructor
		//@{
		//! Default constructor
		BoundingBox();
		//! Constructor
		BoundingBox(const Point3D<T>& min, const Point3D<T>& max);
		//@}

		/*!
		** \brief Get the minimum coordinates of the box
		*/
		const Point3D<T>& min() const {return pMin;}

		/*!
		** \brief Get the maximum coordinates of the box
		*/
		const Point3D<T>& max() const {return pMax;}

		/*!
		** \brief Get the center of the box
		*/
		const Point3D<T>& center() const {return pCenter;}

		/*!
		** \brief Add a point that can possibly grow the bounding box
		*/
		void addPoint(const Point3D<T>& point);

		/*!
		** \brief Add a triangle that can possibly grow the bounding box
		*/
		void addTriangle(const Triangle& tri);

		/*!
		** \brief Is the point inside the bounding box?
		*/
		bool contains(const Point3D<T>& point) const;

		/*!
		** \brief Reset the bounding box to 0 size
		**
		** \param newCenter Reset the bounding box to a given position
		*/
		template<class U> void reset(Point3D<U> newCenter = Point3D<U>());


	private:
		//! Minimum X,Y,Z coordinates of the box
		Point3D<T> pMin;
		//! Maximum X,Y,Z coordinates of the box
		Point3D<T> pMax;
		//! Center of the box
		Point3D<T> pCenter;

	}; // class BoundingBox





} // namespace Yuni

#include "boundingbox.hxx"
