// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
  const Data::Parameters& params,
  Calendar& calendar,
  IResultWriter& resultWriter)
{
    if (adqPatchParams.enabled)
    {
        return std::make_unique<AdqPatchPostProcessList>(adqPatchParams,
                                                         problemeHebdo,
                                                         numSpace,
                                                         areas,
                                                         params,
                                                         calendar,
                                                         resultWriter);
    }
    else
    {
        return std::make_unique<OptPostProcessList>(problemeHebdo,
                                                    numSpace,
                                                    areas,
                                                    params,
                                                    calendar,
                                                    resultWriter);
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
