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

#ifndef __SOLVER_H2O2_J_DONNEES_ANNEE__
#define __SOLVER_H2O2_J_DONNEES_ANNEE__

#define OUI 1
#define NON 0
#define EMERGENCY_SHUT_DOWN 2

#include "antares/solver/hydro/daily2/h2o2_j_donnees_optimisation.h"

/*************************************************************************************************/
/*                    Structure contenant les champs a renseigner par l'appelant */
/*************************************************************************************************/

typedef struct
{
    /* En entree: seules les donnees ci-dessous doivent etre renseignees par l'appelant apres
       avoir appele H2O2_J_Instanciation */
    int NombreDeJoursDuMois; /* A renseigner par l'appelant */
    double TurbineDuMois;    /* A renseigner par l'appelant (somme des turbines cibles du mois) */
    std::vector<double> TurbineMax;   /* A renseigner par l'appelant : 1 valeur par jour */
    std::vector<double> TurbineMin;   /*Minimum Hourly Hydro-Storage Generation*/
    std::vector<double> TurbineCible; /* A renseigner par l'appelant : 1 valeur par jour */
    double reservoirCapacity;
    double NiveauInitialDuMois;
    std::vector<double> niveauBas;
    std::vector<double> apports;

    /* Les resultats */
    char ResultatsValides;       /* Vaut:
                                                               OUI si la solution est exploitable pour
                                    le reservoir NON s'il y a eu un probleme dans la resolution
                                                               EMERGENCY_SHUT_DOWN si la resolution du
                                    probleme a donne lieu a une erreur interne
                                                         */
    std::vector<double> Turbine; /* Resultat a recuperer par l'appelant */
    std::vector<double> niveauxFinJours;
    std::vector<double> overflows;
    std::vector<double> deviations;
    std::vector<double> violations;
    double deviationMax;
    double violationMax;
    double waste;

    double CoutSolution;

    /******************************************************************************************/

    /* Problemes internes (utilise uniquement par l'optimisation) */
    PROBLEME_HYDRAULIQUE_ETENDU ProblemeHydrauliqueEtendu;
} DONNEES_MENSUELLES_ETENDUES;

#endif
