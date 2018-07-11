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
#ifndef __YUNI_EXTENSION_UI_ANIMATION_APPLY_H__
# define __YUNI_EXTENSION_UI_ANIMATION_APPLY_H__

# include <yuni/core/bind.h>
# include <yuni/core/point2D.h>
# include <yuni/core/point3D.h>
# include <yuni/core/color/rgb.h>
# include <yuni/core/color/rgba.h>
# include "easing.h"

namespace Yuni
{
namespace Extension
{
namespace UI
{
namespace Animation
{

	//! Easing function prototype
	typedef Yuni::UI::Animation::Easing::Func EasingFunc;


	template<class T>
	class Apply final
	{
	public:
		static T Transition(const EasingFunc& easing, const T& start, const T& end, float ratio)
		{
			return start + (end - start) * easing(ratio);
		}
	};



	template<class T>
	class Apply<Point2D<T> > final
	{
	public:
		static Point2D<T> Transition(const EasingFunc& easing,
			const Point2D<T>& start, const Point2D<T>& end, float ratio)
		{
			float newRatio = easing(ratio);
			return Point2D<T>(
				start.x + (end.x - start.x) * newRatio,
				start.y + (end.y - start.y) * newRatio);
		}
	};


	template<class T>
	class Apply<Point3D<T> > final
	{
	public:
		static Point3D<T> Transition(const EasingFunc& easing,
			const Point3D<T>& start, const Point3D<T>& end, float ratio)
		{
			float newRatio = easing(ratio);
			return Point3D<T>(
				start.x + (end.x - start.x) * newRatio,
				start.y + (end.y - start.y) * newRatio,
				start.z + (end.z - start.z) * newRatio);
		}
	};


	template<class T>
	class Apply<Yuni::Color::RGB<T> > final
	{
	public:
		static Yuni::Color::RGB<T> Transition(const EasingFunc& easing,
			const Yuni::Color::RGB<T>& start, const Yuni::Color::RGB<T>& end, float ratio)
		{
			float newRatio = easing(ratio);
			return Yuni::Color::RGB<T>(
				start.red + (end.red - start.red) * newRatio,
				start.green + (end.green - start.green) * newRatio,
				start.blue + (end.blue - start.blue) * newRatio);
		}
	};


	template<class T>
	class Apply<Yuni::Color::RGBA<T> > final
	{
	public:
		static Yuni::Color::RGBA<T> Transition(const EasingFunc& easing,
			const Yuni::Color::RGBA<T>& start, const Yuni::Color::RGBA<T>& end, float ratio)
		{
			float newRatio = easing(ratio);
			return Yuni::Color::RGBA<T>(
				start.red + (end.red - start.red) * newRatio,
				start.green + (end.green - start.green) * newRatio,
				start.blue + (end.blue - start.blue) * newRatio,
				start.alpha + (end.alpha - start.alpha) * newRatio);
		}
	};


} // namespace Animation
} // namespace UI
} // namespace Extension
} // namespace Yuni

#endif // __YUNI_EXTENSION_UI_ANIMATION_APPLY_H__
