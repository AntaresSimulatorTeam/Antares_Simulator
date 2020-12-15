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

#include "spotlight.h"
#include "listbox-panel.h"
#include "../../../simulator/application/study.h"
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/display.h>
#include "mini-frame.h"
#include "../../lock.h"
#include "../panel/group.h"
#include <wx/dialog.h>
#include <wx/minifram.h>

#include <iostream>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace // anonymous
{
#include "search-png.hxx"

static Antares::Component::Spotlight* globalSpotlight = nullptr;

} // anonymous namespace

Spotlight::Spotlight(wxWindow* parent, uint flags) :
 Antares::Component::Panel(parent),
 pFlags(flags),
 pEdit(nullptr),
 pLayerFilter(nullptr),
 pListbox(nullptr),
 pItemHeight(18),
 pDisplayHandle(wxNOT_FOUND),
 pLayoutParent(nullptr),
 pAllowFrameResize(true)
{
    createComponents(this, (0 == (flags & optNoSearchInput)), (0 == (flags & optResultsInAPopup)));
}

Spotlight::~Spotlight()
{
    // Destroying all bound events as soon as possible
    destroyBoundEvents();

    pResetProvider(nullptr);
    onUpdateItems.clear();
    pResetProvider.clear();
    pLayoutParent = nullptr;
    pFlags = 0;

    // It would be better to remove this pointer as soon as possible,
    // to avoid corrupt vtable
    if (!(!pDataProvider))
    {
        pDataProvider->pSpotlightComponent = nullptr;
        pDataProvider = nullptr;
    }

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Spotlight::createComponents(Spotlight* parent, bool input, bool results)
{
    // Retrieve the parent sizer
    assert(parent && "invalid parent pointer");

    wxSizer* parentSizer = parent->GetSizer();
    if (!parentSizer)
    {
        parentSizer = new wxBoxSizer(wxVERTICAL);
        parent->SetSizer(parentSizer);
    }

    if (input)
    {
        auto* inputPanel = new Antares::Component::Panel(parent);
        inputPanel->SetBackgroundColour(GetBackgroundColour());

        // The search input
        pEdit = new wxTextCtrl(inputPanel,
                               wxID_ANY,
                               wxEmptyString,
                               wxDefaultPosition,
                               wxDefaultSize,
                               wxTE_PROCESS_ENTER | wxTE_LEFT);

        pLayerFilter = new wxComboBox(inputPanel,
                                      wxID_ANY,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      0,
                                      NULL,
                                      wxCB_READONLY);
        pLayerFilter->SetFont(wxFont(wxFontInfo().Bold()));
        pLayerFilter->AppendString("All");
        pLayerFilter->SetValue("All");
        pLayerFilter->Connect(pLayerFilter->GetId(),
                              wxEVT_COMBOBOX,
                              wxCommandEventHandler(Spotlight::onComboUpdated),
                              nullptr,
                              this);

        vz = new wxBoxSizer(wxVERTICAL);
        auto* hz = new wxBoxSizer(wxHORIZONTAL);
        hzCombo = new wxBoxSizer(wxHORIZONTAL);

        hzCombo->Add(pLayerFilter, 1, wxALL | wxEXPAND);

        // Icon
        {
            wxMemoryInputStream istream(search_png, sizeof(search_png));
            wxImage img(istream, wxBITMAP_TYPE_PNG);

            wxBitmap mask(img);
            auto* bmp = new wxStaticBitmap(inputPanel, wxID_ANY, mask);
            hz->AddSpacer(2);
            hz->Add(bmp, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 2);
        }

        hz->Add(pEdit, 1, wxALL | wxEXPAND, 2);
        hz->AddSpacer(3);

        vz->Add(hzCombo, 1, wxALL | wxEXPAND);
        vz->Hide(hzCombo);

        if (0 != (pFlags & optAutoResizeParent))
        {
            vz->AddSpacer(2);
            vz->Add(hz, 1, wxALL | wxEXPAND);
            vz->AddSpacer(4);

            // Changing the background colour for the border
            Antares::Component::PanelGroup::SetDarkBackgroundColor(inputPanel);
        }
        else
        {
            vz->Add(hz, 1, wxALL | wxEXPAND);
        }

        // Changing the background colour for the edit
        pEdit->SetBackgroundColour(inputPanel->GetBackgroundColour());
        if (0 != (pFlags & optAutoResizeParent))
            Antares::Component::PanelGroup::SetLighterBackgroundColor(pEdit, +40);
        else
            Antares::Component::PanelGroup::SetLighterBackgroundColor(pEdit);

        inputPanel->SetSizer(vz);
        parentSizer->Add(inputPanel, 0, wxALL | wxEXPAND);

        auto* spacePanel = new Antares::Component::Panel(parent);
        spacePanel->SetBackgroundColour(GetBackgroundColour());
        spacePanel->SetSize(10, 5);
        parentSizer->Add(spacePanel, 0, wxALL | wxEXPAND);

        pEdit->Connect(pEdit->GetId(),
                       wxEVT_COMMAND_TEXT_UPDATED,
                       wxCommandEventHandler(Spotlight::onInputUpdated),
                       nullptr,
                       this);
    }

    if (results)
    {
        //			if (input)
        //				parentSizer->AddSpacer(5);

        auto* panel = new ListboxPanel(parent, pFlags);
        if (0 != (pFlags & Spotlight::optBkgWhite))
            panel->SetBackgroundColour(GetBackgroundColour());

        onUpdateItems.connect(panel, &ListboxPanel::updateItems);
        parentSizer->Add(panel, 1, wxALL | wxEXPAND);
        pResetProvider.connect(panel, &ListboxPanel::resetProvider);
        pListbox = panel;
    }

    // Refreshing the layout
    parentSizer->Layout();
}

void Spotlight::provider(IProvider::Ptr provider)
{
    IProvider::Ptr current = pDataProvider;
    if (current != provider)
    {
        if (!(!current))
            current->pSpotlightComponent = nullptr;

        if (!provider)
        {
            pDataProvider = nullptr;
            pResetProvider(nullptr);
        }
        else
        {
            provider->pSpotlightComponent = this;
            pDataProvider = provider;
            pResetProvider(provider);
        }

        // Redo the research with new provider
        search(pLastResearch); // redoResearch
    }
}

void Spotlight::search(const wxString& text)
{
    String s;
    wxStringToString(text, s);
    search(s);
}

void Spotlight::redoResearch()
{
    search(pLastResearch);
}

void Spotlight::search(const String& text)
{
    if (IsGUIAboutToQuit())
        return;

    // Keeping the trace somewhere of the last query, to redo it later
    // if the provider is changed
    if (&pLastResearch != &text) // avoid undefined behavior
        pLastResearch = text;

    // Keeping a reference to the data provider
    IProvider::Ptr provider = pDataProvider;
    if (!provider)
    {
        pResults = nullptr;
        pTokens = nullptr;
    }
    else
    {
        // Extract all tokens
        auto* tokens = new SearchToken::Vector();
        if (not text.empty())
            convertRawTextIntoSearchTokenVector(*tokens, text);

        String layerName = "";
        if (pLayerFilter)
            layerName = std::string(pLayerFilter->GetValue().mb_str());
        // Results
        auto* results = new IItem::Vector();
        provider->search(*results, *tokens, layerName);

        pResults = results;
        pTokens = tokens;
    }

    // Update the GUI
    updateResults();
}

void Spotlight::convertRawTextIntoSearchTokenVector(SearchToken::Vector& out,
                                                    const Yuni::String& text)
{
    // assert
    assert(&out != NULL);
    assert(out.empty() && "The provided vector of search tokens must be empty");
    assert(not text.empty() && "The provided text must not be empty");

    String::Vector ts;
    text.split(ts, " \t|'\"()[]{}:;.?");
    if (ts.empty())
        return;

    float weight = 1.f;
    const String::Vector::const_iterator end = ts.end();
    for (String::Vector::const_iterator i = ts.begin(); i != end; ++i)
    {
        // alias to the current token
        const String& tok = *i;
        if (!tok)
            continue;

        // Looking for prefix modifiers, such as '-' and '+'
        uint offset = 0;
        const String::const_utf8iterator tend = tok.utf8end();
        for (String::const_utf8iterator ti = tok.utf8begin(); ti != tend;
             ++ti, offset += (*ti).size())
        {
            const char c = *ti;
            if (c == '-')
            {
                weight = 0.0f;
                continue;
            }
            if (c == '+')
            {
                weight = 1.0f;
                continue;
            }
            break;
        }
        // The string obviously only contains '-' and '+'
        if (offset >= tok.size())
            continue;

        // Adding a new search token
        auto* searchToken = new SearchToken();
        searchToken->text.assign(tok.c_str() + offset, tok.size() - offset);
        searchToken->weight = weight;
        out.push_back(searchToken);

        // Resetting internal values for the next token
        weight = 1.0f;
    }
}

void Spotlight::updateResults()
{
    if (IsGUIAboutToQuit())
        return;

    if (0 != (pFlags & optAutoResizeParent))
        resizeParentWindow();

    SearchToken::VectorPtr tokens = pTokens;
    IItem::VectorPtr results = pResults;

    onUpdateItems(results, tokens);
}

void Spotlight::resizeParentWindow()
{
    auto* parent = GetParent();
    if (not parent || not pLayoutParent)
        return;

    if (pAllowFrameResize)
    {
        // the relative position of the listbox
        int ltop = 0;
        if (pListbox)
            pListbox->GetPosition(nullptr, &ltop);

        // About the component itself
        wxPoint parentPos = pLayoutParent->GetScreenPosition();

        // Gathering informations about the current display
        // note: it may happen that wx does not provide a valid handle
        //  at the second call to GetFromPoint() for some unknown reasons
        if (pDisplayHandle == wxNOT_FOUND)
        {
            pDisplayHandle = wxDisplay::GetFromPoint(parentPos);
            if (pDisplayHandle == wxNOT_FOUND)
            {
#ifndef NDEBUG
                std::cerr << "error: display monitor not found\n";
#endif
                return;
            }
        }
        else
        {
            int newhandle = wxDisplay::GetFromPoint(parentPos);
            if (newhandle != wxNOT_FOUND)
                pDisplayHandle = newhandle;
        }
        wxDisplay monitor(pDisplayHandle);
        wxRect display = monitor.GetClientArea();

        // re-calculate the good size
        int idealH = (!pResults) ? 0 : (int)(pItemHeight * pResults->size());
        if (!(pFlags & optNoSearchInput))
            idealH += (int)ListboxPanel::searchResultTextHeight;
        idealH += ltop;
        idealH += 6; // arbitrary value, for additional spaces due to borders

        enum
        {
            marginY = 28 // minimum required for Window 7's task bar
        };
        int maxHeight = display.GetHeight() - (parentPos.y) - marginY;
        if (idealH > maxHeight)
        {
            int newMaxHeight = parentPos.y - marginY;
            // using a ratio to keep the display at the botton of the parent control
            // as much as possible
            if (newMaxHeight > maxHeight && (double)maxHeight / newMaxHeight < 0.42)
            {
                maxHeight = newMaxHeight;
                if (idealH > maxHeight)
                    idealH = maxHeight;
                parent->Move(parentPos.x, parentPos.y - idealH);
                // The frame has been moved to the top of the original control
                // As a consequence, we should no longer update the height
                // otherwise it will be annoying for the user
                pAllowFrameResize = false;
            }
            else
                idealH = maxHeight;
        }

        parent->SetSize(parent->GetSize().GetWidth(), idealH);
    }

    assert(parent->GetSizer() != NULL);
    parent->GetSizer()->Layout();
    assert(GetSizer() != NULL);
    GetSizer()->Layout();
    Dispatcher::GUI::Refresh(parent);
}

Spotlight::IProvider::~IProvider()
{
    destroyBoundEvents();
}

void Spotlight::resetSearchInput()
{
    pLastResearch.clear();
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pEdit)
        pEdit->ChangeValue(wxEmptyString);

    redoResearch();
}

void Spotlight::onMapLayerAdded(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter && pLayerFilter->FindString(*text) == wxNOT_FOUND)
    {
        pLayerFilter->AppendString(*text);
        if (pLayerFilter->GetCount() > 1 && !vz->IsShown(hzCombo))
            vz->Show(hzCombo);
    }
    // wxStringToString(*text, pLastResearch);
    // Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::onMapLayerRemoved(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
    {
        auto pos = pLayerFilter->FindString(*text);
        if (pos != wxNOT_FOUND)
        {
            pLayerFilter->Delete(pos);
            pLayerFilter->Select(0);
        }
        if (pLayerFilter->GetCount() == 1 && vz->IsShown(hzCombo))
            vz->Hide(hzCombo);
    }
    /*wxStringToString(*text, pLastResearch);*/
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::onMapLayerChanged(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetValue(*text);

    // wxStringToString(*text, pLastResearch);
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::onMapLayerRenamed(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetString(pLayerFilter->GetSelection(), *text);

    // wxStringToString(*text, pLastResearch);
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::onInputUpdated(wxCommandEvent& evt)
{
    if (IsGUIAboutToQuit())
        return;

    const wxString textinput = evt.GetString();
    wxStringToString(textinput, pLastResearch);
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::onComboUpdated(wxCommandEvent& evt)
{
    if (IsGUIAboutToQuit())
        return;
    wxString temp = pLayerFilter->GetValue();
    OnMapLayerChanged(&temp);
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Spotlight::itemHeight(uint h)
{
    pItemHeight = (h < 18) ? 18 : h;
}

class FrameShowData
{
public:
    typedef SmartPtr<FrameShowData> Ptr;

    static void ReExecute(const Ptr& data)
    {
        if (!(!data))
            Spotlight::FrameShow(data->parent, data->provider, data->flags, data->width);
    }

public:
    wxWindow* parent;
    Antares::Component::Spotlight::IProvider::Ptr provider;
    uint flags;
    uint width;

}; // class FrameShowData

void Spotlight::FrameShow(wxWindow* parent,
                          Antares::Component::Spotlight::IProvider::Ptr provider,
                          uint flags,
                          uint width)
{
    typedef Antares::Private::Spotlight::SpotlightMiniFrame FrameType;

    if (IsGUIAboutToQuit())
        return;

    GUILocker locker;

    auto* frame = FrameType::Instance();
    if (frame)
    {
        globalSpotlight = nullptr;
        frame->Close();

        FrameShowData::Ptr data = new FrameShowData();
        data->parent = parent;
        data->provider = provider;
        data->flags = flags;
        data->width = width;

        Yuni::Bind<void()> callback;
        callback.bind(FrameShowData::ReExecute, data);
        Dispatcher::GUI::Post(callback, 10);
        return;
    }

    // Creating a new window
    {
        uint spotflags = Antares::Component::Spotlight::optAutoResizeParent;
        if (flags & Spotlight::optGroups)
            spotflags |= Spotlight::optGroups;
        if (flags & Spotlight::optNoSearchInput)
            spotflags |= Spotlight::optNoSearchInput;

        // Finding the parent frame
        wxWindow* parentFrame = parent;
        while (true)
        {
            parentFrame = parentFrame->GetParent();
            if (parentFrame)
            {
                if (dynamic_cast<wxFrame*>(parentFrame))
                    break;
                if (dynamic_cast<wxDialog*>(parentFrame))
                    break;
                if (dynamic_cast<wxMiniFrame*>(parentFrame))
                    break;
            }
        }

        frame = new FrameType(parentFrame);

        auto* vz = new wxBoxSizer(wxVERTICAL);
        globalSpotlight = new Antares::Component::Spotlight(frame, spotflags);
        vz->Add(globalSpotlight, 1, wxALL | wxEXPAND, 1);
        vz->Layout();
        frame->SetSizer(vz);

        // Changing the background colour for the border
        Antares::Component::PanelGroup::SetDarkBackgroundColor(frame);
    }
    assert(globalSpotlight);
    assert(frame);

    // Updating the width of the form
    frame->SetSize((int)width, 330);
    // setting the new data provider
    if (globalSpotlight)
    {
        // updating the data provider
        globalSpotlight->provider(provider);
        // reseting the parent control
        globalSpotlight->pLayoutParent = parent;
        // invalidating the previous computed height
        globalSpotlight->pAllowFrameResize = true;
    }

    int offset = flags == optNoSearchInput ? 5 : 65;
    if (globalSpotlight->pResults)
        frame->SetSize(
          (int)width,
          std::min(
            (int)(globalSpotlight->pResults->size() * globalSpotlight->itemHeight()) + offset,
            330));
    // Display the toolbar
    if (parent)
    {
        wxPoint pt = parent->GetScreenPosition();
        wxSize sz = parent->GetSize();
        // Move the window to the desirate coordinates
        frame->Move(pt.x, pt.y + sz.GetHeight());
    }
    else
        frame->Move(0, 0);

    // Display the window
    Dispatcher::GUI::Layout(frame->GetSizer());
    Dispatcher::GUI::Show(frame, false);
}

void Spotlight::FrameClose()
{
    typedef Antares::Private::Spotlight::SpotlightMiniFrame FrameType;

    GUILocker locker;
    auto* frame = FrameType::Instance();
    if (frame)
    {
        globalSpotlight = nullptr;
        frame->Close();
    }
}

} // namespace Component
} // namespace Antares
