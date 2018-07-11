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
#include "connector-data.h"
#include "../../datetime/timestamp.h"



namespace Yuni
{
namespace Private
{
namespace DBI
{

	ConnectorData::~ConnectorData()
	{
		delete instance;
	}



	ChannelPtr ConnectorData::createNewChannelWL(uint64 threadid)
	{
		ChannelPtr newchan = new Channel(settings, adapter);
		channels[threadid] = newchan;
		return newchan;
	}


	uint ConnectorData::closeTooOldChannels(uint idleTime, uint& remainingCount)
	{
		// the current timestamp
		sint64 now = Yuni::DateTime::Now();
		// the number of channel which have been removed
		uint removedCount = 0;

		// avoid concurrent access
		Yuni::MutexLocker locker(mutex);

		// checking each channel
		Channel::Table::iterator it = channels.begin();
		while (it != channels.end())
		{
			// alias to the channel itself
			Channel& channel = *(it->second);

			// we will first check the idle time of the channel, before
			// locking its own mutex

			// beware : the variable `lastUsed` might be in the future in comparison
			// of our variable `now`, since it can be modified without locking
			// our variable `mutex`
			if (now >= channel.lastUsed)
			{
				// checking idle time
				if (idleTime <= (now - channel.lastUsed))
				{
					// our channel may be idle for too long
					// checking if it is not currently in use
					if (channel.mutex.trylock())
					{
						// it is safe to unlock here since no transaction can lock it
						// (our own mutex is already locked)
						channel.mutex.unlock();
						// removing the channel
						channels.erase(it++);
						// statistics
						++removedCount;
						// iterate
						continue;
					}
					else
					{
						// currently in use
						// this channel can not be closed
					}

				} // idle time check
			} // future check

			// next channel
			++it;
		}

		// the thread which would periodically check for idle channels might
		// be no longer necessary. This variable will be the indicator
		remainingCount = (uint) channels.size();

		// return the number of dead channels
		return removedCount;
	}





} // namespace DBI
} // namespace Private
} // namespace Yuni

