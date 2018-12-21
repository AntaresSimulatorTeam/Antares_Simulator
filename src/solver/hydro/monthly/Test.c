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




























# include "h2o_m_donnees_annuelles.h"
# include "h2o_m_fonctions.h"




main( int argc , char ** argv )
{
int i; int NbPdt; int NombreDAnnees; int Nb; int NbReservoirs; int k; double X;
FILE * Flot;
DONNEES_ANNUELLES * DonneesAnnuelles;

NombreDAnnees = 500;
NbReservoirs  = 100;

DonneesAnnuelles = H2O_M_Instanciation( NbReservoirs );

NbPdt = 12;
for ( Nb = 0 ; Nb < NombreDAnnees ; Nb++ ) {
  for ( k = 0 ; k < NbReservoirs ; k++ ) {	
    for ( i = 0 ; i < NbPdt ; i++ ) {
      DonneesAnnuelles->TurbineMax[i] = 1.;
      DonneesAnnuelles->TurbineCible[i] = 0.5;
      DonneesAnnuelles->Apport[i] = (double) rand()/ (double) (RAND_MAX);
      DonneesAnnuelles->VolumeMin[i] = 0.;
      DonneesAnnuelles->VolumeMax[i] = 1.;
    }
    DonneesAnnuelles->VolumeInitial = 0.0;
    DonneesAnnuelles->CoutDepassementVolume = 10.0;
		
    H2O_M_OptimiserUneAnnee( DonneesAnnuelles, k );

		
		
		X = DonneesAnnuelles->Volume[11] - DonneesAnnuelles->Turbine[11] + DonneesAnnuelles->Apport[11];
		if ( fabs(X - DonneesAnnuelles->Volume[0]) > 1.e-6 ) {
		  printf("Erreur reservoir %d\n",k);
			printf("X %e DonneesAnnuelles->Volume[0] %e  VolumeInitial %e\n",X,DonneesAnnuelles->Volume[0],DonneesAnnuelles->VolumeInitial);
		  exit(0);
	  }
		
	  if ( DonneesAnnuelles->ResultatsValides != OUI ) {
      printf("Annee %d Reservoir %d calcul invalide \n",Nb,k);
      Flot = fopen( "Donnees_Probleme_Solveur.mps", "w" ); 
      if( Flot == NULL ) {
        printf("Erreur ouverture du fichier pour l'ecriture du jeu de donnees \n");
        exit(0);
      }			
      H2O_M_EcrireJeuDeDonneesLineaireAuFormatMPS( DonneesAnnuelles, Flot );
      H2O_M_Free( DonneesAnnuelles );			
		  exit(0);	   	  
		}
	}
	printf("Calcul termine annee %d \n",Nb);
}

H2O_M_Free( DonneesAnnuelles );

exit(0);
}
