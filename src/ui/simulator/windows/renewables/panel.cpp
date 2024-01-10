/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "panel.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include <wx/stattext.h>
#include <wx/splitter.h>
#include "cluster.h"

#include <wx/sizer.h>
#include "../../toolbox/input/renewable-cluster.h"
#include "ui/simulator/application/study.h"
#include "../../toolbox/components/datagrid/renderer/area/renewable.areasummary.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Renewable
{
Panel::Panel(Component::Notebook* parent) :
 Component::Panel(parent),
 pageRenewableCommon(nullptr),
 pNotebookCluster(nullptr),
 pAreaForCommonData(nullptr),
 pAreaSelector(nullptr),
 pStudyRevisionIncrement((uint64_t)-1)
{
    // A sizer for our panel
    wxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);

    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page;

    // First initialization
    {
        Component::Notebook* n = new Component::Notebook(this, Component::Notebook::orTop);
        page.first = n;
        n->caption(wxT("Renewable clusters"));
        mainsizer->Add(n, 1, wxALL | wxEXPAND);

        // The input selector for Areas
        page.second = new Toolbox::InputSelector::Area(n);
        pAreaSelector = page.second;

        {
            Component::Panel* separator = new Component::Panel(n);
            separator->SetSize(4, 1);
            n->addCommonControl(separator, 0, wxPoint(0, 0));
        }
        n->addCommonControl(page.second);
    }

    // Summary for the area
    {
        Component::Datagrid::Component* summary = new Component::Datagrid::Component(
          page.first,
          new Component::Datagrid::Renderer::RenewableClusterSummarySingleArea(this, page.second));
        page.first->add(summary, wxT("Summary for the area"));
    }

    // Renewable cluster list
    {
        // The window splitter
        pSplitter = new wxSplitterWindow(
          page.first, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);
        pageRenewableClusterList = page.first->add(pSplitter, wxT("Renewable cluster list"));
        pSplitter->SetSashGravity(0.5);

        // Input selector for renewable clusters
        Toolbox::InputSelector::RenewableCluster* tag
          = new Toolbox::InputSelector::RenewableCluster(pSplitter, page.second);

        // Informations about the current renewable cluster
        Component::Notebook* subbook
          = new Component::Notebook(pSplitter, Component::Notebook::orTop);
        pNotebookCluster = subbook;
        subbook->caption(wxT("Renewable cluster"));
        subbook->theme(Component::Notebook::themeLight);

        // Common properties of the current thermal cluster
        pageRenewableCommon
          = subbook->add(new Window::Renewable::CommonProperties(subbook, tag), wxT("Common"));

        // Time Series
        pageRenewableTimeSeries = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesRenewableCluster(subbook, tag)),
          wxT("Time-Series"));

        // Split the view
        pSplitter->SetMinimumPaneSize(70);
        pSplitter->SplitHorizontally(tag, subbook);

        // event
        tag->onClusterChanged.connect(this, &Panel::onClusterChanged);
    }

    page.second->onAreaChanged.connect(this, &Panel::onAreaChangedForData);
    page.first->onPageChanged.connect(this, &Panel::onPageChanged);

    // Set the global sizer for the panel
    SetSizer(mainsizer);

    // Event: a study has just been loaded or a new study has just been created
    // This event is used to update the graphical components
    OnStudyLoaded.connect(this, &Panel::onStudyLoaded);
}

Panel::~Panel()
{
    destroyBoundEvents();
}

void Panel::onPageChanged(Component::Notebook::Page& page)
{
    if (&page == pageRenewableClusterList)
    {
        uint64_t revID = StudyInMemoryRevisionID();
        if (revID != pStudyRevisionIncrement)
        {
            Data::Area* area = pAreaForCommonData;
            pAreaSelector->onAreaChanged(nullptr);
            pAreaSelector->onAreaChanged(area);
            pStudyRevisionIncrement = revID;
        }
    }
}

void Panel::onClusterChanged(Data::RenewableCluster* cluster)
{
    if (pNotebookCluster)
    {
        pNotebookCluster->caption((cluster && cluster->parentArea)
                                    ? wxString()
                                        << wxStringFromUTF8(cluster->parentArea->name) << wxT(", ")
                                        << wxStringFromUTF8(cluster->name()) << wxT("  ")
                                    : wxString());
    }
}

void Panel::onAreaChangedForData(Data::Area* area)
{
    if (area != pAreaForCommonData)
        pAreaForCommonData = area;
}

void Panel::internalOnStudyLoaded()
{
    Dispatcher::GUI::Post(this, &Panel::delayedResizeSplitter, 20);
}

void Panel::delayedResizeSplitter()
{
    if (pSplitter)
    {
        const int h = pSplitter->GetSize().GetHeight() / 2;
        pSplitter->SetSashPosition(h - h / 4);
    }
}

void Panel::onStudyLoaded()
{
    // delayed execution
    Yuni::Bind<void()> callback;
    callback.bind(this, &Panel::internalOnStudyLoaded);
    Dispatcher::GUI::Post(callback, 50);
}

} // namespace Renewable
} // namespace Window
} // namespace Antares
