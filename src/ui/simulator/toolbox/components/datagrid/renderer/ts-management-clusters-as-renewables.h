// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include "ts-management.h"

namespace Antares::Component::Datagrid::Renderer
{
class TSmanagementRenewableCluster final: public TSmanagement
{
public:
    TSmanagementRenewableCluster();
    ~TSmanagementRenewableCluster() = default;
}; // class TSmanagementRenewableCluster

} // namespace Antares::Component::Datagrid::Renderer
