/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/simulation/base_post_process.h"

#include <memory>

#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_post_process_list.h"
#include "antares/solver/optimisation/optim_post_process_list.h"

namespace Antares::Solver::Simulation
{

basePostProcessCommand::basePostProcessCommand(PROBLEME_HEBDO* problemeHebdo):
    problemeHebdo_(problemeHebdo)
{
}

interfacePostProcessList::interfacePostProcessList(PROBLEME_HEBDO* problemesHebdo, uint numSpace):
    problemeHebdo_(problemesHebdo),
    numSpace_(numSpace)
{
}

std::unique_ptr<interfacePostProcessList> interfacePostProcessList::create(
  AdqPatchParams& adqPatchParams,
  PROBLEME_HEBDO* problemeHebdo,
  uint numSpace,
  AreaList& areas,
  SheddingPolicy sheddingPolicy,
  SimplexOptimization splxOptimization,
  Calendar& calendar)
{
    if (adqPatchParams.enabled)
    {
        return std::make_unique<AdqPatchPostProcessList>(adqPatchParams,
                                                         problemeHebdo,
                                                         numSpace,
                                                         areas,
                                                         sheddingPolicy,
                                                         splxOptimization,
                                                         calendar);
    }
    else
    {
        return std::make_unique<OptPostProcessList>(problemeHebdo,
                                                    numSpace,
                                                    areas,
                                                    sheddingPolicy,
                                                    splxOptimization,
                                                    calendar);
    }
}

void interfacePostProcessList::runAll(const optRuntimeData& opt_runtime_data)
{
    for (auto& post_process: post_process_list)
    {
        post_process->execute(opt_runtime_data);
    }
}

} // namespace Antares::Solver::Simulation
