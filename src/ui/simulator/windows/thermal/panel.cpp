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

#include "panel.h"
#include "../../windows/inspector.h"
#include "../../toolbox/create.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/input/thermal-cluster.h"
#include "../../toolbox/components/datagrid/renderer/area/thermal.areasummary.h"
#include "../../toolbox/components/datagrid/renderer/area/thermalprepro.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include <wx/stattext.h>
#include <wx/splitter.h>
#include "cluster.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Thermal
{
Panel::Panel(Component::Notebook* parent) :
 Component::Panel(parent),
 pageThermalTimeSeries(nullptr),
 pageThermalTimeSeriesFuelCost(nullptr),
 pageThermalTimeSeriesCO2Cost(nullptr),
 pageThermalPrepro(nullptr),
 pageThermalCommon(nullptr),
 pNotebookThermalCluster(nullptr),
 pAreaForThermalCommonData(nullptr),
 pAreaSelector(nullptr),
 pStudyRevisionIncrement((Yuni::uint64)-1)
{
    // A sizer for our panel
    wxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);

    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page;

    // First initialization
    {
        Component::Notebook* n = new Component::Notebook(this, Component::Notebook::orTop);
        page.first = n;
        n->caption(wxT("Thermal dispatchable clusters"));
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
          new Component::Datagrid::Renderer::ThermalClusterSummarySingleArea(this, page.second));
        page.first->add(summary, wxT("Summary for the area"));
    }

    // Thermal cluster list
    {
        // The window splitter
        pSplitter = new wxSplitterWindow(
          page.first, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);
        pageThermalClusterList = page.first->add(pSplitter, wxT("Thermal cluster list"));
        pSplitter->SetSashGravity(0.5);

        // Input selector for thermal clusters
        Toolbox::InputSelector::ThermalCluster* tag
          = new Toolbox::InputSelector::ThermalCluster(pSplitter, page.second);

        // Informations about the current thermal cluster
        Component::Notebook* subbook
          = new Component::Notebook(pSplitter, Component::Notebook::orTop);
        pNotebookThermalCluster = subbook;
        subbook->caption(wxT("Thermal cluster"));
        subbook->theme(Component::Notebook::themeLight);

        // Common properties of the current thermal cluster
        pageThermalCommon
          = subbook->add(new Window::Thermal::CommonProperties(subbook, tag), wxT("Common"));

        // TS-Generator
        pageThermalPrepro = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::ThermalClusterPrepro(subbook, tag)),
          wxT("TS generator"));

        // Time Series
        pageThermalTimeSeries = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesThermalCluster(subbook, tag)),
          wxT("Time-Series"));

        pageThermalTimeSeriesFuelCost = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesThermalClusterFuelCost(subbook, tag)),
          wxT("Fuel Cost"));

        pageThermalTimeSeriesCO2Cost = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesThermalClusterCO2Cost(subbook, tag)),
          wxT("CO2 Cost"));

        // Split the view
        pSplitter->SetMinimumPaneSize(70);
        pSplitter->SplitHorizontally(tag, subbook);

        // event
        tag->onThermalClusterChanged.connect(this, &Panel::onThermalClusterChanged);
    }

    page.second->onAreaChanged.connect(this, &Panel::onAreaChangedForThermalData);
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
    if (&page == pageThermalClusterList)
    {
        Yuni::uint64 revID = StudyInMemoryRevisionID();
        if (revID != pStudyRevisionIncrement)
        {
            Data::Area* area = pAreaForThermalCommonData;
            pAreaSelector->onAreaChanged(nullptr);
            pAreaSelector->onAreaChanged(area);
            pStudyRevisionIncrement = revID;
        }
    }
}

void Panel::onThermalClusterChanged(Data::ThermalCluster* cluster)
{
    if (pNotebookThermalCluster)
    {
        pNotebookThermalCluster->caption(
          (cluster && cluster->parentArea)
            ? wxString() << wxStringFromUTF8(cluster->parentArea->name) << wxT(", ")
                         << wxStringFromUTF8(cluster->name()) << wxT("  ")
            : wxString());
    }
}

void Panel::onAreaChangedForThermalData(Data::Area* area)
{
    if (area != pAreaForThermalCommonData)
        pAreaForThermalCommonData = area;
}

void Panel::internalOnStudyLoaded()
{
    Dispatcher::GUI::Post(this, &Panel::delayedResizeSplitter, 20 /*ms*/);
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
    Dispatcher::GUI::Post(callback, 50 /*ms*/);
}

} // namespace Thermal
} // namespace Window
} // namespace Antares
