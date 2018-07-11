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
#include "geometry/point3D.h"
#include "geometry/vector3D.h"



namespace Yuni
{
namespace Geometry
{

	/*!
	** \brief Intersection point of a line and a plane
	**
	** \param linePoint Any point on the line
	** \param lineDirection Direction vector of the line
	** \param planePoint Any point in the plane
	** \param planeNormal Normal vector of the plane
	*/
	template<typename T>
	static Point3D<T> LinePlaneIntersection(const Point3D<T>& linePoint,
		const Vector3D<T>& lineDirection, const Point3D<T>& planePoint,
		const Vector3D<T>& planeNormal);


	/*!
	** \brief Does a line segment intersect a plane ?
	**
	** \param startPoint Start of the segment
	** \param endPoint End of the segment
	** \param planePoint Any point in the plane
	** \param planeNormal Normal vector of the plane
	*/
	template<typename T>
	static bool SegmentIntersectsPlane(const Point3D<T>& startPoint, const Point3D<T>& endPoint,
		const Point3D<T>& planePoint, const Vector3D<T>& planeNormal);


	/*!
	** \brief Does a line segment intersect a quadrangle ?
	**
	** \param segmentStart Start of the segment
	** \param segmentEnd End of the segment
	** \param p1 First point of the quad
	** \param p2 Second point of the quad
	** \param p3 Third point of the quad
	*/
	template<typename T>
	static bool SegmentIntersectsTriangle(const Point3D<T>& segmentStart, const Point3D<T>& segmentEnd,
		const Point3D<T>& p1, const Point3D<T>& p2, const Point3D<T>& p3);

	/*!
	** \brief Does a line segment intersect a quadrangle ?
	**
	** \param segmentStart Start of the segment
	** \param segmentEnd End of the segment
	** \param p1 First point of the quad
	** \param p2 Second point of the quad
	** \param p3 Third point of the quad
	** \param p4 Fourth point of the quad
	*/
	template<typename T>
	static bool SegmentIntersectsQuad(const Point3D<T>& segmentStart, const Point3D<T>& segmentEnd,
		const Point3D<T>& p1, const Point3D<T>& p2,
		const Point3D<T>& p3, const Point3D<T>& p4);

	/*!
	** \brief Does a line segment intersect a sphere ?
	**
	** \param startPoint Start of the segment
	** \param endPoint End of the segment
	** \param center Center of the sphere
	** \param radius Radius of the sphere
	*/
	template<typename T>
	static bool SegmentIntersectsSphere(const Point3D<T>& startPoint, const Point3D<T>& endPoint,
		const Point3D<T>& center, T radius);

	/*!
	** \brief Projection point of a point on a line
	**
	** \param point The point to project
	** \param linePoint Any point in the line
	** \param lineDirection Direction vector of the line
	*/
	template<typename T>
	static Point3D<T> PointToLineProjection(const Point3D<T>& point,
		const Point3D<T>& linePoint, const Vector3D<T>& lineDirection);


	/*!
	** \brief Projection point of a point on a plane
	**
	** \param point The point to project
	** \param planePoint Any point in the plane
	** \param planeNormal Normal vector of the plane
	*/
	template<typename T>
	static Point3D<T> PointToPlaneProjection(const Point3D<T>& point,
		const Point3D<T>& planePoint, const Vector3D<T>& planeNormal);


	/*!
	** \brief Angle between two vectors
	*/
	template<typename T>
	static T VectorAngle(const Vector3D<T>& start, const Vector3D<T>& destination);




} // namespace Geometry
} // namespace Yuni

#include "geometry.hxx"
