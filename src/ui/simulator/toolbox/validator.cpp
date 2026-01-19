// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "validator.h"

namespace Antares::Toolbox::Validator
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
        {
            l.Add(wxString::FromAscii(c));
        }
        for (char c = 'A'; c <= 'Z'; ++c)
        {
            l.Add(wxString::FromAscii(c));
        }
        for (char c = '0'; c <= '9'; ++c)
        {
            l.Add(wxString::FromAscii(c));
        }
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
        {
            l.Add(wxString::FromAscii(c));
        }
        l.Add(wxT('-'));
        l.Add(wxT('.'));
    }
    validator.SetIncludes(l);
    return validator;
}

} // namespace Antares::Toolbox::Validator
