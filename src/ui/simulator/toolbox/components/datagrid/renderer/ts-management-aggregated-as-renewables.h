// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "ts-management.h"

namespace Antares::Component::Datagrid::Renderer
{
class TSmanagementAggregatedAsRenewable final: public TSmanagement
{
public:
    TSmanagementAggregatedAsRenewable();
    ~TSmanagementAggregatedAsRenewable() = default;
}; // class TSmanagementAggregatedAsRenewable

} // namespace Antares::Component::Datagrid::Renderer
