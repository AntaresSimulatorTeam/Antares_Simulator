#pragma once

#include "../simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
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
} // namespace AdequacyPatch
} // end namespace Data
} // namespace Antares
