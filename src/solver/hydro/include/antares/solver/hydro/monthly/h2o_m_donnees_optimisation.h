// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include <antares/solver/hydro/probleme_spx_wrapper.h>

#include "spx_definition_arguments.h"

#define LINFINI 1.e+80

namespace DonneesOptimisationMensuelle
{
/*--------------------------------------------------------------------------------------*/
/* Matrice des contraintes: il y aura une seule instance pour tous les reservoirs */
/* Dans ce struct il n'y a que des donnees qui sont lues et surtout pas ecrites   */
/* Ce struct est instancie une seule fois                                         */
struct PROBLEME_LINEAIRE_PARTIE_FIXE
{
    int NombreDeVariables{0};
    std::vector<double> CoutLineaire;
    std::vector<double> CoutLineaireBruite; // Ajout de bruit pour forcer l'unicité des solutions
    std::vector<int> TypeDeVariable;

    // La matrice des contraintes
    int NombreDeContraintes{0};
    std::vector<char> Sens;
    std::vector<int> IndicesDebutDeLigne;
    std::vector<int> NombreDeTermesDesLignes;
    std::vector<double> CoefficientsDeLaMatriceDesContraintes;
    std::vector<int> IndicesColonnes;
    int NombreDeTermesAlloues{0};
};

/* Partie variable renseignee avant le lancement de l'optimisation de chaque reservoir */
struct PROBLEME_LINEAIRE_PARTIE_VARIABLE
{
    /* Donnees variables de la matrice des contraintes */
    /* On met quand-meme les bornes dans la partie variable pour le cas ou on voudrait avoir
             un jour des bornes min et max variables dans le temps et en fonction des reservoirs */
    std::vector<double> Xmin;
    std::vector<double> Xmax;
    std::vector<double> SecondMembre;
    /* Tableau de pointeur a des doubles. Ce tableau est parallele a X, il permet
       de renseigner directement les structures de description du reseau avec les
       resultats contenus dans X */
    std::vector<double*> AdresseOuPlacerLaValeurDesVariablesOptimisees;
    // Resultats
    std::vector<double> X;
    /* En Entree ou en Sortie */

    // Existence d'une solution vaut :
    //  OUI_SPX s'il y a une solution,
    //  NON_SPX s'il n'y a pas de solution admissible
    //  SPX_ERREUR_INTERNE si :
    //      + probleme a l'execution (saturation memoire par exemple),
    //        et dans ce cas il n'y a pas de solution SPX_MATRICE_DE_BASE_SINGULIERE
    //      + on n'a pas pu construire de matrice de base reguliere, et dans ce cas il n'y a
    //        pas de solution
    int ExistenceDUneSolution{0};

    // Vecteurs a passer au Simplexe pour recuperer la base optimale
    std::vector<int> PositionDeLaVariable;
    std::vector<int> ComplementDeLaBase;
    // Vecteur a passer au Simplexe pour recuperer les couts reduits
    std::vector<double> CoutsReduits;
    // Vecteur a passer au Simplexe pour recuperer les couts marginaux
    std::vector<double> CoutsMarginauxDesContraintes;
};

/* Les correspondances des variables */
struct CORRESPONDANCE_DES_VARIABLES
{
    std::vector<int> NumeroDeVariableVolume;
    std::vector<int> NumeroDeVariableTurbine;
    std::vector<int> NumeroDeVariableOverflow;
    std::vector<int> NumeroDeVariableDepassementVolumeMax;
    std::vector<int> NumeroDeVariableDepassementVolumeMin;
    int NumeroDeLaVariableViolMaxVolumeMin{0};
    std::vector<int> NumeroDeVariableDEcartPositifAuTurbineCible;
    std::vector<int> NumeroDeVariableDEcartNegatifAuTurbineCible;
    int NumeroDeLaVariableXi{0};
};

/* Structure uniquement exploitee par l'optimisation (donc a ne pas acceder depuis l'exterieur) */
struct PROBLEME_HYDRAULIQUE
{
    int NombreDeReservoirs{0};
    CORRESPONDANCE_DES_VARIABLES CorrespondanceDesVariables{};

    PROBLEME_LINEAIRE_PARTIE_FIXE ProblemeLineairePartieFixe{};
    PROBLEME_LINEAIRE_PARTIE_VARIABLE ProblemeLineairePartieVariable{};

    std::vector<PROBLEME_SPX_WRAPPER> ProblemeSpx; /* Il y en a 1 par reservoir */

    double CoutDeLaSolution{0.};
    double CoutDeLaSolutionBruite{0.};
};
} // namespace DonneesOptimisationMensuelle
