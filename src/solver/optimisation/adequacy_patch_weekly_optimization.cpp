#include "adequacy_patch_weekly_optimization.h"
#include "adq_patch_curtailment_sharing.h"
#include "opt_fonctions.h"
#include "../simulation/simulation.h"
#include "antares/study/area/scratchpad.h"

const int nbHoursInAWeek = 168;

namespace Antares::Solver::Simulation
{
AdequacyPatchOptimization::AdequacyPatchOptimization(PROBLEME_HEBDO* problemeHebdo,
                                                     uint thread_number) :
 interfaceWeeklyOptimization(problemeHebdo, thread_number)
{
}
void AdequacyPatchOptimization::solve(uint weekInTheYear, int hourInTheYear)
{
    problemeHebdo_->adqPatchParams->AdequacyFirstStep = true;
    OPT_OptimisationHebdomadaire(problemeHebdo_, thread_number_);
    problemeHebdo_->adqPatchParams->AdequacyFirstStep = false;

    for (int pays = 0; pays < problemeHebdo_->NombreDePays; ++pays)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[pays]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            memcpy(problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDENS,
                   problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDeDefaillancePositive,
                   problemeHebdo_->NombreDePasDeTemps * sizeof(double));
        else
            memset(problemeHebdo_->ResultatsHoraires[pays]->ValeursHorairesDENS,
                   0,
                   problemeHebdo_->NombreDePasDeTemps * sizeof(double));
    }

    // TODO check if we need to cut SIM_RenseignementProblemeHebdo and just pick out the
    // part that we need
    ::SIM_RenseignementProblemeHebdo(*problemeHebdo_, weekInTheYear, thread_number_, hourInTheYear);
    OPT_OptimisationHebdomadaire(problemeHebdo_, thread_number_);
}

double AdequacyPatchOptimization::calculateDensNewAndTotalLmrViolation(AreaList& areas)
{
    double netPositionInit;
    double densNew;
    double totalNodeBalance;
    double totalLmrViolation = 0.0;
    const int numOfHoursInWeek = 168;

    for (int Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                std::tie(netPositionInit, densNew, totalNodeBalance) = calculateAreaFlowBalance(problemeHebdo_, Area, hour);

                // adjust densNew according to the new specification/request by ELIA
                /* DENS_new (node A) = max [ 0; ENS_init (node A) + net_position_init (node A)
                                        + ∑ flows (node 1 -> node A) - DTG.MRG(node A)] */
                auto& scratchpad = *(areas[Area]->scratchpad[thread_number_]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
                densNew = std::max(0.0, densNew - dtgMrg);
                // write down densNew values for all the hours
                problemeHebdo_->ResultatsHoraires[Area]->ValeursHorairesDENS[hour] = densNew;
                // copy spilled Energy values into spilled Energy values after CSR
                problemeHebdo_->ResultatsHoraires[Area]->ValeursHorairesSpilledEnergyAfterCSR[hour]
                  = problemeHebdo_->ResultatsHoraires[Area]
                      ->ValeursHorairesDeDefaillanceNegative[hour];
                // check LMR violations
                totalLmrViolation
                  += LmrViolationAreaHour(problemeHebdo_, totalNodeBalance, Area, hour);
            }
        }
    }
    return totalLmrViolation;
}

std::vector<double> AdequacyPatchOptimization::calculateENSoverAllAreasForEachHour() const
{
    std::vector<double> sumENS(nbHoursInAWeek, 0.0);
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            double* ENS= problemeHebdo_->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive;
            for (uint h = 0; h < nbHoursInAWeek; ++h)
                sumENS[h] += ENS[h];
        }
    }
    return sumENS;
}

std::set<int> AdequacyPatchOptimization::identifyHoursForCurtailmentSharing(
  std::vector<double> sumENS) const
{
    double threshold = problemeHebdo_->adqPatchParams->ThresholdRunCurtailmentSharingRule;
    std::set<int> triggerCsrSet;
    for (int i = 0; i < nbHoursInAWeek; ++i)
    {
        if (sumENS[i] > threshold)
        {
            triggerCsrSet.insert(i);
        }
    }
    logs.debug() << "number of triggered hours: " << triggerCsrSet.size();
    return triggerCsrSet;
}

std::set<int> AdequacyPatchOptimization::getHoursRequiringCurtailmentSharing() const
{
    const auto sumENS = calculateENSoverAllAreasForEachHour();
    return identifyHoursForCurtailmentSharing(sumENS);
}

void AdequacyPatchOptimization::postProcess(Antares::Data::AreaList& areas,
                                         uint year,
                                         uint week)
{
    double totalLmrViolation = calculateDensNewAndTotalLmrViolation(areas);
    logs.info() << "[adq-patch] Year:" << year + 1 << " Week:" << week + 1
                << ".Total LMR violation:" << totalLmrViolation;
    const std::set<int> hoursRequiringCurtailmentSharing = getHoursRequiringCurtailmentSharing();
    for (int hourInWeek : hoursRequiringCurtailmentSharing)
    {
        logs.info() << "[adq-patch] CSR triggered for Year:" << year + 1
                    << " Hour:" << week * nbHoursInAWeek + hourInWeek + 1;
        HOURLY_CSR_PROBLEM hourlyCsrProblem(hourInWeek, problemeHebdo_);
        hourlyCsrProblem.run(week, year);
    }
}
} // namespace Antares::Solver::Simulation
