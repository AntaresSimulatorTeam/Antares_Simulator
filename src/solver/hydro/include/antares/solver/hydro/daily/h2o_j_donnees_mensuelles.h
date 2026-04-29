// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_H2O_J_DONNEES_ANNEE__
#define __SOLVER_H2O_J_DONNEES_ANNEE__

#define OUI 1
#define NON 0
#define EMERGENCY_SHUT_DOWN 2

#include "h2o_j_donnees_optimisation.h"

/*************************************************************************************************/
/*                    Structure contenant les champs a renseigner par l'appelant */
namespace DoneesOptimisationJournaliere
{
struct DONNEES_MENSUELLES
{
    /* En entree: seules les donnees ci-dessous doivent etre renseignees par l'appelant apres
       avoir appele H2O_J_Instanciation */
    int NombreDeJoursDuMois{0};     /* A renseigner par l'appelant */
    double TurbineDuMois{0};        /* A renseigner par l'appelant (c'est le turbine opt du mois) */
    std::vector<double> TurbineMax; /* A renseigner par l'appelant : 1 valeur par jour */
    std::vector<double> TurbineMin;
    std::vector<double> TurbineCible; /* A renseigner par l'appelant : 1 valeur par jour */
    /* Les resultats */
    char ResultatsValides{
      NON};                      /* Vaut:
                               OUI si la solution est exploitable pour le reservoir
                               NON s'il y a eu un probleme dans la resolution
                                                                                                                         EMERGENCY_SHUT_DOWN si la resolution du probleme a donne lieu a une erreur interne
                                                                                                          */
    std::vector<double> Turbine; /* Resultat a recuperer par l'appelant */

    /******************************************************************************************/
    /* Problemes internes (utilise uniquement par l'optimisation) */
    PROBLEME_HYDRAULIQUE ProblemeHydraulique{};
};
} // namespace DoneesOptimisationJournaliere

#endif
