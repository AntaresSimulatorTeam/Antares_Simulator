// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <wx/intl.h>
#include <iostream>
#include <antares/logs/logs.h>

namespace Antares::Locale
{
char DecimalPoint = '.';

void Init()
{
    DecimalPoint = '.';
    wxString s = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
    if (!s.empty())
    {
        if (s[0] == wxT(','))
        {
            DecimalPoint = ',';
        }
        if (s[0] == wxT('.'))
        {
            DecimalPoint = '.';
        }
    }
}

} // namespace Antares::Locale
