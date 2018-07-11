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

#include <yuni/core/math.h>
#include <yuni/core/math/trigonometric.h>


namespace Yuni
{
namespace UI
{
namespace Animation
{
namespace Easing
{

	//! Prototype for an interpolation / easing function (cf Robert Penner's equations)
	typedef Yuni::Bind<float (float ratio)>  Func;


	namespace // anonymous
	{
		template<class T>
		inline float Invert(const T& func, float ratio)
		{
			return 1.0f - func(1.0f - ratio);
		}

		template<class T>
		inline float MakeInOut(const T& func, float ratio)
		{
			return (ratio < 0.5f) ?
				func(ratio * 2.0f) / 2.0f :
				1.0f - func((1.0f - ratio) * 2.0f) / 2.0f;
		}

	} // namespace anonymous



	inline float None(float /*ratio*/)
	{
		return 0.0f;
	}


	inline float Linear(float ratio)
	{
		return ratio;
	}


	inline float QuadIn(float ratio)
	{
		return ratio * ratio;
	}


	inline float QuadOut(float ratio)
	{
		return Invert(QuadIn, ratio);
	}


	inline float QuadInOut(float ratio)
	{
		return MakeInOut(QuadIn, ratio);
	}


	inline float CubicIn(float ratio)
	{
		return Math::Power(ratio, 3);
	}


	inline float CubicOut(float ratio)
	{
		return Invert(CubicIn, ratio);
	}


	inline float CubicInOut(float ratio)
	{
		return MakeInOut(CubicIn, ratio);
	}


	inline float QuarticIn(float ratio)
	{
		return Math::Power(ratio, 4);
	}


	inline float QuarticOut(float ratio)
	{
		return Invert(QuarticIn, ratio);
	}


	inline float QuarticInOut(float ratio)
	{
		return MakeInOut(QuarticIn, ratio);
	}


	inline float QuinticIn(float ratio)
	{
		return Math::Power(ratio, 5);
	}


	inline float QuinticOut(float ratio)
	{
		return Invert(QuinticIn, ratio);
	}


	inline float QuinticInOut(float ratio)
	{
		return MakeInOut(QuinticIn, ratio);
	}


	inline float CircIn(float ratio)
	{
		return 1.0f - Math::SquareRootNoCheck(1.0f - ratio * ratio);
	}


	inline float CircOut(float ratio)
	{
		return Invert(CircIn, ratio);
	}


	inline float CircInOut(float ratio)
	{
		return MakeInOut(CircIn, ratio);
	}


	inline float Back(float ratio)
	{
		static const float v = 1.70158f;
		ratio -= 1.0f;
		return ratio * ratio * ((v + 1.0f) * ratio + v) + 1.0f;
	}


	inline float ElasticOut(float ratio)
	{
		static const float p = 0.3;
		return (float)(Math::Power(2, -10 * ratio) *
			Math::Sin((ratio - p / 4.0) * (2.0 * YUNI_PI) / p) + 1.0);
	}

	inline float ElasticIn(float ratio)
	{
		return Invert(ElasticOut, ratio);
	}


	inline float ElasticInOut(float ratio)
	{
		return MakeInOut(ElasticIn, ratio);
	}


	inline float BounceOut(float ratio)
	{
		if (ratio < 1 / 2.75)
		{
			return 7.5625 * ratio * ratio;
		}
		else if (ratio < 2 / 2.75)
		{
			ratio -= 1.5 / 2.75;
			return 7.5625 * ratio * ratio + .75;
		}
		else if (ratio < 2.5 / 2.75)
		{
			ratio -= 2.25 / 2.75;
			return 7.5625 * ratio * ratio + .9375;
		}
		ratio -= 2.625 / 2.75;
		return 7.5625 * ratio * ratio + .984375;
	}


	inline float BounceIn(float ratio)
	{
		return Invert(BounceOut, ratio);
	}



} // namespace Easing
} // namespace Animation
} // namespace UI
} // namespace Yuni
