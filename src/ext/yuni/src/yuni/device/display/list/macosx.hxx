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
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/graphics/IOGraphicsLib.h>


#define GET_MODE_WIDTH(mode) GetDictionaryLong((mode), kCGDisplayWidth)
#define GET_MODE_HEIGHT(mode) GetDictionaryLong((mode), kCGDisplayHeight)
#define GET_MODE_REFRESH_RATE(mode) GetDictionaryLong((mode), kCGDisplayRefreshRate)
#define GET_MODE_BITS_PER_PIXEL(mode) GetDictionaryLong((mode), kCGDisplayBitsPerPixel)

#define GET_MODE_SAFE_FOR_HARDWARE(mode) GetDictionaryBoolean((mode), kCGDisplayModeIsSafeForHardware)
#define GET_MODE_STRETCHED(mode) GetDictionaryBoolean((mode), kCGDisplayModeIsStretched)

#define GET_MODE_TELEVISION(mode) GetDictionaryBoolean((mode), kCGDisplayModeIsTelevisionOutput)




namespace Yuni
{
namespace Device
{
namespace Display
{

	namespace // anonymous
	{

		static uint bitDepthFromDisplayMode(CGDisplayModeRef mode)
		{
			CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);

			if (CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				return 32;
			if (CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				return 16;
			if (CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				return 8;

			return 0; // default and invalid value
		}



		static void cocoaGetAllAvailableModesUseful(CGDirectDisplayID display, SmartPtr<OrderedResolutions>& res)
		{
			// get a list of all possible display modes for this system.
			// >= 10.6 is required for CGDisplayCopyAllDisplayModes

			CFArrayRef availableModes = CGDisplayCopyAllDisplayModes(display, NULL);
			if (!availableModes)
				return;

			// Getting the current bits per pixels value
			uint currentModeBitsPerPixel;
			{
				CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display);
				if (!mode)
				{
					CFRelease(availableModes);
					return;
				}
				currentModeBitsPerPixel = bitDepthFromDisplayMode(mode);
				CGDisplayModeRelease(mode);
			}

			uint numberOfAvailableModes = (uint) CFArrayGetCount(availableModes);
			for (uint i = 0; i != numberOfAvailableModes; ++i)
			{
				CGDisplayModeRef mode = (CGDisplayModeRef) CFArrayGetValueAtIndex(availableModes, i);
				if (!mode)
					continue;

				// we are only interested in modes with the same bits per pixel as current.
				// to allow for switching from fullscreen to windowed modes.
				// that are safe for this hardward
				// that are not stretched.
				uint bitsPerPixel = bitDepthFromDisplayMode(mode);
				if (bitsPerPixel != currentModeBitsPerPixel)
					continue;

				uint width  = (uint) CGDisplayModeGetWidth(mode);
				uint height = (uint) CGDisplayModeGetHeight(mode);
				(*res) [width][height][(uint8) bitsPerPixel] = true;
			}

			CFRelease(availableModes);
		}


		static void DictionaryValueToString(String& out, CFStringRef formatString, ...)
		{
			CFStringRef resultString;
			CFDataRef data;
			va_list argList;

			va_start(argList, formatString);
			resultString = CFStringCreateWithFormatAndArguments(NULL, NULL, formatString, argList);
			va_end(argList);

			data = CFStringCreateExternalRepresentation(NULL, resultString, CFStringGetSystemEncoding(), '?');
			if (data != NULL)
			{
				char buffer[128];
				int len = snprintf(buffer, 128, "%.*s", (int)CFDataGetLength(data), CFDataGetBytePtr(data));
				if (len > 0)
					out.append(buffer, (uint)len);

				CFRelease(data);
			}

			CFRelease(resultString);
		}




		/*!
		 ** \brief Get all monitor and their resolutions from the Cocoa Framework
		 **
		 ** \see http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/Reference/reference.html#//apple_ref/doc/uid/TP30001070-CH202-F17085
		 ** \see http://developer.apple.com/documentation/GraphicsImaging/Conceptual/QuartzDisplayServicesConceptual/Articles/DisplayInfo.html#//apple_ref/doc/uid/TP40004272
		 ** \see http://developer.apple.com/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_quartz_services/chapter_952_section_2.html
		 ** \see http://www.cocoabuilder.com/archive/message/cocoa/2006/9/7/170773
		 */
		static void refreshForCocoa(MonitorsFound& lst)
		{
			// All displays
			CGDirectDisplayID displayArray [YUNI_DEVICE_MONITOR_COUNT_HARD_LIMIT];
			// The count of display
			CGDisplayCount numDisplays;

			// Grab all available displays
			CGGetOnlineDisplayList(YUNI_DEVICE_MONITOR_COUNT_HARD_LIMIT, displayArray, &numDisplays);
			if (0 == numDisplays)
				return;

			// Product name
			String monitorProductName;

			// Browse all displays
			for (uint i = 0; i < numDisplays; ++i)
			{
				const CGDirectDisplayID display = displayArray[i];

				monitorProductName.clear();

				// Informations about the display, such as its product name
				io_connect_t displayPort = CGDisplayIOServicePort(display);
				if (displayPort != MACH_PORT_NULL)
				{
					CFDictionaryRef dict = IODisplayCreateInfoDictionary(displayPort, 0);

					CFDictionaryRef names = (CFDictionaryRef)CFDictionaryGetValue(dict, CFSTR(kDisplayProductName));
					// Count items in the dictionary
					CFIndex count = CFDictionaryGetCount(names);

					if (count)
					{
						CFTypeRef* keys   = (CFTypeRef*) ::malloc((uint) count * sizeof(CFTypeRef));
						CFTypeRef* values = (CFTypeRef*) ::malloc((uint) count * sizeof(CFTypeRef));
						CFDictionaryGetKeysAndValues(names, (const void**) keys, (const void**) values);

						DictionaryValueToString(monitorProductName, CFSTR("%@"), values[0]);
						monitorProductName.trim(" \r\n\t");

						::free(keys);
						::free(values);
					}
					CFRelease(dict);
				}

				// int width  = CGDisplayPixelsWide(display);
				// int height = CGDisplayPixelsHigh(display);
				// int bpp    = CGDisplayBitsPerPixel(display);
				bool mainDisplay = CGDisplayIsMain(display);
				bool builtin     = CGDisplayIsBuiltin(display);
				bool ha          = CGDisplayUsesOpenGLAcceleration(display);
				// uint32_t modelNumber = CGDisplayModelNumber(display);
				// uint32_t serialNumer = CGDisplaySerialNumber(display);

				Monitor::Ptr newMonitor(new Monitor(monitorProductName,
													(Monitor::Handle)display, mainDisplay, ha, builtin));

				SmartPtr<OrderedResolutions> res(new OrderedResolutions());
				cocoaGetAllAvailableModesUseful(display, res);

				// Add it to the list
				lst.push_back(SingleMonitorFound(newMonitor, res));
			}
		}


		static inline void refreshOSSpecific(MonitorsFound& lst)
		{
			refreshForCocoa(lst);
		}


	} // anonymous namespace





} // namespace Display
} // namespace Device
} // namespace Yuni
