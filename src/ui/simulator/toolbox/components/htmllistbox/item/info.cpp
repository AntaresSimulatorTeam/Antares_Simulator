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
#include "info.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
Info::Info(const wxString& text) : pText(text)
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

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
