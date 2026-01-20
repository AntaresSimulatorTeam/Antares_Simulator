// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "group.h"

namespace Antares::Component::HTMLListbox::Item
{
Group::Group(const wxString& text):
    pText(text)
{
}

Group::~Group()
{
}

wxString Group::htmlContent(const wxString&)
{
    return wxString() << wxT(
             "<table cellpadding=2><tr><td width=10></td><td><font size=\"-1\" color=\"")
                      << wxT("black") << wxT("\"><b>") << pText
                      << wxT("</b></font></td></tr></table>");
}

} // namespace Antares::Component::HTMLListbox::Item
