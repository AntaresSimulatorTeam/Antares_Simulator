/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/post_process_commands.h"

#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "antares/solver/optimisation/weekly_optimization.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/utils/filename.h"

namespace Antares::Solver::Simulation
{
const uint nbHoursInWeek = 168;

// -----------------------------
// Dispatchable Margin
// -----------------------------
DispatchableMarginPostProcessCmd::DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                                                   unsigned int numSpace,
                                                                   AreaList& areas):
    basePostProcessCommand(problemeHebdo),
    numSpace_(numSpace),
    area_list_(areas)
{
}

void DispatchableMarginPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int hourInYear = opt_runtime_data.hourInTheYear;
    unsigned int year = opt_runtime_data.year;
    area_list_.each(
      [this, &hourInYear, &year](Data::Area& area)
      {
          double* dtgmrg = area.scratchpad[numSpace_].dispatchableGenerationMargin;
          for (uint h = 0; h != nbHoursInWeek; ++h)
          {
              dtgmrg[h] = 0.;
          }

          auto& hourlyResults = problemeHebdo_->ResultatsHoraires[area.index];

          for (const auto& cluster: area.thermal.list.each_enabled_and_not_mustrun())
          {
              const auto& availableProduction = cluster->series.getColumn(year);
              for (uint h = 0; h != nbHoursInWeek; ++h)
              {
                  double production = hourlyResults.ProductionThermique[h]
                                        .ProductionThermiqueDuPalier[cluster->index];
                  dtgmrg[h] += availableProduction[h + hourInYear] - production;
              }
          }
      });
}

// -----------------------------
//  Hydro levels update
// -----------------------------
HydroLevelsUpdatePostProcessCmd::HydroLevelsUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                                                 AreaList& areas,
                                                                 bool remixWasRun,
                                                                 bool computeAnyway):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    remixWasRun_(remixWasRun),
    computeAnyway_(computeAnyway)
{
}

void HydroLevelsUpdatePostProcessCmd::execute(const optRuntimeData&)
{
    computingHydroLevels(area_list_, *problemeHebdo_, remixWasRun_, computeAnyway_);
}

// -----------------------------
//  Remix Hydro
// -----------------------------
RemixHydroPostProcessCmd::RemixHydroPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                                   AreaList& areas,
                                                   SheddingPolicy sheddingPolicy,
                                                   SimplexOptimization simplexOptimization,
                                                   unsigned int numSpace):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    numSpace_(numSpace),
    shedding_policy_(sheddingPolicy),
    splx_optimization_(simplexOptimization)
{
}

void RemixHydroPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int hourInYear = opt_runtime_data.hourInTheYear;
    RemixHydroForAllAreas(area_list_,
                          *problemeHebdo_,
                          shedding_policy_,
                          splx_optimization_,
                          numSpace_,
                          hourInYear);
}

// ----------------------------------
//  Update marginal price after CSR
// ----------------------------------
using namespace Antares::Data::AdequacyPatch;

UpdateMrgPriceAfterCSRcmd::UpdateMrgPriceAfterCSRcmd(PROBLEME_HEBDO* problemeHebdo,
                                                     AreaList& areas,
                                                     unsigned int numSpace):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    numSpace_(numSpace)
{
}

void UpdateMrgPriceAfterCSRcmd::execute(const optRuntimeData&)
{
    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        auto& hourlyResults = problemeHebdo_->ResultatsHoraires[Area];
        const auto& scratchpad = area_list_[Area]->scratchpad[numSpace_];
        const double unsuppliedEnergyCost = area_list_[Area]->thermal.unsuppliedEnergyCost;
        const bool areaInside = problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
                                == physicalAreaInsideAdqPatch;
        for (uint hour = 0; hour < nbHoursInWeek; hour++)
        {
            const bool isHourTriggeredByCsr = problemeHebdo_->adequacyPatchRuntimeData
                                                ->wasCSRTriggeredAtAreaHour(Area, hour);

            // IF UNSP. ENR CSR == 0, MRG. PRICE CSR = MRG. PRICE
            // ELSE, MRG. PRICE CSR = “Unsupplied Energy Cost”
            if (hourlyResults.ValeursHorairesDeDefaillancePositiveCSR[hour] > 0.5 && areaInside)
            {
                hourlyResults.CoutsMarginauxHorairesCSR[hour] = -unsuppliedEnergyCost;
            }
            else
            {
                hourlyResults.CoutsMarginauxHorairesCSR[hour] = hourlyResults
                                                                  .CoutsMarginauxHoraires[hour];
            }

            if (isHourTriggeredByCsr
                && hourlyResults.ValeursHorairesDeDefaillancePositive[hour] > 0.5 && areaInside)
            {
                hourlyResults.CoutsMarginauxHoraires[hour] = -unsuppliedEnergyCost;
            }
        }
    }
}

// -----------------------------
//  DTG margin for adq patch
// -----------------------------
DTGnettingAfterCSRcmd::DTGnettingAfterCSRcmd(PROBLEME_HEBDO* problemeHebdo,
                                             AreaList& areas,
                                             unsigned int numSpace):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    numSpace_(numSpace)
{
}

void DTGnettingAfterCSRcmd::execute(const optRuntimeData&)
{
    // for (uint hour = 0; hour < nbHoursInWeek; hour++)
    // {
    //    if (problemeHebdo_->adequacyPatchRuntimeData->wasCSRTriggeredAtAreaHour(Area, hour);)
    //     {
    //         AdequacyPatchOptimization::solve(,hour)
    //         void AdequacyPatchOptimization::solve(uint weekInTheYear, int hourInTheYear)
    //             ::SIM_RenseignementProblemeHebdo(study_, *problemeHebdo_, weekInTheYear,
    //             thread_number_, hourInTheYear);
    //         OPT_OptimisationHebdomadaire(options_, problemeHebdo_, adqPatchParams_, writer_);

    //     }
    // }

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        auto& hourlyResults = problemeHebdo_->ResultatsHoraires[Area];
        const auto& scratchpad = area_list_[Area]->scratchpad[numSpace_];

        for (uint hour = 0; hour < nbHoursInWeek; hour++)
        {
            const bool isHourTriggeredByCsr = problemeHebdo_->adequacyPatchRuntimeData
                                                ->wasCSRTriggeredAtAreaHour(Area, hour);

            const double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
            const double ens = hourlyResults.ValeursHorairesDeDefaillancePositive[hour];
            const bool areaInside = problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
                                    == physicalAreaInsideAdqPatch;
            if (isHourTriggeredByCsr && areaInside)
            {
                hourlyResults.ValeursHorairesDtgMrgCsr[hour] = std::max(0.0, dtgMrg - ens);
                hourlyResults.ValeursHorairesDeDefaillancePositiveCSR[hour] = std::max(0.0,
                                                                                       ens
                                                                                         - dtgMrg);
            }
            else
            {
                // Default value (when the hour is not triggered by CSR)
                hourlyResults.ValeursHorairesDtgMrgCsr[hour] = dtgMrg;
                hourlyResults.ValeursHorairesDeDefaillancePositiveCSR[hour] = ens;
            }
        }
    }
}

// -----------------------------
//  Interpolate Water Values
// -----------------------------

InterpolateWaterValuePostProcessCmd::InterpolateWaterValuePostProcessCmd(
  PROBLEME_HEBDO* problemeHebdo,
  AreaList& areas,
  const Date::Calendar& calendar):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    calendar_(calendar)
{
}

void InterpolateWaterValuePostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int hourInYear = opt_runtime_data.hourInTheYear;
    interpolateWaterValue(area_list_, *problemeHebdo_, calendar_, hourInYear);
}

// -----------------------------
//  Hydro Levels Final Update
// -----------------------------
// HydroLevelsFinalUpdatePostProcessCmd
HydroLevelsFinalUpdatePostProcessCmd::HydroLevelsFinalUpdatePostProcessCmd(
  PROBLEME_HEBDO* problemeHebdo,
  AreaList& areas):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas)
{
}

void HydroLevelsFinalUpdatePostProcessCmd::execute(const optRuntimeData&)
{
    updatingWeeklyFinalHydroLevel(area_list_, *problemeHebdo_);
}

// --------------------------------------
//  Curtailment sharing for adq patch
// --------------------------------------
CurtailmentSharingPostProcessCmd::CurtailmentSharingPostProcessCmd(
  const AdqPatchParams& adqPatchParams,
  PROBLEME_HEBDO* problemeHebdo,
  AreaList& areas,
  unsigned int numSpace):
    basePostProcessCommand(problemeHebdo),
    area_list_(areas),
    adqPatchParams_(adqPatchParams),
    numSpace_(numSpace)
{
}

void CurtailmentSharingPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int year = opt_runtime_data.year;
    unsigned int week = opt_runtime_data.week;

    double totalLmrViolation = calculateDensNewAndTotalLmrViolation();
    logs.info() << "[adq-patch] Year:" << year + 1 << " Week:" << week + 1
                << ".Total LMR violation:" << totalLmrViolation;
    const std::set<int> hoursRequiringCurtailmentSharing = getHoursRequiringCurtailmentSharing();
    HourlyCSRProblem hourlyCsrProblem(adqPatchParams_, problemeHebdo_);

    auto backup = problemeHebdo_->CorrespondanceVarNativesVarOptim;

    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo_);
    
    for (uint hour = 0; hour < nbHoursInWeek; hour++){
        auto f = problemeHebdo_->ValeursDeNTC[hour].ValeurDuFlux;
        // logs.info() << "[adq-patch] flux Before ADQPTCH:" <<f;// << problemeHebdo_;
    }



    // logs.info()  << "Before:  ADQP::";
    // ens bef adqp
    std::vector<std::vector<double>> ENSBef, ENSAfter, SpillBef, SpillAfter;
    ENSBef.resize(problemeHebdo_->NombreDePays);
    SpillBef.resize(problemeHebdo_->NombreDePays);
    ENSAfter.resize(problemeHebdo_->NombreDePays);
    SpillAfter.resize(problemeHebdo_->NombreDePays);
    
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        // logs.info() << area << " with ens / Spill:";
        ENSBef[area] = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillancePositive;
        SpillBef[area] = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillanceNegative;
    }


    // RUN ADQP
    for (int hourInWeek: hoursRequiringCurtailmentSharing)
    {
        hourlyCsrProblem.setHour(hourInWeek);
        hourlyCsrProblem.run(week, year);
    }

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area){
        logs.info() << area << " with ens / Spill:";
        ENSAfter[area] = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillancePositive;
        SpillAfter[area] = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillanceNegative;
    }

    // Filtering Affected Areas, i.e, areas with weird case
    std::set<uint32_t> affectedAreas;

    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area) {
        for (uint h = 0; h < nbHoursInWeek; ++h) {
            logs.info() << area << " with ENSBef, ENSAfter / SpillBef, SpillAfter:" ;
            logs.info() << ENSBef[area][h] << " " << ENSAfter[area][h] << " " 
                        << SpillBef[area][h] << " " << SpillAfter[area][h] << " ";

            // Check if ENS transitioned from 0 to positive
            if (ENSBef[area][h] == 0 && ENSAfter[area][h] > 0) {
                affectedAreas.insert(area);
            }

            // Check if Spillage transitioned from 0 to positive
            if (SpillBef[area][h] == 0 && SpillAfter[area][h] > 0) {
                affectedAreas.insert(area);
            }
        }
    }

    // HERE WE DISPATCH IN CASES NEEDED 
    if (!affectedAreas.empty()){
        // Print the selected affected areas
        logs.info() << "Affected Areas:";
        for (const auto& area : affectedAreas) {
            logs.info() << "Area " << area;
        }

        // accessing old bounds adress
        std::vector<double>& Xmax = problemeHebdo_->ProblemeAResoudre->Xmax;
        std::vector<double>& Xmin = problemeHebdo_->ProblemeAResoudre->Xmin;

        problemeHebdo_->CorrespondanceVarNativesVarOptim = backup;

        // nonAffected Areas dispatch has to be fixed, we do this by fixing their ENS to the old one
        int var;
        double oldValue;
        for (uint h = 0; h < nbHoursInWeek ; ++h){
            for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area) {
                if (! affectedAreas.contains(area)){
                    // logs.info() << "[adq-patch] Hello NTCs";
                    var = problemeHebdo_->CorrespondanceVarNativesVarOptim[h].NumeroDeVariableDefaillancePositive[area];
                    oldValue = ENSAfter[area][h];
                    Xmax[var] = oldValue + 0.1;
                    Xmin[var] = oldValue - 0.1;
                
                }
            }
        }    

        // the flow is fixer for every connection
        for (uint hourInWeek = 0; hourInWeek < nbHoursInWeek ; ++hourInWeek)// hourInWeek: hoursRequiringCurtailmentSharing)
        {
            for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; ++Interco)
            {
                int var = variableManager.NTCDirect(Interco, hourInWeek);
                auto f = problemeHebdo_->ValeursDeNTC[hourInWeek].ValeurDuFlux[Interco]; 
                Xmax[var] = f + 0.1;
                Xmin[var] = f - 0.1;
                logs.info() << problemeHebdo_->ProblemeAResoudre->NomDesVariables[var];
            }
            // logs.info() << "[adq-patch] Hello NTCs";
        }

        // // here we redispatch truly and smartly
        const int NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                            ->NombreDePasDeTempsPourUneOptimisation;


        int DernierPdtDeLIntervalle;
        for (uint pdtHebdo = 0, numeroDeLIntervalle = 0; pdtHebdo < problemeHebdo_->NombreDePasDeTemps;
            pdtHebdo = DernierPdtDeLIntervalle, numeroDeLIntervalle++)
        {
            int PremierPdtDeLIntervalle = pdtHebdo;
            DernierPdtDeLIntervalle = pdtHebdo + NombreDePasDeTempsPourUneOptimisation;
            auto optPeriodStringGenerator = createOptPeriodAsString(
            problemeHebdo_->OptimisationAuPasHebdomadaire,
            numeroDeLIntervalle,
            problemeHebdo_->weekInTheYear,
            problemeHebdo_->year);
            bool b = OPT_AppelDuSimplexe(opt_runtime_data.weeklyOptimization.options_,
                                    problemeHebdo_,
                                    numeroDeLIntervalle,
                                    1,
                                    *optPeriodStringGenerator,
                                    opt_runtime_data.weeklyOptimization.writer_);
        } // END REDISPATCH
        
    }
    
}

double CurtailmentSharingPostProcessCmd::calculateDensNewAndTotalLmrViolation()
{
    double totalLmrViolation = 0.0;

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (uint hour = 0; hour < nbHoursInWeek; hour++)
            {
                const auto [netPositionInit, densNew, totalNodeBalance] = calculateAreaFlowBalance(
                  problemeHebdo_,
                  adqPatchParams_.setToZeroOutsideInsideLinks,
                  Area,
                  hour);
                // adjust densNew according to the new specification/request by ELIA
                /* DENS_new (node A) = max [ 0; ENS_init (node A) + net_position_init (node A)
                                        + ? flows (node 1 -> node A) - DTG.MRG(node A)] */
                const auto& scratchpad = area_list_[Area]->scratchpad[numSpace_];
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
                // write down densNew values for all the hours
                problemeHebdo_->ResultatsHoraires[Area].ValeursHorairesDENS[hour] = std::max(
                  0.0,
                  densNew);
                // check LMR violations
                totalLmrViolation += LmrViolationAreaHour(
                  problemeHebdo_,
                  totalNodeBalance,
                  adqPatchParams_.curtailmentSharing.thresholdDisplayViolations,
                  Area,
                  hour);
            }
        }
    }
    return totalLmrViolation;
}

std::set<int> CurtailmentSharingPostProcessCmd::getHoursRequiringCurtailmentSharing() const
{
    const auto sumENS = calculateENSoverAllAreasForEachHour();
    return identifyHoursForCurtailmentSharing(sumENS);
}

std::set<int> CurtailmentSharingPostProcessCmd::identifyHoursForCurtailmentSharing(
  const std::vector<double>& sumENS) const
{
    const double threshold = adqPatchParams_.curtailmentSharing.thresholdRun;
    std::set<int> triggerCsrSet;
    for (uint i = 0; i < nbHoursInWeek; ++i)
    {
        if (sumENS[i] > threshold)
        {
            triggerCsrSet.insert(i);
        }
    }
    logs.debug() << "number of triggered hours: " << triggerCsrSet.size();
    return triggerCsrSet;
}

std::vector<double> CurtailmentSharingPostProcessCmd::calculateENSoverAllAreasForEachHour() const
{
    std::vector<double> sumENS(nbHoursInWeek, 0.0);
    for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            const std::vector<double>& ENS = problemeHebdo_->ResultatsHoraires[area]
                                               .ValeursHorairesDeDefaillancePositive;
            for (uint h = 0; h < nbHoursInWeek; ++h)
            {
                sumENS[h] += ENS[h];
            }
        }
    }
    return sumENS;
}

} // namespace Antares::Solver::Simulation


















    // OPT_OptimisationHebdomadaire(opt_runtime_data.weeklyOptimization.options_,
    //                              problemeHebdo_,
    //                              opt_runtime_data.weeklyOptimization.writer_,
    //                              opt_runtime_data.weeklyOptimization.simulationObserver_);



    // opt_runtime_data.weeklyOptimization.solve();

    // opt_runtime_data.weeklyOptimization.solve();




    // for (auto& cnxn: opt_runtime_data.weeklyOptimization.problemeHebdo_->ValeursDeNTC){
    //         logs.info() << "[adq-patch] Hello ValeurDeFlux AFTER adq is:"<<cnxn.ValeurDuFlux;
    //         cnxn.ValeurDeNTCOrigineVersExtremite = cnxn.ValeurDuFlux;
    //         cnxn.ValeurDeNTCExtremiteVersOrigine = cnxn.ValeurDuFlux;
    // }
    // for (auto& cnxn: opt_runtime_data.weeklyOptimization.problemeHebdo_->ValeursDeNTC){
    //     for (auto& v: cnxn.ValeurDeNTCOrigineVersExtremite){
    //         v = v + 1;
    //     }
    //     for (auto& v: cnxn.ValeurDeNTCExtremiteVersOrigine){
    //         v = v + 1;
    //     }
    // }

    // // REDISPATCH
    // for (int hourInWeek: hoursRequiringCurtailmentSharing){
    //     for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; ++Interco){
    //         int var = variableManager.NTCDirect(Interco, hourInWeek);
    //         auto& Xmax = problemeHebdo_->ProblemeAResoudre.get()->Xmax[Interco];// .Xmax[var] =
    //         1; auto& Xmin = problemeHebdo_->ProblemeAResoudre.get()->Xmin[Interco];// .Xmax[var]
    //         = 1;

    //         auto f = problemeHebdo_->ValeursDeNTC[Interco].ValeurDuFlux[Interco];//
    //         ->ValeurDeNTCOrigineVersExtremite[Interco].ValeurDeFlux; Xmax = f + 1; Xmin = f - 1;
    //         // auto c = b[Interco].ValeurDuFlux;

    //     }
    //     logs.info() << "[adq-patch] Hello NTCs";

    // }

    // // REDISPATCH OLD Flow cons
    // for (int hourInWeek: hoursRequiringCurtailmentSharing){
    //     for (uint32_t Interco = 0; Interco < problemeHebdo_->NombreDInterconnexions; ++Interco){
    //         // int var = variableManager.NTCDirect(Interco, hourInWeek);
    //         auto& Xmax = problemeHebdo_->ProblemeAResoudre.get()->Xmax[Interco];// .Xmax[var] =
    //         1; auto& Xmin = problemeHebdo_->ProblemeAResoudre.get()->Xmin[Interco];// .Xmax[var]
    //         = 1;

    //         auto f = problemeHebdo_->ValeursDeNTC[Interco].ValeurDuFlux[Interco];//
    //         ->ValeurDeNTCOrigineVersExtremite[Interco].ValeurDeFlux; Xmax = f + 1; Xmin = f - 1;
    //         // auto c = b[Interco].ValeurDuFlux;

    //     }
    //     logs.info() << "[adq-patch] Hello NTCs";

    // }

    // auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);
    // for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
    //      pdtHebdo++, pdtJour++)
    // {
    //     VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo->ValeursDeNTC[pdtHebdo];

    //     for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    //     {
    //         int var = variableManager.NTCDirect(interco, pdtJour);
    //         const COUTS_DE_TRANSPORT& CoutDeTransport = problemeHebdo->CoutDeTransport[interco];

    //         Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];
    //         Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]);

    //         if (std::isinf(Xmax[var]) && Xmax[var] > 0)
    //         {





    //    logs.info()  << "After ADQP::";    
    // for (uint32_t area = 0; area < problemeHebdo_->NombreDePays; ++area)
    // {
    //     // ens bef adqp
    //     std::vector<double> ENSAfter = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillancePositive;
    //     std::vector<double> SpillAfter = problemeHebdo_->ResultatsHoraires[area].ValeursHorairesDeDefaillanceNegative;

    //     if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[area] == Data::AdequacyPatch::physicalAreaInsideAdqPatch){
    //         logs.info() << area << "with ens / Spill:";
    //         for (uint h = 0; h < 5; ++h)
    //         {                
    //             logs.info() <<  " :" << ENSAfter[h] <<" "<< SpillAfter[h];
    //         }
    //     }
    // }




    // // After the ADQP ran, we check if there is apparition de l'ENS, Spillage qq part
    // for (uint i = 0; i < nbHoursInWeek; ++i)
    // {
    //     int ENSBeforeAdqp = 0; // Adqp stands for Adequacy Patch
    //     int ENSAfterAdqp = 0;
    //     int SpillageBeforeAdqp = 0;
    //     int SpillageAfterAdqp = 0;

    // }
