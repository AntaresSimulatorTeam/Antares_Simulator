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
#include "resolution.h"
#include "../../core/hash/checksum/md5.h"
#include "../../core/math.h"
#include <cassert>
#include <iostream>

namespace Yuni
{
namespace Device
{
namespace Display
{

	Resolution::Resolution(const Resolution& rhs)
		: pWidth(rhs.pWidth)
		, pHeight(rhs.pHeight)
		, pBitsPerPixel(rhs.pBitsPerPixel)
	{}


	Resolution::Resolution(uint width, uint height, uint8 bits)
	{
		assert(width  <= (uint) maximumWidth
			and "display resolution: width greater than Device::Display::Resolution::maximumWidth");
		assert(height <= (uint) maximumHeight
			and "display resolution: height greater than Device::Display::Resolution::maximumHeight");

		pBitsPerPixel = (32 == bits ||  24 == bits || 16 == bits || 8 == bits)
			? (uint8) bits : (uint8)32;
		pWidth  = Math::MinMax<uint>(width,  minimumWidth,  maximumWidth);
		pHeight = Math::MinMax<uint>(height, minimumHeight, maximumHeight);
	}


	String Resolution::toString() const
	{
		String ret;
		ret << pWidth << 'x' << pHeight;
		if (pBitsPerPixel)
		  ret << " (" << (int)pBitsPerPixel << "Bits)";
		return ret;
	}


	bool Resolution::operator < (const Resolution& rhs) const
	{
		if (pWidth < rhs.pWidth)
			return true;

		if (pWidth == rhs.pWidth)
		{
			if (pHeight < rhs.pHeight)
				return true;
			if (pHeight == rhs.pHeight)
				return (pBitsPerPixel < rhs.pBitsPerPixel);
		}
		return false;
	}


	bool Resolution::operator > (const Resolution& rhs) const
	{
		if (pWidth > rhs.pWidth)
			return true;

		if (pWidth == rhs.pWidth)
		{
			if (pHeight > rhs.pHeight)
				return true;
			if (pHeight == rhs.pHeight)
				return (pBitsPerPixel > rhs.pBitsPerPixel);
		}
		return false;
	}


	std::ostream& Resolution::print(std::ostream& out) const
	{
	  out << pWidth << "x" << pHeight;
	  if (pBitsPerPixel)
	    out << " (" << (int) pBitsPerPixel << "Bits)";
	  return out;
	}


	Resolution& Resolution::operator = (const Resolution& p)
	{
		pWidth = p.pWidth;
		pHeight = p.pHeight;
		pBitsPerPixel = p.pBitsPerPixel;
		return (*this);
	}




} // namespace Display
} // namespace Device
} // namespace Yuni

