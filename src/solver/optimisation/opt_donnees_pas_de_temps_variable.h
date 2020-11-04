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
/* Construction de la matrice des contraintes */
/* Construction de la courbe de charge */
/* Optimisations */
/* Fixation des Pmax pour les paliers non manoeuvrants et des pmin */
/* Construction de la matrice des contraintes a pas fin (horaire) */
/* Optimisations */

/* Definition des classes de manoeuvrabilite */
typedef enum  {
  Classe01 =  1 , Classe02 =  2 , Classe03 =  3 , Classe04 =  4 , Classe05 =  5 , Classe06 =  6 ,
  Classe07 =  7 , Classe08 =  8 , Classe09 =  9 , Classe10 = 10 , Classe11 = 11 , Classe12 = 12 ,
  Classe13 = 13 , Classe14 = 14 , Classe15 = 15 , Classe16 = 16 , Classe17 = 17 , Classe18 = 18 ,
  Classe19 = 19 , Classe20 = 20 , Classe21 = 21 , Classe22 = 22 , Classe23 = 23 , Classe24 = 24
}
CLASSE_DE_MANOEUVRABILITE;

typedef struct {
  int     NombreDePaliersThermiques;
  /* Ajout */
  int * minUpDownTime;
  /* Fin ajout */
  double * TailleUnitaireDUnGroupeDuPalierThermique;
  double * PminDuPalierThermiquePendantUneHeure;
  double * PminDuPalierThermiquePendantUnJour;
  /* Ajout */
  int   * DureeDeLaPminDuPalierThermiquePendantUnJour;
  /*   */
  double * PminDuPalierThermiquePendantUneSemaine; 
  int *   NumeroDuPalierDansLEnsembleDesPaliersThermiques; 		      
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout; /* Indice par les paliers */
} PALIERS_THERMIQUES;


void OPT_SauvegarderLesDonneesHoraires()
{
  /* Les donnes a sauvegarder */
  NombreDePasDeTempsRef                   ; ValeursDeNTCRef                       ;
  ConsommationsAbattuesRef                ; NombreDePasDeTempsDUneJourneeRef      ;
  ReserveTournanteHoraireRef              ; ContrainteDePmaxHydrauliqueHoraireRef ;
  PuissanceDisponibleDuPalierThermiqueRef ;  
  return;
}

/* Pour LongueurDuPasDeTemps = 24 à 1 */


/*
NombreDePasDeTemps = NombreDePasDeTempsRef / LongueurDuPasDeTemps;
NombreDePasDeTempsDUneJournee = NombreDePasDeTempsDUneJourneeRef / LongueurDuPasDeTemps;

FAIT: Renseigner ValeursDeNTC a partir de ValeursDeNTCRef
FAIT: Renseigner ConsommationsAbattues a partir de ConsommationsAbattuesRef
FAIT: Renseigner ReserveTournanteHoraire a partir de ReserveTournanteHoraireRef
FAIT: Renseigner ContrainteDePmaxHydrauliqueHoraire a partir de ContrainteDePmaxHydrauliqueHoraireRef:
    -> les pmax hydrauliques : moyenne des Pmax horaire
FAIT: Renseigner ContrainteDEnergieHydrauliqueParJour a partir ContrainteDEnergieHydrauliqueParJourRef:
    contrainte = contrainte / LongueurDuPasDeTemps;
FAIT: Renseigner PuissanceDisponibleDuPalierThermique a partir PuissanceDisponibleDuPalierThermiqueRef

Construire la liste des variables optimisees : meme routine
Initialiser les bornes des variables optimisees : meme routine
Construire la matrice des contraintes : meme routine
Construire les couts : meme routine 
Le second membre : meme routine
On optimise sur la journee:

On calcule les Pmax de la classe en cours et on en deduit une Pmax horaire

*/
