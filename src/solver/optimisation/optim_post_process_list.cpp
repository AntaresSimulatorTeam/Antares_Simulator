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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "optim_post_process_list.h"
#include "post_process_commands.h"

namespace Antares::Solver::Simulation
{
OptPostProcessList::OptPostProcessList(PROBLEME_HEBDO* problemeHebdo,
                                       uint thread_number,
                                       AreaList& areas,
                                       SheddingPolicy sheddingPolicy,
                                       SimplexOptimization splxOptimization,
                                       Calendar& calendar)

 :
 interfacePostProcessList(problemeHebdo, thread_number)
{
    post_process_list.push_back(
      std::make_unique<DispatchableMarginPostProcessCmd>(problemeHebdo_, thread_number_, areas));
    post_process_list.push_back(
      std::make_unique<HydroLevelsUpdatePostProcessCmd>(problemeHebdo_, areas, false, false));
    post_process_list.push_back(std::make_unique<RemixHydroPostProcessCmd>(
      problemeHebdo_, areas, sheddingPolicy, splxOptimization, thread_number));
    post_process_list.push_back(
      std::make_unique<HydroLevelsUpdatePostProcessCmd>(problemeHebdo_, areas, true, false));
    post_process_list.push_back(
      std::make_unique<InterpolateWaterValuePostProcessCmd>(problemeHebdo_, areas, calendar));
    post_process_list.push_back(
      std::make_unique<HydroLevelsFinalUpdatePostProcessCmd>(problemeHebdo_, areas));
}

} // namespace Antares::Solver::Simulation
