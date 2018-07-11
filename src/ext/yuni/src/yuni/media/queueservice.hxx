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
#ifndef __YUNI_MEDIA_QUEUESERVICE_HXX__
# define __YUNI_MEDIA_QUEUESERVICE_HXX__


namespace Yuni
{
namespace Media
{

	inline float QueueService::Emitters::elapsedTime(const AnyString& name)
	{
		return elapsedTime(get(name));
	}


	inline float QueueService::Emitters::elapsedTime(Emitter::Ptr emitter)
	{
		return !emitter ? 0 : emitter->elapsedTime();
	}


	inline bool QueueService::Emitters::play(const AnyString& name)
	{
		return play(get(name));
	}


	inline bool QueueService::Emitters::pause(const AnyString& name)
	{
		return pause(get(name));
	}


	inline bool QueueService::Emitters::stop(const AnyString& name)
	{
		return stop(get(name));
	}




} // namespace Media
} // namespace Yuni

#endif // __YUNI_MEDIA_QUEUESERVICE_HXX__
