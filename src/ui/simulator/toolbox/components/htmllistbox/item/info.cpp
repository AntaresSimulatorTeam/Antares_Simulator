// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "info.h"

namespace Antares::Component::HTMLListbox::Item
{
Info::Info(const wxString& text):
    pText(text)
{
}

Info::~Info()
{
}

wxString Info::htmlContent(const wxString&)
{
    return wxString() << wxT("<table cellpadding=10><tr><td><font size=\"-1\" color=\"")
                      << wxT("#888888") << wxT("\"><i>") << pText
                      << wxT("</i></font></td></tr></table>");
}

} // namespace Antares::Component::HTMLListbox::Item
