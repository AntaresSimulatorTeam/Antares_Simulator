// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include <antares/solver/utils/basis_status.h>
#include "antares/solver/optimisation/LegacyVariableInfo.h"

#include "ortools/linear_solver/linear_solver.h"

/*--------------------------------------------------------------------------------------*/

/* Le probleme a resoudre */
class PROBLEME_ANTARES_A_RESOUDRE final
{
public:
    /* La matrice des contraintes */
    int NombreDeVariables;
    int NombreDeContraintes; /* Il est egal a :
                                 (  NombreDePays contraintes de bilan
                                  + NombreDePays contraintes de reserve
                                  + NombreDeContraintesCouplantes
                                 )* NombreDePasDeTempsDUneJournee
                                  + NombreDePays contraintes de turbine */
    std::string Sens;
    std::vector<int> IndicesDebutDeLigne;
    std::vector<int> NombreDeTermesDesLignes;
    std::vector<double> CoefficientsDeLaMatriceDesContraintes;
    std::vector<int> IndicesColonnes;
    int IncrementDAllocationMatriceDesContraintes;
    int NombreDeTermesDansLaMatriceDesContraintes;
    /* Donnees variables de la matrice des contraintes */
    std::vector<double> CoutQuadratique;
    std::vector<double> CoutLineaire;
    std::vector<int> TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les
                            suivantes (voir le fichier spx_constantes_externes.h mais ne jamais
                            utiliser les valeurs explicites des constantes): VARIABLE_FIXE ,
                              VARIABLE_BORNEE_DES_DEUX_COTES ,
                              VARIABLE_BORNEE_INFERIEUREMENT ,
                              VARIABLE_BORNEE_SUPERIEUREMENT ,
                              VARIABLE_NON_BORNEE
                                            */
    std::vector<double> Xmin;
    std::vector<double> Xmax;
    std::vector<double> SecondMembre;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a X, il permet
       de renseigner directement les structures de description du reseau avec les
       resultats contenus dans X */
    std::vector<double*> AdresseOuPlacerLaValeurDesVariablesOptimisees;
    /* Resultat */
    std::vector<double> X;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a CoutsMarginauxDesContraintes,
       il permet de renseigner directement les structures de description du reseau avec les
       resultats sur les couts marginaux */
    std::vector<double*> AdresseOuPlacerLaValeurDesCoutsMarginaux;
    std::vector<double> CoutsMarginauxDesContraintes;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a CoutsMarginauxDesContraintes,
       il permet de renseigner directement les structures de description du reseau avec les
       resultats sur les couts reduits */
    std::vector<double*> AdresseOuPlacerLaValeurDesCoutsReduits;
    std::vector<double> CoutsReduits;
    /* En Entree ou en Sortie */
    int ExistenceDUneSolution; /* En sortie, vaut :
                                   OUI_SPX s'il y a une solution,
                                   NON_SPX s'il n'y a pas de solution admissible
                                   SPX_ERREUR_INTERNE si probleme a l'execution (saturation memoire
                                  par exemple), et dans ce cas il n'y a pas de solution
                                   SPX_MATRICE_DE_BASE_SINGULIERE si on n'a pas pu construire de
                                  matrice de base reguliere, et dans ce cas il n'y a pas de solution
                                */

    std::vector<std::shared_ptr<operations_research::MPSolver>> ProblemesSpx;

    std::vector<int>
      PositionDeLaVariable; /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    std::vector<int>
      ComplementDeLaBase; /* Vecteur a passer au Simplexe pour recuperer la base optimale */

    /* Vecteurs de travail pour contruire la matrice des contraintes lineaires */
    std::vector<double> Pi;
    std::vector<int> Colonne;

    /* Nommage des variables & contraintes */
    std::vector<std::string> NomDesVariables;
    std::vector<std::string> NomDesContraintes;

    /* Structured legacy description of each variable, parallel to NomDesVariables.
       Filled by VariableNamer at the same time as the variable name. */
    std::vector<std::optional<Antares::Optimization::LegacyVariableInfo>> LegacyVariablesInfo;

    /* Structured legacy description of each constraint, parallel to NomDesContraintes.
       Filled by ConstraintNamer at the same time as the constraint name; only the
       constraints whose dual is needed for an extra output are recorded. */
    std::vector<std::optional<Antares::Optimization::LegacyVariableInfo>> LegacyConstraintsInfo;

    std::vector<bool> VariablesEntieres; // true = int, false = continuous

    // PIMPL is used to break dependency to OR-Tools' linear_solver.h (big header)
    Antares::Optimization::BasisStatus basisStatus;

    void clearBasis()
    {
        basisStatus.clear();
    }
};
