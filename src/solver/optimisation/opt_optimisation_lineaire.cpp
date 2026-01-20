// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <functional>
#include <mutex>

#include <antares/logs/logs.h>
#include "antares/io/outputs/ISimulationTable.h"
#include "antares/solver/optimisation/LinearProblemMatrix.h"
#include "antares/solver/optimisation/OptimisationsSimulationTable.h"
#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"
#include "antares/solver/optimisation/opt_export_structure.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/ISimulationObserver.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/utils/filename.h"

using namespace Antares::Solver;
using Antares::Solver::Optimization::ExportBehavior;
using Antares::Solver::Optimization::OptimizationOptions;

namespace
{
// TODO move
void callIfExport(ExportBehavior exportBehavior, const std::function<void(void)>& function)
{
    switch (exportBehavior)
    {
    case ExportBehavior::Never:
        break;
    case ExportBehavior::Once:
    {
        static std::once_flag once;
        std::call_once(once, function);
        break;
    }
    case ExportBehavior::Always:
        function();
        break;
    default:
        throw std::invalid_argument("Invalid ExportBehavior");
    }
}

double OPT_ObjectiveFunctionResult(const PROBLEME_HEBDO* Probleme,
                                   const int NumeroDeLIntervalle,
                                   const int optimizationNumber)
{
    if (optimizationNumber == PREMIERE_OPTIMISATION)
    {
        return Probleme->coutOptimalSolution1[NumeroDeLIntervalle];
    }
    else
    {
        return Probleme->coutOptimalSolution2[NumeroDeLIntervalle];
    }
}

void OPT_EcrireResultatFonctionObjectiveAuFormatTXT(
  double optimalSolutionCost,
  const OptPeriodStringGenerator& optPeriodStringGenerator,
  int optimizationNumber,
  Solver::IResultWriter& writer)
{
    Yuni::Clob buffer;
    auto filename = createCriterionFilename(optPeriodStringGenerator, optimizationNumber);

    logs.info() << "Solver Criterion File: `" << filename << "'";

    buffer.appendFormat("* Optimal criterion value :   %11.10e\n", optimalSolutionCost);
    std::string bufferStr = buffer.c_str();
    writer.addEntryFromBuffer(filename, bufferStr);
}

void OPT_WriteSolution(const PROBLEME_ANTARES_A_RESOUDRE& pb,
                       const OptPeriodStringGenerator& optPeriodStringGenerator,
                       int optimizationNumber,
                       Solver::IResultWriter& writer)
{
    auto s = [](int x) { return static_cast<size_t>(x); };

    Yuni::Clob buffer;
    auto filename = createSolutionFilename(optPeriodStringGenerator, optimizationNumber);
    for (int var = 0; var < pb.NombreDeVariables; var++)
    {
        buffer.appendFormat("%s\t%11.10e\n", pb.NomDesVariables[s(var)].c_str(), pb.X[s(var)]);
    }
    std::string bufferStr = buffer.c_str();
    writer.addEntryFromBuffer(filename, bufferStr);
    buffer.clear();

    filename = createMarginalCostFilename(optPeriodStringGenerator, optimizationNumber);
    for (int cont = 0; cont < pb.NombreDeContraintes; ++cont)
    {
        buffer.appendFormat("%s\t%11.10e\n",
                            pb.NomDesContraintes[s(cont)].c_str(),
                            pb.CoutsMarginauxDesContraintes[s(cont)]);
    }
    bufferStr = buffer.c_str();
    writer.addEntryFromBuffer(filename, bufferStr);
    buffer.clear();

    filename = createReducedCostFilename(optPeriodStringGenerator, optimizationNumber);
    for (int var = 0; var < pb.NombreDeVariables; ++var)
    {
        buffer.appendFormat("%s\t%11.10e\n",
                            pb.NomDesVariables[s(var)].c_str(),
                            pb.CoutsReduits[s(var)]);
    }
    std::string content = buffer.c_str();
    writer.addEntryFromBuffer(filename, content);
}

namespace
{
void notifyProblemHebdo(const PROBLEME_HEBDO* problemeHebdo,
                        int optimizationNumber,
                        Solver::Simulation::ISimulationObserver& simulationObserver,
                        const OptPeriodStringGenerator* optPeriodStringGenerator)
{
    simulationObserver.notifyHebdoProblem(*problemeHebdo,
                                          optimizationNumber,
                                          createMPSfilename(*optPeriodStringGenerator,
                                                            optimizationNumber));
}
} // namespace

bool runWeeklyOptimization(const SingleOptimOptions& options,
                           PROBLEME_HEBDO* problemeHebdo,
                           Solver::IResultWriter& writer,
                           int optimizationNumber,
                           Solver::Simulation::ISimulationObserver& simulationObserver,
                           Antares::IO::Outputs::ISimulationTable* simulationTable)
{
    const int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                        ->NombreDePasDeTempsPourUneOptimisation;

    int DernierPdtDeLIntervalle;
    for (uint pdtHebdo = 0, numeroDeLIntervalle = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps;
         pdtHebdo = DernierPdtDeLIntervalle, numeroDeLIntervalle++)
    {
        int PremierPdtDeLIntervalle = pdtHebdo;
        DernierPdtDeLIntervalle = pdtHebdo + NombreDePasDeTempsPourUneOptimisation;

        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(problemeHebdo,
                                                               PremierPdtDeLIntervalle,
                                                               DernierPdtDeLIntervalle,
                                                               optimizationNumber);

        OPT_InitialiserLeSecondMembreDuProblemeLineaire(problemeHebdo,
                                                        PremierPdtDeLIntervalle,
                                                        DernierPdtDeLIntervalle,
                                                        numeroDeLIntervalle,
                                                        optimizationNumber);

        OPT_InitialiserLesCoutsLineaire(problemeHebdo,
                                        PremierPdtDeLIntervalle,
                                        DernierPdtDeLIntervalle);

        // An optimization period represents a sequence as <year>-<week> or <year>-<week>-<day>,
        // depending whether the optimization is daily or weekly.
        // These sequences are used when building the names of MPS or criterion files.
        auto optPeriodStringGenerator = createOptPeriodAsString(
          problemeHebdo->OptimisationAuPasHebdomadaire,
          numeroDeLIntervalle,
          problemeHebdo->weekInTheYear,
          problemeHebdo->year);

        notifyProblemHebdo(problemeHebdo,
                           optimizationNumber,
                           simulationObserver,
                           optPeriodStringGenerator.get());

        if (!OPT_AppelDuSimplexe(options,
                                 problemeHebdo,
                                 numeroDeLIntervalle,
                                 optimizationNumber,
                                 *optPeriodStringGenerator,
                                 writer,
                                 simulationTable))
        {
            return false;
        }

        if (problemeHebdo->ExportMPS != Data::mpsExportStatus::NO_EXPORT
            || problemeHebdo->Expansion)
        {
            double optimalSolutionCost = OPT_ObjectiveFunctionResult(problemeHebdo,
                                                                     numeroDeLIntervalle,
                                                                     optimizationNumber);
            OPT_EcrireResultatFonctionObjectiveAuFormatTXT(optimalSolutionCost,
                                                           *optPeriodStringGenerator,
                                                           optimizationNumber,
                                                           writer);
        }
        if (problemeHebdo->exportSolutions)
        {
            OPT_WriteSolution(*problemeHebdo->ProblemeAResoudre,
                              *optPeriodStringGenerator,
                              optimizationNumber,
                              writer);
        }
    }
    return true;
}

void runThermalHeuristic(PROBLEME_HEBDO* problemeHebdo)
{
    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_AjusterLeNombreMinDeGroupesDemarresCoutsDeDemarrage(problemeHebdo);
    }
    else
    {
        OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT(problemeHebdo);
    }
}

void resizeProbleme(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                    unsigned nombreDeVariables,
                    unsigned nombreDeContraintes)
{
    ProblemeAResoudre->CoutQuadratique.resize(nombreDeVariables);
    ProblemeAResoudre->CoutLineaire.resize(nombreDeVariables);
    ProblemeAResoudre->TypeDeVariable.resize(nombreDeVariables);
    ProblemeAResoudre->Xmin.resize(nombreDeVariables);
    ProblemeAResoudre->Xmax.resize(nombreDeVariables);
    ProblemeAResoudre->X.resize(nombreDeVariables);
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees.resize(nombreDeVariables);
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits.resize(nombreDeVariables);
    ProblemeAResoudre->PositionDeLaVariable.resize(nombreDeVariables);
    ProblemeAResoudre->NomDesVariables.resize(nombreDeVariables);
    ProblemeAResoudre->VariablesEntieres.resize(nombreDeVariables);

    ProblemeAResoudre->Sens.resize(nombreDeContraintes);
    ProblemeAResoudre->IndicesDebutDeLigne.resize(nombreDeContraintes);
    ProblemeAResoudre->NombreDeTermesDesLignes.resize(nombreDeContraintes);
    ProblemeAResoudre->SecondMembre.resize(nombreDeContraintes);
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux.resize(nombreDeContraintes);
    ProblemeAResoudre->CoutsMarginauxDesContraintes.resize(nombreDeContraintes);
    ProblemeAResoudre->ComplementDeLaBase.resize(nombreDeContraintes);
    ProblemeAResoudre->NomDesContraintes.resize(nombreDeContraintes);
}
} // namespace

bool OPT_OptimisationLineaire(const OptimizationOptions& options,
                              PROBLEME_HEBDO* problemeHebdo,
                              Solver::IResultWriter& writer,
                              Solver::Simulation::ISimulationObserver& simulationObserver,
                              OptimisationsSimulationTable* simulationTables)
{
    if (!problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                                 ->NombreDePasDeTempsDUneJournee;
    }
    else
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;
    }

    OPT_NumeroDeJourDuPasDeTemps(problemeHebdo);

    OPT_NumeroDIntervalleOptimiseDuPasDeTemps(problemeHebdo);

    OPT_RestaurerLesDonnees(problemeHebdo);

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(problemeHebdo);

    auto builder_data = NewGetConstraintBuilderFromProblemHebdo(problemeHebdo);
    ConstraintBuilder builder(builder_data);
    LinearProblemMatrix linearProblemMatrix(problemeHebdo, builder);
    linearProblemMatrix.Run();
    resizeProbleme(problemeHebdo->ProblemeAResoudre.get(),
                   problemeHebdo->ProblemeAResoudre->NombreDeVariables,
                   problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    callIfExport(options.exportBehavior, [&] { OPT_ExportStructures(problemeHebdo, writer); });
    auto* firstOptimSimulationTable = simulationTables
                                        ? simulationTables->firstOptimSimulationTable()
                                        : nullptr;
    bool ret = runWeeklyOptimization(options.firstOptimOptions,
                                     problemeHebdo,
                                     writer,
                                     PREMIERE_OPTIMISATION,
                                     simulationObserver,
                                     firstOptimSimulationTable);

    // We only need the 2nd optimization when NOT solving with integer variables
    // We also skip the 2nd optimization in the hidden 'Expansion' mode
    // and if the 1st one failed.
    if (ret && !problemeHebdo->Expansion && !problemeHebdo->OptimisationAvecVariablesEntieres)
    {
        // We need to adjust some stuff before running the 2nd optimisation
        runThermalHeuristic(problemeHebdo);
        auto* secondOptimSimulationTable = simulationTables
                                             ? simulationTables->secondOptimSimulationTable()
                                             : nullptr;
        return runWeeklyOptimization(options.secondOptimOptions,
                                     problemeHebdo,
                                     writer,
                                     DEUXIEME_OPTIMISATION,
                                     simulationObserver,
                                     secondOptimSimulationTable);
    }
    return ret;
}
