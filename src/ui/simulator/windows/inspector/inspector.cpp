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

#include <antares/wx-wrapper.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include "../../application/main.h"
#include <wx/aui/aui.h>

#include "inspector.h"
#include "frame.h"
#include "../../application/study.h"
#include "../../toolbox/clipboard/clipboard.h"
#include "../../../common/lock.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Inspector
{
static void DelayedUpdate()
{
    if (gInspector)
        gInspector->apply(gData);
}

void Destroy()
{
    gData = nullptr;

    if (gInspector)
    {
        gInspector->apply(nullptr);
        Hide();
        gInspector->detachFromTheMainForm();
        gInspector->Destroy();
        gInspector = nullptr;
    }
}

void Refresh()
{
    Bind<void()> callback;
    callback.bind(&DelayedUpdate);
    Dispatcher::GUI::Post(callback);
}

void Unselect()
{
    gData = nullptr;
    if (gInspector)
        gInspector->apply(nullptr);
}

uint SelectionAreaCount()
{
    return ((!(!gData)) ? (uint)gData->areas.size() : 0);
}

uint SelectionLinksCount()
{
    return ((!(!gData)) ? (uint)gData->links.size() : 0);
}

// gp : do we have to add a renewable counterpart ? Check where it is called
uint SelectionThermalClusterCount()
{
    return ((!(!gData)) ? (uint)gData->ThClusters.size() : 0);
}

uint SelectionBindingConstraintCount()
{
    return ((!(!gData)) ? (uint)gData->constraints.size() : 0);
}

// gp : we should add contribution of renewable clusters. Where is this function used and why ?
uint SelectionTotalCount()
{
    return (!(!gData)) ? (uint)gData->constraints.size() + (uint)gData->ThClusters.size()
                           + (uint)gData->links.size() + (uint)gData->areas.size()
                       : 0;
}

void Hide()
{
    if (IsGUIAboutToQuit())
        return;

    if (gInspector)
    {
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        wxAuiPaneInfo& pnl = mainFrm.AUIManager().GetPane(gInspector);
        pnl.Hide();
        mainFrm.AUIManager().Update();
    }
}

void Show()
{
    if (IsGUIAboutToQuit())
        return;

    if (!gInspector)
    {
        gInspector = new Frame(Antares::Forms::ApplWnd::Instance(), true);
        gInspector->attachToTheMainForm();

        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        wxAuiPaneInfo& pnl = mainFrm.AUIManager().GetPane(gInspector);
        pnl.Show();
        mainFrm.AUIManager().Update();
        mainFrm.GetSizer()->Layout();
        mainFrm.forceRefresh();
    }
    else
    {
        if (not gInspector->IsShown())
        {
            auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
            auto& pnl = mainFrm.AUIManager().GetPane(gInspector);
            pnl.Show();
            mainFrm.AUIManager().Update();
            mainFrm.GetSizer()->Layout();
            mainFrm.forceRefresh();
        }
    }

    gInspector->apply(gData);
    gInspector->SetFocus();
    Antares::Dispatcher::GUI::Refresh(gInspector);
}

void SelectStudy(const Data::Study::Ptr& study)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (study)
    {
        if (gData->studies.insert(study).second)
            gData->empty = false;
    }

    if (gInspector)
        gInspector->apply(gData);
}

void AddStudy(const Data::Study::Ptr& study)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    if (gData->studies.insert(study).second)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void SelectArea(const Data::Area* area)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (area)
    {
        if (gData->areas.insert(const_cast<Data::Area*>(area)).second)
            gData->empty = false;
    }

    if (gInspector)
        gInspector->apply(gData);
}

void AddArea(const Data::Area* area)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    if (gData->areas.insert(const_cast<Data::Area*>(area)).second)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddAreas(const Data::Area::Vector& list)
{
    if (list.empty())
        return;

    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::Area::Vector::const_iterator end = list.end();
    for (Data::Area::Vector::const_iterator i = list.begin(); i != end; ++i)
    {
        Data::Area* area = const_cast<Data::Area*>(*i);
        if (area)
            notEmpty = gData->areas.insert(area).second || notEmpty;
    }

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddAreas(const Data::Area::Set& list)
{
    if (list.empty())
        return;

    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::Area::Set::const_iterator end = list.end();
    for (Data::Area::Set::const_iterator i = list.begin(); i != end; ++i)
    {
        Data::Area* area = const_cast<Data::Area*>(*i);
        if (area)
            notEmpty = gData->areas.insert(area).second || notEmpty;
    }

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddLinks(const Data::AreaLink::Vector& list)
{
    if (list.empty())
        return;

    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::AreaLink::Vector::const_iterator end = list.end();
    for (Data::AreaLink::Vector::const_iterator i = list.begin(); i != end; ++i)
        notEmpty = gData->links.insert(const_cast<Data::AreaLink*>(*i)).second || notEmpty;

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddLinks(const Data::AreaLink::Set& list)
{
    if (list.empty())
        return;
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::AreaLink::Set::const_iterator end = list.end();
    for (Data::AreaLink::Set::const_iterator i = list.begin(); i != end; ++i)
        notEmpty = gData->links.insert(const_cast<Data::AreaLink*>(*i)).second || notEmpty;

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

const Data::AreaLink::Set& getLinks()
{
    return gData->links;
}

void AddBindingConstraint(const Data::BindingConstraint* constraint)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    using ConstraintPtr = Data::BindingConstraint*;
    if (gData->constraints.insert(const_cast<ConstraintPtr>(constraint)).second)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddBindingConstraints(const Data::BindingConstraint::Set& list)
{
    if (list.empty())
        return;
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    using StudyConstraintType = Data::BindingConstraint;
    bool notEmpty = false;
    StudyConstraintType::Set::const_iterator end = list.end();
    for (StudyConstraintType::Set::const_iterator i = list.begin(); i != end; ++i)
        notEmpty
          = gData->constraints.insert(const_cast<StudyConstraintType*>(*i)).second || notEmpty;

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void AddLink(const Data::AreaLink* link)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    if (gData->links.insert(const_cast<Data::AreaLink*>(link)).second)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

// gp : never used - to be removed
void AddThermalCluster(const Data::ThermalCluster* cluster)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    if (gData->ThClusters.insert(const_cast<Data::ThermalCluster*>(cluster)).second)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

// gp : should we add its renewable counterpart ?
void AddThermalClusters(const Data::ThermalCluster::Vector& list)
{
    if (list.empty())
        return;
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::ThermalCluster::Vector::const_iterator end = list.end();
    for (Data::ThermalCluster::Vector::const_iterator i = list.begin(); i != end; ++i)
        notEmpty
          = gData->ThClusters.insert(const_cast<Data::ThermalCluster*>(*i)).second || notEmpty;

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

// gp : should we add its renewable counterpart ?
void AddThermalClusters(const Data::ThermalCluster::Set& list)
{
    if (list.empty())
        return;
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    bool notEmpty = false;
    Data::ThermalCluster::Set::const_iterator end = list.end();
    for (Data::ThermalCluster::Set::const_iterator i = list.begin(); i != end; ++i)
        notEmpty
          = gData->ThClusters.insert(const_cast<Data::ThermalCluster*>(*i)).second || notEmpty;

    if (notEmpty)
    {
        gData->empty = false;
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveArea(const Data::Area* area)
{
    if (!(!gData) && gData->areas.erase(const_cast<Data::Area*>(area)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveBindingConstraint(const Data::BindingConstraint* constraint)
{
    if (!(!gData) && gData->constraints.erase(const_cast<Data::BindingConstraint*>(constraint)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveLink(const Data::AreaLink* link)
{
    if (!(!gData) && gData->links.erase(const_cast<Data::AreaLink*>(link)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveThermalCluster(const Data::ThermalCluster* cluster)
{
    if (!(!gData) && gData->ThClusters.erase(const_cast<Data::ThermalCluster*>(cluster)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveRenewableCluster(const Data::RenewableCluster* cluster)
{
    if (!(!gData) && gData->RnClusters.erase(const_cast<Data::RenewableCluster*>(cluster)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void RemoveHydroclusterCluster(const Data::HydroclusterCluster* cluster)
{
    if (!(!gData) && gData->HydroclusterClusters.erase(const_cast<Data::HydroclusterCluster*>(cluster)))
    {
        gData->determineEmpty();
        if (gInspector)
            gInspector->apply(gData);
    }
}

void SelectAreas(const Data::Area::Vector& areas)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (!areas.empty())
    {
        bool notEmpty = false;
        const Data::Area::Vector::const_iterator end = areas.end();
        for (Data::Area::Vector::const_iterator i = areas.begin(); i != end; ++i)
        {
            Data::Area* area = const_cast<Data::Area*>(*i);
            if (area)
                notEmpty = gData->areas.insert(area).second || notEmpty;
        }
        if (notEmpty)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectAreas(const Data::Area::Set& areas)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (!areas.empty())
    {
        bool notEmpty = false;
        const Data::Area::Set::const_iterator end = areas.end();
        for (Data::Area::Set::const_iterator i = areas.begin(); i != end; ++i)
        {
            Data::Area* area = const_cast<Data::Area*>(*i);
            if (area)
                notEmpty = gData->areas.insert(area).second || notEmpty;
        }
        if (notEmpty)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectBindingConstraints(const Data::BindingConstraint::Vector& list)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (!list.empty())
    {
        using ConstraintPtr = Data::BindingConstraint*;
        bool notEmpty = false;
        const Data::BindingConstraint::Vector::const_iterator end = list.end();
        for (Data::BindingConstraint::Vector::const_iterator i = list.begin(); i != end; ++i)
            notEmpty = gData->constraints.insert(const_cast<ConstraintPtr>(*i)).second || notEmpty;
        if (notEmpty)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectLink(const Data::AreaLink* lnk)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (lnk)
    {
        if (gData->links.insert(const_cast<Data::AreaLink*>(lnk)).second)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectLinks(const Data::AreaLink::Vector& lnks)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (!lnks.empty())
    {
        bool notEmpty = false;
        const Data::AreaLink::Vector::const_iterator end = lnks.end();
        for (Data::AreaLink::Vector::const_iterator i = lnks.begin(); i != end; ++i)
            notEmpty = gData->links.insert(const_cast<Data::AreaLink*>(*i)).second || notEmpty;
        if (notEmpty)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectThermalCluster(const Data::ThermalCluster* cluster)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (cluster)
    {
        if (gData->ThClusters.insert(const_cast<Data::ThermalCluster*>(cluster)).second)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

// gp : never used - to be removed
void SelectThermalClusters(const Data::ThermalCluster::Vector& clusters)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();

    if (not clusters.empty())
    {
        bool notEmpty = false;
        auto end = clusters.end();
        for (auto i = clusters.begin(); i != end; ++i)
            notEmpty = (gData->ThClusters).insert(const_cast<Data::ThermalCluster*>(*i)).second
                       || notEmpty;

        if (notEmpty)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}

void SelectRenewableCluster(const Data::RenewableCluster* cluster)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (cluster)
    {
        if (gData->RnClusters.insert(const_cast<Data::RenewableCluster*>(cluster)).second)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}


void SelectHydroclusterCluster(const Data::HydroclusterCluster* cluster)
{
    if (!gData)
        gData = std::make_shared<InspectorData>(Data::Study::Current::Get());

    gData->clear();
    if (cluster)
    {
        if (gData->HydroclusterClusters.insert(const_cast<Data::HydroclusterCluster*>(cluster)).second)
            gData->empty = false;
    }
    if (gInspector)
        gInspector->apply(gData);
}


uint CopyToClipboard()
{
    auto studyptr = Data::Study::Current::Get();
    if (!studyptr || !gData)
        return 0; // nothing was copied

    // alias to the current study
    auto& study = *studyptr;
    // string that will be copied into the system clipboard
    String::Ptr s = new String();
    auto& text = *s;

    // Header guard
    text << "antares.study.clipboard;handler=com.rte-france.antares.study;path=" << study.folder
         << "\n";

    // add a flag to know if the study has been modified just before the copy
    // antares has to fail when a paster occurs into another instance to prevent
    // against incomplete data
    if (StudyHasBeenModified())
        text += "was-modified: true\n";

    uint count = 0;

    // copying areas if any
    {
        auto end = gData->areas.end();
        for (auto i = gData->areas.begin(); i != end; ++i)
        {
            Data::Area* area = *i;
            if (area)
            {
                text << "import-area:" << area->name << "\n";
                ++count;
            }
        }
    }
    // gp : to be taken care of for renewable clusters
    // gp : Find "import-thermal-cluster:" and make same thing for renewables
    // copying thermal plants if any
    {
        auto end = gData->ThClusters.end();
        for (auto i = gData->ThClusters.begin(); i != end; ++i)
        {
            text << "import-thermal-cluster:" << (*i)->parentArea->name << '@' << (*i)->name()
                 << "\n";
            ++count;
        }
    }

    // copying links if any
    {
        auto end = gData->links.end();
        for (auto i = gData->links.begin(); i != end; ++i)
        {
            text << "import-link:" << (*i)->from->name << '@' << (*i)->with->name << "\n";
            ++count;
        }
    }

    // copying constraints if any
    {
        auto end = gData->constraints.end();
        for (auto i = gData->constraints.begin(); i != end; ++i)
        {
            text << "import-constraint:" << (*i)->name() << "\n";
            ++count;
        }
    }

    if (count) // at least one item has been selected
    {
        Antares::Toolbox::Clipboard clipboard;
        clipboard.add(s);
        clipboard.copy();
        return count;
    }

    // nothing was copied
    return 0;
}

bool AreasSelected(const Data::Area::NameSet& set,
                   std::map<Antares::Data::AreaName, Antares::Data::AreaName>& nameMap)
{
    if (gData == nullptr || (set.empty() && gData->areas.size()))
        return false;

    auto end = gData->areas.end();
    for (auto i = gData->areas.begin(); i != end; ++i)
    {
        Data::Area* area = *i;
        if (area)
        {
            if (set.find(area->name) != set.end())
                nameMap[*set.find(area->name)] = area->name;
            else
                return false;
        }
    }
    return true;
}

bool isAreaSelected(Antares::Data::AreaName name)
{
    if (name.empty() || gData == nullptr)
        return false;
    auto end = gData->areas.end();
    for (auto i = gData->areas.begin(); i != end; ++i)
    {
        Data::Area* area = *i;
        if (area->name == name)
        {
            return true;
        }
    }
    return false;
}

bool isConstraintSelected(const Yuni::String& constraintName)
{
    if (constraintName.empty() || gData == nullptr)
        return false;
    auto end = gData->constraints.end();
    for (auto i = gData->constraints.begin(); i != end; ++i)
    {
        Data::BindingConstraint* constraint = *i;
        if (constraint->name() == constraintName)
        {
            return true;
        }
    }
    return false;
}

bool ConstraintsSelected(const std::set<Yuni::String>& set)
{
    if (gData == nullptr || (set.empty() && gData->constraints.size()))
        return false;
    auto end = gData->constraints.end();
    for (auto i = gData->constraints.begin(); i != end; ++i)
    {
        Data::BindingConstraint* constraint = *i;
        if (set.find(constraint->name()) == set.end())
        {
            return false;
        }
    }
    return true;
}

bool IsLinkSelected(const Data::AreaName& from, const Data::AreaName& with)
{
    if (gData == nullptr || from.empty() || with.empty())
        return false;

    auto end = gData->links.end();
    for (auto i = gData->links.begin(); i != end; ++i)
    {
        Antares::Data::AreaLink* link = *i;
        if (link->from->name == from && link->with->name == with)
        {
            return true;
        }
    }

    return false;
}

bool LinksSelected(std::map<Data::AreaName, std::map<Data::AreaName, bool>>& set)
{
    if (gData == nullptr || (set.empty() && gData->links.size()))
        return false;
    auto end = gData->links.end();
    for (auto i = gData->links.begin(); i != end; ++i)
    {
        Data::AreaLink* link = *i;
        auto from = set.find(link->from->name);
        if (from != set.end())
        {
            auto with = from->second.find(link->with->name);

            if (with != from->second.end() && with->second == true)
                continue;

            return false;
        }
        else
            return false;
    }
    return true;
}

// gp : never used - to be removed
bool IsThermalClusterSelected(const Data::AreaName& area, const Data::ClusterName& name)
{
    (void)area;
    (void)name;
    // FIXME
    assert(true != true);
    return false;
}

void FirstSelectedArea(Data::AreaName& out)
{
    if (!gData || gData->areas.empty())
    {
        out.clear();
        return;
    }
    out = (*(gData->areas.begin()))->name;
}

void FirstSelectedAreaLink(Data::AreaLink** link)
{
    if (!gData || gData->links.empty())
    {
        *link = nullptr;
        return;
    }
    *link = *(gData->links.begin());
}

} // namespace Inspector
} // namespace Window
} // namespace Antares
