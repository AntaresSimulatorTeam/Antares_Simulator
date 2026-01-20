// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


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

namespace Antares::Forms
{
void ApplWnd::createNBHydro()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    auto page = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook,
                                                                    wxT("hydro"),
                                                                    wxT("Hydro"));

    // Hydro Allocation
    pageHydroAllocation = page.first->add(new Antares::Window::Hydro::Allocation(page.first),
                                          wxT("Allocation"));
    pageHydroAllocation->displayExtraControls(false);

    // Correlation matrix
    pageHydroCorrelation = page.first->add(
      new Antares::Window::CorrelationPanel(page.first, Data::timeSeriesHydro),
      wxT("Spatial correlation"));
    pageHydroCorrelation->displayExtraControls(false);

    // TS Generator
    pageHydroPrepro = page.first->add(new Antares::Window::Hydro::Localdatahydro(page.first,
                                                                                 page.second),
                                      wxT("Local data"));

    // Time-series
    pageHydroTimeSeries = page.first->add(new Antares::Window::Hydro::Series(page.first,
                                                                             page.second),
                                          wxT("Time-series"));
}

} // namespace Antares::Forms
