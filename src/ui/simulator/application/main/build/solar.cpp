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

#include "../main.h"
#include "../../../windows/xcast/xcast.h"
#include "../../../toolbox/input/area.h"
#include "../../../windows/correlation/correlation.h"
#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"
#include "standard-page.hxx"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
void ApplWnd::createNBSolar()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page
      = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook, wxT("solar"), wxT("Solar"));

    // Correlation matrix
    pageSolarCorrelation = page.first->add(
      new Window::CorrelationPanel(page.first, Data::timeSeriesSolar), wxT("Spatial correlation"));
    pageSolarCorrelation->displayExtraControls(false);

    pageSolarPrepro
      = page.first->add(new Component::Datagrid::Component(
                          page.first,
                          new Component::Datagrid::Renderer::XCastAllAreas<Data::timeSeriesSolar>(
                            page.first, page.second)),
                        wxT("Digest"));
    pageSolarPrepro->displayExtraControls(false);

    // TS Generator
    pageSolarPrepro = page.first->add(
      new Window::XCast<Data::timeSeriesSolar>(page.first, page.second), wxT("Local data"));

    // Time-series
    pageSolarTimeSeries = page.first->add(
      new Component::Datagrid::Component(
        page.first, new Component::Datagrid::Renderer::TimeSeriesSolar(page.first, page.second)),
      wxT("Time-series"));
}

} // namespace Forms
} // namespace Antares
