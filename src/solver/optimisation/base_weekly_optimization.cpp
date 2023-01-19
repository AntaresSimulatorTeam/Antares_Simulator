/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <memory>
#include "base_weekly_optimization.h"
#include "weekly_optimization.h"
#include "adequacy_patch_weekly_optimization.h"

namespace Antares::Solver::Simulation
{
interfaceWeeklyOptimization::interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo,
                                                         uint thread_number) :
 problemeHebdo_(problemesHebdo), thread_number_(thread_number)
{
}

std::unique_ptr<interfaceWeeklyOptimization> interfaceWeeklyOptimization::create(
    bool adqPatchEnabled,
    PROBLEME_HEBDO* problemeHebdo,
    uint thread_number)
{
    if (adqPatchEnabled)
        return std::make_unique<AdequacyPatchOptimization>(problemeHebdo, thread_number);
    else
        return std::make_unique<weeklyOptimization>(problemeHebdo, thread_number);
}

void interfaceWeeklyOptimization::solveCSR(Antares::Data::AreaList&, uint, uint, uint)
{
    // By default, do nothing
}

} // namespace Antares::Solver::Simulation
