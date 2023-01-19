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

#pragma once

#include <yuni/yuni.h> // for "uint" definition
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
class interfaceWeeklyOptimization
{
public:
    virtual void solve(uint weekInTheYear, int hourInTheYear) = 0;
    virtual void solveCSR(Antares::Data::AreaList& areas, uint year, uint week, uint numSpace);
    virtual ~interfaceWeeklyOptimization() = default;
    static std::unique_ptr<interfaceWeeklyOptimization> create(bool adqPatchEnabled,
                                                               PROBLEME_HEBDO* problemesHebdo,
                                                               uint numSpace);

protected:
    explicit interfaceWeeklyOptimization(PROBLEME_HEBDO* problemesHebdo, uint numSpace);
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    const uint thread_number_ = 0;
};
} // namespace Antares::Solver::Simulation
