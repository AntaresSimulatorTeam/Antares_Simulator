/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "utils.h"

using namespace Yuni;



namespace Antares
{

	void BeautifyName(YString& out, AnyString oldname)
	{
		out.clear();
		if (oldname.empty())
			return;

		oldname.trim(" \r\n\t");
		if (oldname.empty())
			return;

		out.reserve(oldname.size());

		auto end = oldname.utf8end();
		for (auto i = oldname.utf8begin(); i != end; ++i)
		{
			auto& utf8char = *i;
			// simple char
			char c = (char) utf8char;

			if (c == ' '
				or (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or (c >= '0' and c <= '9')
				or c == '_' or c == '-' or c == '(' or c == ')' or c == ',' or c == '&')
			{
				out += c;
			}
			else
				out += ' ';
		}

		out.trim(" \t\r\n");

		while (std::string(out.c_str()).find("  ") != std::string::npos)
			out.replace("  ", " ");

		out.trim(" \t\r\n");
	}




} // namespace Antares

