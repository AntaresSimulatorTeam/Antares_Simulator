// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "ts-management-clusters-as-renewables.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
TSmanagementRenewableCluster::TSmanagementRenewableCluster():
    TSmanagement()
{
    columns_.push_back(new ColumnRenewableClusters());
    columns_.push_back(new ColumnNTC());

    checkLineNumberInColumns();
}
} // namespace Antares::Component::Datagrid::Renderer
