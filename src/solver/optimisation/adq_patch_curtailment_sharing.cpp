
#include "adq_patch_curtailment_sharing.h"

#include "opt_fonctions.h"
#include <math.h>
#include "../study/area/scratchpad.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{

double LmrViolationAreaHour(PROBLEME_HEBDO* ProblemeHebdo,
                            double totalNodeBalance,
                            int Area,
                            int hour)
{
    double ensInit
      = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    double threshold = ProblemeHebdo->adqPatchParams->ThresholdDisplayLocalMatchingRuleViolations;

    ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 0;
    // check LMR violations
    if ((ensInit > 0.0) && (totalNodeBalance < 0.0)
        && (Math::Abs(totalNodeBalance) > ensInit + Math::Abs(threshold)))
    {
        ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 1;
        return Math::Abs(totalNodeBalance);
    }
    return 0.0;
}

double calculateDensNewAndTotalLmrViolation(PROBLEME_HEBDO* ProblemeHebdo,
                                            AreaList& areas,
                                            uint numSpace)
{
    double netPositionInit;
    double densNew;
    double totalNodeBalance;
    double totalLmrViolation = 0.0;
    const int numOfHoursInWeek = 168;

    for (int Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                std::tie(netPositionInit, densNew, totalNodeBalance)
                  = calculateAreaFlowBalance(ProblemeHebdo, Area, hour);
                // adjust densNew according to the new specification/request by ELIA
                /* DENS_new (node A) = max [ 0; ENS_init (node A) + net_position_init (node A)
                                        + ∑ flows (node 1 -> node A) - DTG.MRG(node A)] */
                auto& scratchpad = *(areas[Area]->scratchpad[numSpace]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
                densNew = Math::Max(0.0, densNew - dtgMrg);
                // write down densNew values for all the hours
                ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDENS[hour] = densNew;
                // copy spilled Energy values into spilled Energy values after CSR
                ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesSpilledEnergyAfterCSR[hour]
                  = ProblemeHebdo->ResultatsHoraires[Area]
                      ->ValeursHorairesDeDefaillanceNegative[hour];
                // check LMR violations
                totalLmrViolation
                  += LmrViolationAreaHour(ProblemeHebdo, totalNodeBalance, Area, hour);
            }
        }
    }
    return totalLmrViolation;
}

std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* ProblemeHebdo,
                                                            int Area,
                                                            int hour)
{
    int Interco;
    double netPositionInit = 0;
    double flowsNode1toNodeA = 0;
    double ensInit;
    double densNew;
    bool includeFlowsOutsideAdqPatchToDensNew
      = !ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;

    Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Area];
    while (Interco >= 0)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit -= ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              -= Math::Min(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
    }
    Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
    while (Interco >= 0)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit += ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              += Math::Max(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
    }

    ensInit = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    if (includeFlowsOutsideAdqPatchToDensNew)
    {
        densNew = Math::Max(0.0, ensInit + netPositionInit + flowsNode1toNodeA);
        return std::make_tuple(netPositionInit, densNew, netPositionInit + flowsNode1toNodeA);
    }
    else
    {
        densNew = Math::Max(0.0, ensInit + netPositionInit);
        return std::make_tuple(netPositionInit, densNew, netPositionInit);
    }
}

void adqPatchPostProcess(const Data::Study& study, PROBLEME_HEBDO& problem, int numSpace)
{
    if (!study.parameters.adqPatch.enabled)
        return;

    const int numOfHoursInWeek = 168;
    for (int Area = 0; Area < problem.NombreDePays; Area++)
    {
        if (problem.adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                // define access to the required variables
                auto& scratchpad = *(study.areas[Area]->scratchpad[numSpace]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];

                auto& hourlyResults = *(problem.ResultatsHoraires[Area]);
                double& dtgMrgCsr = hourlyResults.ValeursHorairesDtgMrgCsr[hour];
                double& ens = hourlyResults.ValeursHorairesDeDefaillancePositive[hour];
                double& mrgCost = hourlyResults.CoutsMarginauxHoraires[hour];
                // calculate DTG MRG CSR and adjust ENS if neccessary
                if (dtgMrgCsr == -1.0) // area is inside adq-patch and it is CSR triggered hour
                {
                    dtgMrgCsr = Math::Max(0.0, dtgMrg - ens);
                    ens = Math::Max(0.0, ens - dtgMrg);
                    // set MRG PRICE to value of unsupplied energy cost, if LOLD=1.0 (ENS>0.5)
                    if (ens > 0.5)
                        mrgCost = -study.areas[Area]->thermal.unsuppliedEnergyCost;
                }
                else
                    dtgMrgCsr = dtgMrg;
            }
        }
    }
}

} // namespace AdequacyPatch
} // end namespace Data
} // namespace Antares

void HOURLY_CSR_PROBLEM::calculateCsrParameters()
{
    double netPositionInit;
    double ensInit;
    double spillageInit;
    int hour = hourInWeekTriggeredCsr;

    for (int Area = 0; Area < problemeHebdo->NombreDePays; Area++)
    {
        if (problemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            // set DTG MRG CSR in all areas inside adq-path for all CSR triggered hours to -1.0
            problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDtgMrgCsr[hour] = -1.0;
            // calculate netPositionInit and the RHS of the AreaBalance constraints
            std::tie(netPositionInit, ignore, ignore)
              = calculateAreaFlowBalance(problemeHebdo, Area, hour);

            ensInit
              = problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
            spillageInit
              = problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillanceNegative[hour];

            rhsAreaBalanceValues[Area] = ensInit + netPositionInit - spillageInit;
        }
    }
    return;
}

void HOURLY_CSR_PROBLEM::resetProblem()
{
    OPT_LiberationProblemesSimplexe(problemeHebdo);
}

void HOURLY_CSR_PROBLEM::buildProblemVariables()
{
    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique_CSR(problemeHebdo, *this);
}

void HOURLY_CSR_PROBLEM::buildProblemConstraintsLHS()
{
    OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique_CSR(problemeHebdo, *this);
}

void HOURLY_CSR_PROBLEM::setVariableBounds()
{
    OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique_CSR(problemeHebdo, *this);
}

void HOURLY_CSR_PROBLEM::buildProblemConstraintsRHS()
{
    OPT_InitialiserLeSecondMembreDuProblemeQuadratique_CSR(problemeHebdo, *this);
}

void HOURLY_CSR_PROBLEM::setProblemCost()
{
    OPT_InitialiserLesCoutsQuadratiques_CSR(problemeHebdo, *this);
}

void HOURLY_CSR_PROBLEM::solveProblem(uint week, int year)
{
    ADQ_PATCH_CSR(problemeHebdo->ProblemeAResoudre, *this, week, year);
}

void HOURLY_CSR_PROBLEM::run(uint week, uint year)
{
    resetProblem();
    calculateCsrParameters();
    buildProblemVariables();
    buildProblemConstraintsLHS();
    setVariableBounds();
    buildProblemConstraintsRHS();
    setProblemCost();
    solveProblem(week, year);
}