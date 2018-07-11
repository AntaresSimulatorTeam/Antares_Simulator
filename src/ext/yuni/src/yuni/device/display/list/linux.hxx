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
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>



namespace Yuni
{
namespace Device
{
namespace Display
{

	namespace // anonymous
	{

		static void refreshForX11(MonitorsFound& lst)
		{
			::Display* display = XOpenDisplay(NULL);
			if (display)
			{
				const Monitor::Handle scCount = ScreenCount(display);

				for (Monitor::Handle i = 0; i != scCount; ++i)
				{
					int depCount;
					int* depths = XListDepths(display, i, &depCount);

					Monitor::Ptr newMonitor(new Monitor(String(), i, (0 == i), true, false));
					SmartPtr<OrderedResolutions> res(new OrderedResolutions());

					// All resolutions
					int count;
					XRRScreenSize* list = XRRSizes(display, i, &count);
					for (int i = 0; i < count; ++i)
					{
						XRRScreenSize* it = list + i;
							for (int j = 0; j < depCount; ++j)
						{
							const int d = *(depths + j);
							if (d == 8 || d == 16 || d == 24 || d == 32)
								(*res)[it->width][it->height][(uint8) d] = true;
						}
					}

					lst.push_back(SingleMonitorFound(newMonitor, res));
					XFree(depths);
				}
				XCloseDisplay(display);
			}
		}


		static inline void refreshOSSpecific(MonitorsFound& lst)
		{
			refreshForX11(lst);
		}

	} // anonymous namespace



} // namespace Display
} // namespace Device
} // namespace Yuni
