/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
/*************************************************************************

   FONCTION: Recherche du meilleur minorant parmi les noeuds evalues
             et non elimines. 

   AUTEUR: R. GONZALEZ

**************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/

void BB_RechercherLeMeilleurMinorant( BB * Bb, char TypeDeRecherche )
{
NOEUD ** NoeudsAExplorer; NOEUD ** NoeudsAExplorerAuProchainEtage;  NOEUD *  Noeud; NOEUD *  NoeudCourant; 
int NombreDeNoeudsAExplorer; int NombreDeNoeudsAExplorerAuProchainEtage; int i; NOEUD * NoeudDuMeilleurMinorant;
char Continuer; double X; double Xmin; double ValeurDuMeilleurMinorant; double MinorantDuCritereAuNoeud;
double CoutDeLaMeilleureSolutionEntiere; 

NoeudsAExplorer = (NOEUD **) malloc( 1 * sizeof( void * ) );
if ( NoeudsAExplorer == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction:  BB_RechercherLeMeilleurMinorant \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}
NombreDeNoeudsAExplorer = 1;
NoeudsAExplorer[0] = Bb->NoeudRacine;

NoeudDuMeilleurMinorant = 0;
Bb->NoeudDuMeilleurMinorant = 0;
CoutDeLaMeilleureSolutionEntiere = Bb->CoutDeLaMeilleureSolutionEntiere;

ValeurDuMeilleurMinorant = PLUS_LINFINI; /* Pour ne pas avoir de warning a la compilation */
if ( TypeDeRecherche == RECHERCHER_LE_PLUS_PETIT ) ValeurDuMeilleurMinorant = PLUS_LINFINI;
else if ( TypeDeRecherche == RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL ) Xmin = -PLUS_LINFINI;
else {
  printf("Bug BB_RechercherLeMeilleurMinorant argument TypeDeRecherche mal iniutialise\n");
	return;
}

while( 1 ) {
  NoeudsAExplorerAuProchainEtage = (NOEUD **) malloc( NombreDeNoeudsAExplorer * 2 * sizeof( void * ) );
  if ( NoeudsAExplorerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_RechercherLeMeilleurMinorant \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorerAuProchainEtage = 0;	
  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudCourant = NoeudsAExplorer[i];  
    Continuer = OUI;
    if ( NoeudCourant != 0 ) {
      if ( NoeudCourant->StatutDuNoeud == EVALUE && NoeudCourant->NoeudTerminal != OUI ) {
				MinorantDuCritereAuNoeud = NoeudCourant->MinorantDuCritereAuNoeud;
        /* Doit avoir au moins 1 fils a evaluer */	
        Noeud = NoeudCourant->NoeudSuivantGauche;
        if ( Noeud != 0 ) {
	        if ( Noeud->StatutDuNoeud == A_EVALUER ) {
	          Continuer = NON;
	          if ( TypeDeRecherche == RECHERCHER_LE_PLUS_PETIT ) {
              if ( MinorantDuCritereAuNoeud < ValeurDuMeilleurMinorant && MinorantDuCritereAuNoeud <= CoutDeLaMeilleureSolutionEntiere ) {
	              ValeurDuMeilleurMinorant = MinorantDuCritereAuNoeud;
	              NoeudDuMeilleurMinorant = NoeudCourant;
	              goto Next;
	            }
	          }	    
	          else {
						  /* TypeDeRecherche est egaa a RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL */
              X = CoutDeLaMeilleureSolutionEntiere - MinorantDuCritereAuNoeud;
	            X/= NoeudCourant->NbValeursFractionnairesApresResolution;	      
              if ( X > Xmin && MinorantDuCritereAuNoeud <= CoutDeLaMeilleureSolutionEntiere ) {							
	              Xmin = X;
	              NoeudDuMeilleurMinorant = NoeudCourant;
	              goto Next;
	            }	    	    
	          }												
	        }
	      }	
        Noeud = NoeudCourant->NoeudSuivantDroit;
        if ( Noeud != 0 ) {				
	        if ( Noeud->StatutDuNoeud == A_EVALUER ) {
	          Continuer = NON;
	          if ( TypeDeRecherche == RECHERCHER_LE_PLUS_PETIT ) {	  
              if ( MinorantDuCritereAuNoeud < ValeurDuMeilleurMinorant && MinorantDuCritereAuNoeud <= CoutDeLaMeilleureSolutionEntiere ) {
	              ValeurDuMeilleurMinorant = MinorantDuCritereAuNoeud;
	              NoeudDuMeilleurMinorant = NoeudCourant;
	              goto Next;
	            }
	          }	    
	          else {
						  /* TypeDeRecherche est egaa a RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL */
              X = CoutDeLaMeilleureSolutionEntiere - MinorantDuCritereAuNoeud;
	            X/= NoeudCourant->NbValeursFractionnairesApresResolution;	      
              if ( X > Xmin && MinorantDuCritereAuNoeud <= CoutDeLaMeilleureSolutionEntiere ) {
	              Xmin = X;
	              NoeudDuMeilleurMinorant = NoeudCourant;
	              goto Next;
	            }
	          }	    	    	          						
	        }
	      }
      }     
      /* Renseigner la table des noeuds du prochain etage. Pour descendre d'un cran il faut que les
         2 fils aient ete evalues */
      Next:
      if ( Continuer == OUI ) {
        Noeud = NoeudCourant->NoeudSuivantGauche;				
        if ( Noeud != 0 ) {
          NombreDeNoeudsAExplorerAuProchainEtage++;
          NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;
	      }
        Noeud = NoeudCourant->NoeudSuivantDroit;
        if ( Noeud != 0 ) {	    
          NombreDeNoeudsAExplorerAuProchainEtage++;
          NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;          
        }
      }
    }
  }
	
                        /* Preparations pour l'etage suivant */
  if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) break; /* Exploration de l'arbre terminee */
  free( NoeudsAExplorer );
  NoeudsAExplorer = (NOEUD **) malloc( NombreDeNoeudsAExplorerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsAExplorer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_RechercherLeMeilleurMinorant\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorer = NombreDeNoeudsAExplorerAuProchainEtage; 
  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudsAExplorer[i] = NoeudsAExplorerAuProchainEtage[i];
  }
  free( NoeudsAExplorerAuProchainEtage );
/* fin while */
}	    

free( NoeudsAExplorer );
free( NoeudsAExplorerAuProchainEtage );

Bb->NoeudDuMeilleurMinorant = NoeudDuMeilleurMinorant;
if ( Bb->NoeudDuMeilleurMinorant != 0 ) {
  if ( TypeDeRecherche == RECHERCHER_LE_PLUS_PETIT ) Bb->ValeurDuMeilleurMinorant = ValeurDuMeilleurMinorant;
}

return;
}







