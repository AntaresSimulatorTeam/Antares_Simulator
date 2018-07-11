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
#include "checksum.h"



namespace Yuni
{
namespace Hash
{
namespace Checksum
{


	inline void IChecksum::reset()
	{
		pValue.clear();
	}


	inline const String& IChecksum::fromString(const String& s)
	{
		return fromRawData(s.data(), s.size());
	}


	inline const String& IChecksum::operator[] (const String& s)
	{
		fromString(s);
		return pValue;
	}


	inline const String& IChecksum::value() const
	{
		return pValue;
	}


	inline const String& IChecksum::operator() () const
	{
		return pValue;
	}



} // namespace Checksum
} // namespace Hash
} // namespace Yuni
