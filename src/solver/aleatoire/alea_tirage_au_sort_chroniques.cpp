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

#include "../simulation/sim_extern_variables_globales.h"
#include "alea_fonctions.h"
#include <cassert>

using namespace Yuni;
using namespace Antares;
using namespace Antares::Data;

void ApplyRandomTSnumbers(const Study& study,
                          unsigned int year,
                          uint numSpace)
{
    // ------------------------------
    // Transmission capacities
    // ------------------------------
    // each link
    for (unsigned int linkIndex = 0; linkIndex < study.runtime->interconnectionsCount(); ++linkIndex)
    {
        AreaLink* link = study.runtime->areaLink[linkIndex];
        assert(year < link->timeseriesNumbers.height);
        NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION& ptchro
          = NumeroChroniquesTireesParInterconnexion[numSpace][linkIndex];
        const uint directWidth = link->directCapacities.width;
        [[maybe_unused]] const uint indirectWidth = link->indirectCapacities.width;
        assert(directWidth == indirectWidth);
        ptchro.TransmissionCapacities
          = (directWidth != 1) ? link->timeseriesNumbers[0][year] : 0; // zero-based
    }
}
