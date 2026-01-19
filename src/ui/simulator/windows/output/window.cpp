// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "window.h"
#include <wx/sizer.h>
#include <wx/icon.h>
#include "../../toolbox/resources.h"
#include <ui/common/dispatcher/gui.h>

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares::Window::OutputViewer
{
BEGIN_EVENT_TABLE(Window, wxFrame)
EVT_CLOSE(Window::onClose)
END_EVENT_TABLE()

Window::Window(wxFrame* parent, const OutputViewer::Component* viewer):
    Antares::Component::Frame::WxLocalFrame(parent,
                                            wxID_ANY,
                                            wxT("Output viewer"),
                                            wxDefaultPosition,
                                            wxSize(1000, 600),
                                            wxCAPTION | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU
                                              | wxRESIZE_BORDER | wxCLIP_CHILDREN)
{
    wxIcon icon(Resources::WxFindFile("icons/study.ico"), wxBITMAP_TYPE_ICO);
    SetIcon(icon);

    // The main sizer
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* newviewer = new OutputViewer::Component(this, true);
    newviewer->updateLayerList();
    sizer->Add(newviewer, 1, wxALL | wxEXPAND);

    if (viewer)
    {
        newviewer->copyFrom(*viewer);
    }

    auto study = GetCurrentStudy();
    if (!(!study))
    {
        SetTitle(wxString(wxT("Output: ")) << wxStringFromUTF8(study->header.caption));
    }

    // Content
    SetSizer(sizer);
    sizer->Layout();
}

Window::~Window()
{
    // Destroy all components before
    if (GetSizer())
    {
        GetSizer()->Clear(true);
    }
}

void Window::onClose(wxCloseEvent&)
{
    if (GetSizer())
    {
        GetSizer()->Clear(true);
    }
    Dispatcher::GUI::Destroy(this);
}

} // namespace Antares::Window::OutputViewer
