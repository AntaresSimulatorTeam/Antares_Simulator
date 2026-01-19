// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/base_post_process.h"
#include "antares/study/parameters/adq-patch-params.h"

namespace Antares::Solver::Simulation
{

class AdqPatchPostProcessList final: public interfacePostProcessList
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    AdqPatchPostProcessList(const AdqPatchParams& adqPatchParams,
                            PROBLEME_HEBDO* problemeHebdo,
                            uint numSpace,
                            AreaList& areas,
                            const Data::Parameters& params,
                            Calendar& calendar,
                            IResultWriter& resultWriter);

    virtual ~AdqPatchPostProcessList() = default;
};

} // namespace Antares::Solver::Simulation
