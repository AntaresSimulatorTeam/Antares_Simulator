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
