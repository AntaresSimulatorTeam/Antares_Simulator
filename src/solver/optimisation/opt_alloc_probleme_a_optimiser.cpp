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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_structure_probleme_economique.h"

#include "opt_fonctions.h"
#include <stdio.h>

#include <logs/logs.h>

using namespace Antares;

#ifdef _MSC_VER
#define SNPRINTF sprintf_s
#else
#define SNPRINTF snprintf
#endif

void OPT_AllocateFromNumberOfVariableConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                                                 int NbTermes)
{
    const size_t nbVariables = ProblemeAResoudre->NombreDeVariables;
    const size_t nbConstraints = ProblemeAResoudre->NombreDeContraintes;

    ProblemeAResoudre->Sens.resize(nbConstraints);
    ProblemeAResoudre->IndicesDebutDeLigne.assign(nbConstraints, 0);
    ProblemeAResoudre->NombreDeTermesDesLignes.assign(nbConstraints, 0);

    ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes.assign(NbTermes, 0.);
    ProblemeAResoudre->IndicesColonnes.assign(NbTermes, 0);

    ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
    ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes = (int)(0.1 * NbTermes);

    ProblemeAResoudre->CoutQuadratique.assign(nbVariables, 0.);
    ProblemeAResoudre->CoutLineaire.assign(nbVariables, 0.);
    ProblemeAResoudre->TypeDeVariable.assign(nbVariables, 0);
    ProblemeAResoudre->Xmin.assign(nbVariables, 0.);
    ProblemeAResoudre->Xmax.assign(nbVariables, 0.);
    ProblemeAResoudre->X.assign(nbVariables, 0.);

    ProblemeAResoudre->SecondMembre.assign(nbConstraints, 0.);

    ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees.assign(nbVariables, nullptr);
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits.assign(nbVariables, nullptr);
    ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux.assign(nbConstraints, nullptr);

    ProblemeAResoudre->CoutsMarginauxDesContraintes.assign(nbConstraints, 0.);
    ProblemeAResoudre->CoutsReduits.assign(nbVariables, 0.);

    ProblemeAResoudre->PositionDeLaVariable.assign(nbVariables, 0);
    ProblemeAResoudre->ComplementDeLaBase.assign(nbConstraints, 0);

    ProblemeAResoudre->Pi.assign(nbVariables, 0.);
    ProblemeAResoudre->Colonne.assign(nbVariables, 0);

    ProblemeAResoudre->NomDesVariables.resize(nbVariables);
    ProblemeAResoudre->NomDesContraintes.resize(nbConstraints);
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

    OPT_AllocateFromNumberOfVariableConstraints(problemeHebdo->ProblemeAResoudre, NbTermes);

    int NbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    ProblemeAResoudre->ProblemesSpx.assign(NbIntervalles, nullptr);

    logs.info();
    logs.info() << " Status of Preliminary Allocations for Generic Problem Resolution : Successful";
    logs.info();
}

void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    problemeHebdo->ProblemeAResoudre = new PROBLEME_ANTARES_A_RESOUDRE;

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

    ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes.resize(NbTermes);

    ProblemeAResoudre->IndicesColonnes.resize(NbTermes);

    ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
}

void OPT_LiberationMemoireDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    if (ProblemeAResoudre)
        delete ProblemeAResoudre;
}
