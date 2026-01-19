// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


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
