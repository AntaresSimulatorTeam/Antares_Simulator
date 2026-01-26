// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "mini-frame.h"
#include "../../dispatcher/gui.h"
#include <wx/timer.h>
#include <antares/logs/logs.h>
#include <wx/app.h>
#include <wx/sizer.h>

using namespace Yuni;

namespace Antares::Private::Spotlight
{
namespace // anonymous
{
// Global pointer for the frame
// We may only have one opened frame for the whole process
static SpotlightMiniFrame* gInstance = nullptr;

class AutoCloseTimer: public wxTimer
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
        {
            Antares::Dispatcher::GUI::Close(gInstance);
        }
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

SpotlightMiniFrame::SpotlightMiniFrame(wxWindow* parent):
    wxFrame(parent,
            wxID_ANY,
            wxT("Selection"),
            wxDefaultPosition,
            wxDefaultSize,
            wxSTAY_ON_TOP | wxBORDER_NONE | wxFRAME_NO_TASKBAR)
{
    SetSize(340, 300);

    if (gInstance && gInstance != this)
    {
        gInstance->Close();
    }
    gInstance = this;

    if (!gAutoCloseTimer)
    {
        gAutoCloseTimer = new AutoCloseTimer();
    }

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
    {
        return;
    }
    removeRefToMySelf();
    Antares::Dispatcher::GUI::Close(this);
}

} // namespace Antares::Private::Spotlight
