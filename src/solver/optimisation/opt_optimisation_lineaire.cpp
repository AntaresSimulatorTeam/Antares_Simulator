/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/LpsFromAntares.h"

#include <antares/logs/logs.h>
#include "antares/solver/utils/filename.h"
#include "antares/solver/simulation/OutputWriter.h"

using namespace Antares;
using namespace Yuni;
using Antares::Solver::Optimization::OptimizationOptions;

namespace
{

double OPT_ObjectiveFunctionResult(const PROBLEME_HEBDO* Probleme,
                                   const int NumeroDeLIntervalle,
                                   const int optimizationNumber)
{
    if (optimizationNumber == PREMIERE_OPTIMISATION)
        return Probleme->coutOptimalSolution1[NumeroDeLIntervalle];
    else
        return Probleme->coutOptimalSolution2[NumeroDeLIntervalle];
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
    writer.addEntryFromBuffer(filename, buffer);
}

template<class T, class U>
void copy(const T& in, U& out) {
    std::copy(in.begin(),
              in.end(),
              std::back_inserter(out));
}

bool runWeeklyOptimization(const OptimizationOptions& options,
                           PROBLEME_HEBDO* problemeHebdo,
                           const AdqPatchParams& adqPatchParams,
                           Solver::IResultWriter& writer,
                           int optimizationNumber,
                           Solver::OutputWriter& output)
{
    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    int DernierPdtDeLIntervalle;
    for (uint pdtHebdo = 0, numeroDeLIntervalle = 0; pdtHebdo < problemeHebdo->NombreDePasDeTemps;
         pdtHebdo = DernierPdtDeLIntervalle, numeroDeLIntervalle++)
    {
        int PremierPdtDeLIntervalle = pdtHebdo;
        DernierPdtDeLIntervalle = pdtHebdo + NombreDePasDeTempsPourUneOptimisation;

        OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(problemeHebdo,
                                                               adqPatchParams,
                                                               PremierPdtDeLIntervalle,
                                                               DernierPdtDeLIntervalle,
                                                               optimizationNumber);

        OPT_InitialiserLeSecondMembreDuProblemeLineaire(problemeHebdo,
                                                        PremierPdtDeLIntervalle,
                                                        DernierPdtDeLIntervalle,
                                                        numeroDeLIntervalle,
                                                        optimizationNumber);

        OPT_InitialiserLesCoutsLineaire(
          problemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);

        // An optimization period represents a sequence as <year>-<week> or <year>-<week>-<day>,
        // depending whether the optimization is daily or weekly.
        // These sequences are used when building the names of MPS or criterion files.
        auto optPeriodStringGenerator
          = createOptPeriodAsString(problemeHebdo->OptimisationAuPasHebdomadaire,
                                    numeroDeLIntervalle,
                                    problemeHebdo->weekInTheYear,
                                    problemeHebdo->year);

        /* rend accessible les problems à haut niveau */
        LpsFromAntares * lps = &output.lps;
        int const year = problemeHebdo->year + 1;
        int const week = problemeHebdo->weekInTheYear + 1;
        int const n = optimizationNumber;
        int nvars = problemeHebdo->ProblemeAResoudre->NombreDeVariables;
        int ncons = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        int neles = problemeHebdo->ProblemeAResoudre->IndicesDebutDeLigne[ncons - 1] + problemeHebdo->ProblemeAResoudre->NombreDeTermesDesLignes[ncons - 1];

        //LpFromAntaresPtr lp(new LpFromAntares);
        if (week == 1 && n == 1) {
            ConstantDataFromAntaresPtr year_ptr(new ConstantDataFromAntares);
            year_ptr->NombreDeVariables = nvars;
            year_ptr->NombreDeCoefficients = neles;
            year_ptr->NombreDeContraintes = ncons;

            copy(problemeHebdo->ProblemeAResoudre->TypeDeVariable, year_ptr->TypeDeVariable);

            copy(problemeHebdo->ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes, year_ptr->CoefficientsDeLaMatriceDesContraintes);
            copy(problemeHebdo->ProblemeAResoudre->IndicesColonnes, year_ptr->IndicesColonnes);

            copy(problemeHebdo->ProblemeAResoudre->IndicesDebutDeLigne, year_ptr->Mdeb);
            copy(problemeHebdo->ProblemeAResoudre->NomDesVariables, year_ptr->variables);
            copy(problemeHebdo->ProblemeAResoudre->NomDesContraintes, year_ptr->constraints);

            lps->_constant = year_ptr;
        }

        if (n == 1) //Export only the first optimisation
        {
            HebdoDataFromAntaresPtr week_ptr(new HebdoDataFromAntares);

            copy(problemeHebdo->ProblemeAResoudre->CoutLineaire, week_ptr->CoutLineaire);
            copy(problemeHebdo->ProblemeAResoudre->Xmax, week_ptr->Xmax);
            copy(problemeHebdo->ProblemeAResoudre->Xmin, week_ptr->Xmin);

            copy(problemeHebdo->ProblemeAResoudre->SecondMembre, week_ptr->SecondMembre);
            copy(problemeHebdo->ProblemeAResoudre->Sens, week_ptr->Sens);
            std::string problemName = createMPSfilename(*optPeriodStringGenerator, optimizationNumber);
            copy(problemName, week_ptr->name);

            lps->_hebdo[{static_cast<unsigned int>(year), static_cast<unsigned int>(week)}] = week_ptr;
        }

        /* Fin */
        if (!OPT_AppelDuSimplexe(options,
                                 problemeHebdo,
                                 numeroDeLIntervalle,
                                 optimizationNumber,
                                 *optPeriodStringGenerator,
                                 writer))
            return false;

        if (problemeHebdo->ExportMPS != Data::mpsExportStatus::NO_EXPORT
            || problemeHebdo->isExpansion)
        {
            double optimalSolutionCost
              = OPT_ObjectiveFunctionResult(problemeHebdo, numeroDeLIntervalle, optimizationNumber);
            OPT_EcrireResultatFonctionObjectiveAuFormatTXT(
              optimalSolutionCost, *optPeriodStringGenerator, optimizationNumber, writer);
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
} // namespace


bool OPT_OptimisationLineaire(const OptimizationOptions& options,
                              PROBLEME_HEBDO* problemeHebdo,
                              const AdqPatchParams& adqPatchParams,
                              Solver::IResultWriter& writer,
                              Solver::OutputWriter& outputWriter
                              )
{
    if (!problemeHebdo->OptimisationAuPasHebdomadaire)
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation
          = problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
    {
        problemeHebdo->NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTemps;
    }

    OPT_NumeroDeJourDuPasDeTemps(problemeHebdo);

    OPT_NumeroDIntervalleOptimiseDuPasDeTemps(problemeHebdo);

    OPT_RestaurerLesDonnees(problemeHebdo);

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(problemeHebdo);

    OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(problemeHebdo, writer);

    bool ret = runWeeklyOptimization(
      options, problemeHebdo, adqPatchParams, writer, PREMIERE_OPTIMISATION, outputWriter);

    // We only need the 2nd optimization when NOT solving with integer variables
    // We also skip the 2nd optimization in the hidden 'Expansion' mode
    // and if the 1st one failed.
    if (ret && !problemeHebdo->isExpansion && !problemeHebdo->OptimisationAvecVariablesEntieres)
    {
        // We need to adjust some stuff before running the 2nd optimisation
        runThermalHeuristic(problemeHebdo);
        return runWeeklyOptimization(
          options, problemeHebdo, adqPatchParams, writer, DEUXIEME_OPTIMISATION, outputWriter);
    }
    return ret;
}
