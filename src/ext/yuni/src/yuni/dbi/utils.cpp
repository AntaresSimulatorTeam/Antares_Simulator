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
#include "utils.h"


namespace Yuni
{
namespace DBI
{


	bool IsValidIdentifier(AnyString text)
	{
		// removing all blanks / spaces
		text.trim();

		if (YUNI_UNLIKELY(text.empty() or text.size() > 256))
			return false;

		// checking each char individually
		for (uint i = 0; i != text.size(); ++i)
		{
			char c = text[i];
			if (YUNI_UNLIKELY(not String::IsAlpha(c) and not String::IsDigit(c) and c != '_' and c != '.'))
				return false;
		}

		// invalid start / end
		if (YUNI_UNLIKELY(text.first() == '.' or text.last() == '.'))
			return false;

		return true;
	}




} // namespace DBI
} // namespace Yuni

