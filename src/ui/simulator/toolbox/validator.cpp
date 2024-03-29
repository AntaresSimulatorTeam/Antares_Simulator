/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
