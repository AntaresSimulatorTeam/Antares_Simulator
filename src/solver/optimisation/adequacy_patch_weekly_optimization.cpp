#include "adequacy_patch_weekly_optimization.h"
#include "opt_fonctions.h"
#include "../simulation/simulation.h"

const int nbHoursInAWeek = 168;

namespace Antares
{
namespace Solver
{
namespace Simulation
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

vector<double> AdequacyPatchOptimization::calculateENSoverAllAreasForEachHour() const
{
    std::vector<double> sumENS(nbHoursInAWeek, 0.0);
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            addArray(sumENS,
                     problemeHebdo_->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive);
    }
    return sumENS;
}

std::set<int> AdequacyPatchOptimization::identifyHoursForCurtailmentSharing(vector<double> sumENS) const
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
    vector<double> sumENS = calculateENSoverAllAreasForEachHour();
    return identifyHoursForCurtailmentSharing(sumENS);
}

void AdequacyPatchOptimization::solveCSR(Antares::Data::AreaList& areas,
                                         uint year,
                                         uint week,
                                         uint numSpace)
{
    double totalLmrViolation
      = calculateDensNewAndTotalLmrViolation(problemeHebdo_, areas, numSpace);
    logs.info() << "[adq-patch] Year:" << year + 1 << " Week:" << week + 1
                << ".Total LMR violation:" << totalLmrViolation;
    const std::set<int> hoursRequiringCurtailmentSharing
      = getHoursRequiringCurtailmentSharing();
    for (int hourInWeek : hoursRequiringCurtailmentSharing)
    {
        logs.info() << "[adq-patch] CSR triggered for Year:" << year + 1
                    << " Hour:" << week * nbHoursInAWeek + hourInWeek + 1;
        HOURLY_CSR_PROBLEM hourlyCsrProblem(hourInWeek, problemeHebdo_);
        hourlyCsrProblem.run(week, year);
    }
}


} // namespace Simulation
} // namespace Solver
} // namespace Antares
