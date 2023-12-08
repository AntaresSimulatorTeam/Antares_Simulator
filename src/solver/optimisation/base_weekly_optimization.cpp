/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Free Software Foundation, either version 3 of the License, or
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
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include <memory>
#include "base_weekly_optimization.h"
#include "weekly_optimization.h"
#include "adequacy_patch_local_matching/adequacy_patch_weekly_optimization.h"

using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

namespace Antares::Solver::Optimization
{
WeeklyOptimization::WeeklyOptimization(const OptimizationOptions& options,
                                       PROBLEME_HEBDO* problemesHebdo,
                                       AdqPatchParams& adqPatchParams,
                                       uint thread_number,
                                       IResultWriter& writer) :
    options_(options),
    problemeHebdo_(problemesHebdo),
    adqPatchParams_(adqPatchParams),
    thread_number_(thread_number),
    writer_(writer)
{
}

std::unique_ptr<WeeklyOptimization> WeeklyOptimization::create(
    const Antares::Data::Study& study,
    const OptimizationOptions& options,
    AdqPatchParams& adqPatchParams,
    PROBLEME_HEBDO* problemeHebdo,
    uint thread_number,
    IResultWriter& writer)
{
    if (adqPatchParams.enabled && adqPatchParams.localMatching.enabled)
        return std::make_unique<AdequacyPatchOptimization>(study, options, problemeHebdo, adqPatchParams, thread_number, writer);
    else
        return std::make_unique<DefaultWeeklyOptimization>(options, problemeHebdo, adqPatchParams, thread_number, writer);
}

} // namespace Antares::Solver::Optimization
