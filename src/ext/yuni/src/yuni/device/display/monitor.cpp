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
#include <cassert>
#include "monitor.h"
#include "../../core/hash/checksum/md5.h"


namespace Yuni
{
namespace Device
{
namespace Display
{

	const Monitor::Handle Monitor::InvalidHandle = YUNI_DEVICE_DISPLAY_INVALIDHANDLE;


	void Monitor::addSafeResolutions()
	{
		// Default (and assumed safe) resolutions
		pResolutions.reserve(pResolutions.size() + 3);
		pResolutions.push_back(new Resolution(1024, 768, 32));
		pResolutions.push_back(new Resolution(800, 600, 32));
		pResolutions.push_back(new Resolution(640, 480, 32));
	}


	Monitor::Monitor() :
		pHandle(Monitor::InvalidHandle),
		pPrimary(false),
		pHardwareAcceleration(false),
		pBuiltin(false)
	{
		addSafeResolutions();
	}


	Monitor::Monitor(const String& nm, const Monitor::Handle hwn, bool p, bool a, bool b) :
		pHandle(hwn),
		pProductName(nm),
		pResolutions(),
		pPrimary(p),
		pHardwareAcceleration(a),
		pBuiltin(b)
	{
		#ifdef YUNI_OS_WINDOWS
		if (NULL != hwn)
		{
			YUNI_MEMCPY(pDeviceID, sizeof(wchar_t) * 128, hwn, sizeof(wchar_t) * 128);
			pHandle = pDeviceID;
		}
		else
			memset(pDeviceID, 0, sizeof(wchar_t) * 128);
		#endif
		addSafeResolutions();
	}


	Monitor::Monitor(const Monitor& copy) :
		pHandle(copy.pHandle),
		pProductName(copy.pProductName),
		pResolutions(copy.pResolutions),
		pPrimary(copy.pPrimary),
		pHardwareAcceleration(copy.pHardwareAcceleration),
		pBuiltin(copy.pBuiltin)
	{}


	Monitor::~Monitor()
	{
		pResolutions.clear();
	}


	Resolution::Ptr Monitor::recommendedResolution() const
	{
		// we assume that the first resolution is the highest available
		return (!pResolutions.empty())
			? *(pResolutions.begin())
			: new Resolution(640, 480);
	}


	void Monitor::clear()
	{
		pResolutions.clear();
	}


	const String& Monitor::guid() const
	{
		// We assume that `pResolutions` is not empty
		if (pMD5Cache.empty())
		{
			String bld;
			bld << this->pProductName << '|';
			if (!pResolutions.empty())
				bld << (*pResolutions.begin())->toString();
			bld << '|' << pPrimary << '|' << pBuiltin << '|' << pHardwareAcceleration;

			Hash::Checksum::MD5 md5;
			pMD5Cache = md5.fromString(bld);
		}
		return pMD5Cache;
	}


	void Monitor::add(const Resolution::Ptr& resolution)
	{
		pResolutions.push_back(resolution);
	}


	bool Monitor::resolutionIsValid(const Resolution::Ptr& rhs) const
	{
		if (!(!rhs)) // The pointer must be valid
		{
			// Browse all available resolutions
			// The lookup should be done in the usual way since it is a sorted descendant list
			Resolution::Vector::const_iterator end = pResolutions.end();
			for (Resolution::Vector::const_iterator it = pResolutions.begin(); it != end; ++it)
			{
				if (*(*it) == *rhs)
					return true;
			}
		}
		return false;
	}


	Monitor& Monitor::operator += (Resolution* rhs)
	{
		pResolutions.push_back(rhs);
		return *this;
	}


	Monitor& Monitor::operator += (const Resolution::Ptr& rhs)
	{
		pResolutions.push_back(rhs);
		return *this;
	}


	Monitor& Monitor::operator << (Resolution* rhs)
	{
		pResolutions.push_back(rhs);
		return *this;
	}


	Monitor& Monitor::operator << (const Resolution::Ptr& rhs)
	{
		pResolutions.push_back(rhs);
		return *this;
	}





} // namespace Display
} // namespace Device
} // namespace Yuni

