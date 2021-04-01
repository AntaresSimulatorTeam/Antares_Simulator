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

#include "startupwizard.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/settings.h>
#include <wx/statline.h>

#include "../../../internet/license.h"
#include <antares/study/finder.h>
#include "../toolbox/resources.h"
#include "../toolbox/components/button.h"
#include "../application/study.h"
#include "../application/main.h"
#include "../toolbox/create.h"
#include "../application/recentfiles.h"
#include "../application/menus.h"
#include "../application/wait.h"
#include "version.h"
#include "../../../config.h"
#include <ui/common/component/panel.h>
#include <ui/common/component/spotlight.h>
#include "../toolbox/dispatcher/study.h"
#include "../internet/limits.h"

using namespace Yuni;
namespace Antares
{
namespace Window
{
namespace // anonymous
{
static StartupWizard* globalWndStartupWizard = nullptr;

typedef std::pair<String, String> PairStringString;
typedef std::vector<PairStringString> OutputList;

class ExampleProvider;

class MyStudyFinder final : public Antares::Data::StudyFinder
{
public:
    MyStudyFinder(OutputList& list) : pOutputList(list)
    {
    }

    virtual ~MyStudyFinder()
    {
        foreach (auto& it, pList)
            pOutputList.push_back(PairStringString(it.first, it.second));
    }

    virtual void onStudyFound(const String& folder, Data::Version) override
    {
        if (Data::Study::TitleFromStudyFolder(folder, pTitle, false))
        {
            pTitle.replace("  ", " ");
            pList[pTitle] = folder;
        }
    }

public:
    OutputList& pOutputList;
    String pTitle;
    typedef std::map<String, String> List;
    List pList;

}; // class MyStudyFinder

class ExampleProvider final : public Antares::Component::Spotlight::IProvider
{
public:
    //! The spotlight component (alias)
    typedef Antares::Component::Spotlight Spotlight;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ExampleProvider(const String& folder)
    {
        MyStudyFinder finder(pOutputs);
        finder.lookup(folder);
        finder.wait();
    }

    //! Destructor
    virtual ~ExampleProvider()
    {
    }
    //@}

    void extractNumber(const String& title, String& newtitle, String& number)
    {
        if (title.size() > 4 and title[3] == ' ')
        {
            if (String::IsDigit(title[0]) and String::IsDigit(title[1])
                and String::IsDigit(title[2]))
            {
                number.assign(title.c_str(), 3);
                newtitle.assign(title.c_str() + 4, title.size() - 4);
                newtitle.trim(" \r\t\n");
                return;
            }
        }
        number = "???";
        newtitle = title;
    }

    /*!
    ** \brief Perform a new search
    */
    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        const Yuni::String& text = "") override
    {
        String title;
        String number;
        uint index = 0;
        if (tokens.empty())
        {
            auto end = pOutputs.end();
            for (auto i = pOutputs.begin(); i != end; ++i, ++index)
            {
                auto* item = new Spotlight::IItem();
                extractNumber(i->first, title, number);
                item->caption(title);
                item->tag = index;
                // item->addTag("Example", 227, 229, 245);
                item->addTag(number, 118, 139, 163);
                out.push_back(item);
            }
        }
        else
        {
            auto tend = tokens.end();
            auto end = pOutputs.end();
            for (auto i = pOutputs.begin(); i != end; ++i, ++index)
            {
                auto ti = tokens.begin();
                for (; ti != tend; ++ti)
                {
                    const String& text = (*ti)->text;
                    if (i->first.icontains(text))
                    {
                        auto* item = new Spotlight::IItem();
                        item->caption(i->first);
                        item->tag = index;
                        item->addTag("example", 177, 209, 245);
                        out.push_back(item);
                        break;
                    }
                }
            }
        }
    }

    /*!
    ** \brief An item has been selected
    */
    virtual bool onSelect(Spotlight::IItem::Ptr& item) override
    {
        if (!item)
            return false;

        uint index = item->tag;
        if (index < pOutputs.size())
        {
            Component::Spotlight::FrameClose();
            StartupWizard::Close();
            Dispatcher::StudyOpen(pOutputs[index].second);
            return true;
        }
        return false;
    }

private:
    //! All outputs, already sorted by their title
    OutputList pOutputs;
    // Nakama
    friend class MyStudyFinder;

}; // class ExampleProvider

} // anonymous namespace

void StartupWizard::Show()
{
    if (not globalWndStartupWizard)
        globalWndStartupWizard = new StartupWizard(Forms::ApplWnd::Instance());

    Dispatcher::GUI::Show(globalWndStartupWizard, true, true);
}

void StartupWizard::Close()
{
    if (globalWndStartupWizard)
    {
        Component::Spotlight::FrameClose();
        Dispatcher::GUI::Destroy(globalWndStartupWizard);
    }
}

StartupWizard::StartupWizard(wxFrame* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT("Getting started"),
          wxDefaultPosition,
          wxDefaultSize,
          wxCAPTION | wxCLIP_CHILDREN | wxWS_EX_BLOCK_EVENTS),
 pPanelButtonsID(-1),
 pLastSelected(nullptr),
 pAllowDisplaySelection(true),
 pMenuExamples(nullptr),
 pBtnExamples(nullptr)
{
    globalWndStartupWizard = this;
    WIP::Locker wip;

    // Title of the Form
    SetLabel(wxT("Getting started"));
    SetTitle(GetLabel());

    pSelectedBgColor.Set(213, 217, 220);
    pSelectedBgColor.Set(231, 234, 239);

    // The main sizer
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    {
        auto* hz = new wxBoxSizer(wxHORIZONTAL);

#ifdef ANTARES_BETA
        hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/misc/beta.gif"),
                0,
                wxALIGN_LEFT | wxALIGN_TOP);
#elif ANTARES_RC != 0
        hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/misc/rc.gif"),
                0,
                wxALIGN_LEFT | wxALIGN_TOP);
#else
        hz->AddSpacer(20);
#endif

        auto* hz2 = new wxBoxSizer(wxVERTICAL);
        hz2->AddSpacer(20);
        wxStaticText* welcome
          = Component::CreateLabel(this, wxT("Welcome to Antares Simulator"), true, false, +4);
        welcome->SetForegroundColour(wxColour(0, 0, 0));

        hz2->Add(welcome, 0, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_CENTER_HORIZONTAL);

        wxString versSTR;
        versSTR << wxT(ANTARES_VERSION_STR)
#if ANTARES_RC != 0
                << wxT("rc") << int(ANTARES_RC)
#endif
          ;
        auto* version = Component::CreateLabel(this, versSTR, false, false, -1);
        version->SetForegroundColour(wxColour(161, 166, 180));
        hz2->Add(version, 0, wxALIGN_CENTER_VERTICAL | wxALL | wxALIGN_CENTER_HORIZONTAL);
        hz2->AddSpacer(20);

        hz->Add(hz2, 1, wxALL | wxEXPAND);
#ifdef ANTARES_BETA
        hz->AddSpacer(40);
#elif ANTARES_RC != 0
        hz->AddSpacer(40);
#else
        hz->AddSpacer(20);
#endif

        sizer->Add(hz, 0, wxALL | wxEXPAND);
    }

    SetSizer(sizer);

    // New study
    addControls(wxT("new"),
                "images/32x32/new.png",
                wxT("Start a new empty Study"),
                wxT("Create an new study, with default settings and an empty system"));
    addControls(wxT("open"),
                "images/32x32/open.png",
                wxT("Open a Study"),
                wxT("Open a study from a local folder"));

    // Recent files
    loadRecentFiles();

    // Temporary store the true background color
    pDefaultBgColor = GetBackgroundColour();
    SetBackgroundColour(wxColour(255, 255, 255));

    sizer->AddSpacer(30);
    sizer->Add(new wxStaticLine(this), 0, wxALL | wxEXPAND);

    auto* pnlBtns = new Component::Panel(this);
    // Show this wizard at startup
    auto* boxAtStartup = new wxBoxSizer(wxHORIZONTAL);
    boxAtStartup->Add(20, 10);

    boxAtStartup->AddStretchSpacer();

    auto* vpsizer = new wxBoxSizer(wxVERTICAL);
    vpsizer->AddSpacer(8);
    vpsizer->Add(boxAtStartup, 0, wxALL | wxEXPAND);
    vpsizer->AddSpacer(8);
    pnlBtns->SetSizer(vpsizer);
    pnlBtns->SetBackgroundColour(pDefaultBgColor);
    pPanelButtonsID = pnlBtns->GetId();

    if (not Resources::FindExampleFolder(pExFolder))
    {
        pExFolder.clear();
    }
    else
    {
        if (not IO::Directory::Exists(pExFolder))
            pExFolder.clear();
    }

    // Open a example
    if (not pExFolder.empty())
    {
        auto* button = new Component::Button(pnlBtns, wxT("Open an example"));
        button->menu(true);
        button->onPopupMenu(this, &StartupWizard::onDropDownExamples);

        pBtnExamples = button;
        boxAtStartup->Add(pBtnExamples, 0, wxALL | wxEXPAND);
    }

    // space
    boxAtStartup->AddSpacer(5);

    // Cancel
    auto* btnCancel
      = Component::CreateButton(pnlBtns, wxT("  Close  "), this, &StartupWizard::onClose);
    btnCancel->SetDefault();

    boxAtStartup->Add(btnCancel, 0, wxALL | wxEXPAND);
    boxAtStartup->Add(24, 10);
    // Bottom panel
    sizer->Add(pnlBtns, 0, wxALL | wxEXPAND);

    sizer->Layout();

    // Recentre the window
    sizer->Fit(this);
    wxSize p = GetSize();
    p.SetWidth(p.GetWidth() + 20);
    if (p.GetWidth() < 500)
        p.SetWidth(500);
    else
    {
        if (p.GetWidth() > 800)
            p.SetWidth(800);
    }
    SetSize(p);
    Centre(wxBOTH);

    // Reset the default background color to our custom value
    // This value will be used to reset the background colour of the main panels
    pDefaultBgColor.Set(255, 255, 255);

    Connect(
      GetId(), wxEVT_MOTION, wxMouseEventHandler(StartupWizard::onWindowMotion), nullptr, this);
}

StartupWizard::~StartupWizard()
{
    // MakeModal(false);
    if (globalWndStartupWizard == this)
        globalWndStartupWizard = nullptr;
    Component::Spotlight::FrameClose();

    delete pMenuExamples;

    Dispatcher::GUI::Show(Forms::ApplWnd::Instance(), true);
}

void StartupWizard::loadRecentFiles()
{
    const RecentFiles::ListPtr lst = RecentFiles::Get();
    auto* sizer = GetSizer();

    if (not lst->empty())
    {
        addControls(
          wxString(), "images/32x32/recent.png", wxT("Open a recent Study"), wxEmptyString, false);
        if (sizer)
            sizer->AddSpacer(4);

        foreach (auto& it, *lst)
            addRecentItem(it.first, it.second, it.first);
    }
    else
    {
        if (sizer)
            sizer->AddSpacer(20);
    }
}

void StartupWizard::addRecentItem(const wxString& mapping,
                                  const wxString& title,
                                  const wxString& path)
{
    auto* bigPanel = new Component::Panel(this);
    bigPanel->SetBackgroundColour(wxColour(255, 255, 255));
    {
        pMapping[bigPanel->GetId()] = mapping;
        bigPanel->Connect(bigPanel->GetId(),
                          wxEVT_MOTION,
                          wxMouseEventHandler(StartupWizard::onMotion),
                          nullptr,
                          this);
        bigPanel->Connect(bigPanel->GetId(),
                          wxEVT_LEFT_DOWN,
                          wxCommandEventHandler(StartupWizard::onClick),
                          nullptr,
                          this);
    }

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    bigPanel->SetSizer(sizer);

    auto* lblTitle = Component::CreateLabel(bigPanel, title);
    auto* lblPath = Component::CreateLabel(bigPanel, wxString() << path << wxT("  "), false, true);
    sizer->Add(40, 5);
    sizer->Add(lblTitle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2);
    sizer->Add(10, 5);
    sizer->Add(lblPath, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);

    //
    lblTitle->Connect(lblTitle->GetId(),
                      wxEVT_LEFT_DOWN,
                      wxCommandEventHandler(StartupWizard::onClick),
                      nullptr,
                      this);
    lblTitle->Connect(
      lblTitle->GetId(), wxEVT_MOTION, wxMouseEventHandler(StartupWizard::onMotion), nullptr, this);
    lblPath->Connect(lblPath->GetId(),
                     wxEVT_LEFT_DOWN,
                     wxCommandEventHandler(StartupWizard::onClick),
                     nullptr,
                     this);
    lblPath->Connect(
      lblPath->GetId(), wxEVT_MOTION, wxMouseEventHandler(StartupWizard::onMotion), nullptr, this);

    wxSizer* s = new wxBoxSizer(wxHORIZONTAL);
    s->Add(25, 5);
    s->Add(bigPanel, 1, wxALL | wxEXPAND);
    s->Add(15, 5);
    GetSizer()->Add(s, 0, wxALL | wxEXPAND);
}

void StartupWizard::addControls(const wxString& mapping,
                                const char* image,
                                const wxString& title,
                                const wxString& subtitle,
                                bool useEvents)
{
    auto* bigPanel = new Component::Panel(this);
    bigPanel->SetBackgroundColour(wxColour(255, 255, 255));
    if (useEvents)
    {
        pMapping[bigPanel->GetId()] = mapping;
        bigPanel->Connect(bigPanel->GetId(),
                          wxEVT_MOTION,
                          wxMouseEventHandler(StartupWizard::onMotion),
                          nullptr,
                          this);
        bigPanel->Connect(bigPanel->GetId(),
                          wxEVT_LEFT_DOWN,
                          wxCommandEventHandler(StartupWizard::onClick),
                          nullptr,
                          this);
    }
    else
    {
        bigPanel->Connect(bigPanel->GetId(),
                          wxEVT_MOTION,
                          wxMouseEventHandler(StartupWizard::onWindowMotion),
                          nullptr,
                          this);
    }

    auto* topSizer = new wxBoxSizer(wxVERTICAL);

    if (not title.IsEmpty())
        topSizer->AddSpacer(6);

    auto* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(mainSizer, 0, wxALL | wxEXPAND);

    if (not title.IsEmpty())
        mainSizer->Add(10, 5);

    // Icon
    if (image)
    {
        auto* bmp = Resources::StaticBitmapLoadFromFile(bigPanel, wxID_ANY, image);
        mainSizer->Add(bmp);
        mainSizer->Add(10, 10);
        if (useEvents)
        {
            bmp->Connect(bmp->GetId(),
                         wxEVT_MOTION,
                         wxMouseEventHandler(StartupWizard::onMotion),
                         nullptr,
                         this);
            bmp->Connect(bmp->GetId(),
                         wxEVT_LEFT_DOWN,
                         wxCommandEventHandler(StartupWizard::onClick),
                         nullptr,
                         this);
            pMapping[bmp->GetId()] = mapping;
        }
    }
    else
        mainSizer->Add(50, 10);

    // Text
    {
        auto* sizer = new wxBoxSizer(wxVERTICAL);
        wxStaticText* lblTitle = nullptr;
        wxStaticText* lblSubTitle = nullptr;

        if (not subtitle.IsEmpty())
        {
            // Title
            if (not title.IsEmpty())
            {
                lblTitle = Component::CreateLabel(bigPanel, title);
                lblTitle->SetFont(
                  wxFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)));
                sizer->Add(lblTitle, 0, wxALL | wxEXPAND);
            }

            // SubTitle
            lblSubTitle = Component::CreateLabel(bigPanel, subtitle);
            sizer->Add(lblSubTitle, 0, wxALL | wxEXPAND, title.IsEmpty() ? 2 : 1);
        }
        else
        {
            sizer->Add(1, 8);
            // Title
            lblTitle = Component::CreateLabel(bigPanel, title);
            lblTitle->SetFont(
              wxFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD)));
            sizer->Add(lblTitle, 0, wxALL);
        }

        mainSizer->Add(sizer, 1, wxALL | wxEXPAND);

        if (useEvents)
        {
            if (lblTitle)
            {
                lblTitle->Connect(lblTitle->GetId(),
                                  wxEVT_MOTION,
                                  wxMouseEventHandler(StartupWizard::onMotion),
                                  nullptr,
                                  this);
                lblTitle->Connect(lblTitle->GetId(),
                                  wxEVT_LEFT_DOWN,
                                  wxCommandEventHandler(StartupWizard::onClick),
                                  nullptr,
                                  this);
                pMapping[lblTitle->GetId()] = mapping;
            }
            if (lblSubTitle)
            {
                lblSubTitle->Connect(lblSubTitle->GetId(),
                                     wxEVT_MOTION,
                                     wxMouseEventHandler(StartupWizard::onMotion),
                                     nullptr,
                                     this);
                lblSubTitle->Connect(lblSubTitle->GetId(),
                                     wxEVT_LEFT_DOWN,
                                     wxCommandEventHandler(StartupWizard::onClick),
                                     nullptr,
                                     this);
                pMapping[lblSubTitle->GetId()] = mapping;
            }
        }
    }

    bigPanel->SetSizer(topSizer);

    if (useEvents and not title.IsEmpty())
        topSizer->AddSpacer(6);

    // Adding the big panel to the sizer
    auto* s = new wxBoxSizer(wxHORIZONTAL);
    s->AddSpacer(15);
    s->Add(bigPanel, 1, wxALL | wxEXPAND);
    s->AddSpacer(15);
    GetSizer()->Add(s, 0, wxALL | wxEXPAND);
}

void StartupWizard::resetAllBackgroundColors(wxWindow* selected)
{
    if (pLastSelected != selected)
    {
        if (pLastSelected)
        {
            pLastSelected->SetBackgroundColour(pDefaultBgColor);
            pLastSelected->Refresh();
        }
        pLastSelected = (pAllowDisplaySelection) ? selected : nullptr;
        if (pLastSelected)
        {
            pLastSelected->SetBackgroundColour(pSelectedBgColor);
            pLastSelected->Refresh();
        }
    }
}

void StartupWizard::onClick(wxCommandEvent& evt)
{
    // Close any remaining window
    Component::Spotlight::FrameClose();

    // The object that triggered the event
    auto* obj = dynamic_cast<wxWindow*>(evt.GetEventObject());

    // Getting the top-level object
    while (obj and not dynamic_cast<wxDialog*>(obj->GetParent()))
        obj = obj->GetParent();

    pAllowDisplaySelection = false;
    // Resetting its color
    if (obj)
    {
        const wxString& mapping = pMapping[obj->GetId()];

        if (wxT("new") == mapping)
        {
            // New study
            Dispatcher::StudyNew();
        }
        else
        {
            if (wxT("open") == mapping)
            {
                const wxString path = Forms::ApplWnd::Instance()->openStudyFolder(false);
                if (path.empty())
                {
                    // The focus may have been lost. Restoring it.
                    SetFocus();
                    // Re-Initializing the color states
                    resetAllBackgroundColors(nullptr);
                    // Re-Allow the selection from the user (hover)
                    pAllowDisplaySelection = true;
                    return;
                }
                String studyfolder;
                wxStringToString(path, studyfolder);

                String title;
                if (Data::Study::TitleFromStudyFolder(studyfolder, title))
                {
                    Dispatcher::StudyOpen(studyfolder);
                }
                else
                {
                    // The focus may have been lost. Restoring it.
                    SetFocus();
                    // Re-Initializing the color states
                    resetAllBackgroundColors(nullptr);
                    // Re-Allow the selection from the user (hover)
                    pAllowDisplaySelection = true;

                    logs.error() << "This folder does not seem to be a valid study: "
                                 << studyfolder;
                    return;
                }
            }
            else
            {
                // Recent files
                if (not mapping.IsEmpty())
                {
                    String t;
                    wxStringToString(mapping, t);
                    Dispatcher::StudyOpen(t);
                }
            }
        }
    }

    // Close the window
    Dispatcher::GUI::Destroy(this);
}

void StartupWizard::onWindowMotion(wxMouseEvent& evt)
{
    resetAllBackgroundColors(nullptr);
    evt.Skip();
}

void StartupWizard::onMotion(wxMouseEvent& evt)
{
    // The object that triggered the event
    auto* obj = dynamic_cast<wxWindow*>(evt.GetEventObject());

    // Getting the top-level object
    while (obj and !dynamic_cast<wxDialog*>(obj->GetParent()))
        obj = obj->GetParent();

    if (dynamic_cast<wxPanel*>(obj))
        resetAllBackgroundColors(obj);
    else
        resetAllBackgroundColors(nullptr);
    evt.Skip();
}

void StartupWizard::onClose(void*)
{
    Component::Spotlight::FrameClose();
    Dispatcher::GUI::Destroy(this);
}

void StartupWizard::onDropDownExamples(Component::Button&, wxMenu&, void*)
{
    if (pBtnExamples)
        pBtnExamples->Enable(false);
    // Antares::License::displayStartupWizard = true;
    Dispatcher::GUI::Post(this, &StartupWizard::showAllExamples);
}

void StartupWizard::showAllExamples()
{
    enum
    {
#ifdef YUNI_OS_WINDOWS
        width = 280,
#else
        width = 320,
#endif
    };
    Component::Spotlight::FrameShow(pBtnExamples, new ExampleProvider(pExFolder), 0, width);
    if (pBtnExamples)
        pBtnExamples->Enable(true);
}

void StartupWizard::onDisplayPopupMenu()
{
    if (pBtnExamples)
    {
        // Popuuuuup !
        auto rect = pBtnExamples->GetScreenRect();
        pBtnExamples->PopupMenu(pMenuExamples, 0, rect.GetHeight());
        // Re-Enabled the button
        pBtnExamples->SetLabel(wxT("Open an example..."));
        pBtnExamples->Enable(true);
        Dispatcher::GUI::Refresh(pBtnExamples);
    }
}

} // namespace Window
} // namespace Antares
