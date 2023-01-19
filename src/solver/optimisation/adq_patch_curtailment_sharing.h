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

#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares::Data::AdequacyPatch
{
//! A default threshold value for initiate curtailment sharing rule
const double defaultThresholdToRunCurtailmentSharing = 0.0;
//! A default threshold value for display local matching rule violations
const double defaultThresholdDisplayLocalMatchingRuleViolations = 0.0;
//! CSR Variables relaxation threshold
const int defaultValueThresholdVarBoundsRelaxation = 3;


/*!
 * Calculates curtailment sharing rule parameters netPositionInit, densNew and totalNodeBalance per
 * given area and hour.
 */
std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* ProblemeHebdo,
                                                            int Area,
                                                            int hour);

/*!
 * Calculate total local matching rule violation per one area, per one hour.
 */
double LmrViolationAreaHour(const PROBLEME_HEBDO* ProblemeHebdo,
                            double totalNodeBalance,
                            int Area,
                            int hour);

/*!
 * Calculate densNew values for all hours and areas inside adequacy patch and places them into
 * ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDENS[hour] to be displayed in output.
 * copy-pastes spilled Energy values into spilled Energy values after CSR
 * calculates total LMR violations and LMR violations per area per hour inside
 * ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour]
 */
double calculateDensNewAndTotalLmrViolation(PROBLEME_HEBDO* ProblemeHebdo,
                                            AreaList& areas,
                                            uint numSpace);

/*!
** ** \brief Calculate Dispatchable margin for all areas after CSR optimization and adjust ENS
** ** values if neccessary. If LOLD=1, Sets MRG COST to the max value (unsupplied energy cost)
** **
** ** \param study The Antares study
** ** \param problem The weekly problem, from the solver
** ** \return
** */
void adqPatchPostProcess(const Data::Study& study, PROBLEME_HEBDO& problem, int numSpace);
} // namespace Antares::Data::AdequacyPatch
