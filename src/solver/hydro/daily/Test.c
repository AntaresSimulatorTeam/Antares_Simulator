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




























# include "h2o_j_donnees_mensuelles.h"
# include "h2o_j_fonctions.h"




main( int argc , char ** argv )
{
int i; int NbPdt; int NombreDAnnees; int Nb; int Mois; int NbReservoirs; int k;
FILE * Flot;
DONNEES_MENSUELLES * DonneesMensuelles;

NombreDAnnees = 500;
NbReservoirs  = 100;

DonneesMensuelles = H2O_J_Instanciation( );

NbPdt = 28;
for ( Nb = 0 ; Nb < NombreDAnnees ; Nb++ ) {
  for ( Mois = 0 ; Mois < 12 ; Mois++ ) {
    if ( NbPdt == 28 ) NbPdt = 29;
	  else if ( NbPdt == 29 ) NbPdt = 30;
	  else if ( NbPdt == 30 ) NbPdt = 31;
	  else if ( NbPdt == 31 ) NbPdt = 28;
	  else {
	    printf("BUG\n");
	  	exit(0);
	  }
	
    DonneesMensuelles->NombreDeJoursDuMois = NbPdt;

		for ( k = 0 ; k < NbReservoirs ; k++ ) {
      DonneesMensuelles->TurbineDuMois = NbPdt * ( (double) rand() / RAND_MAX );
	
      for ( i = 0 ; i < NbPdt ; i++ ) {
        
        DonneesMensuelles->TurbineMax[i] = 1.1 * ( DonneesMensuelles->TurbineDuMois / NbPdt );
        
        DonneesMensuelles->TurbineCible[i] = DonneesMensuelles->TurbineDuMois / NbPdt;
      }

      H2O_J_OptimiserUnMois( DonneesMensuelles );
	
	    if ( DonneesMensuelles->ResultatsValides != OUI ) {
        printf("Annee %d Mois %d calcul invalide \n",Nb,Mois);
        Flot = fopen( "Donnees_Probleme_Solveur.mps", "w" ); 
        if( Flot == NULL ) {
          printf("Erreur ouverture du fichier pour l'ecriture du jeu de donnees \n");
          exit(0);
        }			
        H2O_J_EcrireJeuDeDonneesLineaireAuFormatMPS( DonneesMensuelles, Flot );
        H2O_J_Free( DonneesMensuelles );			
		    exit(0);
	    }	
		}
	}
	printf("Calcul termine annee %d \n",Nb);
}

H2O_J_Free( DonneesMensuelles );

exit(0);
}
