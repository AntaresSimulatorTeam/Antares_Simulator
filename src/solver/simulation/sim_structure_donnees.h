/*
** Copyright 2007-2018 RTE
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
#ifndef __SOLVER_SIMULATION_DATA_STRUCTS_H__
#define __SOLVER_SIMULATION_DATA_STRUCTS_H__

typedef struct
{
    /* donnees communes adequation et economie */
    /*-----------------------------------------*/
    /* hydraulique si generation en ligne */
    double* PuissanceHydrauliqueFilMoyenne;
    double* PuissanceHydrauliqueFilVariance;
    double* CoefficientHydraulicite;
    double* ProbabiliteHydraulicite;
} DONNEES_PAR_PAYS;

typedef struct
{
    int* ThermiqueParPalier;
    int* RenouvelableParPalier;
    int Hydraulique;
    int Eolien;
    int Consommation;
    int Solar;
} NUMERO_CHRONIQUES_TIREES_PAR_PAYS;

typedef struct
{
    int TransmissionCapacities;
} NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION;

typedef struct
{
    double* HydrauliqueModulableQuotidien; /* indice par jour */
    double* AleaCoutDeProductionParPalier; /* epsilon sur le cout de production */
    double* NiveauxReservoirsDebutJours;   // Niveaux (quotidiens) du reservoir de début de jour (en
                                           // cas de gestion des reservoirs).
    double* NiveauxReservoirsFinJours; // Niveaux (quotidiens) du reservoir de fin de jour (en cas
                                       // de gestion des reservoirs).
} VALEURS_GENEREES_PAR_PAYS;

typedef struct
{
    double* Horaire;
} PRODUCTION_THERMIQUE;

typedef struct
{
    double* DefaillancePositive;                /* horaire */
    double* DefaillanceNegative;                /* horaire */
    double* DefaillanceEnReserve;               /* horaire */
    double* Turbinage;                          /* horaire */
    double* Solde;                              /* horaire */
    double* CoutMarginal;                       /* horaire */
    PRODUCTION_THERMIQUE** ProductionThermique; /* indice par paliers */
} PAYS_PAR_ANNEE_SIMULEE;

typedef struct
{
    double* ParPalier; /* indice par palier */
    double* Minimum;
    int* MinimumNo;
    double* Maximum;
    int* MaximumNo;
    double* StdDev;
    double* Annuel; /*Productions annuelles*/
} PRODUCTION_THERMIQUE_MOYENNE;

typedef struct
{
    double* TransitMoyen;
    double* TransitMinimum;
    int* TransitMinimumNo;
    double* TransitMaximum;
    int* TransitMaximumNo;
    double* TransitStdDev;
    double* TransitMoyenRecalculQuadratique;
    double* VariablesDualesMoyennes;
    double* TransitAnnuel;
    double* RenteHoraire;
} RESULTATS_PAR_INTERCONNEXION;

typedef struct
{
    double* ParLigne;
} MATRICE_2D;

/* Old define */
#define DEFINITION_STRUCTURES_DONNEES

#endif /* __SOLVER_SIMULATION_DATA_STRUCTS_H__ */
