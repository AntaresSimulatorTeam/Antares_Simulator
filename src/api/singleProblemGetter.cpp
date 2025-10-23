#include "antares/api/singleProblemGetter.h"

#include <ranges>

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/solver/hydro/management/HydroInputsChecker.h"
#include "antares/solver/optimisation/LinearProblemMatrix.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/simulation.h"

namespace
{
constexpr int optimizationNumber = 1;  // the 1st optim is available for now
constexpr int numeroDeLIntervalle = 0; // simplex-range = week
constexpr int numSpace = 0;            // full sequential
constexpr int PremierPdtDeLIntervalle = 0;
constexpr int DernierPdtDeLIntervalle = 168; // 1 week = 7*24 hours
const std::string kName = "my-name";         // Arbitrary
} // namespace

namespace Antares::Solver
{
void SingleProblemGetter::load(const std::filesystem::path& study_path)
{
    FileTreeStudyLoader study_loader(study_path);
    study_ = study_loader.load();
    SIM_InitialisationProblemeHebdo(*study_,
                                    pb_,
                                    /* NombreDePasDeTemps = */ 168,
                                    numSpace);

    study_->computePThetaInfForThermalClusters(); // PthetaInf

    scratchmap_ = study_->areas.buildScratchMap(numSpace);

    // TODO duplication
    if (!pb_.LeProblemeADejaEteInstancie)
    {
        pb_.NombreDeJours = (int)(pb_.NombreDePasDeTemps / pb_.NombreDePasDeTempsDUneJournee);

        if (!pb_.OptimisationAuPasHebdomadaire)
        {
            pb_.NombreDePasDeTempsPourUneOptimisation = pb_.NombreDePasDeTempsDUneJournee;
        }
        else
        {
            pb_.NombreDePasDeTempsPourUneOptimisation = pb_.NombreDePasDeTemps;
        }

        OPT_AllocDuProblemeAOptimiser(&pb_);

        OPT_ChainagesDesIntercoPartantDUnNoeud(&pb_);

        pb_.LeProblemeADejaEteInstancie = true;
    }
}

ConstantDataFromAntares SingleProblemGetter::getConstantData()
{
    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(&pb_);

    auto builder_data = NewGetConstraintBuilderFromProblemHebdo(&pb_);
    ConstraintBuilder builder(builder_data);
    LinearProblemMatrix linearProblemMatrix(&pb_, builder);
    linearProblemMatrix.Run();

    return translator_.commonProblemData(pb_.ProblemeAResoudre.get());
}

// TODO duplication (economy)
void prepareClustersInMustRunMode(const Antares::Data::Study& study,
                                  Data::Area::ScratchMap& scratchmap,
                                  uint year)
{
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *study.areas[i];
        auto& scratchpad = scratchmap.at(&area);

        std::ranges::fill(scratchpad.mustrunSum, 0);

        auto& mrs = scratchpad.mustrunSum;
        for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
        {
            const auto& availableProduction = cluster->series.getColumn(year);
            for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
            {
                mrs[h] += availableProduction[h];
            }
        }
    }
}

const Details::YearlyData& SingleProblemGetter::getYearlyData(unsigned year)
{
    // TODO Use std::find for a single search
    if (allData_.contains(year)) // We already have data for this year
    {
        return allData_.at(year);
    }

    int nbYears = 0;
    std::map<unsigned int, bool> isYearPerformed; // TODO check year number
    for (uint year = 0; year < study_->parameters.nbYears; ++year)
    {
        isYearPerformed[year] = study_->parameters.yearsFilter[year];
        if (study_->parameters.yearsFilter[year])
        {
            ++nbYears;
        }
    }

    auto& dataForYear = allData_[year];
    auto& randomForParallelYears = dataForYear.randomForParallelYears;

    randomForParallelYears.emplace(nbYears, study_->parameters.power.fluctuations);
    randomForParallelYears->allocate(*study_);

    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study_->parameters.seed[Data::seedHydroManagement]);
    randomForParallelYears->compute(*study_, 1, isYearPerformed, randomHydroGenerator);

    // Getting random tables for this year
    // Index of the current year in the list of structures
    uint indexYear = randomForParallelYears->yearNumberToIndex[year];

    /*
      Side effects for HydroInputsChecker are limited to the year scope
      inside the study.
      more specifically, area.hydro.managementData[year]
      So "out-of-order" such as calls "y=0, y=4, y=0" should be fine
    */
    Antares::HydroInputsChecker hydroInputsChecker(*study_);
    hydroInputsChecker.Execute(year);
    hydroInputsChecker.CheckForErrors();

    prepareClustersInMustRunMode(*study_, scratchmap_, year);

    auto& randomForCurrentYear = randomForParallelYears->pYears[indexYear];
    std::tie(dataForYear.hydroLevels, dataForYear.ventilationResults) = computeHydroLevels(
      year,
      randomForCurrentYear.pReservoirLevels);

    return dataForYear;
}

std::pair<Details::HydroLevels, Antares::HYDRO_VENTILATION_RESULTS>
SingleProblemGetter::computeHydroLevels(unsigned year, const std::vector<double>& initialLevel)
{
    // For each year
    Antares::Solver::NullResultWriter resultWriter;
    Antares::HydroManagement hydroManagement(study_->areas,
                                             study_->parameters,
                                             study_->calendar,
                                             resultWriter);

    Details::HydroLevels hydroLevels;
    hydroManagement.makeVentilation(initialLevel, year, scratchmap_);

    const auto& ventilationResults = hydroManagement.ventilationResults();
    const auto& calendar = study_->calendar;

    int areaIndex = 0;
    for (const auto& [_, area]: study_->areas)
    {
        auto inflows = area->hydro.series->storage.getColumn(year);
        auto& level = hydroLevels[area];

        // Initialize first week level
        uint firstDay = calendar.weeks[0].daysYear.first;
        level[0] = ventilationResults[areaIndex].NiveauxReservoirsDebutJours[firstDay]
                   * area->hydro.reservoirCapacity;

        // Compute sums for first week to use in week 1
        double inflowsPrevWeek = 0.0, turbinedPrevWeek = 0.0;
        for (uint day = calendar.weeks[0].daysYear.first; day < calendar.weeks[0].daysYear.end;
             ++day)
        {
            inflowsPrevWeek += inflows[day];
            turbinedPrevWeek += ventilationResults[areaIndex].HydrauliqueModulableQuotidien[day];
        }

        // Loop over remaining weeks
        for (unsigned week = 1; week < calendar.maxWeeksInYear; ++week)
        {
            // Update level using previous week values
            level[week] = level[week - 1] + inflowsPrevWeek - turbinedPrevWeek;

            // Compute sums for this week to use next iteration
            inflowsPrevWeek = 0.0;
            turbinedPrevWeek = 0.0;
            for (uint day = calendar.weeks[week].daysYear.first;
                 day < calendar.weeks[week].daysYear.end;
                 ++day)
            {
                inflowsPrevWeek += inflows[day];
                turbinedPrevWeek += ventilationResults[areaIndex]
                                      .HydrauliqueModulableQuotidien[day];
            }

            logs.debug() << "week=" << week << " prevInflows=" << inflowsPrevWeek
                         << " prevTurbined=" << turbinedPrevWeek << " level=" << level[week];
        }

        areaIndex++;
    }
    return {hydroLevels, ventilationResults};
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    const auto [year, week] = id;

    pb_.year = id.year;
    pb_.weekInTheYear = week;

    auto [hydroLevels, randomForParallelYears, ventilationResults] = getYearlyData(year);

    const auto hourInTheYear = 168 * week; // TODO
    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   week,
                                   hourInTheYear,
                                   ventilationResults,
                                   scratchmap_);

    // Apply hydro levels
    for (uint areaIndex = 0; areaIndex < study_->areas.size(); ++areaIndex)
    {
        const auto* area = study_->areas.byIndex[areaIndex];
        double initialLevel = hydroLevels[area][week];
        pb_.CaracteristiquesHydrauliques[areaIndex].NiveauInitialReservoir = initialLevel;
    }

    uint indexYear = randomForParallelYears->yearNumberToIndex[year];
    auto& randomForCurrentYear = randomForParallelYears->pYears[indexYear];

    // required at least for OPT_SommeDesPminThermiques (RHS)
    Antares::Solver::Simulation::BuildThermalPartOfWeeklyProblem(
      *study_,
      pb_,
      hourInTheYear,
      randomForCurrentYear.pThermalNoisesByArea,
      year);

    OPT_VerifierPresenceReserveJmoins1(&pb_);

    OPT_InitialiserLesPminHebdo(&pb_);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(&pb_);

    OPT_MaxDesPmaxHydrauliques(&pb_);

    if (pb_.OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(&pb_);
    }

    OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(&pb_,
                                                           PremierPdtDeLIntervalle,
                                                           DernierPdtDeLIntervalle,
                                                           optimizationNumber);

    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&pb_,
                                                    PremierPdtDeLIntervalle,
                                                    DernierPdtDeLIntervalle,
                                                    numeroDeLIntervalle,
                                                    optimizationNumber);

    OPT_InitialiserLesCoutsLineaire(&pb_, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    return translator_.translate(pb_.ProblemeAResoudre.get(), kName);
}

} // namespace Antares::Solver
