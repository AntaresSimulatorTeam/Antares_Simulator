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

#include "apply.h"


namespace Yuni
{
namespace UI
{
namespace Animation
{



	template<class T>
	inline T Transition<T>::operator () (uint timeMS) const
	{
		return currentValue(timeMS);
	}


	template<class T>
	T Transition<T>::currentValue(uint timeMS) const
	{
		if (pKeyFrames.size() < 2)
			return 	pKeyFrames.size() > 0 ? pKeyFrames.begin()->second : T();

		if (pLoop)
		{
			while (timeMS > pDuration)
				timeMS -= pDuration;
		}

		auto end = pKeyFrames.end();
		auto it = pKeyFrames.begin();
		auto prev = it++;
		for (; end != it; ++it, ++prev)
		{
			if (timeMS < it->first)
			{
				auto easingIt = pEasings.find(prev->first);
				assert(easingIt != pEasings.end() and
					"Transition : There must always be an easing defined out of any key frame.");
				return apply(easingIt->second, prev->second, it->second,
					(float)(timeMS - prev->first) / (it->first - prev->first));
			}
		}
		return prev->second;
	}


	template<class T>
	inline T Transition<T>::apply(const EasingFunc& easing, const T& start, const T& end, float ratio) const
	{
		return Yuni::Extension::UI::Animation::Apply<T>::Transition(easing, start, end, ratio);
	}




} // namespace Animation
} // namespace UI
} // namespace Yuni
