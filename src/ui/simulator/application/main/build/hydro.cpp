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
#include "../../../toolbox/input/area.h"
#include "../../../windows/correlation/correlation.h"
#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include "../../../windows/hydro/series.h"
#include "../../../windows/hydro/prepro.h"
#include "../../../windows/hydro/localdatahydro.h"
#include "../../../windows/hydro/allocation.h"
#include "standard-page.hxx"

using namespace Yuni;

namespace Antares
{
namespace Forms
{
void ApplWnd::createNBHydro()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    auto page
      = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook, wxT("hydro"), wxT("Hydro"));

    // Hydro Allocation
    pageHydroAllocation
      = page.first->add(new Antares::Window::Hydro::Allocation(page.first), wxT("Allocation"));
    pageHydroAllocation->displayExtraControls(false);

    // Correlation matrix
    pageHydroCorrelation
      = page.first->add(new Antares::Window::CorrelationPanel(page.first, Data::timeSeriesHydro),
                        wxT("Spatial correlation"));
    pageHydroCorrelation->displayExtraControls(false);

    // TS Generator
    pageHydroPrepro = page.first->add(
      new Antares::Window::Hydro::Localdatahydro(page.first, page.second), wxT("Local data"));

    // Time-series
    pageHydroTimeSeries = page.first->add(
      new Antares::Window::Hydro::Series(page.first, page.second), wxT("Time-series"));
}

} // namespace Forms
} // namespace Antares
