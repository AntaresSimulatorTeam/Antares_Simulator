// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#define OUI 1
#define NON 0
#define EMERGENCY_SHUT_DOWN 2

constexpr unsigned nbMonths = 12;

#include "h2o_m_donnees_optimisation.h"

/*************************************************************************************************/
/*                    Structure contenant les champs a renseigner par l'appelant */

struct DONNEES_ANNUELLES
{
    // ==========================================
    // En entree:
    //      seules les donnees ci-dessous doivent etre renseignees par l'appelant apres
    //      avoir appele " H2O_M_Instanciation "
    // ==========================================
    double CoutDepassementVolume{0.};
    double CoutViolMaxDuVolumeMin{0.};
    double overflowfCost{0.};

    double VolumeInitial{0.};

    // Commence a 0 pour le 1er janvier et se termine a 11 pour le 1er decembre
    std::vector<double> TurbineMax;
    std::vector<double> TurbineMin;
    std::vector<double> TurbineCible;
    std::vector<double> Apport;

    // Pour decrire la bande de volumes permise
    std::vector<double> VolumeMin; /* A renseigner par l'appelant : 1 valeur par mois */
    std::vector<double> VolumeMax; /* A renseigner par l'appelant : 1 valeur par mois */

    // =============================================
    // Les resultats (a recuperer par l'appelant)
    // =============================================
    //  Resultats Valides ?
    //      OUI si la solution est exploitable pour le reservoir
    //      NON s'il y a eu un probleme dans la resolution
    //      EMERGENCY_SHUT_DOWN si la resolution du probleme a donne lieu a une erreur interne
    char ResultatsValides{NON};

    std::vector<double> Turbine;
    std::vector<double> Volume;
    std::vector<double> overflow;

    // Problemes internes (utilise uniquement par l'optimisation)
    DonneesOptimisationMensuelle::PROBLEME_HYDRAULIQUE ProblemeHydraulique{};
};
