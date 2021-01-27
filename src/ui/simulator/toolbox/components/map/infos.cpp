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

#include "infos.h"
#include "../../components/captionpanel.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/wupdlock.h>
#include <wx/textctrl.h>
#include "nodes/node.h"
#include "nodes/connection.h"
#include "settings.h"
#include <iostream>
#include "../../validator.h"

using namespace Yuni;

namespace Antares
{
namespace Map
{
BEGIN_EVENT_TABLE(PanelInfos, wxPanel)
EVT_COLOURPICKER_CHANGED(idChangeColor, PanelInfos::onChangeColor)
EVT_BUTTON(idDefaultColor, PanelInfos::onChangeToDefaultColor)
EVT_CHECKBOX(idOrientation, PanelInfos::onChangeOrientation)
EVT_TEXT(idAreaName, PanelInfos::onChangeAreaName)
END_EVENT_TABLE()

wxBoxSizer* PanelInfos::createColorPicker(const wxColour& color)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    pColorPicker = new wxColourPickerCtrl(this, idChangeColor, color);
    sizer->Add(pColorPicker, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    sizer->SetItemMinSize(pColorPicker, 55, 22);
    wxButton* btn = new wxButton(this,
                                 PanelInfos::idDefaultColor,
                                 wxT("Default"),
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxBU_EXACTFIT);
    sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
    sizer->SetItemMinSize(btn, -1, 22);
    sizer->Layout();
    return sizer;
}

PanelInfos::PanelInfos(wxWindow* parent) :
 wxPanel(parent, wxID_ANY), pItemList(), pColorPicker(nullptr)
{
    SetSize(200, 50);
    ItemList emptyList;
    this->refresh(emptyList, true);
}

PanelInfos::~PanelInfos()
{
    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void PanelInfos::onChangeOrientation(wxCommandEvent& evt)
{
    if (!pItemList.empty())
    {
        const ItemList::iterator end = pItemList.end();
        for (ItemList::iterator i = pItemList.begin(); i != end; ++i)
        {
            if ((*i)->type() == Item::tyConnection)
                (dynamic_cast<Connection*>(*i))
                  ->direction((evt.IsChecked() ? Connection::dirDirect : Connection::dirIndirect));
        }
        GetParent()->Refresh();
    }
}

void PanelInfos::onChangeAreaName(wxCommandEvent& evt)
{
    if (!evt.GetString().IsEmpty())
    {
        // Get the new name for all selected areas
        wxString newValue = evt.GetString();
        // Trimming - Forbidden are already excluded with the validator
        newValue.Trim(true);
        newValue.Trim(false);

        const ItemList::iterator end = pItemList.end();
        for (ItemList::iterator i = pItemList.begin(); i != end; ++i)
        {
            if ((*i)->type() == Item::tyNode)
                (dynamic_cast<Node*>(*i))->caption(newValue);
        }
        GetParent()->Refresh();
    }
}

void PanelInfos::onChangeColor(wxColourPickerEvent& evt)
{
    const ItemList::iterator end = pItemList.end();
    for (ItemList::iterator i = pItemList.begin(); i != end; ++i)
    {
        if ((*i)->type() == Item::tyNode)
            (dynamic_cast<Node*>(*i))->color(evt.GetColour());
    }
    GetParent()->Refresh();
}

void PanelInfos::onChangeToDefaultColor(wxCommandEvent&)
{
    if (pColorPicker)
    {
        pColorPicker->SetColour(Settings::defaultNodeBackground);
        wxColourPickerEvent evt;
        evt.SetColour(Settings::defaultNodeBackground);
        onChangeColor(evt);
    }
}

void PanelInfos::countItemSpecies(const PanelInfos::ItemList& list,
                                  int& nodes,
                                  int& connections) const
{
    nodes = 0;
    connections = 0;
    if (!list.empty())
    {
        const ItemList::const_iterator end = list.end();
        for (ItemList::const_iterator i = list.begin(); i != end; ++i)
        {
            switch ((*i)->type())
            {
            case Item::tyNode:
                ++nodes;
                break;
            case Item::tyConnection:
                ++connections;
                break;
            default:
                break;
            }
        }
    }
}

wxCheckBoxState PanelInfos::findConnectionOrientation(const ItemList& list) const
{
    int state = -1;
    if (!list.empty())
    {
        const ItemList::const_iterator end = list.end();
        for (ItemList::const_iterator i = list.begin(); i != end; ++i)
        {
            if ((*i)->type() == Item::tyConnection)
            {
                switch (dynamic_cast<Connection*>(*i)->direction())
                {
                case Connection::dirDirect:
                {
                    if (state == -1)
                        state = 1;
                    else
                    {
                        if (state == 0)
                            return wxCHK_UNDETERMINED;
                    }
                    break;
                }
                case Connection::dirIndirect:
                {
                    if (state == -1)
                        state = 0;
                    else
                    {
                        if (state == 1)
                            return wxCHK_UNDETERMINED;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    return (state == -1 ? wxCHK_UNDETERMINED : (state == 1 ? wxCHK_CHECKED : wxCHK_UNCHECKED));
}

wxColour PanelInfos::findCommonColour(const ItemList& list) const
{
    wxColour ret;
    const ItemList::const_iterator end = list.end();
    for (ItemList::const_iterator i = list.begin(); i != end; ++i)
    {
        if ((*i)->type() == Item::tyNode)
        {
            const wxColour c = (dynamic_cast<Node*>(*i))->color();
            if (ret.IsOk() && c != ret)
                return wxColour(0, 0, 0);
            ret = c;
        }
    }
    return (ret.IsOk() ? ret : wxColour(0, 0, 0));
}

bool PanelInfos::listIsEquals(const ItemList& list) const
{
    if (list.size() != pItemList.size())
        return false;
    const uint m = (uint)list.size();
    for (uint i = 0; i != m; ++i)
    {
        if (list[i] != pItemList[i])
            return false;
    }
    return true;
}

void PanelInfos::layoutAddTitle(const wxString& title)
{
    if (GetSizer())
        GetSizer()->Add(new Antares::Component::CaptionPanel(this, title), 0, wxALL | wxEXPAND);
}

void PanelInfos::layoutNoSelection()
{
    // The inspector
    layoutAddTitle(wxT("Inspector"));
    // A spacer
    GetSizer()->AddStretchSpacer();
    // A label to indicate to the user there is nothing to do here
    GetSizer()->Add(
      new wxStaticText(
        this, wxID_ANY, wxT("No selection"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE),
      0,
      wxALL | wxEXPAND);
    // Another spacer
    GetSizer()->AddStretchSpacer();
}

void PanelInfos::layoutPropertiesForNodes(const int nodeCount)
{
    // Title
    layoutAddTitle(((nodeCount == 1) ? wxT("Inspector - Area") : wxT("Inspector - Areas")));

    wxSizer* sizer = GetSizer();

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 1, 0);
    sizer->Add(gridSizer, 0, wxALL | wxEXPAND, 5);

    if (1 == nodeCount)
    {
        // Name
        gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Name ")),
                       0,
                       wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        wxTextCtrl* edit = new wxTextCtrl(this,
                                          idAreaName,
                                          pItemList[0]->caption(),
                                          wxDefaultPosition,
                                          wxDefaultSize,
                                          0,
                                          Toolbox::Validator::Default());
        edit->SetMaxLength(ant_k_area_name_max_length - 1);
        gridSizer->Add(edit, 1, wxALL | wxEXPAND);
    }

    // Color
    wxColour color(0, 0, 0);
    gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Color ")),
                   0,
                   wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    gridSizer->Add(
      createColorPicker((nodeCount == 1) ? pItemList[0]->color() : findCommonColour(pItemList)),
      0,
      wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

    // Updating the sizers
    gridSizer->AddGrowableCol(1, 1);
    gridSizer->Layout();
}

void PanelInfos::layoutPropertiesForConnections(const int connectionCount)
{
    wxSizer* sizer = GetSizer();
    layoutAddTitle(
      ((connectionCount == 1) ? wxT("Inspector - Connection") : wxT("Inspector - Connections")));
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 1, 0);
    sizer->Add(gridSizer, 0, wxALL | wxEXPAND, 5);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, wxT("Orientation ")),
                   0,
                   wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    wxCheckBox* chk = new wxCheckBox(this, idOrientation, wxT("Direct"));
    chk->Set3StateValue(findConnectionOrientation(pItemList));
    gridSizer->Add(chk, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

    // Updating the sizers
    gridSizer->AddGrowableCol(1, 1);
    gridSizer->Layout();
}

void PanelInfos::layoutFiltersForNodes(const int nodeCount)
{
    // Those components are temporary disabled
    return;

    wxSizer* sizer = GetSizer();
    // Filters
    sizer->AddSpacer(10);
    layoutAddTitle((nodeCount == 1) ? wxT("Output Filters - Area") : wxT("Output Filters - Areas"));
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Area filter Dummy 1")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Area filter Dummy 2")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Area filter Dummy 3")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Area filter Dummy 4")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Area filter Dummy 5")), 0, wxALL | wxEXPAND);
}

void PanelInfos::layoutFiltersForConnections(const int connectionCount)
{
    // Those components are temporary disabled
    return;

    wxSizer* sizer = GetSizer();
    // Filters
    sizer->AddSpacer(10);
    layoutAddTitle((connectionCount == 1) ? wxT("Output Filters - Connection")
                                          : wxT("Output Filters - Connections"));
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Cnnx filter Dummy 1")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Cnnx filter Dummy 2")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Cnnx filter Dummy 3")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Cnnx filter Dummy 4")), 0, wxALL | wxEXPAND);
    sizer->Add(new wxCheckBox(this, wxID_ANY, wxT("Cnnx filter Dummy 5")), 0, wxALL | wxEXPAND);
}

void PanelInfos::refresh(PanelInfos::ItemList& newList, const bool force)
{
    if (!force && listIsEquals(newList)) // Nothing to do
        return;

    pColorPicker = nullptr;
    // Make a copy of the list
    pItemList = newList;
    // Recreate the sizer
    if (GetSizer())
        GetSizer()->Clear(true);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer, true);

    if (newList.empty())
    {
        layoutNoSelection();
    }
    else
    {
        // How many item of each species ?
        int nodeCount;
        int connectionCount;
        countItemSpecies(newList, nodeCount, connectionCount);

        if (nodeCount)
            layoutPropertiesForNodes(nodeCount);

        if (connectionCount)
            layoutPropertiesForConnections(connectionCount);

        if (nodeCount)
            layoutFiltersForNodes(nodeCount);

        if (connectionCount)
            layoutFiltersForConnections(connectionCount);
    }
    sizer->Fit(this);
    sizer->Layout();
    if (GetParent() && GetParent()->GetSizer())
        GetParent()->GetSizer()->Layout();
}

} // namespace Map
} // namespace Antares
