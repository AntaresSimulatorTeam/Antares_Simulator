// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/logs/logs.h>
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

void OPT_AllocateFromNumberOfVariableConstraints(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    const size_t nbVariables = ProblemeAResoudre->NombreDeVariables;
    const size_t nbConstraints = ProblemeAResoudre->NombreDeContraintes;

    ProblemeAResoudre->Sens.resize(nbConstraints);
    ProblemeAResoudre->IndicesDebutDeLigne.assign(nbConstraints, 0);
    ProblemeAResoudre->NombreDeTermesDesLignes.assign(nbConstraints, 0);

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

static void optimisationAllocateProblem(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    OPT_AllocateFromNumberOfVariableConstraints(problemeHebdo->ProblemeAResoudre.get());

    int NbIntervalles = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;

    ProblemeAResoudre->ProblemesSpx.resize(NbIntervalles);
}

void OPT_AllocDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(problemeHebdo);

    optimisationAllocateProblem(problemeHebdo);
}
