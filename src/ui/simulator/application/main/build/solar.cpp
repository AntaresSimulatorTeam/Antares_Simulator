// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../main.h"
#include "../../../windows/xcast/xcast.h"
#include "../../../toolbox/input/area.h"
#include "../../../windows/correlation/correlation.h"
#include "../../../toolbox/components/datagrid/renderer/area/timeseries.h"
#include "../../../toolbox/components/datagrid/renderer/area/xcast-allareas.h"
#include "standard-page.hxx"

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::createNBSolar()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*>
      page = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook,
                                                                 wxT("solar"),
                                                                 wxT("Solar"));

    // Correlation matrix
    pageSolarCorrelation = page.first->add(new Window::CorrelationPanel(page.first,
                                                                        Data::timeSeriesSolar),
                                           wxT("Spatial correlation"));
    pageSolarCorrelation->displayExtraControls(false);

    pageSolarPrepro = page.first->add(
      new Component::Datagrid::Component(
        page.first,
        new Component::Datagrid::Renderer::XCastAllAreas<Data::timeSeriesSolar>(page.first,
                                                                                page.second)),
      wxT("Digest"));
    pageSolarPrepro->displayExtraControls(false);

    // TS Generator
    pageSolarPrepro = page.first->add(new Window::XCast<Data::timeSeriesSolar>(page.first,
                                                                               page.second),
                                      wxT("Local data"));

    // Time-series
    pageSolarTimeSeries = page.first->add(
      new Component::Datagrid::Component(
        page.first,
        new Component::Datagrid::Renderer::TimeSeriesSolar(page.first, page.second)),
      wxT("Time-series"));
}

} // namespace Antares::Forms
