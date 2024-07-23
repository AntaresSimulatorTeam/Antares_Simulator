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

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "connection.h"
#include "../components/captionpanel.h"
#include "../../application/study.h"
#include "../../windows/inspector.h"
#include <assert.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/wupdlock.h>
#include <wx/imaglist.h>
#include "../../application/main.h"
#include "../resources.h"
#include <ui/common/lock.h>

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
class TreeLeaf final : public wxTreeItemData
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    TreeLeaf(Data::AreaLink* lnk) : pLink(lnk)
    {
    }
    //! Destructor
    virtual ~TreeLeaf()
    {
    }
    //@}

    //! Get the attached link
    Data::AreaLink* link() const
    {
        return pLink;
    }

private:
    //! Pointer to an AreaLink
    Data::AreaLink* pLink;

}; // class TreeLeaf

Connections::Connections(wxWindow* parent) :
 AInput(parent), pLayerFilter(nullptr), pListbox(nullptr), pLastSelected(nullptr)
{
    // Default size
    SetSize(420, 300);

    // Create all needed controls
    internalBuildSubControls();
    // Updating of their values
    update();

    // Connect to the global event
    OnStudyAreasChanged.connect(this, &Connections::update);
    OnStudyAreaRename.connect(this, &Connections::onStudyAreaUpdate);
    OnStudyAreaDelete.connect(this, &Connections::onStudyAreaUpdate);
    OnStudyLinkAdded.connect(this, &Connections::onStudyLinkUpdate);
    OnStudyLinkDelete.connect(this, &Connections::onStudyLinkUpdate);
    OnStudyClosed.connect(this, &Connections::onStudyClosed);
    OnStudyEndUpdate.connect(this, &Connections::update);

    OnMapLayerChanged.connect(this, &Connections::onMapLayerChanged);
    OnMapLayerAdded.connect(this, &Connections::onMapLayerAdded);
    OnMapLayerRemoved.connect(this, &Connections::onMapLayerRemoved);
    OnMapLayerRenamed.connect(this, &Connections::onMapLayerRenamed);
}

Connections::~Connections()
{
    destroyBoundEvents();
    if (pListbox)
        pListbox->DeleteAllItems();
}

void Connections::internalBuildSubControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    sizer->AddSpacer(1);
    // Layer filter
    pLayerFilter = new wxComboBox(
      this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
    pLayerFilter->SetFont(wxFont(wxFontInfo().Bold()));
    pLayerFilter->AppendString("All");
    pLayerFilter->SetValue("All");
    pLayerFilter->Connect(pLayerFilter->GetId(),
                          wxEVT_COMBOBOX,
                          wxCommandEventHandler(Connections::layerFilterChanged),
                          nullptr,
                          this);
    sizer->Add(pLayerFilter, 0, wxALL | wxEXPAND);
    sizer->AddSpacer(2);
    sizer->Hide(pLayerFilter);
    // Listbox
    pListbox
      = new wxTreeCtrl(this,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT | wxTR_NO_LINES | wxTR_HAS_BUTTONS
                         | wxTR_FULL_ROW_HIGHLIGHT | wxTR_SINGLE | wxBORDER_NONE);

    pListbox->Connect(pListbox->GetId(),
                      wxEVT_COMMAND_TREE_SEL_CHANGED,
                      wxTreeEventHandler(Connections::onSelectionChanged),
                      nullptr,
                      this);

    // Image List
    wxImageList* imgList = new wxImageList(16, 16);

    // links
    {
        wxBitmap* bmp = Resources::BitmapLoadFromFile("images/16x16/link.png");
        imgList->Add(*bmp);
        delete bmp;
    }
    // Areas
    {
        wxBitmap* bmp = Resources::BitmapLoadFromFile("images/16x16/area.png");
        imgList->Add(*bmp);
        delete bmp;
    }
    // Areas
    {
        wxBitmap* bmp = Resources::BitmapLoadFromFile("images/16x16/blocks.png");
        imgList->Add(*bmp);
        delete bmp;
    }
    pListbox->AssignImageList(imgList);

    // Update the layout
    sizer->Add(pListbox, 1, wxALL | wxEXPAND);
    sizer->Layout();
}

static void addUpstreamDownstream(const wxTreeItemId& rootId,
                                  wxTreeCtrl* listbox,
                                  const Data::AreaList& areas,
                                  size_t layerID)
{
    auto localRootId = listbox->AppendItem(rootId, wxString(wxT("Upstream / Downstream")), 2);
    listbox->SetItemBold(localRootId, true);
    {
        for (const auto& [links, area] : areas)
        {
            if (area->isVisibleOnLayer(layerID))
            {
                wxTreeItemId id;
                // Foreach Interconnection for the area
                for (const auto& [unused, lnk] : area->links)
                {
                    if (lnk->isVisibleOnLayer(layerID))
                    {
                        if (!id)
                        {
                            // We have to create the item corresponding to the area
                            id = listbox->AppendItem(
                              localRootId,
                              wxString() << wxT(' ') << wxStringFromUTF8(area->name) << wxT(' '),
                              1,
                              1);
                            listbox->SetItemBold(id, true);
                        }
                        // Adding the item for the interconnection
                        listbox->AppendItem(
                          id, /*parent*/
                          // caption
                          wxString() << wxT(' ') << wxStringFromUTF8(lnk->with->name) << wxT(' '),
                          0,
                          0,
                          new TreeLeaf(lnk));
                    }
                }
            }
        }
    }
}

static void addByArea(const wxTreeItemId& rootId,
                      wxTreeCtrl* listbox,
                      const Data::AreaList& areas,
                      size_t layerID)
{
    // Root Node
    auto localRootId = listbox->AppendItem(rootId, wxString(wxT("By area")), 2);
    listbox->SetItemBold(localRootId, true);

    // For each area, list the links related to it. For each link, the area is either
    // its origin or extremity.
    using ListOfLinks = std::vector<Data::AreaLink*>;

    // 1. Build hierarchy
    std::map<Data::AreaName, ListOfLinks> areaToListOfLinks;
    // AreaName area1 -> {Link* lnk1, Link* lnk2, ...}
    for (const auto& [unused1, area] : areas)
    {
        if (area->isVisibleOnLayer(layerID))
        {
            for (const auto& [unused2, lnk] : area->links)
            {
                if (lnk->isVisibleOnLayer(layerID))
                {
                    areaToListOfLinks[area->name].push_back(lnk);
                    areaToListOfLinks[lnk->with->name].push_back(lnk);
                }
            }
        }
    }
    // 2. Create nodes, etc.
    for (const auto& [area, links] : areaToListOfLinks)
    {
        // Reference to the area
        wxTreeItemId id;
        // Foreach Interconnection for the area
        auto count = links.size();
        for (auto lnk : links)
        {
            if (!id)
            {
                // We have to create the item corresponding to the area
                id = listbox->AppendItem(localRootId,
                                         wxString() << wxT(' ') << wxStringFromUTF8(area)
                                                    << wxT(" (") << count << wxT(')'),
                                         1,
                                         1);
                listbox->SetItemBold(id, true);
            }

            const bool isAreaOriginOfLink = (area == lnk->from->name);
            // Adding the item for the interconnection
            listbox->AppendItem(
              id, /*parent*/
              // caption
              wxString() << wxT(' ')
                         << wxStringFromUTF8(isAreaOriginOfLink ? lnk->with->name : lnk->from->name)
                         << wxT(' '),
              0,
              0,
              new TreeLeaf(lnk));
        }
    }
}

void Connections::update()
{
    assert(pListbox);

    GUILocker locker;
    if (pLastSelected)
    {
        pLastSelected = nullptr;
        onConnectionChanged(nullptr);
    }

    if (!pListbox)
        return;

    // Set all items at once
    pListbox->DeleteAllItems();

    if (not CurrentStudyIsValid())
        return;
    auto& study = *GetCurrentStudy();

    pListbox->Freeze();

    String layerName = "";
    size_t layerID = 0;
    if (pLayerFilter)
        layerName = std::string(pLayerFilter->GetValue().mb_str());
    auto layerListEnd = study.layers.end();
    for (auto layerIt = study.layers.begin(); layerIt != layerListEnd; layerIt++)
    {
        if (layerIt->second == layerName)
        {
            layerID = layerIt->first;
        }
    }

    // The current ROOT node ID
    wxTreeItemId rootId = pListbox->AddRoot(wxString(wxT("Links")));
    pListbox->SetItemBold(rootId, true);

    // UPSTREAM / DOWNSTREEAM
    addUpstreamDownstream(rootId, pListbox, study.areas, layerID);

    // By area
    addByArea(rootId, pListbox, study.areas, layerID);

    // Expand all items and subitems
    pListbox->SetQuickBestSize(true);
    pListbox->ExpandAll();

    pListbox->Thaw();
}

void Connections::onStudyAreaUpdate(Data::Area*)
{
    update();
}

void Connections::onStudyLinkUpdate(Data::AreaLink*)
{
    update();
}

void Connections::onStudyClosed()
{
    GUILocker locker;
    pLastSelected = nullptr;
    onConnectionChanged(nullptr);
    if (pListbox)
        pListbox->DeleteAllItems();
}

void Connections::onSelectionChanged(wxTreeEvent& evt)
{
    assert(pListbox);

    GUILocker locker;
    wxTreeItemId id = evt.GetItem();
    if (id.IsOk() and pListbox)
    {
        wxTreeItemData* data = pListbox->GetItemData(id);
        if (data)
        {
            Yuni::Bind<void()> callback;
            callback.bind(this, &Connections::delayedSelection, data);
            Dispatcher::GUI::Post(callback, 10);
        }
    }
}

void Connections::delayedSelection(wxTreeItemData* data)
{
    GUILocker locker;
    if (data and dynamic_cast<TreeLeaf*>(data))
    {
        TreeLeaf* leaf = dynamic_cast<TreeLeaf*>(data);
        Data::AreaLink* link = leaf->link();
        if (link != pLastSelected)
        {
            // Lock the window to prevent flickering
            Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
            wxWindowUpdateLocker updater(&mainFrm);

            onConnectionChanged(link);
            pLastSelected = link;
            Window::Inspector::SelectLink(pLastSelected);
        }
    }
}

void Connections::onMapLayerAdded(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter && pLayerFilter->FindString(*text) == wxNOT_FOUND)
    {
        pLayerFilter->AppendString(*text);
        if (pLayerFilter->GetCount() > 1 && !GetSizer()->IsShown(pLayerFilter))
            GetSizer()->Show(pLayerFilter);
    }
    // wxStringToString(*text, pLastResearch);
    // Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void Connections::onMapLayerRemoved(const wxString* text)
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
        if (pLayerFilter->GetCount() == 1 && GetSizer()->IsShown(pLayerFilter))
            GetSizer()->Hide(pLayerFilter);
    }
    Dispatcher::GUI::Post(this, &Connections::update);
}

void Connections::onMapLayerChanged(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetValue(*text);

    // wxStringToString(*text, pLastResearch);
    Dispatcher::GUI::Post(this, &Connections::update);
}

void Connections::onMapLayerRenamed(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetString(pLayerFilter->GetSelection(), *text);

    // wxStringToString(*text, pLastResearch);
    Dispatcher::GUI::Post(this, &Connections::update);
}

void Connections::layerFilterChanged(wxCommandEvent& /* evt */)
{
    if (IsGUIAboutToQuit())
        return;
    wxString temp = pLayerFilter->GetValue();
    OnMapLayerChanged(&temp);
    Dispatcher::GUI::Post(this, &Connections::update);
}

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares
