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

#ifndef __SOLVER_H2O_M_DONNEES_ANNEE__
#define __SOLVER_H2O_M_DONNEES_ANNEE__

#define OUI 1
#define NON 0
#define EMERGENCY_SHUT_DOWN 2

#include "h2o_m_donnees_optimisation.h"

/*************************************************************************************************/
/*                    Structure contenant les champs a renseigner par l'appelant */

typedef struct
{
    /* En entree: seules les donnees ci-dessous doivent etre renseignees par l'appelant apres
       avoir appele " H2O_M_Instanciation " */
    /* Commence a 0 pour le 1er janvier et se termine a 11 pour le 1er decembre */
    double CoutDepassementVolume;   /* A renseigner par l'appelant : 1 valeur */
    double CoutViolMaxDuVolumeMin;  // A renseigner par l'appelant : 1 valeur
    double VolumeInitial;           /* A renseigner par l'appelant : 1 valeur */
    std::vector<double> TurbineMax; /* A renseigner par l'appelant : 1 valeur par mois */
    std::vector<double> TurbineMin;
    std::vector<double> TurbineCible; /* A renseigner par l'appelant : 1 valeur par mois */
    std::vector<double> Apport;       /* A renseigner par l'appelant : 1 valeur par mois */
    /* Pour decrire la bande de volumes permise */
    std::vector<double> VolumeMin; /* A renseigner par l'appelant : 1 valeur par mois */
    std::vector<double> VolumeMax; /* A renseigner par l'appelant : 1 valeur par mois */

    /* Les resultats */
    char
      ResultatsValides;          /* Vaut:
                                    OUI si la solution est exploitable pour le reservoir
                                    NON s'il y a eu un probleme dans la resolution
                                                                                                                              EMERGENCY_SHUT_DOWN si la resolution du probleme a donne lieu a une erreur interne
                                                                                                               */
    std::vector<double> Turbine; /* Resultat a recuperer par l'appelant */
    std::vector<double> Volume;  /* Resultat a recuperer par l'appelant */

    /******************************************************************************************/
    /* Problemes internes (utilise uniquement par l'optimisation) */
    PROBLEME_HYDRAULIQUE ProblemeHydraulique;
    int NombreDePasDeTemps; /* 12 */
} DONNEES_ANNUELLES;

#endif
