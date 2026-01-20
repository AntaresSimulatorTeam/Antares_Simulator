// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "grid.h"

namespace Antares::Window::Inspector
{
BEGIN_EVENT_TABLE(InspectorGrid, wxPropertyGrid)
EVT_PG_CHANGING(wxID_ANY, InspectorGrid::OnPropertyChanging)
END_EVENT_TABLE()

} // namespace Antares::Window::Inspector
