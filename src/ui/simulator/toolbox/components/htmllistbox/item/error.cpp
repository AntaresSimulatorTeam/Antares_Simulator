/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "error.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
Error::Error(const wxString& text, bool warning) : pText(text), pWarning(warning)
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

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
