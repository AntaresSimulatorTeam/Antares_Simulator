
#include "post_process_commands.h"
#include "../simulation/common-eco-adq.h"
#include "adequacy_patch_csr/adq_patch_curtailment_sharing.h"

namespace Antares::Solver::Simulation
{
const uint nbHoursInWeek = 168;
// -----------------------------
// Dispatchable Margin
// -----------------------------
DispatchableMarginPostProcessCmd::DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                                                   unsigned int thread_number,
                                                                   AreaList& areas) :
 basePostProcessCommand(problemeHebdo), thread_number_(thread_number), area_list_(areas)
{
}

void DispatchableMarginPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int hourInYear = opt_runtime_data.hourInTheYear;

    area_list_.each([&](Data::Area& area) {
        double* dtgmrg = area.scratchpad[thread_number_]->dispatchableGenerationMargin;
        for (uint h = 0; h != nbHoursInWeek; ++h)
            dtgmrg[h] = 0.;

        if (not area.thermal.list.empty())
        {
            auto& hourlyResults = *(problemeHebdo_->ResultatsHoraires[area.index]);
            auto end = area.thermal.list.end();

            for (auto i = area.thermal.list.begin(); i != end; ++i)
            {
                auto& cluster = *(i->second);
                uint chro = NumeroChroniquesTireesParPays[thread_number_][area.index]
                              ->ThermiqueParPalier[cluster.areaWideIndex];
                auto& matrix = cluster.series->series;
                assert(chro < matrix.width);
                auto& column = matrix.entry[chro];
                assert(hourInYear + nbHoursInWeek <= matrix.height && "index out of bounds");

                for (uint h = 0; h != nbHoursInWeek; ++h)
                {
                    double production = hourlyResults.ProductionThermique[h]
                                          ->ProductionThermiqueDuPalier[cluster.index];
                    dtgmrg[h] += column[h + hourInYear] - production;
                }
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
                                                                 bool computeAnyway) :
 basePostProcessCommand(problemeHebdo),
 area_list_(areas),
 remixWasRun_(remixWasRun),
 computeAnyway_(computeAnyway)
{
}

void HydroLevelsUpdatePostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
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
                                                   unsigned int thread_number) :
 basePostProcessCommand(problemeHebdo),
 area_list_(areas),
 shedding_policy_(sheddingPolicy),
 splx_optimization_(simplexOptimization),
 thread_number_(thread_number)
{
}

void RemixHydroPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    unsigned int hourInYear = opt_runtime_data.hourInTheYear;
    RemixHydroForAllAreas(area_list_,
                          *problemeHebdo_,
                          shedding_policy_,
                          splx_optimization_,
                          thread_number_,
                          hourInYear);
}

// -----------------------------
//  DTG margin for adq patch
// -----------------------------
using namespace Antares::Data::AdequacyPatch;

DTGmarginForAdqPatchPostProcessCmd::DTGmarginForAdqPatchPostProcessCmd(
  PROBLEME_HEBDO* problemeHebdo,
  AreaList& areas,
  unsigned int thread_number) :
 basePostProcessCommand(problemeHebdo), area_list_(areas), thread_number_(thread_number)
{
}

/*!
** Calculate Dispatchable margin for all areas after CSR optimization and adjust ENS
** values if neccessary. If LOLD=1, Sets MRG COST to the max value (unsupplied energy cost)
** */
void DTGmarginForAdqPatchPostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    for (int Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[Area] != physicalAreaInsideAdqPatch)
            continue;

        for (int hour = 0; hour < nbHoursInWeek; hour++)
        {
            // define access to the required variables
            const auto& scratchpad = *(area_list_[Area]->scratchpad[thread_number_]);
            double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];

            auto& hourlyResults = *(problemeHebdo_->ResultatsHoraires[Area]);
            double& dtgMrgCsr = hourlyResults.ValeursHorairesDtgMrgCsr[hour];
            double& ens = hourlyResults.ValeursHorairesDeDefaillancePositive[hour];
            double& mrgCost = hourlyResults.CoutsMarginauxHoraires[hour];
            // calculate DTG MRG CSR and adjust ENS if neccessary
            if (dtgMrgCsr == -1.0) // area is inside adq-patch and it is CSR triggered hour
            {
                dtgMrgCsr = std::max(0.0, dtgMrg - ens);
                ens = std::max(0.0, ens - dtgMrg);
                // set MRG PRICE to value of unsupplied energy cost, if LOLD=1.0 (ENS>0.5)
                if (ens > 0.5)
                    mrgCost = -area_list_[Area]->thermal.unsuppliedEnergyCost;
            }
            else
                dtgMrgCsr = dtgMrg;
        }
    }
}

// -----------------------------
//  Interpolate Water Values
// -----------------------------

InterpolateWaterValuePostProcessCmd::InterpolateWaterValuePostProcessCmd(
  PROBLEME_HEBDO* problemeHebdo,
  AreaList& areas,
  const Date::Calendar& calendar) :
 basePostProcessCommand(problemeHebdo), area_list_(areas), calendar_(calendar)
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
  AreaList& areas) :
 basePostProcessCommand(problemeHebdo), area_list_(areas)
{
}

void HydroLevelsFinalUpdatePostProcessCmd::execute(const optRuntimeData& opt_runtime_data)
{
    updatingWeeklyFinalHydroLevel(area_list_, *problemeHebdo_);
}

// --------------------------------------
//  Curtailment sharing for adq patch
// --------------------------------------
CurtailmentSharingPostProcessCmd::CurtailmentSharingPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                                                   AreaList& areas,
                                                                   unsigned int thread_number) :
 basePostProcessCommand(problemeHebdo), area_list_(areas), thread_number_(thread_number)
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
    HourlyCSRProblem hourlyCsrProblem(problemeHebdo_);
    for (int hourInWeek : hoursRequiringCurtailmentSharing)
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

    for (int Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < nbHoursInWeek; hour++)
            {
                const auto [netPositionInit, densNew, totalNodeBalance]
                  = calculateAreaFlowBalance(problemeHebdo_, Area, hour);
                // adjust densNew according to the new specification/request by ELIA
                /* DENS_new (node A) = max [ 0; ENS_init (node A) + net_position_init (node A)
                                        + ? flows (node 1 -> node A) - DTG.MRG(node A)] */
                const auto& scratchpad = *(area_list_[Area]->scratchpad[thread_number_]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
                // write down densNew values for all the hours
                problemeHebdo_->ResultatsHoraires[Area]->ValeursHorairesDENS[hour]
                  = std::max(0.0, densNew - dtgMrg);
                ;
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

std::set<int> CurtailmentSharingPostProcessCmd::getHoursRequiringCurtailmentSharing() const
{
    const auto sumENS = calculateENSoverAllAreasForEachHour();
    return identifyHoursForCurtailmentSharing(sumENS);
}

std::set<int> CurtailmentSharingPostProcessCmd::identifyHoursForCurtailmentSharing(
  std::vector<double> sumENS) const
{
    double threshold = problemeHebdo_->adqPatchParams->ThresholdRunCurtailmentSharingRule;
    std::set<int> triggerCsrSet;
    for (int i = 0; i < nbHoursInWeek; ++i)
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
    for (int area = 0; area < problemeHebdo_->NombreDePays; ++area)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData.areaMode[area]
            == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            const double* ENS
              = problemeHebdo_->ResultatsHoraires[area]->ValeursHorairesDeDefaillancePositive;
            for (uint h = 0; h < nbHoursInWeek; ++h)
                sumENS[h] += ENS[h];
        }
    }
    return sumENS;
}

} // namespace Antares::Solver::Simulation
