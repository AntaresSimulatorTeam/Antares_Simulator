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
#include "array.h"
#include <iostream>


namespace Yuni
{
namespace Bit
{

	namespace // anonymous
	{

		template<bool ValueT>
		static inline uint Find(const Bit::Array::BufferType& pBuffer, uint pCount, uint offset)
		{
			// bitmask
			static const uchar mask[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
			// alias to npos
			enum { npos = Yuni::Bit::Array::npos };

			for (uint i = (offset >> 3); i < pBuffer.size(); ++i)
			{
				if (static_cast<uchar>(pBuffer[i]) != (ValueT ? static_cast<uchar>(0) : static_cast<uchar>(0xFF)))
				{
					const uchar c = static_cast<uchar>(pBuffer[i]);

					// trivial cases
					if (ValueT and c == 0xFF)
					{
						uint p = i * 8;
						p = (p < offset) ? offset : p;
						return (p < pCount) ? p : npos;
					}
					if (not ValueT and c == 0)
					{
						uint p = i * 8;
						p = (p < offset) ? offset : p;
						return (p < pCount) ? p : npos;
					}

					// the current absolute position
					const uint absOffset = i * 8;

					// bit scan
					if (absOffset < offset)
					{
						for (uint p = 0; p != 8; ++p)
						{
							if (ValueT == (0 != (c & mask[p])))
							{
								p += absOffset;
								if (p >= offset)
									return (p < pCount) ? p : npos;
								// restoring previous value
								p -= absOffset;
							}
						}
					}
					else
					{
						// TODO : we can use ffs here
						for (uint p = 0; p != 8; ++p)
						{
							if (ValueT == (0 != (c & mask[p])))
							{
								p += absOffset;
								return (p < pCount) ? p : npos;
							}
						}

					}
				}
			}
			return npos;
		}

	} // anonymous namespace



	template<> uint Array::find<true>(uint offset) const
	{
		return Find<true>(pBuffer, pCount, offset);
	}


	template<> uint Array::find<false>(uint offset) const
	{
		return Find<false>(pBuffer, pCount, offset);
	}



	bool Array::any() const
	{
		// try to find a byte not null
		for (uint i = 0; i != pBuffer.size(); ++i)
		{
			if (pBuffer[i] != 0)
				return true;
		}
		return false;
	}


	bool Array::none() const
	{
		for (uint i = 0; i != pBuffer.size(); ++i)
		{
			if (pBuffer[i] != 0)
				return false;
		}
		return true;
	}


	bool Array::all() const
	{
		for (uint i = 0; i != pBuffer.size(); ++i)
		{
			if (static_cast<uchar>(pBuffer[i]) != 0xFF)
				return false;
		}
		return true;
	}




} // namespace Bit
} // namespace Yuni

