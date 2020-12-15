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
#ifndef __SOLVER_SIMULATION_ADE_STRUCTS_H__
#define __SOLVER_SIMULATION_ADE_STRUCTS_H__

typedef struct
{
    double MargeHorsReseau;
    double MargeAvecReseau;
    double Offre;
    char DefaillanceHorsReseau;
    char DefaillanceAvecReseau;
} RESULTATS_HORAIRES_ADEQUATION;

typedef struct
{
    double PuissanceThermiqueCumulee;
    double Consommation;
    double Reserve;
} DONNEES_ADEQUATION;

typedef struct
{
    DONNEES_ADEQUATION** DonneesParPays;
    RESULTATS_HORAIRES_ADEQUATION** ResultatsParPays;
    char DefaillanceEuropeenneHorsReseau; /* vaut OUI_SIM ou NON_SIM */
    char DefaillanceEuropeenneAvecReseau; /* vaut OUI_SIM ou NON_SIM */
    long* IndicesDesVoisins;              /* indice par PointeurSurIndices */
    long* PointeurSurIndices; /* PointeurSurIndices[p] indique ou chercher le numero du premier
                                 voisin de p dans IndicesDesVoisins   */
    /* PointeurSurIndices[p+1]-1-PointeurSurIndices[p] donne donc le nombre de noeuds auxquels p est
     * relie */
    /* IndicesDesVoisins[PointeurSurIndices[p]] ... IndicesDesVoisins[PointeurSurIndices[p+1]-1]
     * sont donc */
    /* les pays auxquels p est relie */
    long* RangDansVoisinage; /* est aussi indice par PointeurSurIndice ; n'est en realite rien de
                                plus que :                        */
    /* for(q = PointeurSurIndices[p] ; q < PointeurSurIndices[p+1] ; q++) */
    /*   RangDansVoisinage[q] = q - PointeurSurIndices[p] ; */
    double* QuellesNTCPrendre;    /* Indice par NombreDElementsChainage : contient le numero de
                                     l'interconnexion correspondante */
    double* ValeursEffectivesNTC; /* vraies valeurs recuperees en utilisant QuellesNTCPrendre */
    long NombreDElementsChainage;
    /* Utilise par l'algorithme de flot */
    double* FluxDeSecours;
    double* NTCDeReference;
    long* A;
    long* B;
    long* X;
    double* S;

} PROBLEME_HORAIRE_ADEQUATION;

typedef struct
{
    double* Consommation;
    double* PuissanceThermiqueCumulee;
    double* Reserve;

} VALEURS_ANNUELLES;

typedef struct
{
    double EENSGWHorsReseau;
    double LOLEHorsReseau;
    double LOLPHorsReseau;
    double LOLPHorsReseauTemporaire;
    double* LOLPHoraireHorsReseau;
    double* EENSHoraireHorsReseau;
    long* LOLEAnnuelleHorsReseau;
    double* MargeMinAnnuelleHorsReseau;
    double EENSGWAvecReseau;
    double LOLEAvecReseau;
    double LOLPAvecReseau;
    double LOLPAvecReseauTemporaire;
    double* LOLPHoraireAvecReseau;
    double* EENSHoraireAvecReseau;
    long* LOLEAnnuelleAvecReseau;
    double* MargeMinAnnuelleAvecReseau;
} RESULTATS_PAR_PAYS_ADEQUATION;

typedef struct
{
    RESULTATS_PAR_PAYS_ADEQUATION** AdequationParPays;
    double* LOLPEuropeenneHoraireHorsReseau;
    double* LOLPEuropeenneHoraireAvecReseau;
    double* EENSEuropeenneHoraireHorsReseau;
    double* EENSEuropeenneHoraireAvecReseau;
    long* LOLEEuropeenneAnnuelleHorsReseau;
    long* LOLEEuropeenneAnnuelleAvecReseau;
    long NombreTotalHeuresTraitees;
    double LOLEEuropeenneHorsReseau;
    double LOLPEuropeenneHorsReseau;
    double LOLPEuropeenneHorsReseauTemporaire;
    double LOLEEuropeenneAvecReseau;
    double LOLPEuropeenneAvecReseau;
    double LOLPEuropeenneAvecReseauTemporaire;

} RESULTATS_ADEQUATION;

#endif /* __SOLVER_SIMULATION_ADE_STRUCTS_H__ */
