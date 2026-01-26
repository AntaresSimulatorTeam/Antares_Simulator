// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../main.h"
#include "../../../windows/xcast/xcast.h"
#include "../../../toolbox/input/area.h"
#include "../../../windows/correlation/correlation.h"
#include "../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"
#include "standard-page.hxx"

using namespace Yuni;

namespace Antares::Forms
{
void ApplWnd::createNBNodalOptimization()
{
    assert(pNotebook);

    // Create a standard page with an input selector
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*>
      page = createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook,
                                                                 wxT("nodal"),
                                                                 wxT("Economic Opt."));

    pageNodalOptim = page.first->add(
      new Component::Datagrid::Component(page.first,
                                         new Component::Datagrid::Renderer::NodalOptimization(
                                           page.first),
                                         wxEmptyString,
                                         true,
                                         true,
                                         false,
                                         false,
                                         true),
      wxT("All areas"));
    pageNodalOptim->displayExtraControls(false);
}

} // namespace Antares::Forms
