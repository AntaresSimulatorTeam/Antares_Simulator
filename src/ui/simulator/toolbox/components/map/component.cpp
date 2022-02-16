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

#include "component.h"
#include "drawingcontext.h"
#include "settings.h"

#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/dcbuffer.h>

#include "../../../application/main.h"
#include "../../../application/menus.h"
#include "../../../application/main/internal-ids.h"
#include "../../../application/study.h"
#include "../../resources.h"
#include "../../create.h"
#include "../../create.h"
#include "../../components/button.h"

#include <wx/valtext.h>

#include <wx/colour.h>

#include <wx/dcsvg.h>

using namespace Yuni;

static wxBitmapType formatWXConstant[]
  = {wxBITMAP_TYPE_PNG, wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_INVALID};

namespace Antares
{
namespace Map
{
BEGIN_EVENT_TABLE(Component::Drawer, Component::Panel)
EVT_PAINT(Component::Drawer::onDraw)
EVT_SIZE(Component::Drawer::onSize)
END_EVENT_TABLE()

Component::Component(wxWindow* parent) :
 Panel(parent),
 pMapActiveLayer(nullptr),
 pInfosAreaCount(nullptr),
 pInfosConnxCount(nullptr),
 pSelectionPopUpMenu(nullptr)
{
    // Create Manager Singleton Instance
    new Manager(*this);

    // The component's Main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // The map Notebook
    pNoteBook = new Antares::Component::MapNotebook(this, Antares::Component::Notebook::orTop);
    pNoteBook->theme(Antares::Component::Notebook::themeLight);
    // Always create layer 0
    addNewLayer("All");
    pMapActiveLayer = mapLayersPtrList[0];

    pNoteBook->select(wxT("All"));

    // Events
    pNoteBook->onPageChanged.connect(this, &Component::onPageChanged);
    pNoteBook->onAddPage.connect(this, &Component::addNewEmptyLayer);
    pNoteBook->onPageDelete.connect(this, &Component::removeLayer);

    mainSizer->Add(pNoteBook, 1, wxALL | wxEXPAND);

    // Updating the layout

    SetSizer(mainSizer);

    pSelectionPopUpMenu = new wxMenu();
    Menu::CreateItem(pSelectionPopUpMenu,
                     Antares::Forms::MenusID::mnIDPopupSelectionHide,
                     wxT("Hide "),
                     "images/16x16/empty.png",
                     wxEmptyString);
    Menu::CreateItem(pSelectionPopUpMenu,
                     Antares::Forms::MenusID::mnIDPopupSelectionShow,
                     wxT("Show "),
                     "images/16x16/empty.png",
                     wxEmptyString);
    pSelectionPopUpMenu->Connect(Antares::Forms::MenusID::mnIDPopupSelectionHide,
                                 wxEVT_COMMAND_MENU_SELECTED,
                                 wxCommandEventHandler(Component::evtOnSelectionHide),
                                 NULL,
                                 this);
    pSelectionPopUpMenu->Connect(Antares::Forms::MenusID::mnIDPopupSelectionShow,
                                 wxEVT_COMMAND_MENU_SELECTED,
                                 wxCommandEventHandler(Component::evtOnSelectionShow),
                                 NULL,
                                 this);

    mainSizer->Layout();

    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this, &Component::onApplicationQuit);
}

Component::~Component()
{
    // Destroy all bound events as soon as possible to avoid some stupid
    // race conditions
    destroyBoundEvents();

    // Make sure the study is detached to avoid unwanted changes
    detachStudy(false);
    // destroy all components on the map
    if (pMapActiveLayer)
        pMapActiveLayer->nodes.clear();

    // A bit paranoid
    pMapActiveLayer = nullptr;
    pInfosAreaCount = nullptr;
    pInfosConnxCount = nullptr;
    pBtnSelectionArea = nullptr;
    pBtnSelectionLink = nullptr;
    pBtnSelectionPlant = nullptr;
    pBtnSelectionConstraint = nullptr;

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

// TODO
void Component::onPageChanged(Antares::Component::Notebook::Page& page)
{
    for (size_t i = 0, size = pNoteBook->pPages.size(); i < size; i++)
    {
        if (pNoteBook->pPages[i] == &page && i < mapLayersPtrList.size())
        {
            pMapActiveLayer = mapLayersPtrList[i];
            pMapActiveLayer->reset();
            pMapActiveLayer->nodes.study()->activeLayerID = pMapActiveLayer->getUid();
            pMapActiveLayer->pBtnSelectionArea->pushed(pMapActiveLayer->nodes.mouseSelectionArea);
            pMapActiveLayer->pBtnSelectionLink->pushed(pMapActiveLayer->nodes.mouseSelectionLinks);
            pMapActiveLayer->pBtnSelectionConstraint->pushed(
              pMapActiveLayer->nodes.mouseSelectionConstraints);
        }
    }
    refresh();
}

Antares::Component::MapNotebook::Page* Component::addNewLayer(wxString pageName, size_t uID)
{
    wxPanel* p = new wxPanel(pNoteBook, wxID_ANY);

    Control* pMapLayer;
    // The map
    if (uID == 0)
        pMapLayer = new Control(p, *this);
    else
        pMapLayer = new Control(p, *this, uID);

    mapLayersPtrList.push_back(pMapLayer);
    pMapLayer->onPopupEvent.connect(this, &Component::evtOnPopupEvent);
    // The drawer
    auto* externalDrawer = new Drawer(p, *this);
    externalDrawer->SetSize(100, 50);

    // The page's main sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizerV = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* sizerMap = new wxBoxSizer(wxVERTICAL);
    auto* panelHeader = new Antares::Component::Panel(p);

    wxSizer* vs = new wxBoxSizer(wxVERTICAL);
    wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);

    wxPanel* hline = new Antares::Component::Panel(panelHeader);
    hline->SetBackgroundColour(wxColour(170, 170, 170));
    vs->Add(hs, 1, wxALL | wxEXPAND, 1);
    vs->Add(hline, 0, wxALL | wxEXPAND);
    vs->SetItemMinSize(hline, 10, 1);

    panelHeader->SetSizer(vs);
    panelHeader->SetBackgroundColour(Settings::background);

    hs->AddSpacer(15);

    wxWindow* lblMouse
      = Antares::Component::CreateLabel(panelHeader, wxT("mouse :"), false, true, -1);
    lblMouse->SetToolTip(wxT("Mouse selection"));
    hs->Add(lblMouse, 0, wxALL | wxALIGN_CENTER_VERTICAL);
    hs->AddSpacer(4);

    pMapLayer->pBtnSelectionArea
      = new Antares::Component::Button(panelHeader,
                                       wxEmptyString,
                                       "images/16x16/area.png",
                                       this,
                                       &Component::onToggleMouseSelectionArea);
    pMapLayer->pBtnSelectionArea->SetBackgroundColour(Settings::background);
    pMapLayer->pBtnSelectionArea->autoToggle(true);
    pMapLayer->pBtnSelectionArea->pushed(true);
    pBtnSelectionArea = pMapLayer->pBtnSelectionArea;
    hs->Add(pMapLayer->pBtnSelectionArea, 0, wxALL | wxEXPAND);

    pMapLayer->pBtnSelectionLink
      = new Antares::Component::Button(panelHeader,
                                       wxEmptyString,
                                       "images/16x16/link.png",
                                       this,
                                       &Component::onToggleMouseSelectionLink);
    pMapLayer->pBtnSelectionLink->SetBackgroundColour(Settings::background);
    pMapLayer->pBtnSelectionLink->autoToggle(true);
    pMapLayer->pBtnSelectionLink->pushed(true);
    pBtnSelectionLink = pMapLayer->pBtnSelectionLink;
    hs->Add(pMapLayer->pBtnSelectionLink, 0, wxALL | wxEXPAND);

    // auto* btnPlant =
    // 	new Antares::Component::Button(pnlHeader, wxEmptyString, "images/16x16/thermal.png",
    //		this, &Component::onToggleMouseSelectionPlant);
    // btnPlant->SetBackgroundColour(Settings::background);
    // btnPlant->autoToggle(true);
    // pBtnSelectionPlant = btnPlant;
    // hs->Add(btnPlant, 0, wxALL| wxEXPAND);

    pMapLayer->pBtnSelectionConstraint
      = new Antares::Component::Button(panelHeader,
                                       wxEmptyString,
                                       "images/16x16/constraint.png",
                                       this,
                                       &Component::onToggleMouseSelectionConstraint);
    pMapLayer->pBtnSelectionConstraint->SetBackgroundColour(Settings::background);
    pMapLayer->pBtnSelectionConstraint->autoToggle(true);
    pMapLayer->pBtnSelectionConstraint->pushed(true);
    pBtnSelectionConstraint = pMapLayer->pBtnSelectionConstraint;
    hs->Add(pMapLayer->pBtnSelectionConstraint, 0, wxALL | wxEXPAND);

    // separator
    Antares::Component::AddVerticalSeparator(panelHeader, hs);

    // New area
    auto* btnNewArea = new Antares::Component::Button(
      panelHeader, wxEmptyString, "images/16x16/plus.png", this, &Component::onNew);
    btnNewArea->SetBackgroundColour(Settings::background);
    btnNewArea->dropDown(true);
    btnNewArea->onPopupMenu(this, &Component::onNewDropdown);
    hs->Add(btnNewArea, 0, wxALL | wxEXPAND);

    // separator
    Antares::Component::AddVerticalSeparator(panelHeader, hs);

    // Copy
    auto* btnCopy = new Antares::Component::Button(
      panelHeader, wxT("Copy"), "images/16x16/copy.png", this, &Component::onCopy);
    btnCopy->SetBackgroundColour(Settings::background);
    btnCopy->dropDown(true);
    btnCopy->onPopupMenu(this, &Component::onCopyDropdown);
    hs->Add(btnCopy, 0, wxALL | wxEXPAND);

    // Paste
    auto* btnPaste = new Antares::Component::Button(
      panelHeader, wxT("Paste"), "images/16x16/paste.png", this, &Component::onPaste);
    btnPaste->SetBackgroundColour(Settings::background);
    btnPaste->dropDown(true);
    btnPaste->onPopupMenu(this, &Component::onPasteDropdown);
    hs->Add(btnPaste, 0, wxALL | wxEXPAND);

    // Select all
    auto* btnSelectAll = new Antares::Component::Button(
      panelHeader, wxT("Select all"), nullptr, this, &Component::onSelectAll);
    btnSelectAll->SetBackgroundColour(Settings::background);
    hs->Add(btnSelectAll, 0, wxALL | wxEXPAND);

    // separator
    Antares::Component::AddVerticalSeparator(panelHeader, hs);

    // Re-center
    auto* btnCenter = new Antares::Component::Button(
      panelHeader, wxEmptyString, "images/16x16/target.png", this, &Component::onCenterXY);
    btnCenter->SetBackgroundColour(Settings::background);
    hs->Add(btnCenter, 0, wxALL | wxEXPAND);

    /*(wxTextCtrl*)*/
    wxWindow* lblX = Antares::Component::CreateLabel(panelHeader, wxT("x :"), false, true, -1);
    lblX->SetToolTip(wxT("X coordinate"));
    hs->Add(lblX, 0, wxALL | wxALIGN_CENTER_VERTICAL);
    hs->AddSpacer(4);

    wxString testString;
    auto* pointX = new wxTextCtrl(panelHeader,
                                  wxID_ANY,
                                  wxT("0"),
                                  wxDefaultPosition,
                                  wxSize(50, -1),
                                  wxTE_PROCESS_ENTER,
                                  wxTextValidator(wxFILTER_NUMERIC, &testString));
    pointX->SetBackgroundColour(Settings::background);
    pointX->Connect(
      wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(Component::onEvtCenterXY), 0, this);
    hs->Add(pointX, 0, wxALL | wxEXPAND);
    pMapLayer->setPointX(pointX);
    hs->AddSpacer(2);

    wxWindow* lblY = Antares::Component::CreateLabel(panelHeader, wxT("y :"), false, true, -1);
    lblY->SetToolTip(wxT("Y coordinate"));
    hs->Add(lblY, 0, wxALL | wxALIGN_CENTER_VERTICAL);
    hs->AddSpacer(4);

    wxString testString2;
    auto* pointY = new wxTextCtrl(panelHeader,
                                  wxID_ANY,
                                  wxT("0"),
                                  wxDefaultPosition,
                                  wxSize(50, -1),
                                  wxTE_PROCESS_ENTER,
                                  wxTextValidator(wxFILTER_NUMERIC, &testString2));
    pointY->SetBackgroundColour(Settings::background);
    pointY->Connect(
      wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(Component::onEvtCenterXY), 0, this);
    hs->Add(pointY, 0, wxALL | wxEXPAND);
    pMapLayer->setPointY(pointY);
    hs->AddSpacer(2);

    // onTextEnter.connect(this, &Component::onCenterXY);

    // separator
    Antares::Component::AddVerticalSeparator(panelHeader, hs);

    // Re-size
    auto* btnResize = new Antares::Component::Button(
      panelHeader, wxEmptyString, "images/16x16/crop.png", this, &Component::onCenter);
    btnResize->SetBackgroundColour(Settings::background);
    hs->Add(btnResize, 0, wxALL | wxEXPAND);

    // Spacer
    hs->AddStretchSpacer();

    // N areas
    pInfosAreaCount = Antares::Component::CreateLabel(panelHeader, wxT("0 area"));
    pInfosAreaCount->SetForegroundColour(wxColour(62, 141, 94));
    pMapLayer->setpInfosAreaCount(pInfosAreaCount);
    hs->Add(pInfosAreaCount, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL);

    // N connexions
    pInfosConnxCount = Antares::Component::CreateLabel(panelHeader, wxT("0 link"));
    pInfosConnxCount->SetForegroundColour(wxColour(127, 143, 244));
    pMapLayer->setpInfosConnxCount(pInfosConnxCount);
    hs->Add(pInfosConnxCount, 0, wxALL | wxALIGN_CENTER_VERTICAL);
    hs->AddSpacer(8);

    sizerMap->Add(panelHeader, 0, wxALL | wxEXPAND);
    sizerMap->AddSpacer(1);
    sizerMap->Add(pMapLayer, 1, wxALL | wxEXPAND);

    sizerV->Add(sizerMap, 1, wxALL | wxEXPAND);
    sizer->Add(sizerV, 1, wxALL | wxEXPAND);

    // The drawer
    sizer->Add(externalDrawer, 0, wxALL | wxEXPAND);
    sizer->Show(externalDrawer, false);

    p->SetSizer(sizer);

    sizer->Layout();
    sizerV->Layout();
    Antares::Component::MapNotebook::Page* resultPage;
    if (pageName == wxString(""))
    {
        size_t numberOffset = 0;

        while (
          pNoteBook->find(wxString::FromUTF8("Map ") << pMapLayer->getUid() + numberOffset, false)
          != nullptr)
            numberOffset++;

        resultPage
          = pNoteBook->add(p,
                           wxString::FromUTF8("Map ") << pMapLayer->getUid() + numberOffset,
                           wxString::FromUTF8("Map ") << pMapLayer->getUid() + numberOffset);
        Antares::MarkTheStudyAsModified();
    }
    else
    {
        resultPage = pNoteBook->add(p, pageName, pageName);
    }

    pMapLayer->pPage = resultPage;

    if (pMapLayer->nodes.study() != nullptr)
        pMapLayer->nodes.study()->layers[pMapLayer->getUid()] = resultPage->name().ToStdString();

    OnMapLayerAdded(&resultPage->name());

    return resultPage;
}

void Component::removeLayer(Antares::Component::MapNotebook::Page& page)
{
    if (pNoteBook->pPages[0]
        == &page) // close action on the first layer is dealed with in a specific way
    {
        pMapActiveLayer->nodes.study()->showAllLayer = false;
        pNoteBook->pPages[0]->visible(false);
        pNoteBook->select(pNoteBook->pPages[1]->name());
        Antares::MarkTheStudyAsModified();
        return;
    }

    for (size_t i = 1, size = pNoteBook->pPages.size(); i < size; i++) // ignoring first Layer (All)
    {
        if (pNoteBook->pPages[i] == &page && i < mapLayersPtrList.size() && i != 0)
        {
            size_t guid = mapLayersPtrList[i]->getUid();
            pMapActiveLayer->nodes.removeLayerVisibility(guid);
            mapLayersPtrList.erase(mapLayersPtrList.begin() + i);
            pMapActiveLayer->nodes.study()->layers.erase(guid);
            OnMapLayerRemoved(&page.name());
            pNoteBook->remove(&page);
            if (pNoteBook->pPages[0]->visible())
            {
                pNoteBook->select(wxT("All"));
                pMapActiveLayer = mapLayersPtrList[0];
                pMapActiveLayer->nodes.study()->activeLayerID = pMapActiveLayer->getUid();
            }
            else
            {
                pNoteBook->select(pNoteBook->pPages[1]->name());
                pMapActiveLayer = mapLayersPtrList[1];
                pMapActiveLayer->nodes.study()->activeLayerID = pMapActiveLayer->getUid();
            }

            Antares::MarkTheStudyAsModified();
            break;
        }
    }
}

void Component::drawerVisible(bool v)
{
    wxSizer* sizer = GetSizer();
    if (sizer)
    {
        sizer->Layout();
    }
}

void Component::showLayerAll()
{
    pNoteBook->pPages[0]->visible(true);
    pNoteBook->select(wxT("All"));
    pMapActiveLayer = mapLayersPtrList[0];
    pMapActiveLayer->nodes.study()->activeLayerID = pMapActiveLayer->getUid();
    pMapActiveLayer->nodes.study()->showAllLayer = true;
    Antares::MarkTheStudyAsModified();
}

void Component::attachStudy(Data::Study::Ptr study)
{
    unselectAll();
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.attachStudy(study);

        for (size_t i = 1, size = pNoteBook->pPages.size(); i < size; i++)
        {
            mapLayersPtrList.erase(mapLayersPtrList.begin() + 1);
            OnMapLayerRemoved(&(pNoteBook->pPages[1]->name()));
            pNoteBook->remove(pNoteBook->pPages[1]);
        }

        pMapActiveLayer = mapLayersPtrList[0];
        pNoteBook->select(pNoteBook->pPages[0]->name(), false);
        pNoteBook->pPages[0]->visible(true);

        if (pMapActiveLayer->nodes.study()->layers.size() > 0)
        {
            // Create the layers according to study Content
            std::map<size_t, std::string>::iterator iterator
              = pMapActiveLayer->nodes.study()->layers.begin();
            mapLayersPtrList[0]->pPage->name(iterator->second);
            size_t i = 1;
            for (iterator++; iterator != pMapActiveLayer->nodes.study()->layers.end();
                 iterator++, i++)
            {
                Antares::Component::Notebook::Page* newPage
                  = addNewLayer(iterator->second, iterator->first);
                size_t activeLayerID = pMapActiveLayer->nodes.study()->activeLayerID;
                if (activeLayerID == iterator->first)
                {
                    pMapActiveLayer = mapLayersPtrList[i];
                    pNoteBook->select(newPage->name());
                    pNoteBook->onPageChanged(*newPage);
                    OnMapLayerChanged(&newPage->name());
                }
            }

            if (pMapActiveLayer->nodes.study()->showAllLayer == false)
            {
                pNoteBook->pPages[0]->visible(false);
                // pNoteBook->select(pNoteBook->pPages[1]->name());
            }
        }
        else
        {
            pMapActiveLayer->nodes.study()->layers[0] = "All";
        }
    }
}

void Component::detachStudy(bool canRefresh)
{
    unselectAll(canRefresh);
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.attachStudy(nullptr);
    }
}

Data::Study::Ptr Component::attachedStudy()
{
    return pMapActiveLayer ? pMapActiveLayer->nodes.study() : nullptr;
}

bool Component::loadFromStudy(Data::Study& study)
{
    if (!pMapActiveLayer)
        return false;

    logs.debug() << "[map] loading from study...";
    pMapActiveLayer->nodes.beginUpdate();
    unselectAll();
    bool ret = pMapActiveLayer->nodes.loadFromStudy(study);
    pMapActiveLayer->nodes.endUpdate();
    Dispatcher::GUI::Refresh(this);
    return ret;
}

void Component::clear()
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->nodes.beginUpdate();
        unselectAll();
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.clear();
        pMapActiveLayer->nodes.endUpdate();
    }
}

bool Component::loadFromAttachedStudy()
{
    if (!pMapActiveLayer)
        return false;

    logs.debug() << "[map] loading from study...";
    pMapActiveLayer->nodes.beginUpdate();
    unselectAll();
    pMapActiveLayer->reset();
    bool ret = pMapActiveLayer->nodes.loadFromAttachedStudy();
    pMapActiveLayer->nodes.endUpdate();
    Dispatcher::GUI::Refresh(this);
    return ret;
}

bool Component::saveToStudy(Data::Study& /*study*/, bool incremental)
{
    if (!pMapActiveLayer)
        return false;

    if (incremental)
    {
        if (pMapActiveLayer->nodes.hasChanges())
            pMapActiveLayer->nodes.setChangesFlag(false);
        return true;
    }
    pMapActiveLayer->nodes.setChangesFlag(false);
    return true;
}

bool Component::saveToAttachedStudy(bool incremental)
{
    auto study = attachedStudy();
    return (!(!study)) ? saveToStudy(*study, incremental) : false;
}

bool Component::saveToImageFile(const AnyString& filePath,
                                const Antares::Map::MapRenderOptions& options)
{
    if (not pMapActiveLayer)
        return false;

    if (filePath.empty())
        return false;

    switch (options.fileFormat)
    {
    case mfPNG:
    case mfJPG:
    {
        wxBitmap imageBmp = pMapActiveLayer->getRenderedMapImage(options);
        imageBmp.SaveFile(wxStringFromUTF8(filePath), formatWXConstant[options.fileFormat]);

        break;
    }

    case mfSVG:
    {
        // The image should contain no more than the areas boxing
        BoundingBox box = pMapActiveLayer->nodes.boundingBox(pMapActiveLayer->getUid());
        wxRect boundsRect(box.first, box.second);
        // That is another matter
        wxSVGFileDC svgDC(
          wxStringFromUTF8(filePath), boundsRect.GetWidth() + 80, boundsRect.GetHeight() + 80);

        if (options.transparentBackground)
            pMapActiveLayer->setBackgroundColor(wxNullColour);
        else
            pMapActiveLayer->setBackgroundColor(options.backgroundColor);

        pMapActiveLayer->paintGraph(svgDC, true);

        pMapActiveLayer->setBackgroundColor(Settings::background);

        return svgDC.IsOk();
        // break;
    }
    }

    return true; // all was fine
}

void Component::setFocus()
{
    assert(pMapActiveLayer);
    if (pMapActiveLayer)
        pMapActiveLayer->SetFocus();
}

void Component::Drawer::onDraw(wxPaintEvent&)
{
    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc); // wxScrolledWindow
    // Draw all items
    drawFromDevice(dc);
}

void Component::Drawer::onSize(wxSizeEvent& evt)
{
    Dispatcher::GUI::Refresh(this);
    evt.Skip();
}

void Component::Drawer::drawFromDevice(wxDC& dc)
{
    // Reset the font
    dc.SetFont(*wxSWISS_FONT);

    // null point
    static const wxPoint nullPoint = wxPoint(0, 0);
    // Bounding box
    const wxRect& r = this->GetRect();
    const wxPoint size(r.GetWidth(), r.GetHeight());

    // Drawing context
    DrawingContext drawingContext(dc,        // DC
                                  nullPoint, // origin
                                  nullPoint, // offset
                                  nullPoint, // scroll
                                  size,      // clientSize
                                  size);     // bottom right

    // ReDraw the map
    if (pComponent.pMapActiveLayer)
        pComponent.pMapActiveLayer->nodes.drawExternalDrawer(drawingContext);
}

void Component::invalidate()
{
    if (pMapActiveLayer)
        pMapActiveLayer->nodes.invalidateAllNodes();
}

void Component::refresh()
{
    refreshHeaderInformations();
    Dispatcher::GUI::Refresh(pMapActiveLayer);
    Dispatcher::GUI::Refresh(this);
}

void Component::refreshHeaderInformations()
{
    if (pMapActiveLayer)
    {
        uint cArea = pMapActiveLayer->areasCount();
        pInfosAreaCount = pMapActiveLayer->getpInfosAreaCount();
        uint cCnnx = pMapActiveLayer->connectionsCount();

        if (cArea < 2)
            pMapActiveLayer->getpInfosAreaCount()->SetLabel(wxString(wxT(' '))
                                                            << cArea << wxT(" area, "));
        else
            pMapActiveLayer->getpInfosAreaCount()->SetLabel(wxString(wxT(' '))
                                                            << cArea << wxT(" areas, "));

        if (cCnnx < 2)
            pMapActiveLayer->getpInfosConnxCount()->SetLabel(wxString() << cCnnx << wxT(" link "));
        else
            pMapActiveLayer->getpInfosConnxCount()->SetLabel(wxString() << cCnnx << wxT(" links "));
    }
    else
    {
        pInfosAreaCount->SetLabel(wxT("0 area"));
        pInfosConnxCount->SetLabel(wxT("0 link"));
    }
    pInfosAreaCount->GetParent()->GetSizer()->Layout();
}

void Component::onApplicationQuit()
{
    detachStudy();
}

void Component::renameNodeFromArea(const Data::Area* area)
{
    if (pMapActiveLayer && area)
    {
        auto* node = pMapActiveLayer->nodes.find(area);
        if (node)
            node->caption(wxStringFromUTF8(area->name));
    }
}

void Component::reimportNodeColors(const Data::Area* area)
{
    if (pMapActiveLayer && area)
    {
        auto* node = pMapActiveLayer->nodes.find(area);
        if (node)
            node->color(area->ui->color[0], area->ui->color[1], area->ui->color[2]);
    }
}

void Component::moveNodeFromAreaX(const Data::Area* area, int x)
{
    if (pMapActiveLayer && area)
    {
        auto* node = pMapActiveLayer->nodes.find(area);
        if (node)
            node->x(x);
    }
}

void Component::moveNodeFromAreaY(const Data::Area* area, int y)
{
    if (pMapActiveLayer && area)
    {
        auto* node = pMapActiveLayer->nodes.find(area);
        if (node)
            node->y(y);
    }
}

uint Component::selectedAreaCount() const
{
    return pMapActiveLayer ? pMapActiveLayer->nodes.selectedItemsCount() : 0u;
}

void Component::unselectAll(bool canRefresh)
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.unselectAll();
        if (canRefresh)
            refresh();
    }
}

void Component::selectAll()
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.selectAllItems(pMapActiveLayer->getUid());
        refresh();
    }
}

void Component::selectOnly(const Data::Area::Vector& areas)
{
    if (areas.empty())
    {
        unselectAll();
    }
    else
    {
        if (pMapActiveLayer)
        {
            pMapActiveLayer->reset();
            pMapActiveLayer->nodes.selectOnly(areas);
            refresh();
        }
    }
}

void Component::selectOnly(const Data::Area::Vector& areas, const Data::AreaLink::Vector& links)
{
    if (areas.empty() && links.empty())
    {
        unselectAll();
    }
    else
    {
        if (pMapActiveLayer)
        {
            pMapActiveLayer->reset();
            pMapActiveLayer->nodes.selectOnly(areas, links);
            refresh();
        }
    }
}

void Component::reverseSelection()
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.reverseSelection();
        pMapActiveLayer->invalidate();
        refresh();
    }
}

void Component::evtOnPopupEvent(int x, int y)
{
    // onPopupEvent(x, y);
    if (mapLayersPtrList[0] != pMapActiveLayer)
        PopupMenu(pSelectionPopUpMenu, x + 1, y + 48);
}

void Component::evtOnSelectionHide(wxCommandEvent&)
{
    pMapActiveLayer->nodes.hideAllSelected(pMapActiveLayer->getUid());
    pMapActiveLayer->computeBoundingBoxOfSelectedNodes();
    Antares::MarkTheStudyAsModified();
    refresh();
}

void Component::evtOnSelectionShow(wxCommandEvent&)
{
    pMapActiveLayer->nodes.showAllSelected(pMapActiveLayer->getUid());
    pMapActiveLayer->computeBoundingBoxOfSelectedNodes();
    Antares::MarkTheStudyAsModified();
    refresh();
}

void Component::onSelectAll(void*)
{
    selectAll();
}

void Component::onCopy(void*)
{
    if (pMapActiveLayer)
        pMapActiveLayer->reset();

    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->copyToClipboard();
}

void Component::onNew(void*)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode();
}

void Component::evtPopupCopy(wxCommandEvent&)
{
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->copyToClipboard();
}

void Component::evtPopupCopyAll(wxCommandEvent&)
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.selectAllItems(pMapActiveLayer->getUid());
        Dispatcher::GUI::Refresh(this);
    }
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->copyToClipboard();
}

void Component::evtPopupCopyAllAreas(wxCommandEvent&)
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.selectAllAreas(pMapActiveLayer->getUid());
        Dispatcher::GUI::Refresh(this);
    }
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->copyToClipboard();
}

void Component::evtPopupCopyAllLinks(wxCommandEvent&)
{
    if (pMapActiveLayer)
    {
        pMapActiveLayer->reset();
        pMapActiveLayer->nodes.selectAllLinks(pMapActiveLayer->getUid());
        Dispatcher::GUI::Refresh(this);
    }
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->copyToClipboard();
}

void Component::onCopyDropdown(Antares::Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Copy the selection to the clipboard\tCtrl+C"), "images/16x16/copy.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupCopy),
                 nullptr,
                 this);

    menu.AppendSeparator();

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Copy all items"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupCopyAll),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Copy all areas"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupCopyAllAreas),
                 nullptr,
                 this);
    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Copy all links"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupCopyAllLinks),
                 nullptr,
                 this);
}

void Component::onPaste(void*)
{
    if (pMapActiveLayer)
        pMapActiveLayer->reset();
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->pasteFromClipboard(false);
}

void Component::evtPopupPaste(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->reset();
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->pasteFromClipboard(false);
}

void Component::evtPopupPasteSpecial(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->reset();
    auto* mainFrm = Forms::ApplWnd::Instance();
    if (mainFrm)
        mainFrm->pasteFromClipboard(true);
}

void Component::onPasteDropdown(Antares::Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Paste\tCtrl+V"), "images/16x16/copy.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupPaste),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Paste special..."), "images/16x16/empty.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtPopupPasteSpecial),
                 nullptr,
                 this);
}

void Component::onToggleMouseSelectionArea(void*)
{
    typedef Antares::Component::Button ButtonType;
    auto* button = dynamic_cast<ButtonType*>(pBtnSelectionArea);
    if (button)
        pMapActiveLayer->nodes.mouseSelectionArea = !pMapActiveLayer->nodes.mouseSelectionArea;
}

void Component::onToggleMouseSelectionLink(void*)
{
    pMapActiveLayer->nodes.mouseSelectionLinks = !pMapActiveLayer->nodes.mouseSelectionLinks;
}

void Component::onToggleMouseSelectionPlant(void*)
{
    pMapActiveLayer->nodes.mouseSelectionPlants = !pMapActiveLayer->nodes.mouseSelectionPlants;
}

void Component::onToggleMouseSelectionConstraint(void*)
{
    pMapActiveLayer->nodes.mouseSelectionConstraints
      = !pMapActiveLayer->nodes.mouseSelectionConstraints;
}

void Component::onNewDropdown(Antares::Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Create a new area"), "images/16x16/area.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtNewArea),
                 nullptr,
                 this);

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Tip : Press N to directly create a new area"), nullptr);
    it->Enable(false);

    menu.AppendSeparator();

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Create 2 new areas"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtNewArea2),
                 nullptr,
                 this);
    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Create 4 new areas"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtNewArea4),
                 nullptr,
                 this);
    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Create 6 new areas"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtNewArea6),
                 nullptr,
                 this);
    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Create 8 new areas"), nullptr);
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::evtNewArea8),
                 nullptr,
                 this);
}

void Component::evtNewArea(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode();
}

void Component::evtNewArea2(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode(2);
}

void Component::evtNewArea4(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode(4);
}

void Component::evtNewArea6(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode(6);
}

void Component::evtNewArea8(wxCommandEvent&)
{
    if (pMapActiveLayer)
        pMapActiveLayer->addNewNode(8);
}

void Component::recenterView()
{
    for (int i = 0, size = mapLayersPtrList.size(); i < size; i++)
        if (mapLayersPtrList[i])
        {
            Yuni::Bind<void()> callback;
            callback.bind(mapLayersPtrList[i], &Control::recenter);
            Dispatcher::GUI::Post(callback, 20); // arbitrary
        }
}

void Component::onCenter(void*)
{
    if (pMapActiveLayer)
        pMapActiveLayer->recenter();
}

void Component::onCenterXY(void*)
{
    std::string xValue(this->pMapActiveLayer->getPointX()->GetValue().mb_str());
    std::string yValue(this->pMapActiveLayer->getPointY()->GetValue().mb_str());

    if (pMapActiveLayer)
    {
        pMapActiveLayer->recenter(atoi(xValue.c_str()), atoi(yValue.c_str()));
    }
}

void Component::onEvtCenterXY(wxCommandEvent& WXUNUSED(event))
{
    onCenterXY(this);
}

} // namespace Map
} // namespace Antares
