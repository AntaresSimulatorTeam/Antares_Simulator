// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/optim_post_process_list.h"

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/solver/optimisation/post_process_commands.h"

namespace Antares::Solver::Simulation
{
OptPostProcessList::OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                                       uint numSpace,
                                       AreaList& areas,
                                       const Data::Parameters& params,
                                       Calendar& calendar,
                                       IResultWriter& resultWriter,
                                       IO::Outputs::OptimisationsSimulationTable* simulationTables):
    interfacePostProcessList(problemeHebdo, numSpace)
{
    post_process_list.push_back(
      std::make_unique<DispatchableMarginPostProcessCmd>(problemeHebdo_, numSpace_, areas));
    post_process_list.push_back(std::make_unique<RemixHydroPostProcessCmd>(problemeHebdo_,
                                                                           areas,
                                                                           params,
                                                                           numSpace,
                                                                           resultWriter));
    post_process_list.push_back(std::make_unique<DumpSimulationTablePostProcessCmd>(
      problemeHebdo_,
      IO::Outputs::OptimisationsSimulationTable::remixHydroStage,
      simulationTables));
    post_process_list.push_back(
      std::make_unique<InterpolateWaterValuePostProcessCmd>(problemeHebdo_, areas, calendar));
    post_process_list.push_back(
      std::make_unique<HydroLevelsFinalUpdatePostProcessCmd>(problemeHebdo_, areas));
}

} // namespace Antares::Solver::Simulation
