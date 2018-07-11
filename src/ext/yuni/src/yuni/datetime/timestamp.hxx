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
#include <cstdlib>
#include "timestamp.h"



namespace Yuni
{
namespace Private
{
namespace DateTime
{

	// forward declaration
	char* FormatTimestampToString(const AnyString& format, sint64 timestamp);


} // namespace DateTime
} // namespace Private
} // namespace Yuni



namespace Yuni
{
namespace DateTime
{

	template<class StringT>
	inline bool TimestampToString(StringT& out, const AnyString& format, Timestamp timestamp, bool emptyBefore)
	{
		if (emptyBefore)
			out.clear();
		if (format.empty())
			return true;

		char* buffer = Yuni::Private::DateTime::FormatTimestampToString(format, timestamp);
		if (buffer)
		{
			out += (const char*) buffer;
			::free(buffer);
			return true;
		}
		return false;
	}




} // namespace DateTime
} // namespace Yuni
