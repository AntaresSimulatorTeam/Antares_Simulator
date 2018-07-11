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
#ifndef __YUNI_MEDIA_EMITTER_HXX__
# define __YUNI_MEDIA_EMITTER_HXX__

# include "../yuni.h"

namespace Yuni
{
namespace Media
{

	inline Emitter::Emitter(bool loop):
		pLoop(loop),
		pGain(DefaultGain),
		pSource(nullptr),
		pReady(false),
		pPlaying(false),
		pPaused(false),
		pModified(false)
	{}


	inline Emitter::Emitter(const Point3D<>& position, bool loop):
		pPosition(position),
		pLoop(loop),
		pGain(DefaultGain),
		pSource(nullptr),
		pReady(false),
		pPlaying(false),
		pPaused(false),
		pModified(false)
	{}


	inline Emitter::Emitter(const Point3D<>& position, const Vector3D<>& velocity,
		const Vector3D<>& direction, bool loop = false):
		pPosition(position),
		pVelocity(velocity),
		pDirection(direction),
		pLoop(loop),
		pGain(DefaultGain),
		pSource(nullptr),
		pReady(false),
		pPlaying(false),
		pPaused(false),
		pModified(false)
	{}


	inline bool Emitter::attached() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return !(!pSource);
	}


	inline bool Emitter::playing() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pPlaying;
	}


	inline bool Emitter::paused() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pPaused;
	}


	inline void Emitter::position(const Point3D<>& position)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (pPosition != position)
		{
			pPosition = position;
			pModified = true;
		}
	}


	inline Point3D<> Emitter::position() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pPosition;
	}


	inline void Emitter::velocity(const Vector3D<>& velocity)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (pVelocity != velocity)
		{
			pVelocity = velocity;
			pModified = true;
		}
	}


	inline Vector3D<> Emitter::velocity() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pVelocity;
	}


	inline void Emitter::direction(const Vector3D<>& direction)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (pDirection != direction)
		{
			pDirection = direction;
			pModified = true;
		}
	}


	inline Vector3D<> Emitter::direction() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pDirection;
	}


	inline void Emitter::loop(bool looping)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (pLoop != looping)
		{
			pLoop = looping;
			pModified = true;
		}
	}

	inline bool Emitter::loop() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pLoop;
	}


	inline void Emitter::gain(float newGain)
	{
		ThreadingPolicy::MutexLocker locker(*this);
		if (!Math::Equals(pGain, newGain))
		{
			pGain = newGain;
			pModified = true;
		}
	}


	inline float Emitter::gain() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pGain;
	}


	inline unsigned int Emitter::id() const
	{
		ThreadingPolicy::MutexLocker locker(*this);
		return pID;
	}




} // namespace Media
} // namespace Yuni

#endif // __YUNI_MEDIA_EMITTER_HXX__
