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
#include "area.h"
#include <wx/sizer.h>
#include "../../application/study.h"
#include "../../application/main.h"
#include "../../windows/inspector.h"
#include "../resources.h"
#include <cassert>
#include <ui/common/component/panel.h>
#include <ui/common/component/spotlight.h>
#include <ui/common/lock.h>
#include "../spotlight/area.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
// static variables
wxPanel* Area::pSharedSupport = nullptr;
Yuni::Event<void(Data::Area*)> Area::onAreaChanged;

//! Reference to the last area
static Data::Area* gLastArea = nullptr;

class SpotlightProviderArea final : public Component::Spotlight::IProvider
{
public:
    using Spotlight = Antares::Component::Spotlight;

public:
    SpotlightProviderArea() : pAutoTriggerSelection(true)
    {
        OnStudyAreaRename.connect(this, &SpotlightProviderArea::onStudyAreaRename);
        OnStudyEndUpdate.connect(this, &SpotlightProviderArea::onStudyEndUpdate);
        OnStudyAreasChanged.connect(this, &SpotlightProviderArea::onStudyAreasChanged);
        OnStudyAreaDelete.connect(this, &SpotlightProviderArea::onStudyAreaDeleted);
        OnStudyAreaAdded.connect(this, &SpotlightProviderArea::onStudyAreaAdded);
        OnStudyAreaColorChanged.connect(this, &SpotlightProviderArea::onStudyAreaColorChanged);
        OnStudyClosed.connect(this, &SpotlightProviderArea::onStudyClosed);
        OnMainNotebookChanged.connect(this, &SpotlightProviderArea::onMainNotebookChanged);
    }

    virtual ~SpotlightProviderArea()
    {
        destroyBoundEvents(); // avoid corrupt vtable
        gLastArea = nullptr;
    }

    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        const Yuni::String& text) override
    {
        if (not CurrentStudyIsValid())
            return;
        auto& study = *GetCurrentStudy();
        if (study.areas.empty())
            return;

        std::vector<Antares::Data::Area*> layerFilteredItems;
        if (!text.empty())
        {
            auto layerListEnd = study.layers.end();
            for (auto layerIt = study.layers.begin(); layerIt != layerListEnd; layerIt++)
            {
                if (layerIt->second == text)
                {
                    auto end = study.areas.end();
                    for (auto i = study.areas.begin(); i != end; ++i)
                    {
                        auto* area = i->second;
                        if (!area)
                            continue;

                        if (area->isVisibleOnLayer(layerIt->first))
                            layerFilteredItems.push_back(area);
                    }

                    if (gLastArea && !gLastArea->isVisibleOnLayer(layerIt->first))
                    {
                        gLastArea = nullptr;
                        pLastAreaID.clear();
                    }

                    break;
                }
            }
        }

        search(out, tokens, layerFilteredItems);
    }

    virtual void search(Spotlight::IItem::Vector& out,
                        const Spotlight::SearchToken::Vector& tokens,
                        std::vector<Antares::Data::Area*>& in)
    {
        if (not CurrentStudyIsValid())
            return;
        auto& study = *GetCurrentStudy();
        if (study.areas.empty())
            return;

        uint currentEquipment = 0;
        auto* mainFrm = Forms::ApplWnd::Instance();
        if (mainFrm)
            currentEquipment = mainFrm->mainNotebookCurrentEquipmentPage();

        if (tokens.empty())
        {
            for (auto i = in.begin(); i != in.end(); ++i)
            {
                auto* area = *i;
                if (area)
                    pushArea(out, area, currentEquipment);
            }
        }
        else
        {
            auto tend = tokens.end();

            for (auto i = in.begin(); i != in.end(); ++i)
            {
                auto* area = *i;
                if (!area)
                    continue;

                for (auto ti = tokens.begin(); ti != tend; ++ti)
                {
                    const String& text = (*ti)->text;
                    if (area->name.icontains(text))
                        pushArea(out, area, currentEquipment);
                }
            }
        }

        // We may have no selection at all. For this container, we always want
        // something selected
        if (!pLastAreaID || !out.empty())
        {
            Dispatcher::GUI::Post(this, &SpotlightProviderArea::broadcastAreaChange, 20);
        }
    }

    virtual bool onSelect(Spotlight::IItem::Ptr& item) override
    {
        if (!CurrentStudyIsValid() || GUIIsLock())
            return false;
        GUILocker locker;
        auto itemarea = std::dynamic_pointer_cast<Toolbox::Spotlight::ItemArea>(item);
        if (!(!itemarea))
        {
            auto area = itemarea->area;
            if (area)
            {
                // pAutoTriggerSelection = false;
                pLastAreaID = area->id;
                gLastArea = area;

                // To reduce flickering, this event should be delayed
                Dispatcher::GUI::Post(this, &SpotlightProviderArea::broadcastAreaChange, 20);

                // redoResearch();

                // Updating the inspector
                Window::Inspector::SelectArea(area);
                Window::Inspector::Refresh();

                return true;
            }
        }
        return false;
    }

    virtual bool onSelect(const Spotlight::IItem::Vector&) override
    {
        if (!CurrentStudyIsValid() || GUIIsLock())
            return false;
        GUILocker locker;
        return true;
    }

protected:
    void broadcastAreaChange()
    {
        Area::onAreaChanged(gLastArea);
    }

    void pushArea(Spotlight::IItem::Vector& out, Data::Area* area, uint equipment)
    {
        assert(area && "Invalid area");

        auto item = std::make_shared<Toolbox::Spotlight::ItemArea>(area);
        if (area->id == pLastAreaID)
            item->select();

        if (0 != (equipment & Data::timeSeriesThermal))
        {
            if (area->thermal.list.allClustersCount() > 0)
            {
                CString<32, false> text;
                text << area->thermal.list.allClustersCount();
                const uint8_t R_COLOR = 210;
                const uint8_t G_COLOR = 217;
                const uint8_t B_COLOR = 216;
                item->addRightTag(text, R_COLOR, G_COLOR, B_COLOR);
                if (area->thermal.hasForcedTimeseriesGeneration())
                    item->addRightTag(wxT("G"), R_COLOR, G_COLOR, B_COLOR);
                if (area->thermal.hasForcedNoTimeseriesGeneration())
                    item->addRightTag(wxT("NG"), R_COLOR, G_COLOR, B_COLOR);
            }
        }
        if (0 != (equipment & Data::timeSeriesRenewable))
        {
            if (area->renewable.list.allClustersCount() > 0)
            {
                CString<32, false> text;
                text << area->renewable.list.allClustersCount();
                item->addRightTag(text, 210, 217, 216);
            }
        }

        out.push_back(item);
    }

    void onStudyEndUpdate()
    {
        redoResearch();
    }

    void onMainNotebookChanged()
    {
        refresh();
    }

    void refresh()
    {
        if (CurrentStudyIsValid())
        {
            GUILocker locker;
            redoResearch();
        }
    }

    void onStudyAreasChanged()
    {
        pAutoTriggerSelection = true;
        redoResearch();
    }

    void onStudyAreaAdded(Data::Area*)
    {
        GUILocker locker;
        redoResearch();
    }

    void onStudyAreaColorChanged(Data::Area*)
    {
        GUILocker locker;
        redoResearch();
    }

    void onStudyAreaDeleted(Data::Area* area)
    {
        GUILocker locker;
        if (area && area->id == pLastAreaID)
        {
            pLastAreaID.clear();
            gLastArea = nullptr;
        }
        pAutoTriggerSelection = true;
        redoResearch();
    }

    void onStudyAreaRename(Data::Area* area)
    {
        GUILocker locker;
        pAutoTriggerSelection = true;
        if (gLastArea == area)
            pLastAreaID = area->id;
        redoResearch();
    }

    void onStudyClosed()
    {
        GUILocker locker;
        pLastAreaID.clear();
        gLastArea = nullptr;
        pAutoTriggerSelection = true;
        if (component())
        {
            // The component will be updated
            component()->resetSearchInput();
        }
        else
            redoResearch(); // automatically called by resetSearchInput
    }

private:
    //! Last area name
    Data::AreaName pLastAreaID;
    //! Flag to know if the component must trigger the selection of an item
    bool pAutoTriggerSelection;

}; // class SpotlightProviderArea

BEGIN_EVENT_TABLE(Area, Component::Panel)
EVT_PAINT(Area::onDraw)
END_EVENT_TABLE()

Area::Area(wxWindow* parent) : AInput(parent)
{
    SetSize(300, 300);
    // sizer
    SetSizer(new wxBoxSizer(wxVERTICAL));
    this->internalBuildSubControls();
    this->update();

    // Connect to the global event
    OnStudyEndUpdate.connect(this, &Area::onStudyEndUpdate);

    // The listbox must be empty to avoid SegV at exit
    Forms::ApplWnd::Instance()->onApplicationQuit.connect(this, &Area::onApplicationOnQuit);
    OnStudyClosed.connect(this, &Area::onStudyClosed);
}

Area::~Area()
{
    if (pSharedSupport && pSharedSupport->GetParent() == dynamic_cast<wxWindow*>(this))
        pSharedSupport = nullptr;

    onAreaChanged(nullptr);
    destroyBoundEvents();
    clear();
}

Data::Area* Area::lastArea()
{
    return gLastArea;
}

void Area::internalBuildSubControls()
{
    if (pSharedSupport)
        return;

    // alias to the sizer of the panel
    wxSizer* sizer = GetSizer();
    assert(sizer);

    // Creating the shared support
    pSharedSupport = new Component::Panel(this);
    wxSizer* subsizer = new wxBoxSizer(wxVERTICAL);

    {
        Component::Spotlight* spotlight
          = new Component::Spotlight(pSharedSupport, 0); // Component::Spotlight::optGroups);
        OnLayerNodeUIChanged.connect(spotlight, &Component::Spotlight::redoResearch);
        OnMapLayerChanged.connect(spotlight, &Component::Spotlight::onMapLayerChanged);
        OnMapLayerAdded.connect(spotlight, &Component::Spotlight::onMapLayerAdded);
        OnMapLayerRemoved.connect(spotlight, &Component::Spotlight::onMapLayerRemoved);
        OnMapLayerRenamed.connect(spotlight, &Component::Spotlight::onMapLayerRenamed);
        spotlight->provider(std::make_shared<SpotlightProviderArea>());
        wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
        hz->AddSpacer(5);
        hz->Add(spotlight, 1, wxALL | wxEXPAND);
        subsizer->AddSpacer(2);
        subsizer->Add(hz, 1, wxALL | wxEXPAND);
    }

    pSharedSupport->SetSizer(subsizer);

    subsizer->Layout();

    sizer->Add(pSharedSupport, 1, wxALL | wxEXPAND);
    sizer->Layout();
}

void Area::clear()
{
    GUILocker locker;
    /*if (pSharedSupport)
    {
            pSharedSupport->Destroy();
            pSharedSupport = nullptr;
    }*/

    // Broadcasting the news
    gLastArea = nullptr;
    onAreaChanged(nullptr);
    Refresh();
}

void Area::onApplicationOnQuit()
{
    clear();
}

void Area::reloadLastArea()
{
    GUILocker locker;
    // Reload
    if (gLastArea)
    {
        Data::Area* last = gLastArea;
        gLastArea = nullptr;
        onAreaChanged(nullptr);
        gLastArea = last;
        onAreaChanged(gLastArea);
    }
    else
        onAreaChanged(nullptr);
}

void Area::onStudyEndUpdate()
{
    GUILocker locker;
    // We will force an update of the current area
    // Since there is a check with the last selected area, we have to reset it
    // to null first
    onAreaChanged(nullptr);
    // The action must be delayed
    Dispatcher::GUI::Post(this, &Area::reloadLastArea, 10);
}

void Area::onStudyClosed()
{
    clear();
}

void Area::onDraw(wxPaintEvent& evt)
{
    if (!pSharedSupport)
    {
        internalBuildSubControls();
    }
    else
    {
        if (pSharedSupport->GetParent() != dynamic_cast<wxWindow*>(this))
        {
            assert(pSharedSupport->GetParent());
            assert(pSharedSupport->GetParent()->GetSizer());

            wxSizer* srcSizer = pSharedSupport->GetContainingSizer();
            if (srcSizer)
                srcSizer->Detach(pSharedSupport);

            // parent
            pSharedSupport->Reparent(this);

            if (srcSizer)
                srcSizer->Layout();
            wxSizer* localSizer = GetSizer();
            assert(localSizer);
            localSizer->Add(pSharedSupport, 1, wxALL | wxEXPAND);
            localSizer->Layout();
        }
    }

    evt.Skip();
}

void Area::update()
{
}

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares
