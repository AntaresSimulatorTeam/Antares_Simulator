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
#include "math.h"


namespace Yuni
{
namespace Math
{


	/*!
	** \brief Compute the distance between two points (2D)
	*/
	template<typename T> inline T Distance2D(T x1, T y1, T x2, T y2);

	/*!
	** \brief Compute the distance between two points (3D)
	*/
	template<typename T> inline T Distance3D(T x1, T y1, T z1, T x2, T y2, T z2);




} // namespace Math
} // namespace Yuni

#include "distance.hxx"
