// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENTS_SCROLLBOX_H__
#define __ANTARES_TOOLBOX_COMPONENTS_SCROLLBOX_H__

#include <wx-wrapper.h>
#include <wx/scrolwin.h>

namespace Antares::Component
{
class Scrollbox: public wxScrolledWindow
{
public:
    Scrollbox();
    Scrollbox(wxWindow* parent,
              int id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);

    virtual ~Scrollbox();

private:
    DECLARE_DYNAMIC_CLASS(Scrollbox)
    DECLARE_EVENT_TABLE()

}; // class Scrollbox

} // namespace Antares::Component

#endif // __ANTARES_TOOLBOX_COMPONENTS_SCROLLBOX_H__
