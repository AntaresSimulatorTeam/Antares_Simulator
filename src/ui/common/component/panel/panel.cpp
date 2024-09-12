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

#include "panel.h"
#include "../../lock.h"
#include <wx/sizer.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
static Panel* gLastWindowUnderMouse = nullptr;

Panel::Panel(wxWindow* parent) :
 wxPanel(parent,
         wxID_ANY,
         wxDefaultPosition,
         wxDefaultSize,
         wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxFULL_REPAINT_ON_RESIZE)
{
    assert(parent && "invalid parent");

    Connect(GetId(), wxEVT_MOTION, wxMouseEventHandler(Panel::onInternalMotion), nullptr, this);
    Connect(GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler(Panel::onInternalMouseDown), nullptr, this);
    Connect(GetId(), wxEVT_LEFT_UP, wxMouseEventHandler(Panel::onInternalMouseUp), nullptr, this);
}

Panel::~Panel()
{
    if (gLastWindowUnderMouse == this)
        gLastWindowUnderMouse = nullptr;

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Panel::onInternalMouseDown(wxMouseEvent& evt)
{
    onMouseDown(evt);
}

void Panel::onInternalMouseUp(wxMouseEvent& evt)
{
    if (GUIIsLock())
        return;

    onMouseUp(evt);

    if (triggerMouseClickEvent())
    {
        // Delay the click
        Yuni::Bind<void()> callback;
        callback.bind(this, &Panel::onMouseClick);
        Dispatcher::GUI::Post(callback, 50);
    }
}

void Panel::OnMouseMoveFromExternalComponent()
{
    if (gLastWindowUnderMouse && not GUIIsLock())
    {
        gLastWindowUnderMouse->onMouseLeave();
        gLastWindowUnderMouse = nullptr;
    }
}

void Panel::onInternalMotion(wxMouseEvent& evt)
{
    if (this == gLastWindowUnderMouse || GUIIsLock())
        return;
    if (gLastWindowUnderMouse)
        gLastWindowUnderMouse->onMouseLeave();
    gLastWindowUnderMouse = this;
    gLastWindowUnderMouse->onMouseEnter();

    const auto& position = evt.GetPosition();
    onMouseMoved(position.x, position.y);
}

} // namespace Component
} // namespace Antares
