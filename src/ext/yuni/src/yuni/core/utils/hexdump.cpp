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
#include "hexdump.h"

namespace Yuni
{
namespace Core
{
namespace Utils
{


	void Hexdump::dumpHexadecimal(String& line, const char* buffer, uint size) const
	{
		for (uint printed = 0; printed < 0x10; ++printed)
		{
			if (printed < size)
				line.appendFormat("%02x", (uint) (*(unsigned char *)(buffer + printed)));
			else
				line.append("  ");

			if (0 != printed % 2)
				line.append(' ');
		}
	}


	void Hexdump::dumpPrintable(String& line, const char* buffer, uint size) const
	{
		uint printed;

		line.append('|');
		for (printed = 0; printed < size; ++printed)
		{
			char c = *(buffer + printed);
			if (c < 0x20 or c > 0x7E) // c is not printable ASCII, replace it by a dot '.'
				c = '.';
			line.append(c);
		}
		line.append('|');
		while (printed++ < 0x10)
			line.append(' ');
	}


	Hexdump& Hexdump::operator = (const Hexdump& rhs)
	{
		if (this != &rhs)
		{
			this->pBuffer = rhs.pBuffer;
			this->pSize = rhs.pSize;
		}
		return *this;
	}




} // namespace Utils
} // namespace Core
} // namespace Yuni

