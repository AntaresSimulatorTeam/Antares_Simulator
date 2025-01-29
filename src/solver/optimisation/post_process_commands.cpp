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
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/common-eco-adq.h"

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
    for (int hourInWeek: hoursRequiringCurtailmentSharing)
    {
        logs.info() << "[adq-patch] CSR triggered for Year:" << year + 1
                    << " Hour:" << week * nbHoursInWeek + hourInWeek + 1;
        hourlyCsrProblem.setHour(hourInWeek);
        hourlyCsrProblem.run(week, year);
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
