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
#include "md5.h"



namespace Yuni
{
namespace Hash
{
namespace Checksum
{

	inline String MD5::FromString(const String& s)
	{
		return MD5().fromString(s);
	}


	inline String MD5::FromRawData(const void* rawdata, uint64 size)
	{
		return MD5().fromRawData(rawdata, size);
	}




} // namespace Checksum
} // namespace Hash
} // namespace Yuni
