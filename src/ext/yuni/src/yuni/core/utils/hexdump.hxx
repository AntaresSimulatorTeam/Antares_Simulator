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
#include "hexdump.h"



namespace Yuni
{
namespace Core
{
namespace Utils
{

	inline Hexdump::Hexdump(const char* buffer, uint size)
		: pBuffer(buffer), pSize(size)
	{}


	inline Hexdump::Hexdump(const Hexdump& rhs)
		: pBuffer(rhs.pBuffer), pSize(rhs.pSize)
	{}


	template<class U>
	inline Hexdump::Hexdump(const U& buffer)
		: pBuffer((const char *)buffer.data()), pSize(buffer.sizeInBytes())
	{}


	template <class U>
	void Hexdump::dump(U& stream) const
	{
		Yuni::String line;
		uint printed;
		uint remains = pSize;

		for (printed = 0; printed <	pSize; printed += 0x10)
		{
			remains = pSize - printed;

			// Print the line's address
			line.appendFormat("%08.8p: ", (pBuffer + printed));

			// Print the next 16 bytes (or less) in hex.
			dumpHexadecimal(line, pBuffer + printed, (remains > 0x10) ? 0x10 : remains);

			// Print the next 16 bytes (or less) in printable chars.
			dumpPrintable(line, pBuffer + printed, (remains > 0x10) ? 0x10 : remains);

			// Add the position in the buffer, padded to 2 bytes.
			line.appendFormat(" %04.4x-%04.4x\n", printed, printed + 0x0f);

			// Put the line in the stream
			stream << line;
			line.clear();
		}
	}


	inline String Hexdump::dump() const
	{
		String s;
		dump(s);
		return s;
	}



} // namespace Utils
} // namespace Core
} // namespace Yuni




inline std::ostream& operator<< (std::ostream& outStream, const Yuni::Core::Utils::Hexdump& hexDumper)
{
	hexDumper.dump(outStream);
	return outStream;
}
