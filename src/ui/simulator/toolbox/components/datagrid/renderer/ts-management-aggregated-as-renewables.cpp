// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "ts-management-aggregated-as-renewables.h"

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
TSmanagementAggregatedAsRenewable::TSmanagementAggregatedAsRenewable():
    TSmanagement()
{
    columns_.push_back(new classicColumn(Antares::Data::timeSeriesWind, "      Wind      "));
    columns_.push_back(new classicColumn(Antares::Data::timeSeriesSolar, "      Solar      "));
    columns_.push_back(new ColumnNTC());

    checkLineNumberInColumns();
}
} // namespace Antares::Component::Datagrid::Renderer
