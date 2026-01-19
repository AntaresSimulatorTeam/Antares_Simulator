// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/frame.h>
#include "frame.h"

namespace Antares::Component::Frame
{
class WxLocalFrame: public wxFrame, public IFrame
{
public:
    WxLocalFrame();
    WxLocalFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxT("registered-frame"));

    virtual ~WxLocalFrame();

    virtual bool Destroy() override;

    virtual bool excludeFromMenu() override;
    virtual void updateOpenWindowsMenu() override;
    virtual void frameRaise() override;
    virtual Yuni::String frameTitle() const override;
    virtual int frameID() const override;

    // Override Frame title
    virtual void SetTitle(const wxString& title) override;

}; // class WxLocalFrame

} // namespace Antares::Component::Frame

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
