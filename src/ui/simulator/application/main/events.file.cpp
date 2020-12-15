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

#include "main.h"
#include "../study.h"
#include "../recentfiles.h"
#include "../menus.h"
#include "../../windows/saveas.h"
#include "../../windows/exportmap.h"
#include "../../windows/message.h"
#include "internal-data.h"
#include "internal-ids.h"
#include "../../toolbox/dispatcher/study.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include <ui/common/lock.h>

//#include <wx/utils.h>
#include <wx/dirdlg.h>

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>

#include <ui/common/component/panel.h>
#include <ui/common/component/spotlight.h>
#include <ui/common/component/frame/registry.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
class ItemWindow : public Component::Spotlight::IItem
{
public:
    //! Smart ptr
    typedef Yuni::SmartPtr<ItemWindow> Ptr;

    explicit ItemWindow(const Yuni::String& title, int id) : title(title), id(id)
    {
        caption(title);
    };

public:
    Yuni::String title;
    int id;
};

class SpotlightProviderWindows : public Component::Spotlight::IProvider
{
public:
    typedef Antares::Component::Spotlight Spotlight;

public:
    SpotlightProviderWindows()
    {
    }

    virtual ~SpotlightProviderWindows()
    {
    }

    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        const Yuni::String& text = "")
    {
        auto& framelist = Component::Frame::Registry::List();
        int elemIdx = -1;

        foreach (auto* frame, framelist)
        {
            ++elemIdx;
            if (frame->excludeFromMenu())
                continue; // -> framelist foreach

            if (tokens.empty())
            {
                out.push_back(new ItemWindow(frame->frameTitle(), elemIdx));
            }
            else
            {
                for (uint si = 0; si != tokens.size(); ++si)
                {
                    if (frame->frameTitle().icontains(tokens[si]->text))
                    {
                        out.push_back(new ItemWindow(frame->frameTitle(), elemIdx));
                        break; // -> tokens foreach
                    }
                } // tokens foreach
            }
        } // framelist foreach
    }

    virtual bool onSelect(Spotlight::IItem::Ptr& item)
    {
        ItemWindow::Ptr itemwin = Spotlight::IItem::Ptr::DynamicCast<ItemWindow::Ptr>(item);
        if (!itemwin)
            return false;

        auto& framelist = Component::Frame::Registry::List();
        if ((uint)itemwin->id < framelist.size())
        {
            auto* frame = framelist[itemwin->id];
            if (frame->frameTitle() == itemwin->title)
            {
                frame->frameRaise();
                return true;
            }
        }

        ::wxMessageBox(wxT("Invalid Window, it may have already been closed."),
                       wxT("Invalid Window"),
                       wxOK | wxICON_ERROR);
        return true;
    }

    virtual bool onSelect(const Spotlight::IItem::Vector&)
    {
        return true;
    }

}; // class SpotlightProviderWindows

class RaiseWindowBox : public wxDialog
{
public:
    RaiseWindowBox(wxWindow* parent);
    virtual ~RaiseWindowBox()
    {
    }

private:
    //! Event: the user requested to close the window
    void onClose(void*);

}; // class RaiseWindowBox

RaiseWindowBox::RaiseWindowBox(wxWindow* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Raise Window Box"),
          wxDefaultPosition,
          wxDefaultSize,
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    // Informations about the study
    wxColour defaultBgColor = GetBackgroundColour();
    SetBackgroundColour(wxColour(255, 255, 255));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->AddSpacer(10);

    {
        Component::Spotlight* spotlight = new Component::Spotlight(this, 0);
        spotlight->provider(new SpotlightProviderWindows());
        wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
        hz->AddSpacer(5);
        hz->Add(spotlight, 1, wxALL | wxEXPAND);
        hz->AddSpacer(5);
        sizer->Add(hz, 1, wxALL | wxEXPAND);
    }

    sizer->AddSpacer(5);

    sizer->Add(new wxStaticLine(this), 0, wxALL | wxEXPAND);

    auto* panel = new Antares::Component::Panel(this);
    wxBoxSizer* sizerBar = new wxBoxSizer(wxHORIZONTAL);
    sizerBar->AddStretchSpacer();
    panel->SetSizer(sizerBar);
    panel->SetBackgroundColour(defaultBgColor);
    wxPanel* pPanel = panel;
    wxBoxSizer* pPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    sizerBar->Add(pPanelSizer, 0, wxALL | wxEXPAND, 8);
    sizerBar->Add(15, 5);

    sizer->AddSpacer(50);
    sizer->Add(new wxStaticLine(this), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    sizer->Layout();
    SetSizer(sizer);

    // Close button
    {
        auto* btn
          = Component::CreateButton(pPanel, wxT("   Close   "), this, &RaiseWindowBox::onClose);
        pPanelSizer->Add(btn, 0, wxFIXED_MINSIZE | wxALIGN_CENTRE_VERTICAL | wxALL);
        pPanelSizer->Add(5, 2);
        btn->SetDefault();
        btn->SetFocus();
    }

    GetSizer()->Fit(this);
    wxSize p = GetSize();
    p.SetWidth(p.GetWidth() + 20);
    if (p.GetWidth() < 390)
        p.SetWidth(390);
    else
    {
        if (p.GetWidth() > 600)
            p.SetWidth(600);
    }
    p.SetHeight(400);
    SetSize(p);

    Centre(wxBOTH);
}

void RaiseWindowBox::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

} // namespace Window
} // namespace Antares

// ----- END Window Menu others management -----

namespace Antares
{
namespace Forms
{
static inline bool IsCurrentStudyReadonly()
{
    auto study = Data::Study::Current::Get();
    return !(!study) && study->parameters.readonly;
}

wxString ApplWnd::openStudyFolder(bool autoLoad)
{
    Forms::Disabler<ApplWnd> disabler(*this);

    wxString folder;
    if (gLastOpenedStudyFolder.IsEmpty())
        wxGetHomeDir(&folder); // Home sweet home
    else
        folder = gLastOpenedStudyFolder;

    // The dialog window
    auto* dlg = new wxDirDialog(
      this, wxT("Open a study"), folder, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (wxID_OK == dlg->ShowModal())
    {
        // It is quite better to delete the windows before loading the study
        const wxString path = dlg->GetPath();
        delete dlg;

        if (!path.empty())
        {
            if (!wouldYouLikeToSaveTheStudy())
                return wxEmptyString;
            // Load the study
            if (autoLoad)
            {
                String t;
                wxStringToString(path, t);
                Dispatcher::StudyOpen(t, true);
            }
        }
        return path;
    }
    delete dlg;
    return wxEmptyString;
}

void ApplWnd::evtOnNewStudy(wxCommandEvent&)
{
    if (GUIIsLock())
        return;
    Dispatcher::StudyNew();
}

void MainFormData::onToolbarNewStudy(void*)
{
    Dispatcher::StudyNew();
}

void ApplWnd::evtOnOpenStudy(wxCommandEvent&)
{
    if (GUIIsLock())
        return;
    openStudyFolder();
}

void MainFormData::onToolbarOpenLocalStudy(void*)
{
    pMainForm.openStudyFolder();
}

void ApplWnd::evtOnSave(wxCommandEvent&)
{
    if (GUIIsLock())
        return;
    if (Data::Study::Current::Valid())
    {
        if (IsCurrentStudyReadonly())
        {
            Window::Message message(ApplWnd::Instance(),
                                    wxT("Save changes"),
                                    wxT("Impossible to save the study"),
                                    wxT("The study is read-only. Use 'Save as' instead."),
                                    "images/misc/error.png");
            message.add(Window::Message::btnCancel);
            message.showModal();
        }
        else
            Dispatcher::GUI::Post(this, &ApplWnd::saveStudy, 80 /*ms*/);
    }
}

void MainFormData::onToolbarSave(void*)
{
    if (GUIIsLock())
        return;

    if (Data::Study::Current::Valid())
    {
        if (IsCurrentStudyReadonly())
        {
            Window::Message message(ApplWnd::Instance(),
                                    wxT("Save changes"),
                                    wxT("Impossible to save the study"),
                                    wxT("The study is read-only. Use 'Save as' instead."),
                                    "images/misc/error.png");
            message.add(Window::Message::btnCancel);
            message.showModal();
        }
        else
            Dispatcher::GUI::Post(&pMainForm, &ApplWnd::saveStudy, 80 /*ms*/);
    }
}

void ApplWnd::evtOnSaveAs(wxCommandEvent&)
{
    if (GUIIsLock())
        return;

    // Save the current study
    if (Data::Study::Current::Valid())
        Dispatcher::GUI::CreateAndShowModal<Window::SaveAs>(this);
}

void ApplWnd::evtOnExportMap(wxCommandEvent&)
{
    if (GUIIsLock())
        return;

    // Export the current study map
    if (Data::Study::Current::Valid())
        Dispatcher::GUI::CreateAndShowModal<Window::ExportMap>(this);
}

void ApplWnd::evtOnQuickOpenStudy(wxCommandEvent& evt)
{
    if (GUIIsLock())
        return;

    const int indx = evt.GetId() - mnIDOpenRecents_0;
    if (indx >= 0 && indx < (int)RecentFiles::Max)
    {
        RecentFiles::ListPtr lst = RecentFiles::Get();
        int ci = 0;
        auto end = lst->end();
        for (auto i = lst->begin(); i != end; ++i)
        {
            if (ci == indx)
            {
                String t;
                wxStringToString(i->first, t);
                Dispatcher::StudyOpen(t);
                return;
            }
            ++ci;
        }
    }
}

void ApplWnd::evtOnRaiseWindow(wxCommandEvent& evt)
{
    if (GUIIsLock())
        return;

    if (evt.GetId() == mnIDWindowRaise_Other)
    {
        // not yet implemented
        Forms::Disabler<ApplWnd> disabler(*this);
        Window::RaiseWindowBox openother(this);
        openother.ShowModal();
        return;
    }

    auto& framelist = Component::Frame::Registry::List();

    uint indx = (uint)(evt.GetId() - mnIDWindowRaise_0);
    if (indx < framelist.size())
        framelist[indx]->frameRaise();
}

void ApplWnd::evtOnCloseStudy(wxCommandEvent&)
{
    if (GUIIsLock())
        return;
    Dispatcher::StudyClose();
}

void ApplWnd::evtOnDropDownRecents(wxAuiToolBarEvent& evt)
{
    if (evt.IsDropDownClicked())
    {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

        tb->SetToolSticky(evt.GetId(), true);

        // create the popup menu
        wxMenu menuPopup;

        Menu::RebuildRecentFiles(&menuPopup, true);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(evt.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);

        PopupMenu(&menuPopup, pt);

        // make sure the button is "un-stuck"
        tb->SetToolSticky(evt.GetId(), false);
    }
}

void MainFormData::onToolbarOpenRecentMenu(Component::Button&, wxMenu& menu, void*)
{
    Menu::RebuildRecentFiles(&menu, true);
}

} // namespace Forms
} // namespace Antares
