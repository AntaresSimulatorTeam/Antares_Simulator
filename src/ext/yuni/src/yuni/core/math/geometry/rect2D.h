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
#include "point2D.h"



namespace Yuni
{

	/*!
	** \brief 2D-Rectangle
	*/
	template<class T>
	class YUNI_DECL Rect2D final
	{
	public:
		//! The original type T
		typedef T Type;
		//! The original type T
		typedef T value_type;
		//! Complete type for Rect2D
		typedef Rect2D<Type> Rect2DType;
		//! Point2D
		typedef Point2D<Type> PointType;

	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default constructor
		*/
		Rect2D();

		/*!
		** \brief Construct the rectangle to cover two 2D-points
		**
		** \param a The first point to cover
		** \param b The second point to cover
		*/
		template<class U, class V>
		Rect2D(const Point2D<U>& a, const Point2D<V>& b);

		/*!
		** \brief Constructor with given coordinates
		**
		** \param ax1 The top-left X-coordinate
		** \param ay1 The top-left Y-coordinate
		** \param ax2 The bottom-right X-coordinate
		** \param ay2 The bottom-right Y-coordinate
		*/
		template<class U, class V, class W, class X>
		Rect2D(const U ax1, const V ay1, const W ax2, const X ay2);

		/*!
		** \brief Construct the rectangle to cover two other rectangles
		**
		** \param a The first rectangle to cover
		** \param b The second rectangle to cover
		*/
		template<class U, class V>
		Rect2D(const Rect2D<U>& a, const Rect2D<V>& b);
		//@} // Constructors


		//! \name Reset values
		//@{
		/*!
		** \brief Reset all coordinates to zero
		** \return Always *this
		*/
		Rect2D<T>& reset();

		/*!
		** \brief Reset the rectangle coordinates to cover two 2D-points
		**
		** \param a The first point to cover
		** \param b The second point to cover
		** \return Always *this
		*/
		template<class U, class V>
		Rect2D<T>& reset(const Point2D<U>& a, const Point2D<V>& b);

		/*!
		** \brief Reset the rectangle coordinates to cover two 2D-points
		**
		** \param a The first X-coordinate
		** \param b The first Y-coordinate
		** \param c The second X-coordinate
		** \param d The second Y-coordinate
		** \return Always *this
		*/
		template<class U, class V, class W, class X>
		Rect2D<T>& reset(const U a, const V b, const W c, const X d);

		/*!
		** \brief Reset the rectangle coordinates to cover two other rectangles
		**
		** \param a The first rectangle to cover
		** \param b The second rectangle to cover
		** \return Always *this
		*/
		template<class U, class V>
		Rect2D<T>& reset(const Rect2D<U>& a, const Rect2D<V>& b);
		//@}

		/*!
		** \brief Get the equivalent Point2D for the top left
		*/
		PointType topLeft() const;
		/*!
		** \brief Get the equivalent Point2D for the bottom right
		*/
		PointType bottomRight() const;


		//! \name Measurement
		//@{
		/*!
		** \brief Get the surface occupied by the rectangle
		*/
		T size() const;
		/*!
		** \brief Get the width of the rectangle
		*/
		T width() const;
		/*!
		** \brief Get the height of the rectangle
		*/
		T height() const;
		//@}


		//! \name Move the entire rect
		//@{
		/*!
		** \brief Move the entire rect
		**
		** \param ax1 The new top left X-coordinate
		** \param ay1 The new top left Y-coordinate
		*/
		template<class U> void move(const U ax1, const U ay1);

		/*!
		** \brief Move the entire rect
		**
		** \param p The new top left coordinates
		*/
		template<class U> void move(const Point2D<U>& p);

		/*!
		** \brief Move the entire rect
		**
		** \param ax1 The new top left X-coordinate
		** \param ay1 The new top left Y-coordinate
		*/
		template<class U, class V> void operator () (const U& ax1, const V& ay1);

		/*!
		** \brief Move the entire rect
		** \param p The new top left coordinates
		*/
		template<class U> void operator () (const Point2D<U>& p);

		/*!
		** \brief Reset the rectangle coordinates to cover two 2D-points
		**
		** \param a The first point to cover
		** \param b The second point to cover
		*/
		template<class U, class V>
		void operator () (const Point2D<U>& a, const Point2D<V>& b);

		/*!
		** \brief Reset the rectangle coordinates to cover two other rectangles
		**
		** \param a The first rectangle to cover
		** \param b The second rectangle to cover
		*/
		template<class U, class V>
		void operator () (const Rect2D<U>& a, const Rect2D<V>& b);
		//@}


		//! \name Geometrical localisation
		//@{
		/*!
		** \brief Get if a point is contained inside the rect
		**
		** \param p The point
		** \return True if the point is contained inside the rect
		*/
		template<class U> bool contains(const Point2D<U>& p) const;
		/*!
		** \brief Get if a point is contained inside the rect
		**
		** \param nx The X-coordinate of the point
		** \param ny The Y-coordinate of the point
		** \return True if the point is contained inside the rect
		*/
		template<class U, class V> bool contains(const U nx, const V ny) const;

		/*!
		** \brief Get if another rectangle is contained inside this rectangle
		**
		** \param r The rect to test if it is inside the rect
		** \return True if the rect `r` is inside the rect `this`
		*/
		template<class U> bool contains(const Rect2D<U>& r) const;

		/*!
		** \brief Get if another rectangle collides with this rectangle
		*/
		template<class U> bool collidedWith(const Rect2D<U>& r) const;
		//@} Geometrical localisation


		//! \name Assignment
		//@{
		/*!
		** \brief Move the entire rect
		**
		** \param p The new top left coordinates
		** \return Always *this
		*/
		template<class U> Rect2D<T>& operator = (const Point2D<U>& p);

		/*!
		** \brief Assign new values for all coordinates from another rect
		**
		** \param p The new coordinates
		** \return Always *this
		*/
		template<class U> Rect2D<T>& operator = (const Rect2D<U>& p);
		//@}


		//! \name Stream printing
		//@{
		/*!
		** \brief Print the point
		*/
		std::ostream& print(std::ostream& out) const;
		//@}


		/*!
		** \brief Swap coordinates if needed to have x1/y1 at the top-left and
		** x2/y2 at the bottom-right
		*/
		void normalize();

	public:
		//! The top left X-coordinate
		T x1;
		//! The top left Y-coordinate
		T y1;
		//! The bottom right X-coordinate
		T x2;
		//! The bottom right Y-coordinate
		T y2;

	}; // class Rect2D





} // namespace Yuni

#include "rect2D.hxx"



//! name Operator overload for stream printing
//@{
template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Rect2D<T>& p)
{ return p.print(out); }
//@}
