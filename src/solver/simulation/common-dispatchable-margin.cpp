/*
** Copyright 2007-2018 RTE
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

#include <yuni/yuni.h>
#include <antares/study.h>
#include <antares/study/memory-usage.h>
#include <antares/study/area/scratchpad.h>
#include "common-eco-adq.h"
#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"

namespace Antares::Solver::Simulation
{
void DispatchableMarginForAllAreas(const Data::Study& study,
                                   PROBLEME_HEBDO& problem,
                                   uint numSpace,
                                   uint hourInYear)
{
    assert(study.parameters.mode == Data::stdmEconomy);
    const uint nbHoursInWeek = 168;

    study.areas.each([&](Data::Area& area) {
        double* dtgmrg = area.scratchpad[numSpace]->dispatchableGenerationMargin;
        for (uint h = 0; h != nbHoursInWeek; ++h)
            dtgmrg[h] = 0.;

        if (not area.thermal.list.empty())
        {
            auto& hourlyResults = *(problem.ResultatsHoraires[area.index]);
            auto end = area.thermal.list.end();

            for (auto i = area.thermal.list.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);
                uint chro = NumeroChroniquesTireesParPays[numSpace][area.index]
                              ->ThermiqueParPalier[cluster.areaWideIndex];
                auto& matrix = cluster.series->series;
                assert(chro < matrix.width);
                auto& column = matrix.entry[chro];
                assert(hourInYear + nbHoursInWeek <= matrix.height && "index out of bounds");

                for (uint h = 0; h != nbHoursInWeek; ++h)
                {
                    double production = hourlyResults.ProductionThermique[h]
                                          ->ProductionThermiqueDuPalier[cluster.index];
                    dtgmrg[h] += column[h + hourInYear] - production;
                }
            }
        }
    });
}

} // namespace Antares::Solver::Simulation
