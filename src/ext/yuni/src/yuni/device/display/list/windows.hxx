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
#include "../../../core/system/windows.hdr.h"



namespace Yuni
{
namespace Device
{
namespace Display
{

	namespace // anonymous
	{

		static SingleMonitorFound* findMonitor(const wchar_t* monitorID, MonitorsFound& lst)
		{
			// Converting from wide char to multibyte in order to compare with Yuni::String
			const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, monitorID, -1, NULL, 0,  NULL, NULL);
			if (sizeRequired <= 0)
				return nullptr;

			String newID;
			newID.reserve((uint) sizeRequired);
			WideCharToMultiByte(CP_UTF8, 0, monitorID, -1, (char*)newID.data(), sizeRequired,  NULL, NULL);
			newID.resize(((uint) sizeRequired) - 1);

			uint i;
			for (i = 0; i < lst.size() and lst[i].first->guid() != newID; ++i)
			{
				// does nothing
			}

			return (i >= lst.size()) ? nullptr : &lst[i];
		}


		static void addResolutions(DISPLAY_DEVICEW& device, SmartPtr<OrderedResolutions> res)
		{
			DEVMODEW devMode;
			devMode.dmSize = (WORD)sizeof(devMode);
			devMode.dmDriverExtra = 32;

			for (uint i = 0; EnumDisplaySettingsW(device.DeviceName, i, &devMode); ++i)
			{
				(*res)[devMode.dmPelsWidth][devMode.dmPelsHeight][(uint8)devMode.dmBitsPerPel] = true;
			}
		}


		/*!
		 ** \brief Windows-specific implementation for the monitor / resolution list refresh
		 */
		static void refreshForWindows(MonitorsFound& lst)
		{
			DISPLAY_DEVICEW displayDevice;
			displayDevice.cb = (DWORD)sizeof(DISPLAY_DEVICEW);
			// Loop on all display devices
			for (uint countDevices = 0; EnumDisplayDevicesW(NULL, countDevices, (DISPLAY_DEVICEW*)&displayDevice, 0); ++countDevices)
			{
				// Ignore mirrored displays
				if (not (displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) and (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
				{
					DISPLAY_DEVICEW monitorDisplayDevice;
					monitorDisplayDevice.cb = (DWORD)sizeof(DISPLAY_DEVICEW);
					// A second call is necessary to get the monitor name associated with the display
					EnumDisplayDevicesW(displayDevice.DeviceName, 0, (DISPLAY_DEVICEW*)&monitorDisplayDevice, 0);
					bool mainDisplay = (0 != (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE));

					// Check if we have already stored info on this monitor
					SingleMonitorFound* monitorWithRes = findMonitor(monitorDisplayDevice.DeviceID, lst);
					bool newMonitor = (NULL == monitorWithRes);
					Monitor::Ptr monitor;
					SmartPtr<OrderedResolutions> res;

					if (newMonitor)
					{
						// Converting from wide char to multibyte in order to compare with Yuni::String
						const int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, monitorDisplayDevice.DeviceString, -1, NULL, 0,  NULL, NULL);
						if (sizeRequired <= 0)
							continue;

						String newID;
						newID.reserve((uint) sizeRequired);
						WideCharToMultiByte(CP_UTF8, 0, monitorDisplayDevice.DeviceString, -1, (char*)newID.data(), sizeRequired,  NULL, NULL);
						newID.resize(((uint) sizeRequired) - 1);
						// Create the new monitor
						monitor = new Monitor(newID, (Monitor::Handle)monitorDisplayDevice.DeviceID, mainDisplay, true, true);
						res = new OrderedResolutions();
					}
					else
					{
						monitor = monitorWithRes->first;
						res = monitorWithRes->second;
					}

					// Add associated resolutions
					addResolutions(displayDevice, res);

					// Add the monitor and its resolutions to the list if necessary
					if (newMonitor)
						lst.push_back(SingleMonitorFound(monitor, res));
					if (countDevices > YUNI_DEVICE_MONITOR_COUNT_HARD_LIMIT)
						break;
				}
			}
		}



		static inline void refreshOSSpecific(MonitorsFound& lst)
		{
			refreshForWindows(lst);
		}


	} // anonymous namespace



} // namespace Display
} // namespace Device
} // namespace Yuni
