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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "component.h"
#include <wx/sizer.h>
#include "item/info.h"
#include "../../resources.h"
#include "../../create.h"
#include "../../../application/study.h"
#include "../button.h"
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
BEGIN_EVENT_TABLE(Component, Panel)
EVT_PAINT(Component::onDraw)
END_EVENT_TABLE()

/*!
** \brief Additional data for a single item in a listbox
*/
class CustomClientData final : public wxClientData
{
public:
    //! List
    using List = std::list<CustomClientData*>;
    using Vector = std::vector<CustomClientData*>;

public:
    CustomClientData(const Item::IItem::Ptr& it) : item(it)
    {
    }
    virtual ~CustomClientData()
    {
    }

    Item::IItem::Ptr item;
};

Component::Component(wxWindow* parent) :
 Panel(parent),
 pListbox(nullptr),
 pSizerForDatasources(nullptr),
 pSearchEdit(nullptr),
 pCurrentDatasource(nullptr),
 pInvalidated(true)
{
    // The Listbox
    pListbox = new wxSimpleHtmlListBox(
      this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxBORDER_NONE);
    pListbox->SetAutoLayout(true);
    pListbox->Connect(pListbox->GetId(),
                      wxEVT_COMMAND_LISTBOX_SELECTED,
                      wxCommandEventHandler(Component::onSelectionChanged),
                      nullptr,
                      this);
    pListbox->Connect(pListbox->GetId(),
                      wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,
                      wxCommandEventHandler(Component::onSelectionDblClick),
                      nullptr,
                      this);

    // Sizer
    pSizerForDatasources = new wxBoxSizer(wxHORIZONTAL);
    pSizerForDatasources->AddSpacer(15);
    pSizerForDatasources->Add(
      CreateLabel(this, wxT("sort"), false, true), 0, wxALIGN_CENTER_VERTICAL | wxALL);
    pSizerForDatasources->AddSpacer(2);

    // Get the main sizer
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // Adding the main listbox
    sizer->Add(pSizerForDatasources, 0, wxALL | wxEXPAND);
    sizer->Add(pListbox, 1, wxALL | wxEXPAND);
#ifdef WYUNI_OS_WINDOWS
    sizer->Show(pSizerForDatasources, false, true);
#endif
    sizer->Layout();

    SetSizer(sizer);
    Refresh();

    OnStudyClosed.connect(this, &Component::onStudyClosed);
}

Component::~Component()
{
    // Make sure all structures have been released
    if (pLastSelectedItem)
    {
        pLastSelectedItem = nullptr;
        onItemSelected(pLastSelectedItem);
    }

    destroyBoundEvents();

    internalClearTheListbox();
    pListbox = nullptr;
    pItems.clear();

    // Remove all datasources
    const auto end = pDatasources.end();
    for (auto i = pDatasources.begin(); i != end; ++i)
        delete i->second;
    pDatasources.clear();

    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Component::add(Item::IItem::Ptr it)
{
    // Pushing back the item into the list
    pItems.push_back(it);
    forceReload();
}

void Component::setElement(Item::IItem::Ptr it, int index_item)
{
    pItems[index_item] = it;
}

int Component::size()
{
    return pItems.size();
}

void Component::resizeTo(int newSize)
{
    pItems.resize(newSize);
}

void Component::forceRedraw()
{
    pInvalidated = true;
}

void Component::forceReload()
{
    if (!pInvalidated)
    {
        pInvalidated = true;
        pItems.clear();
        internalClearTheListbox();
    }
}

void Component::clear()
{
    internalClearTheListbox();
    pItems.clear();
    if (!(!pLastSelectedItem))
    {
        pLastSelectedItem = nullptr;
        onItemSelected(pLastSelectedItem);
    }
    pInvalidated = true;
}

void Component::forceUpdate()
{
    pItems.clear();
    internalClearTheListbox();
    pInvalidated = true;
    internalUpdateItems();
}

void Component::internalUpdateItems()
{
    // Updating the content of the listbox first
    internalClearTheListbox();

    if (not pListbox)
        return;

    if (pItems.empty())
    {
        // Keep the user informed that there is nothing to display
        pListbox->Append(wxEmptyString,
                         new CustomClientData(std::make_shared<Item::Info>(wxT("No item"))));
        if (pLastSelectedItem)
        {
            pLastSelectedItem = nullptr;
            onItemSelected(pLastSelectedItem);
        }
    }
    else
    {
        int selection = -1;
        Item::IItem::Ptr newSelection;

        // wxListbox are really slow when inserting many items. We have to use
        // a wxArrayString to prevent against stupid refresh by the control, since
        // the methods Freeze and Thaw are not mandatory.....
        CustomClientData::Vector pts;
        wxArrayString arr;
        arr.Alloc(pItems.size());
        wxString tmp;

        auto end = pItems.end();
        for (auto i = pItems.begin(); i != end; ++i)
        {
            if ((*i)->visible())
            {
                // Keeping a reference to the client data
                auto* clientdata = new CustomClientData(*i);
                pts.push_back(clientdata);
                tmp = clientdata->item->htmlContent(wxEmptyString);

                if (pLastSelectedItem == *i)
                {
                    selection = (int)arr.Add(tmp);
                    newSelection = pLastSelectedItem;
                }
                else
                {
                    if (selection == -1 && (*i)->interactive())
                    {
                        selection = (int)arr.Add(tmp);
                        newSelection = *i;
                    }
                    else
                        arr.Add(tmp);
                }
            }
        }

        assert((uint)arr.GetCount() == pts.size());
        if (pListbox)
            pListbox->Append(arr);

        for (uint i = 0; i != pts.size(); ++i)
            pListbox->SetClientObject(i, pts[i]);

        if (selection == -1)
        {
            if (pLastSelectedItem)
            {
                pLastSelectedItem = nullptr;
                onItemSelected(pLastSelectedItem);
            }
        }
        else
        {
            pListbox->SetSelection(selection);
            pLastSelectedItem = newSelection;
            onItemSelected(pLastSelectedItem);
        }
    }
}

void Component::updateHtmlContent()
{
    if (pListbox)
    {
        for (uint i = 0; i < pListbox->GetCount(); ++i)
        {
            const auto* cd = dynamic_cast<CustomClientData*>(pListbox->GetClientObject(i));
            if (cd)
                pListbox->SetString(i, cd->item->htmlContent(wxEmptyString));
        }
    }
}

void Component::onSelectionChanged(wxCommandEvent& evt)
{
    if (not GUIIsLock() && pListbox)
    {
        GUILocker locker;
        const auto* c
          = dynamic_cast<CustomClientData*>(pListbox->GetClientObject(evt.GetSelection()));
        if (c)
        {
            pLastSelectedItem = c->item;
            onItemSelected(c->item);
        }
    }
}

void Component::onSelectionDblClick(wxCommandEvent& evt)
{
    if (not GUIIsLock() && pListbox)
    {
        GUILocker locker;
        const auto* c
          = dynamic_cast<CustomClientData*>(pListbox->GetClientObject(evt.GetSelection()));
        if (c)
        {
            pLastSelectedItem = c->item;
            onItemDblClick(c->item);
        }
    }
}

void Component::internalAddDatasource(Datasource::IDatasource* ds)
{
    if (ds)
    {
        using ButtonType = Antares::Component::Button;
        auto* btn
          = new ButtonType(this, wxEmptyString, ds->icon(), this, &Component::onDatasourceClicked);
        pSizerForDatasources->Add(btn, 0, wxALL | wxEXPAND);
        btn->userdata(ds);
        // Keep a reference somewhere
        pDatasources[btn->GetId()] = ds;

#ifdef WYUNI_OS_WINDOWS
        auto* sizer = GetSizer();
        if (sizer)
            sizer->Show(pSizerForDatasources, true, true);
#endif
        pSizerForDatasources->Layout();
    }
}

void Component::onDatasourceClicked(void* ds)
{
    // getting the new current datasource
    pCurrentDatasource = reinterpret_cast<Datasource::IDatasource*>(ds);
    // The control must be invalidated to force the update
    forceReload();
    Refresh();
}

void Component::onDraw(wxPaintEvent& evt)
{
    if (pInvalidated)
    {
        // avoid useless memory flush
        MemoryFlushLocker memflushlocker;

        // The control is invalidated
        // It must be refresh from the datasource
        if (pCurrentDatasource)
            pCurrentDatasource->refresh();
        // Update the internal structure
        internalUpdateItems();
        pInvalidated = false;
    }
    // Continue
    evt.Skip();
}

void Component::internalClearTheListbox()
{
    if (pListbox && not pListbox->IsEmpty())
        pListbox->Clear();
}

void Component::onStudyClosed()
{
    clear();
}

} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
