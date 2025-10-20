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

Details::YearlyData SingleProblemGetter::getYearlyData(unsigned year)
{
    // TODO Use std::find for a single search
    if (allData_.contains(year)) // We already have data for this year
    {
        return allData_.at(year);
    }

    auto scratchmap = study_->areas.buildScratchMap(numSpace);

    // For each year
    Antares::Solver::NullResultWriter resultWriter;
    Antares::HydroManagement hydroManagement(study_->areas,
                                             study_->parameters,
                                             study_->calendar,
                                             resultWriter);

    int nbYears = 0;
    for (uint year = 0; year < study_->parameters.nbYears; ++year)
    {
        if (study_->parameters.yearsFilter[year])
        {
            ++nbYears;
        }
    }

    auto& dataForYear = allData_[year];
    auto& randomForParallelYears = dataForYear.randomForParallelYears;

    randomForParallelYears.emplace(nbYears, study_->parameters.power.fluctuations);
    randomForParallelYears->allocate(*study_);
    std::map<unsigned int, bool> isYearPerformed{{0, true}}; // TODO check year number

    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study_->parameters.seed[Data::seedHydroManagement]);
    randomForParallelYears->compute(*study_, 1, isYearPerformed, randomHydroGenerator);

    // Getting random tables for this year
    // Index of the current year in the list of structures
    uint indexYear = randomForParallelYears->yearNumberToIndex[year];
    auto& randomForCurrentYear = randomForParallelYears->pYears[indexYear];
    const auto& hydroReservoirLevel = randomForCurrentYear.pReservoirLevels;

    /*
      Side effects for HydroInputsChecker are limited to the year scope
      inside the study.
      more specifically, area.hydro.managementData[year]
      So "out-of-order" such as calls "y=0, y=4, y=0" should be fine
    */
    Antares::HydroInputsChecker hydroInputsChecker(*study_);
    hydroInputsChecker.Execute(year);
    hydroInputsChecker.CheckForErrors();

    prepareClustersInMustRunMode(*study_, scratchmap, year);

    hydroManagement.makeVentilation(hydroReservoirLevel, year, scratchmap);
    dataForYear.ventilationResults = hydroManagement.ventilationResults();

    // Compute hydro levels from ventilation results
    // WeeklyGeneratingModulation is IGNORED (for independence)
    int areaIndex = 0;
    const uint weekFirstDay = study_->calendar.hours[hourInTheYear].dayYear;
    for (const auto& [_, area]: study_->areas)
    {
        auto inflows = area->hydro.series->storage.getColumn(year);
        const auto& calendar = study_->calendar;

        auto getWeekInflows = [&](int week)
        {
            double ret = 0;
            for (uint day = calendar.weeks[week].daysYear.first;
                 day < calendar.weeks[week].daysYear.end;
                 ++day)
            {
                ret += inflows[day];
            }
            return ret;
        };

        auto& level = dataForYear.hydroLevels[area];
        level[0] = dataForYear.ventilationResults[areaIndex]
                     .NiveauxReservoirsDebutJours[weekFirstDay]
                   * area->hydro.reservoirCapacity;
        for (unsigned week = 1; week < calendar.maxWeeksInYear - 1; week++)
        {
            level[week] = level[week - 1];
            for (int day = 0; day < 7; ++day)
            {
                level[week] -= dataForYear.ventilationResults[areaIndex]
                                 .HydrauliqueModulableQuotidien[day]; // Subtract turbined
                level[week] += getWeekInflows(week);                  // Add inflows
            }
        }
        areaIndex++;
    }

    return dataForYear;
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    const auto [year, week] = id;

    pb_.year = id.year;
    pb_.weekInTheYear = week;

    auto [hydroLevels, randomForParallelYears, ventilationResults] = getYearlyData(year);

    auto scratchmap = study_->areas.buildScratchMap(numSpace);

    const auto hourInTheYear = 168 * week; // TODO
    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   week,
                                   hourInTheYear,
                                   ventilationResults,
                                   scratchmap);

    study_->computePThetaInfForThermalClusters(); // PthetaInf

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
