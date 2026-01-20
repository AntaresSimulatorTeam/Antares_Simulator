// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <yuni/yuni.h>
#include "local-frame.h"
#include "registry.h"

using namespace Yuni;

namespace Antares::Component::Frame
{
bool WxLocalFrame::Destroy()
{
    // the earlier the better
    Registry::UnregisterFrame(this);
    return wxFrame::Destroy();
}

WxLocalFrame::WxLocalFrame()
{
    Registry::RegisterFrame(this);
}

WxLocalFrame::WxLocalFrame(wxWindow* parent,
                           wxWindowID id,
                           const wxString& title,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name):
    wxFrame(parent, id, title, pos, size, style, name),
    IFrame()
{
    Registry::RegisterFrame(this);
}

WxLocalFrame::~WxLocalFrame()
{
    // Just in case
    Registry::UnregisterFrame(this);
}

bool WxLocalFrame::excludeFromMenu()
{
    return false;
}

void WxLocalFrame::updateOpenWindowsMenu()
{
}

void WxLocalFrame::SetTitle(const wxString& title)
{
    // Title
    wxFrame::SetTitle(title);
    // Notifying update
    Registry::DispatchUpdate();
}

void WxLocalFrame::frameRaise()
{
    Raise();
}

String WxLocalFrame::frameTitle() const
{
    String title;
    wxStringToString(GetTitle(), title);
    return title;
}

int WxLocalFrame::frameID() const
{
    return GetId();
}

} // namespace Antares::Component::Frame
