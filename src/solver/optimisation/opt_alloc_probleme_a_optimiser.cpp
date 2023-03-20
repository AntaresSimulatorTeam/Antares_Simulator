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

#include <limits>

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"
#include <stdio.h>

#include <antares/logs.h>
#include <antares/emergency.h>

using namespace Antares;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

void OPT_AllocateFromNumberOfVariableConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                                 int NbTermes)
{
    const size_t szNbVarsDouble = ProblemeAResoudre->NombreDeVariables * sizeof(double);
    const size_t szNbVarsint = ProblemeAResoudre->NombreDeVariables * sizeof(int);
    const size_t szNbContint = ProblemeAResoudre->NombreDeContraintes * sizeof(int);

    ProblemeAResoudre->Sens
      = (char*)MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof(char));
    ProblemeAResoudre->IndicesDebutDeLigne = (int*)MemAlloc(szNbContint);
    ProblemeAResoudre->NombreDeTermesDesLignes = (int*)MemAlloc(szNbContint);

    ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes
      = (double*)MemAlloc(NbTermes * sizeof(double));
    ProblemeAResoudre->IndicesColonnes = (int*)MemAlloc(NbTermes * sizeof(int));

    ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
    ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes = (int)(0.1 * NbTermes);

    ProblemeAResoudre->CoutQuadratique = (double*)MemAlloc(szNbVarsDouble);
    ProblemeAResoudre->CoutLineaire = (double*)MemAlloc(szNbVarsDouble);
    ProblemeAResoudre->TypeDeVariable = (int*)MemAlloc(szNbVarsint);
    ProblemeAResoudre->Xmin = (double*)MemAlloc(szNbVarsDouble);
    ProblemeAResoudre->Xmax = (double*)MemAlloc(szNbVarsDouble);
    ProblemeAResoudre->X = (double*)MemAlloc(szNbVarsDouble);

    ProblemeAResoudre->SecondMembre
      = (double*)MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof(double));

    ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees
      = (double**)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(void*));
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits
      = (double**)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(void*));
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux
      = (double**)MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof(void*));

    ProblemeAResoudre->CoutsMarginauxDesContraintes
      = (double*)MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof(double));
    ProblemeAResoudre->CoutsReduits = (double*)MemAlloc(szNbVarsDouble);

    ProblemeAResoudre->PositionDeLaVariable
      = (int*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int));
    ProblemeAResoudre->ComplementDeLaBase
      = (int*)MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof(int));

    ProblemeAResoudre->Pi
      = (double*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(double));
    ProblemeAResoudre->Colonne = (int*)MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int));

    ProblemeAResoudre->NomDesVariables.resize(ProblemeAResoudre->NombreDeVariables);
    ProblemeAResoudre->NomDesContraintes.resize(ProblemeAResoudre->NombreDeContraintes);
}

void OPT_FreeOptimizationData(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    MemFree(ProblemeAResoudre->Sens);
    MemFree(ProblemeAResoudre->IndicesDebutDeLigne);
    MemFree(ProblemeAResoudre->NombreDeTermesDesLignes);
    MemFree(ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes);
    MemFree(ProblemeAResoudre->IndicesColonnes);
    MemFree(ProblemeAResoudre->CoutQuadratique);
    MemFree(ProblemeAResoudre->CoutLineaire);
    MemFree(ProblemeAResoudre->TypeDeVariable);
    MemFree(ProblemeAResoudre->Xmin);
    MemFree(ProblemeAResoudre->Xmax);
    MemFree(ProblemeAResoudre->X);
    MemFree(ProblemeAResoudre->SecondMembre);
    MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees);
    MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits);
    MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux);
    MemFree(ProblemeAResoudre->CoutsMarginauxDesContraintes);
    MemFree(ProblemeAResoudre->CoutsReduits);

    MemFree(ProblemeAResoudre->PositionDeLaVariable);
    MemFree(ProblemeAResoudre->ComplementDeLaBase);
    MemFree(ProblemeAResoudre->Pi);
    MemFree(ProblemeAResoudre->Colonne);

    ProblemeAResoudre->NomDesVariables.clear();
    ProblemeAResoudre->NomDesContraintes.clear();
}

static void optimisationAllocateProblem(PROBLEME_HEBDO* problemeHebdo, const int mxPaliers)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    int Sparsity = mxPaliers * problemeHebdo->NombreDePays;
    Sparsity += problemeHebdo->NombreDInterconnexions;
    if (Sparsity > 100)
        Sparsity = 100;

    int NbTermes = 0;
    NbTermes += ProblemeAResoudre->NombreDeContraintes;

    int Adder = mxPaliers;
    Adder += 4;
    Adder *= problemeHebdo->NombreDePays;
    Adder += 2 * problemeHebdo->NombreDInterconnexions;
    Adder *= NombreDePasDeTempsPourUneOptimisation;

    NbTermes += Adder;

    NbTermes += Adder;

    Adder = 3 * problemeHebdo->NombreDInterconnexions * NombreDePasDeTempsPourUneOptimisation;
    NbTermes += Adder;

    Adder = Sparsity * problemeHebdo->NombreDeContraintesCouplantes;
    Adder *= (NombreDePasDeTempsPourUneOptimisation);
    Adder += Sparsity * (7 + 7) * problemeHebdo->NombreDeContraintesCouplantes;

    NbTermes += Adder;

    NbTermes += 3 * problemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation;
    NbTermes += problemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation * 4;
    NbTermes += problemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation * 5;

    NbTermes += problemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation
                * 2; /*inequality constraint on final hydros level*/
    NbTermes += 1;   /* constraint includes hydro generation, pumping and final level */
    NbTermes += 101; /* constraint expressing final level as a sum of stock layers */

    NbTermes += problemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage;

    logs.info();
    logs.info()
      << " Starting Memory Allocation for a Weekly Optimization problem in Canonical form ";
    logs.info() << " ( Problem Size :" << ProblemeAResoudre->NombreDeVariables << " variables "
                << ProblemeAResoudre->NombreDeContraintes << " Constraints) ";
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : " << NbTermes;
    logs.info();

    if ((uint)NbTermes > (std::numeric_limits<std::size_t>::max() / 8) - 1)
    {
        logs.fatal() << "Optimisation problem too large to be allocated.";
        AntaresSolverEmergencyShutdown();
    }

    OPT_AllocateFromNumberOfVariableConstraints(problemeHebdo->ProblemeAResoudre, NbTermes);

    int NbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    ProblemeAResoudre->ProblemesSpx = (PROBLEMES_SIMPLEXE*)MemAlloc(sizeof(PROBLEMES_SIMPLEXE));
    ProblemeAResoudre->ProblemesSpx->ProblemeSpx = (void**)MemAlloc(NbIntervalles * sizeof(void*));
    for (int NumIntervalle = 0; NumIntervalle < NbIntervalles; NumIntervalle++)
        ProblemeAResoudre->ProblemesSpx->ProblemeSpx[NumIntervalle] = NULL;

    logs.info();
    logs.info() << " Status of Preliminary Allocations for Generic Problem Resolution : Successful";
    logs.info();
}

void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    auto ProblemeAResoudre
      = (PROBLEME_ANTARES_A_RESOUDRE*)MemAllocMemset(sizeof(PROBLEME_ANTARES_A_RESOUDRE));
    problemeHebdo->ProblemeAResoudre = ProblemeAResoudre;

    int mxPaliers = OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(problemeHebdo);

    optimisationAllocateProblem(problemeHebdo, mxPaliers);
}

void OPT_AugmenterLaTailleDeLaMatriceDesContraintes(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    int NbTermes = ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes;
    NbTermes += ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes;

    logs.info();
    logs.info() << " Expected Number of Non-zero terms in Problem Matrix : increased to : "
                << NbTermes;
    logs.info();

    if (NbTermes > (std::numeric_limits<std::size_t>::max() / 8) - 1)
    {
        logs.fatal() << "Optimisation problem too large to be allocated.";
        AntaresSolverEmergencyShutdown();
    }

    ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes = (double*)MemRealloc(
      ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes, NbTermes * sizeof(double));

    ProblemeAResoudre->IndicesColonnes
      = (int*)MemRealloc(ProblemeAResoudre->IndicesColonnes, NbTermes * sizeof(int));

    ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}

void OPT_LiberationMemoireDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    if (ProblemeAResoudre)
    {
        OPT_FreeOptimizationData(ProblemeAResoudre);
        if (ProblemeAResoudre->ProblemesSpx)
        {
            MemFree(ProblemeAResoudre->ProblemesSpx->ProblemeSpx);
            MemFree(ProblemeAResoudre->ProblemesSpx);
        }
        MemFree(ProblemeAResoudre);
        ProblemeAResoudre = nullptr;
    }
}
