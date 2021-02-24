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

#include "scrollbox.h"
#include <wx/sizer.h>

namespace Antares
{
namespace Component
{
IMPLEMENT_DYNAMIC_CLASS(Scrollbox, wxScrolledWindow)

BEGIN_EVENT_TABLE(Scrollbox, wxScrolledWindow)
END_EVENT_TABLE()

Scrollbox::Scrollbox()
{
}

Scrollbox::Scrollbox(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size) :
 wxScrolledWindow(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    SetAutoLayout(true);
}

Scrollbox::~Scrollbox()
{
    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

} // namespace Component
} // namespace Antares
