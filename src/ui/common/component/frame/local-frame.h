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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__

#include <yuni/yuni.h>
#include "../../wx-wrapper.h"
#include <wx/frame.h>
#include "frame.h"

namespace Antares
{
namespace Component
{
namespace Frame
{
class WxLocalFrame : public wxFrame, public IFrame
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

} // namespace Frame
} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_LOCAL_FRAME_H__
