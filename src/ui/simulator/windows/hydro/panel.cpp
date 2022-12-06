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
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include <wx/stattext.h>
#include <wx/splitter.h>
#include "cluster.h"

#include <wx/sizer.h>
#include "../../toolbox/input/hydrocluster-cluster.h"
#include "ui/simulator/application/study.h"
#include "../../toolbox/components/datagrid/renderer/area/hydrocluster.areasummary.h" 

#include "levelsandvalues.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Hydro
{
Panel::Panel(Component::Notebook* parent) :
 Component::Panel(parent),
 pageHydroclusterLocaldata(nullptr),
 pNotebookCluster(nullptr),
 pAreaForCommonData(nullptr),
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
        n->caption(wxT("Hydrocluster clusters"));
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
          new Component::Datagrid::Renderer::HydroclusterClusterSummarySingleArea(this, page.second));
        page.first->add(summary, wxT("Summary for the area"));
    }

    // Hydrocluster cluster list
    {
        // The window splitter
        pSplitter = new wxSplitterWindow(
          page.first, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);
        pageHydroclusterClusterList = page.first->add(pSplitter, wxT("Hydrocluster cluster list"));
        pSplitter->SetSashGravity(0.5);

        // Input selector for hydrocluster clusters
        Toolbox::InputSelector::HydroclusterCluster* tag
          = new Toolbox::InputSelector::HydroclusterCluster(pSplitter, page.second);

        // Informations about the current hydrocluster cluster
        Component::Notebook* subbook
          = new Component::Notebook(pSplitter, Component::Notebook::orTop);
        pNotebookCluster = subbook;
        subbook->caption(wxT("Hydrocluster cluster"));
        subbook->theme(Component::Notebook::themeLight);

        // Common properties of the current thermal cluster
        pageHydroclusterLocaldata
          = subbook->add(new Window::Hydro::CommonProperties(subbook, tag), wxT("Local data"));

        // Time Series ROR
        pageHydroclusterTimeSeriesRor = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesHydroclusterClusterRor(subbook, tag)),
          wxT("Run-of-the-river (ROR)"));

        // Time Series Storage
        pageHydroclusterTimeSeriesStorage = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesHydroclusterClusterStorage(subbook, tag)),
          wxT("Hydro Storage"));

        //Minimum Generation
        pageHydroclusterTimeSeriesMingen = subbook->add(
          new Component::Datagrid::Component(
            subbook, new Component::Datagrid::Renderer::TimeSeriesHydroclusterClusterMingen(subbook, tag)),
          wxT("Minimum Generation"));

        // pageLevelsAndValuesHydrocluster = subbook->add(
        //   new Component::Datagrid::Component(
        //     subbook, new Component::Datagrid::Renderer::TimeSeriesHydroclusterClusterMingen(subbook, tag)),
        //   wxT("Reservoir levels and water values"));

      auto* page4 = new Antares::Window::Hydro::LevelsAndValuesHydrocluster(subbook, tag);
      pageLevelsAndValuesHydrocluster = subbook->add(page4, wxT("Reservoir levels and water values"));

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
    if (&page == pageHydroclusterClusterList)
    {
        Yuni::uint64 revID = StudyInMemoryRevisionID();
        if (revID != pStudyRevisionIncrement)
        {
            Data::Area* area = pAreaForCommonData;
            pAreaSelector->onAreaChanged(nullptr);
            pAreaSelector->onAreaChanged(area);
            pStudyRevisionIncrement = revID;
        }
    }
}

void Panel::onClusterChanged(Data::HydroclusterCluster* cluster)
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

} // namespace Hydro
} // namespace Window
} // namespace Antares
