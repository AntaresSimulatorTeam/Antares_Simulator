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
#include "escape.h"



namespace Yuni
{


	template<class StringT>
	void AppendEscapedString(StringT& out, const AnyString& string, char quote)
	{
		if (not string.empty())
		{
			char escapequote[2] = {'\\', quote};
			uint offset = 0;
			do
			{
				uint pos = string.find(quote, offset);
				if (pos >= string.size())
				{
					out.append(string.c_str() + offset, string.size() - offset);
					break;
				}

				out.append(string.c_str() + offset, pos - offset);
				out.append(escapequote, 2);
				offset = pos + 1;
			}
			while (offset < string.size());
		}
		else
		{
			out += string;
		}
	}



} // namespace Yuni
