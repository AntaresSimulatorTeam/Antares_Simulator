// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "error.h"

namespace Antares::Component::HTMLListbox::Item
{
Error::Error(const wxString& text, bool warning):
    pText(text),
    pWarning(warning)
{
}

Error::~Error()
{
}

wxString Error::htmlContent(const wxString&)
{
    return wxString() << wxT("<table border=0 cellpadding=0 cellspacing=0 width=\"100%\"><tr>")
                      << wxT("<td width=5></td><td width=16 bgcolor=\"")
                      << (pWarning ? wxT("#FF953E") : wxT("#FF0000"))
                      << wxT("\" align=center nowrap><font size=\"-3\" color=\"#FFFFFF\">&nbsp;")
                      << (pWarning ? wxT("warning") : wxT("error"))
                      << wxT("&nbsp;</font></td><td width=8></td><td><font size=\"-1\">") << pText
                      << wxT("</font></td></tr></table>");
}

} // namespace Antares::Component::HTMLListbox::Item
