// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

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
double LmrViolationAreaHour(PROBLEME_HEBDO* problemeHebdo,
                            double totalNodeBalance,
                            double threshold,
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
