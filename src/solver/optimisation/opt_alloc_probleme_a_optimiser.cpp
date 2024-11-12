/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <antares/logs/logs.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO*);

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

    // Names
    ProblemeAResoudre->NomDesVariables.resize(nbVariables);
    ProblemeAResoudre->NomDesContraintes.resize(nbConstraints);
    // Integer variables ? (MILP)
    ProblemeAResoudre->VariablesEntieres.resize(nbVariables);
}

static void optimisationAllocateProblem(PROBLEME_HEBDO* problemeHebdo, const int mxPaliers)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    int Sparsity = mxPaliers * problemeHebdo->NombreDePays;
    Sparsity += problemeHebdo->NombreDInterconnexions;
    if (Sparsity > 100)
    {
        Sparsity = 100;
    }

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

    OPT_AllocateFromNumberOfVariableConstraints(problemeHebdo->ProblemeAResoudre.get(), NbTermes);

    int NbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    ProblemeAResoudre->ProblemesSpx.assign(NbIntervalles, nullptr);

    logs.info();
    logs.info() << " Status of Preliminary Allocations for Generic Problem Resolution : Successful";
    logs.info();
}

void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    problemeHebdo->ProblemeAResoudre = std::make_unique<PROBLEME_ANTARES_A_RESOUDRE>();

    int mxPaliers = OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(problemeHebdo);

    optimisationAllocateProblem(problemeHebdo, mxPaliers);
}
