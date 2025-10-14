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
const std::string kName = "my-name";
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

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    const int week = id.week;
    const int year = id.year;
    const int PremierPdtDeLIntervalle = 168 * week;
    const int DernierPdtDeLIntervalle = 168 * (week + 1) - 1;

    pb_.year = id.year;
    pb_.weekInTheYear = week;

    const auto scratchmap = study_->areas.buildScratchMap(numSpace);

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

    allocateMemoryForRandomNumbers(randomForParallelYears);
    computeRandomNumbers(randomForParallelYears, endYear, isYearPerformed, randomHydroGenerator);

    hydroManagement.makeVentilation(randomReservoirLevel, year, scratchmap);
    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   week,
                                   PremierPdtDeLIntervalle /* TODO check */,
                                   hydroManagement.ventilationResults(),
                                   scratchmap);

    // required at least for OPT_SommeDesPminThermiques (RHS)
    Antares::Solver::Simulation::BuildThermalPartOfWeeklyProblem(
      *study_,
      pb_,
      PremierPdtDeLIntervalle /* TODO check */,
      {} /* const std::vector<std::vector<double>>& thermalNoises*/,
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
