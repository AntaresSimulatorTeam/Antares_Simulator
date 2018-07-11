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
#include "complex.h"


namespace Yuni
{
namespace Private
{
namespace IStringImpl
{


	// std::complex
	template<class MemBufT, class U>
	struct YUNI_DECL Append<MemBufT, std::complex<U> > final
	{
		static void Do(MemBufT& memoryBuffer, const std::complex<U>& rhs)
		{
			memoryBuffer << rhs.real() << ',' << rhs.imag();
		}
	};




} // namespace IStringImpl
} // namespace Private
} // namespace Yuni
