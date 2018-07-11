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
#include "monitor.h"



namespace Yuni
{
namespace Device
{
namespace Display
{

	inline Monitor::Handle Monitor::handle() const
	{
		return pHandle;
	}


	inline bool Monitor::valid() const
	{
		return pHandle != Monitor::InvalidHandle;
	}


	inline const String& Monitor::productName() const
	{
		return pProductName;
	}


	inline const Resolution::Vector& Monitor::resolutions() const
	{
		return pResolutions;
	}


	inline bool Monitor::primary() const
	{
		return pPrimary;
	}


	inline bool Monitor::hardwareAcceleration() const
	{
		return pHardwareAcceleration;
	}


	inline bool Monitor::builtin() const
	{
		return pBuiltin;
	}




} // namespace Display
} // namespace Device
} // namespace Yuni
