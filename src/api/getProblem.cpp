#include "antares/api/getProblem.h"

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/solver/optimisation/LinearProblemMatrix.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/study/study.h"

namespace
{
constexpr int optimizationNumber = 1;  // the 1st optim is available for now
constexpr int numeroDeLIntervalle = 0; // simplex-range = week
const std::string kName = "my-name";
} // namespace

namespace Antares::Solver
{
void SingleProblemGetter::load(const std::filesystem::path& study_path)
{
    FileTreeStudyLoader study_loader(study_path);
    auto study = study_loader.load();
    SIM_AllocationProblemeHebdo(*study, pb_, 168 /* TODO*/);

    OPT_AllocDuProblemeAOptimiser(&pb_);
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
    const int PremierPdtDeLIntervalle = 0;
    const int DernierPdtDeLIntervalle = 167;

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
