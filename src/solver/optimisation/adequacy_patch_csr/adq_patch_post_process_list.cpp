// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_post_process_list.h"

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/solver/optimisation/post_process_commands.h"

namespace Antares::Solver::Simulation
{

AdqPatchPostProcessList::AdqPatchPostProcessList(
  const AdqPatchParams& adqPatchParams,
  PROBLEME_HEBDO* problemeHebdo,
  uint numSpace,
  AreaList& areas,
  const Data::Parameters& params,
  Calendar& calendar,
  IResultWriter& writer,
  IO::Outputs::OptimisationsSimulationTable* simulationTables):
    interfacePostProcessList(problemeHebdo, numSpace)
{
    post_process_list.push_back(
      std::make_unique<DispatchableMarginPostProcessCmd>(problemeHebdo_, numSpace_, areas));

    post_process_list.push_back(
      std::make_unique<RemixHydroPostProcessCmd>(problemeHebdo_, areas, params, numSpace, writer));

    post_process_list.push_back(std::make_unique<DumpSimulationTablePostProcessCmd>(
      problemeHebdo_,
      IO::Outputs::OptimisationsSimulationTable::remixHydroStage,
      simulationTables));

    if (params.adqPatchDebug)
    {
        post_process_list.push_back(std::make_unique<WriteDebugAdequacyPatch>(problemeHebdo_,
                                                                              areas,
                                                                              numSpace_,
                                                                              writer,
                                                                              "before"));
    }

    post_process_list.push_back(
      std::make_unique<CurtailmentSharingPostProcessCmd>(adqPatchParams,
                                                         problemeHebdo_,
                                                         areas,
                                                         numSpace_,
                                                         params.optOptions));
    post_process_list.push_back(
      std::make_unique<DTGnettingAfterCSRcmd>(problemeHebdo_, areas, numSpace));
    post_process_list.push_back(
      std::make_unique<UpdateMrgPriceAfterCSRcmd>(problemeHebdo_, areas, numSpace));

    // After curtailment sharing, DTG netting and the marginal price update, i.e.
    // once the whole patch has been applied.
    post_process_list.push_back(std::make_unique<DumpSimulationTablePostProcessCmd>(
      problemeHebdo_,
      IO::Outputs::OptimisationsSimulationTable::adequacyPatchStage,
      simulationTables));

    if (params.adqPatchDebug)
    {
        post_process_list.push_back(std::make_unique<WriteDebugAdequacyPatch>(problemeHebdo_,
                                                                              areas,
                                                                              numSpace_,
                                                                              writer,
                                                                              "after"));
    }

    post_process_list.push_back(
      std::make_unique<InterpolateWaterValuePostProcessCmd>(problemeHebdo_, areas, calendar));
    post_process_list.push_back(
      std::make_unique<HydroLevelsFinalUpdatePostProcessCmd>(problemeHebdo_, areas));
}

} // namespace Antares::Solver::Simulation
