// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/base_post_process.h"

namespace Antares::Solver::Simulation
{
class OptPostProcessList final: public interfacePostProcessList
{
public:
    OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                       uint numSpace,
                       AreaList& areas,
                       const Data::Parameters& params,
                       Calendar& calendar,
                       IResultWriter& resultWriter);

    virtual ~OptPostProcessList() = default;
};

} // namespace Antares::Solver::Simulation
