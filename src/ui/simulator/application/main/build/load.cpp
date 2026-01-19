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
void ApplWnd::createNBLoad()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*>
      page = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook,
                                                                 wxT("load"),
                                                                 wxT("Load"));

    // Correlation matrix
    pageLoadCorrelation = page.first->add(new Window::CorrelationPanel(page.first,
                                                                       Data::timeSeriesLoad),
                                          wxT("Spatial correlation"));
    pageLoadCorrelation->displayExtraControls(false);

    pageLoadPrepro = page.first->add(
      new Component::Datagrid::Component(
        page.first,
        new Component::Datagrid::Renderer::XCastAllAreas<Data::timeSeriesLoad>(page.first,
                                                                               page.second)),
      wxT("Digest"));
    pageLoadPrepro->displayExtraControls(false);

    // TS Generator
    pageLoadPrepro = page.first->add(new Window::XCast<Data::timeSeriesLoad>(page.first,
                                                                             page.second),
                                     wxT("Local data"));

    // Time-series
    pageLoadTimeSeries = page.first->add(
      new Component::Datagrid::Component(
        page.first,
        new Component::Datagrid::Renderer::TimeSeriesLoad(page.first, page.second)),
      wxT("Time-series"));
}

} // namespace Antares::Forms
