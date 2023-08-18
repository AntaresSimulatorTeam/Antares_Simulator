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

#include "sim_structure_probleme_economique.h"

namespace Antares::Data::AdequacyPatch
{

/*!
 * Calculates curtailment sharing rule parameters netPositionInit, densNew and totalNodeBalance per
 * given area and hour.
 */
std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* problemeHebdo,
                                                            bool setNTCOutsideToInsideToZero,
                                                            int Area,
                                                            int hour);

/*!
 * Calculate total local matching rule violation per one area, per one hour.
 */
double LmrViolationAreaHour(const PROBLEME_HEBDO* problemeHebdo,
                            double totalNodeBalance,
                            const double threshold,
                            int Area,
                            int hour);

/*!
 * Calculate densNew values for all hours and areas inside adequacy patch and places them into
 * problemeHebdo->ResultatsHoraires[Area].ValeursHorairesDENS[hour] to be displayed in output.
 * copy-pastes spilled Energy values into spilled Energy values after CSR
 * calculates total LMR violations and LMR violations per area per hour inside
 * problemeHebdo->ResultatsHoraires[Area].ValeursHorairesLmrViolations[hour]
 */
double calculateDensNewAndTotalLmrViolation(PROBLEME_HEBDO* problemeHebdo,
                                            AreaList& areas,
                                            uint numSpace);
} // namespace Antares::Data::AdequacyPatch
