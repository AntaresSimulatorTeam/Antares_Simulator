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

#include "sim_structure_probleme_economique.h"
#include "opt_fonctions.h"
#include "LpsFromAntares.h"

#include <antares/logs/logs.h>
#include "../utils/filename.h"

using namespace Antares;
using namespace Yuni;
using Antares::Solver::Optimization::OptimizationOptions;

namespace
{

template<class T, class U>
void copy(int n, T * in, U & out) {
    out.resize(n);
    std::copy(in, in + n, out.begin());
}

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

bool runWeeklyOptimization(const OptimizationOptions& options,
                           PROBLEME_HEBDO* problemeHebdo,
                           const AdqPatchParams& adqPatchParams,
                           Solver::IResultWriter& writer,
                           int optimizationNumber)
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
        auto& study = *Antares::Data::Study::Current::Get();
        if (study._lps != NULL) {
            LpsFromAntares * lps = (LpsFromAntares*)study._lps;
            int const year = study.runtime->currentYear[numSpace] + 1;
            int const week = study.runtime->weekInTheYear[numSpace] + 1;
            int const n = ProblemeHebdo->numeroOptimisation[NumeroDeLIntervalle];
            int nvars = ProblemeHebdo->ProblemeAResoudre->NombreDeVariables;
            int ncons = ProblemeHebdo->ProblemeAResoudre->NombreDeContraintes;
            int neles = ProblemeHebdo->ProblemeAResoudre->IndicesDebutDeLigne[ncons - 1] + ProblemeHebdo->ProblemeAResoudre->NombreDeTermesDesLignes[ncons - 1];

            //LpFromAntaresPtr lp(new LpFromAntares);
            if (week == 1 && n == 1) {
                ConstantDataFromAntaresPtr year_ptr(new ConstantDataFromAntares);
                year_ptr->NombreDeVariables = nvars;
                year_ptr->NombreDeCoefficients = neles;
                year_ptr->NombreDeContraintes = ncons;

                copy(nvars, ProblemeHebdo->ProblemeAResoudre->TypeDeVariable, year_ptr->TypeDeVariable);

                copy(neles, ProblemeHebdo->ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes, year_ptr->CoefficientsDeLaMatriceDesContraintes);
                copy(neles, ProblemeHebdo->ProblemeAResoudre->IndicesColonnes, year_ptr->IndicesColonnes);

                copy(ncons, ProblemeHebdo->ProblemeAResoudre->IndicesDebutDeLigne, year_ptr->Mdeb);
                copy(ncons, ProblemeHebdo->ProblemeAResoudre->NombreDeTermesDesLignes, year_ptr->Nbterm);
                lps->_constant[year] = year_ptr;
            }
            {
                HebdoDataFromAntaresPtr week_ptr(new HebdoDataFromAntares);

                copy(nvars, ProblemeHebdo->ProblemeAResoudre->CoutLineaire, week_ptr->CoutLineaire);
                copy(nvars, ProblemeHebdo->ProblemeAResoudre->Xmax, week_ptr->Xmax);
                copy(nvars, ProblemeHebdo->ProblemeAResoudre->Xmin, week_ptr->Xmin);

                copy(ncons, ProblemeHebdo->ProblemeAResoudre->SecondMembre, week_ptr->SecondMembre);
                copy(ncons, ProblemeHebdo->ProblemeAResoudre->Sens, week_ptr->Sens);

                lps->_hedbo[{year, week, n}] = week_ptr;
            }
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
            || problemeHebdo->Expansion)
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
                              Solver::IResultWriter& writer)
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
      options, problemeHebdo, adqPatchParams, writer, PREMIERE_OPTIMISATION);

    // We only need the 2nd optimization when NOT solving with integer variables
    // We also skip the 2nd optimization in the hidden 'Expansion' mode
    // and if the 1st one failed.
    if (ret && !problemeHebdo->Expansion && !problemeHebdo->OptimisationAvecVariablesEntieres)
    {
        // We need to adjust some stuff before running the 2nd optimisation
        runThermalHeuristic(problemeHebdo);
        return runWeeklyOptimization(
          options, problemeHebdo, adqPatchParams, writer, DEUXIEME_OPTIMISATION);
    }
    return ret;
}
