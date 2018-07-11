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

#include "../../yuni.h"
#include "../../core/noncopyable.h"
#include "easing.h"


namespace Yuni
{
namespace UI
{
namespace Animation
{


	//! A timeline stores key values and transitions to animate a given variable in the program
	template<class T = float>
	class Transition final : private NonCopyable<Transition<T> >
	{
	public:
		//! Smart pointer
		typedef SmartPtr<Transition<T> >  Ptr;

		//! Prototype for an interpolation / easing function (cf Robert Penner's equations)
		typedef Easing::Func EasingFunc;

		//! Key frame map : key = frame time in ms, value = state at the given time
		typedef std::map<uint, T> FrameMap;

		//! Easing map : key = frame time in ms, value = easing used for this frame to the next
		typedef std::map<uint, EasingFunc> EasingMap;

	public:
		//! Constructor
		Transition(uint durationMS, bool loop = false):
			pDuration(durationMS),
			pLoop(loop)
		{}

		void addKeyFrame(const EasingFunc& easing, uint timeMS, const T& state)
		{
			pKeyFrames[timeMS] = state;
			pEasings[timeMS] = easing;
		}

		void removeKeyFrame(uint timeMS)
		{
			auto frameIt = pKeyFrames.find(timeMS);
			if (pKeyFrames.end() != frameIt)
				pKeyFrames.erase(frameIt);
			auto easingIt = pEasings.find(timeMS);
			if (pEasings.end() != frameIt)
				pEasings.erase(frameIt);
		}

		void clear()
		{
			pKeyFrames.clear();
			pEasings.clear();
		}

		//! Duration of the full animation
		uint duration() const { return pDuration; }
		//! Change animation duration
		void duration(uint newDurationMS) { pDuration = newDurationMS; }

		//! Should the animation loop ?
		bool loop() const { return pLoop; }
		//! Set whether the animation should loop
		void loop(bool shouldLoop) { pLoop = shouldLoop; }

		/*!
		** \brief Current interpolated value given the time in milliseconds along the timeline
		**
		** \remark
		**   It is legal to provide timeMS > duration(). Behaviour depends on the value of loop()
		*/
		T currentValue(uint timeMS) const;
		T operator () (uint timeMS) const;

	private:
		//! Apply a given easing at a given ratio ([0,1])
		T apply(const EasingFunc& easing, const T& start, const T& end, float ratio) const;

	private:
		//! Duration in milliseconds of the timeline
		uint pDuration;

		//! Must the animation loop ?
		bool pLoop;

		//! Key frames
		FrameMap pKeyFrames;

		//! Easings
		EasingMap pEasings;

	}; // class Transition



} // namespace Animation
} // namespace UI
} // namespace Yuni


#include "transition.hxx"

