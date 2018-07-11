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
#include "list.h"
#include <map>




namespace Yuni
{
namespace Device
{
namespace Display
{

	typedef std::map<uint32, std::map<uint32, std::map<uint8, bool> > >  OrderedResolutions;

	typedef std::pair<Monitor::Ptr, SmartPtr<OrderedResolutions> > SingleMonitorFound;

	typedef std::vector<SingleMonitorFound> MonitorsFound;


} // namespace Display
} // namespace Device
} // namespace Yuni




#ifdef YUNI_OS_MAC
#	include "macosx.hxx"
#else
#	ifdef YUNI_OS_WINDOWS
#		include "windows.hxx"
#	endif
#	ifdef YUNI_OS_UNIX
#		include "linux.hxx"
#	endif
#endif





namespace Yuni
{
namespace Device
{
namespace Display
{


	List::List()
		: pNullMonitor(new Monitor(YUNI_DEVICE_DISPLAY_LIST_FAIL_SAFE_NAME))
	{
		// Adding the default monitor
		pMonitors.push_back(pNullMonitor);
		pPrimary = pNullMonitor;
	}


	List::List(const List& c)
		: pMonitors(c.pMonitors)
		, pPrimary(c.pPrimary)
		, pNullMonitor(c.pNullMonitor)
	{}


	List& List::operator = (const List& rhs)
	{
		pMonitors    = rhs.pMonitors;
		pPrimary     = rhs.pPrimary;
		pNullMonitor = rhs.pNullMonitor;
		return *this;
	}


	bool List::refresh(uint32 minWidth, uint32 minHeight, uint8 minDepth)
	{
		// No [primary] device for now
		pPrimary = pNullMonitor;
		// No monitor for now
		pMonitors.clear();

		// Get the list of monitors from a specific OS-Dependant implementation
		// into a temporary mapo
		MonitorsFound lst;
		refreshOSSpecific(lst);

		// We will browse each monitor found to see if it is suitable for our needs
		// In this case, it will be added to the result list
		const MonitorsFound::iterator& itEnd = lst.end();
		for (MonitorsFound::iterator it = lst.begin(); it != itEnd; ++it)
		{
			OrderedResolutions& resolutions = *(it->second);

			// A monitor without any resolution is useless
			if (resolutions.empty()) // no available resolutions
				continue;

			// Keeping a reference to our monitor for code clarity
			Monitor::Ptr& monitor = it->first;
			// Removing all its default resolutions
			monitor->clear();

			// Browsing all resolutions, in the reverse order
			// It is important since we must have the higher resolution at the
			// beginning
			{
				const OrderedResolutions::reverse_iterator& jEnd = resolutions.rend();
				for (OrderedResolutions::reverse_iterator j = resolutions.rbegin(); j != jEnd; ++j)
				{
					// j->first  : width
					// k->first  : height
					// k->second : color depth

					// Do not accept resolution with a width below minWidth
					if (j->first < minWidth)
						continue;

					for (std::map<uint32, std::map<uint8,bool> >::reverse_iterator k = j->second.rbegin(); k != j->second.rend(); ++k)
					{
						// Do not accept resolutions with a height below minHeight
						if (k->first < minHeight)
							continue;

						for (std::map<uint8,bool>::reverse_iterator l = k->second.rbegin(); l != k->second.rend(); ++l)
						{
							if (l->first >= minDepth)
								*monitor << new Resolution(j->first, k->first, l->first);
						}
					}
				}
			}

			if (not monitor->resolutions().empty()) // at least one resolution
			{
				pMonitors.push_back(monitor);
				if (monitor->primary())
					pPrimary = monitor;
			}

			// Hard limit
			if (pMonitors.size() == YUNI_DEVICE_MONITOR_COUNT_HARD_LIMIT)
				break;
		}

		// No available monitor/resolution
		// The list must not be empty
		if (pMonitors.empty())
		{
			pMonitors.push_back(pNullMonitor);
			return false;
		}
		return true;
	}


	Monitor::Ptr List::findByHandle(const Monitor::Handle hwn) const
	{
		const MonitorVector::const_iterator& end = pMonitors.end();
		for (MonitorVector::const_iterator it = pMonitors.begin(); it != end; ++it)
		{
			if (hwn == (*it)->handle())
				return *it;
		}
		return pNullMonitor;
	}


	Monitor::Ptr List::findByGUID(const String& guid) const
	{
		const MonitorVector::const_iterator& end = pMonitors.end();
		for (MonitorVector::const_iterator it = pMonitors.begin(); it != end; ++it)
		{
			if (guid == (*it)->guid())
				return *it;
		}
		return pNullMonitor;
	}





} // namespace Display
} // namespace Device
} // namespace Yuni
