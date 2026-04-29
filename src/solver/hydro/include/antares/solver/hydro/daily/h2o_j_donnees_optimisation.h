// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_J_STRUCTURE_INTERNE__
#define __SOLVER_H2O_J_STRUCTURE_INTERNE__

#include <string>
#include <vector>

#include <antares/solver/hydro/probleme_spx_wrapper.h>

#include "spx_definition_arguments.h"

#define LINFINI 1.e+80

namespace DoneesOptimisationJournaliere
{
/*--------------------------------------------------------------------------------------*/
/* Matrice des contraintes: il y aura une seule instance pour tous les reservoirs */
/* Dans ce struct il n'y a que des donnees qui sont lues et surtout pas ecrites   */
/* Ce struct est instancie une seule fois                                         */
struct PROBLEME_LINEAIRE_PARTIE_FIXE
{
    int NombreDeVariables{0};
    std::vector<double> CoutLineaire;
    std::vector<int> TypeDeVariable; /* Indicateur du type de variable, il ne doit prendre que les
                       suivantes (voir le fichier spx_constantes_externes.h mais ne jamais
                       utiliser les valeurs explicites des constantes): VARIABLE_FIXE ,
                        VARIABLE_BORNEE_DES_DEUX_COTES ,
                        VARIABLE_BORNEE_INFERIEUREMENT ,
                        VARIABLE_BORNEE_SUPERIEUREMENT ,
                        VARIABLE_NON_BORNEE
                                       */
    /* La matrice des contraintes */
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
    /* Resultat */
    std::vector<double> X;
    /* En Entree ou en Sortie */
    int ExistenceDUneSolution{NON_SPX}; /* En sortie, vaut :
                              OUI_SPX s'il y a une solution,
                                                      NON_SPX s'il n'y a pas de solution
                              admissible SPX_ERREUR_INTERNE si probleme a l'execution
                              (saturation memoire par exemple), et dans ce cas il n'y a pas de
                              solution SPX_MATRICE_DE_BASE_SINGULIERE si on n'a pas pu
                              construire de matrice de base reguliere, et dans ce cas il n'y a
                              pas de solution
                                             */

    std::vector<int>
      PositionDeLaVariable; /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    std::vector<int>
      ComplementDeLaBase; /* Vecteur a passer au Simplexe pour recuperer la base optimale */
    std::vector<double>
      CoutsReduits; /* Vecteur a passer au Simplexe pour recuperer les couts reduits */
    std::vector<double> CoutsMarginauxDesContraintes; /* Vecteur a passer au Simplexe pour recuperer
                                         les couts marginaux */
};

/* Les correspondances fixes des contraintes */
struct CORRESPONDANCE_DES_CONTRAINTES
{
    int NumeroDeContrainteDEnergieMensuelle{0};    /* Somme des turbines = somme des cibles */
    std::vector<int> NumeroDeContrainteSurXi;      /* turbine[t] + xi[t] >= cible[t] */
    std::vector<int> NumeroDeContrainteSurXiSym;   /* cible[t] + xi[t] >= turbine[t] */
    std::vector<int> NumeroDeContrainteSurXiPlus;  /* turbine[t] + xi_plus >= cible[t] */
    std::vector<int> NumeroDeContrainteSurXiMoins; /* cible[t] + xi_moins >= turbine[t] */
};

/* Les correspondances des variables */
struct CORRESPONDANCE_DES_VARIABLES
{
    std::vector<int> NumeroDeVariableTurbine; /* Turbines */
    // int NumeroDeLaVariableMu{0};              /* Variable de deversement (total sur la periode)
    // */
    std::vector<int> NumeroDeLaVariableXi; /* Variable decrivant l'ecart max au turbine cible quand
                             le turbine est inferieur au turbine cible */

    int NumeroDeLaVariableXiPlus{0};  /* Ecart global positif */
    int NumeroDeLaVariableXiMoins{0}; /* Ecart global négatif */
};

/* Structure uniquement exploitee par l'optimisation (donc a ne pas acceder depuis l'exterieur) */
struct PROBLEME_HYDRAULIQUE
{
    int NombreDeProblemes{0};
    std::vector<int> NbJoursDUnProbleme;

    std::vector<CORRESPONDANCE_DES_VARIABLES> CorrespondanceDesVariables;
    std::vector<CORRESPONDANCE_DES_CONTRAINTES> CorrespondanceDesContraintes;

    std::vector<PROBLEME_LINEAIRE_PARTIE_FIXE> ProblemeLineairePartieFixe;
    std::vector<PROBLEME_LINEAIRE_PARTIE_VARIABLE> ProblemeLineairePartieVariable;

    /* Il y en a 1 par reservoir. Un probleme couvre 1 mois */
    std::vector<PROBLEME_SPX_WRAPPER> ProblemeSpx;
};
} // namespace DoneesOptimisationJournaliere
#endif
