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
#ifndef __YUNI_MEDIA_SOURCE_HXX__
# define __YUNI_MEDIA_SOURCE_HXX__


namespace Yuni
{
namespace Media
{


	inline Source::Source() :
		pAStream(nullptr),
		pVStream(nullptr),
		pBufferCount(),
		pSecondsElapsed(),
		pSecondsCurrent()
	{
	}


	inline Source::~Source()
	{
	}


	inline uint Source::duration() const
	{
		ThreadingPolicy::MutexLocker lock(*this);
		if (pAStream)
			return pAStream->duration();

		if (pVStream)
			return pVStream->duration();

		return 0u;
	}



} // namespace Media
} // namespace Yuni

#endif // __YUNI_MEDIA_SOURCE_HXX__
