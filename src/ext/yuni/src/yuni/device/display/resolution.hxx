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
#include "resolution.h"


namespace Yuni
{
namespace Device
{
namespace Display
{

	inline uint Resolution::width() const
	{
		return pWidth;
	}


	inline uint Resolution::height() const
	{
		return pHeight;
	}


	inline uint8 Resolution::bitPerPixel() const
	{
		return pBitsPerPixel;
	}


	inline bool Resolution::operator == (const Resolution& rhs) const
	{
		return rhs.pWidth == pWidth and rhs.pHeight == pHeight
			and rhs.pBitsPerPixel == pBitsPerPixel;
	}


	inline bool Resolution::operator != (const Resolution& rhs) const
	{
		return !(*this == rhs);
	}


	inline bool Resolution::operator <= (const Resolution& rhs) const
	{
		return pWidth <= rhs.pWidth and pHeight <= rhs.pHeight
			and pBitsPerPixel <= rhs.pBitsPerPixel;
	}


	inline bool Resolution::operator >= (const Resolution& rhs) const
	{
		return pWidth >= rhs.pWidth and pHeight >= rhs.pHeight
			and pBitsPerPixel >= rhs.pBitsPerPixel;
	}





} // namespace Display
} // namespace Device
} // namespace Yuni
