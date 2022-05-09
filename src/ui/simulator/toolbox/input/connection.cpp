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

#include <yuni/yuni.h>
#include <antares/study.h>
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
      this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
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

    if (not Data::Study::Current::Valid())
        return;
    auto& study = *Data::Study::Current::Get();

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
    wxTreeItemId localRootId;

    // UPSTREAM / DOWNSTREEAM
    // Root Node
    localRootId = pListbox->AppendItem(rootId, wxString(wxT("Upstream / Downstream")), 2);
    pListbox->SetItemBold(localRootId, true);
    {
        const Data::Area::Map::iterator end = study.areas.end();
        for (Data::Area::Map::iterator i = study.areas.begin(); i != end; ++i)
        {
            // Reference to the area
            Data::Area& area = *(i->second);
            if (area.isVisibleOnLayer(layerID))
            {
                wxTreeItemId id;
                // Foreach Interconnection for the area
                const Data::AreaLink::Map::iterator end = area.links.end();
                for (Data::AreaLink::Map::iterator i = area.links.begin(); i != end; ++i)
                {
                    Data::AreaLink* lnk = i->second;

                    if (lnk->isVisibleOnLayer(layerID))
                    {
                        if (!id)
                        {
                            // We have to create the item corresponding to the area
                            id = pListbox->AppendItem(
                              localRootId,
                              wxString() << wxT(' ') << wxStringFromUTF8(area.name) << wxT(' '),
                              1,
                              1);
                            pListbox->SetItemBold(id, true);
                        }
                        // Adding the item for the interconnection
                        /* wxTreeItemId itemID = */ pListbox->AppendItem(
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

    // Flat
    // Root Node
    localRootId = pListbox->AppendItem(rootId, wxString(wxT("Flat")), 2);
    pListbox->SetItemBold(localRootId, true);
    {
        const Data::Area::Map::iterator end = study.areas.end();
        for (Data::Area::Map::iterator i = study.areas.begin(); i != end; ++i)
        {
            // Reference to the area
            Data::Area& area = *(i->second);

            if (area.isVisibleOnLayer(layerID))
            {
                // Foreach Interconnection for the area
                const Data::AreaLink::Map::iterator end = area.links.end();
                for (Data::AreaLink::Map::iterator i = area.links.begin(); i != end; ++i)
                {
                    Data::AreaLink* lnk = i->second;

                    if (lnk->isVisibleOnLayer(layerID))
                    {
                        // Adding the item for the interconnection
                        /*wxTreeItemId itemID = */ pListbox->AppendItem(
                          localRootId, /*parent*/
                          // caption
                          wxString() << wxT(' ') << wxStringFromUTF8(lnk->from->name) << wxT(" / ")
                                     << wxStringFromUTF8(lnk->with->name) << wxT(' '),
                          0,
                          0,
                          new TreeLeaf(lnk));
                    }
                }
            }
        }
    }

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
    /*wxStringToString(*text, pLastResearch);*/
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
