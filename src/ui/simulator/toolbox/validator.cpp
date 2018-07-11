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

#include "validator.h"



namespace Antares
{
namespace Toolbox
{
namespace Validator
{

	wxTextValidator Default()
	{
		wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
		static wxArrayString l;
		if (l.empty())
		{
			// Find a better way for doing this.
			// Actually it seems impossible to have wxFILTER_ALPHANUMERIC and
			// wxFILTER_INCLUDE_LIST in the same time using a `|`.
			for (char c = 'a'; c <= 'z'; ++c)
				l.Add(wxString::FromAscii(c));
			for (char c = 'A'; c <= 'Z'; ++c)
				l.Add(wxString::FromAscii(c));
			for (char c = '0'; c <= '9'; ++c)
				l.Add(wxString::FromAscii(c));
			l.Add(wxT(' '));
			l.Add(wxT('-'));
			l.Add(wxT('_'));
			l.Add(wxT('.'));
		}
		validator.SetIncludes(l);
		return validator;
	}


	wxTextValidator Numeric()
	{
		wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
		static wxArrayString l;
		if (l.empty())
		{
			// Find a better way for doing this.
			// Actually it seems impossible to have wxFILTER_ALPHANUMERIC and
			// wxFILTER_INCLUDE_LIST in the same time using a `|`.
			for (char c = '0'; c <= '9'; ++c)
				l.Add(wxString::FromAscii(c));
			l.Add(wxT('-'));
			l.Add(wxT('.'));
		}
		validator.SetIncludes(l);
		return validator;
	}





} // namespace Validator
} // namespace Toolbox
} // namespace Antares


