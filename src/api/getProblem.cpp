#include "antares/api/getProblem.h"

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/solver/hydro/management/management.h"
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
constexpr int DernierPdtDeLIntervalle = 167; // 1 week = 7*24 hours
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

    OPT_VerifierPresenceReserveJmoins1(&pb_);

    OPT_InitialiserLesPminHebdo(&pb_);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(&pb_);

    OPT_MaxDesPmaxHydrauliques(&pb_);

    if (pb_.OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(&pb_);
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

// TODO (economy)
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

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    const auto [year, week] = id;

    pb_.year = id.year;
    pb_.weekInTheYear = week;

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

    Antares::Solver::Simulation::randomNumbers randomForParallelYears(
      nbYears,
      study_->parameters.power.fluctuations);

    randomForParallelYears.allocate(*study_);
    std::map<unsigned int, bool> isYearPerformed{{0, true}};

    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study_->parameters.seed[Data::seedHydroManagement]);

    randomForParallelYears.compute(*study_, 1, isYearPerformed, randomHydroGenerator);

    // Getting random tables for this year
    // Index of the current year in the list of structures
    uint indexYear = randomForParallelYears.yearNumberToIndex[year];
    auto& randomForCurrentYear = randomForParallelYears.pYears[indexYear];
    const auto& hydroReservoirLevel = randomForCurrentYear.pReservoirLevels;

    prepareClustersInMustRunMode(*study_, scratchmap, year);

    hydroManagement.makeVentilation(hydroReservoirLevel, year, scratchmap);
    const auto hourInTheYear = 168 * week;
    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   week,
                                   hourInTheYear,
                                   hydroManagement.ventilationResults(),
                                   scratchmap);

    // required at least for OPT_SommeDesPminThermiques (RHS)
    Antares::Solver::Simulation::BuildThermalPartOfWeeklyProblem(
      *study_,
      pb_,
      hourInTheYear,
      randomForCurrentYear.pThermalNoisesByArea,
      year);

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
