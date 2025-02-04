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

#include "antares/solver/optimisation/optim_post_process_list.h"

#include "antares/solver/optimisation/post_process_commands.h"

namespace Antares::Solver::Simulation
{
OptPostProcessList::OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                                       uint numSpace,
                                       AreaList& areas,
                                       SheddingPolicy sheddingPolicy,
                                       SimplexOptimization splxOptimization,
                                       Calendar& calendar)

    :
    interfacePostProcessList(problemeHebdo, numSpace)
{
    post_process_list.push_back(
      std::make_unique<DispatchableMarginPostProcessCmd>(problemeHebdo_, numSpace_, areas));
    post_process_list.push_back(
      std::make_unique<HydroLevelsUpdatePostProcessCmd>(problemeHebdo_, areas, false, false));
    post_process_list.push_back(std::make_unique<RemixHydroPostProcessCmd>(problemeHebdo_,
                                                                           areas,
                                                                           sheddingPolicy,
                                                                           splxOptimization,
                                                                           numSpace));
    post_process_list.push_back(
      std::make_unique<HydroLevelsUpdatePostProcessCmd>(problemeHebdo_, areas, true, false));
    post_process_list.push_back(
      std::make_unique<InterpolateWaterValuePostProcessCmd>(problemeHebdo_, areas, calendar));
    post_process_list.push_back(
      std::make_unique<HydroLevelsFinalUpdatePostProcessCmd>(problemeHebdo_, areas));
}

} // namespace Antares::Solver::Simulation
