/*
** Copyright 2007-2022 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#ifndef __SOLVER_ADEQUACY_FUNCTIONS_H__
#define __SOLVER_ADEQUACY_FUNCTIONS_H__

#include <antares/study/fwd.h>
#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
//! A default threshold value for initiate curtailment sharing rule
const double defaultValueThresholdInitiateCurtailmentSharingRule = 0.0;
//! A default threshold value for display local matching rule violations
const double defaultValueThresholdDisplayLocalMatchingRuleViolations = 0.0;
//! CSR Variables relaxation threshold
const int defaultValueThresholdVarBoundsRelaxation = 3;
/*!
 * Determines restriction type for transmission links for first step of adequacy patch.
 *
 * @param ProblemeHebdo PROBLEME_HEBDO*: Weekly problem structure.
 *
 * @param Interco int: Index of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo, int Interco);

/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is inside adq path (type 2).
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
ntcSetToZeroStatus_AdqPatchStep1 SetNTCForAdequacyFirstStepOriginNodeInsideAdq(AdequacyPatchMode ExtremityNodeAdequacyPatchType);

/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is outside adq path (type 1).
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @param setToZeroNTCfromOutToIn_AdqPatch bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param setToZeroNTCfromOutToOut_AdqPatch bool: Switch to cut links between nodes outside adq
 * patch (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch);

/*!
 * Sets link bounds for first step of adequacy patch or leaves default values if adequacy patch is
 * not used.
 */
void setNTCbounds(double& Xmax,
                  double& Xmin,
                  VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo);

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
double LmrViolationAreaHour(PROBLEME_HEBDO* ProblemeHebdo,
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
double calculateDensNewAndTotalLmrViolation(PROBLEME_HEBDO* ProblemeHebdo);

/*!
** ** \brief add values of a array B to vector A, A[i]=A[i]+B[i]
** **
** ** \param A A vector
** ** \param B An array
** ** \return
** */
void addArray(std::vector<double>& A, const double* B);

} // end namespace Antares
} // end namespace Data
} // end namespace AdequacyPatch
#endif /* __SOLVER_ADEQUACY_FUNCTIONS_H__ */
