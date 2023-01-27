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

#include "mini-frame.h"
#include "../../dispatcher/gui.h"
#include <wx/timer.h>
#include <antares/logs.h>
#include <wx/app.h>
#include <wx/sizer.h>

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace Spotlight
{
namespace // anonymous
{
// Global pointer for the frame
// We may only have one opened frame for the whole process
static SpotlightMiniFrame* gInstance = nullptr;

class AutoCloseTimer : public wxTimer
{
public:
    AutoCloseTimer()
    {
    }
    virtual ~AutoCloseTimer()
    {
    }

    void Notify()
    {
        // Thanks wx !!! (ironic of course...)
        if (gInstance && not gInstance->IsActive())
            Antares::Dispatcher::GUI::Close(gInstance);
    }

}; // class AutoCloseTimer

static AutoCloseTimer* gAutoCloseTimer = nullptr;

} // anonymous namespace

BEGIN_EVENT_TABLE(SpotlightMiniFrame, wxFrame)
EVT_CLOSE(SpotlightMiniFrame::onClose)
EVT_KILL_FOCUS(SpotlightMiniFrame::onKillFocus)
END_EVENT_TABLE()

SpotlightMiniFrame* SpotlightMiniFrame::Instance()
{
    return gInstance;
}

SpotlightMiniFrame::SpotlightMiniFrame(wxWindow* parent) :
 wxFrame(parent,
         wxID_ANY,
         wxT("Selection"),
         wxDefaultPosition,
         wxDefaultSize,
         wxSTAY_ON_TOP | wxBORDER_NONE | wxFRAME_NO_TASKBAR)
{
    SetSize(340, 300);

    if (gInstance && gInstance != this)
        gInstance->Close();
    gInstance = this;

    if (!gAutoCloseTimer)
        gAutoCloseTimer = new AutoCloseTimer();

    gAutoCloseTimer->Start(160, wxTIMER_CONTINUOUS);
}

SpotlightMiniFrame::~SpotlightMiniFrame()
{
    removeRefToMySelf();
}

void SpotlightMiniFrame::removeRefToMySelf()
{
    if (gInstance == this)
    {
        gInstance = nullptr;
        if (gAutoCloseTimer)
        {
            gAutoCloseTimer->Stop();
            Antares::Dispatcher::GUI::Destroy(gAutoCloseTimer);
            gAutoCloseTimer = nullptr;
        }
    }
}

void SpotlightMiniFrame::onClose(wxCloseEvent&)
{
    removeRefToMySelf();
    Destroy();
}

void SpotlightMiniFrame::onKillFocus(wxFocusEvent& evt)
{
    evt.Skip();
    if (IsActive())
        return;
    removeRefToMySelf();
    Antares::Dispatcher::GUI::Close(this);
}

} // namespace Spotlight
} // namespace Private
} // namespace Antares
