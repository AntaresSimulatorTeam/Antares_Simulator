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

#include "wx-wrapper.h"
#include <wx/window.h>
#include <wx/frame.h>

using namespace Yuni;

#ifdef YUNI_OS_MSVC
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

wxString DoubleToWxString(double f, uint precision)
{
    static const char* const formats[]
      = {"%0.f", "%1.f", "%2.f", "%3.f", "%4.f", "%5.f", "%6.f", "%7.f", "%8.f", "%9.f"};
    char tmp[128];
    if (precision < 10)
        SNPRINTF(tmp, sizeof(tmp), formats[precision], f);
    else
        SNPRINTF(tmp, sizeof(tmp), "%.f", f);

    AnyString adapter = (const char*)tmp;
    if (adapter.contains('.'))
    {
        CString<128, false> s = adapter;
        s.trimRight('0');
        if (s.last() == '.')
            s.removeLast();
        return wxStringFromUTF8(s);
    }
    return wxStringFromUTF8(adapter);
}

wxString DoubleToWxString(double f)
{
    CString<128, false> tmp;
    tmp = f;
    if (tmp.contains('.'))
    {
        tmp.trimRight('0');
        if (tmp.last() == '.')
            tmp.removeLast();
    }
    return wxStringFromUTF8(tmp);
}

double wxStringToDouble(const wxString& s, double defValue)
{
    double d;
    if (!s.ToDouble(&d))
        return defValue;
    return d;
}

wxWindow* wxFindFrameParent(wxWindow* control)
{
    while (control)
    {
        if (dynamic_cast<wxFrame*>(control))
            return control;
        control = control->GetParent();
    }
    return nullptr;
}
