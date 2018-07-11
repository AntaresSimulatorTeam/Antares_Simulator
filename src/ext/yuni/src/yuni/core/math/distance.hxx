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
#include "../static/types.h"


namespace Yuni
{
namespace Private
{
namespace MathImpl
{

	template<typename T, bool IsDecimal = Static::Type::IsDecimal<T>::Yes>
	struct Distance2D
	{
		static inline T Compute(T x1, T y1, T x2, T y2)
		{
			return Math::SquareRoot<T>((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
		}
	};

	template<typename T>
	struct Distance2D<T,0>
	{
		static inline T Compute(T x1, T y1, T x2, T y2)
		{
			return Math::RoundToInt<double,T>::Value(
				Math::SquareRoot<double>(
					(double)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))));
		}
	};


	template<typename T, bool IsDecimal = Static::Type::IsDecimal<T>::Yes>
	struct Distance3D
	{
		static inline T Compute(T x1, T y1, T z1, T x2, T y2, T z2)
		{
			return Math::SquareRoot<T>((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
		}
	};

	template<typename T>
	struct Distance3D<T,0>
	{
		static inline T Compute(T x1, T y1, T z1, T x2, T y2, T z2)
		{
			return Math::RoundToInt<double,T>::Value(
				Math::SquareRoot<double>(
					(double)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1))));
		}
	};


} // namespace MathImpl
} // namespace Private
} // namespace Yuni

namespace Yuni
{
namespace Math
{


	template<typename T> inline T Distance2D(T x1, T y1, T x2, T y2)
	{
		return Private::MathImpl::Distance2D<T>::Compute(x1, y1, x2, y2);
	}

	template<typename T> inline T Distance3D(T x1, T y1, T z1, T x2, T y2, T z2)
	{
		return Private::MathImpl::Distance3D<T>::Compute(x1, y1, z1, x2, y2, z2);
	}



} // namespace Math
} // namespace Yuni
