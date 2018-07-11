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
#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/
/* 
Appele a chaque fois qu'on trouve une solution entiere ameliorante. 
On compare son cout aux minorants relaxes, on marque les noeuds sous optimaux, on supprime les branches
sous optimales.
*/

void BB_EliminerLesNoeudsSousOptimaux( BB * Bb )
{
NOEUD ** NoeudsAExplorer; NOEUD ** NoeudsAExplorerAuProchainEtage;  NOEUD * Noeud; NOEUD * NoeudCourant; 
int NombreDeNoeudsAExplorer; int NombreDeNoeudsAExplorerAuProchainEtage; int ProfondeurRelative;
int AnalyserEnDessousDuNoeud; int i; double Seuil; double Seuil_1; double Marge;
/*
printf("\n Eliminer le noeuds sous optimaux ");
*/
/* Le noeud de depart est le noeud racine, on balaye l'arborescence en largeur */

Seuil =  fabs( Bb->CoutDeLaMeilleureSolutionEntiere * TOLERANCE_OPTIMALITE );
if( Seuil > 0.001   ) Seuil = 0.001;
if( Seuil < 0.00001 ) Seuil = 0.00001;

Seuil = -1.e-10; /* Sinon on risque de conserver des branches avec des solutions equivalentes en pagaille */
Seuil = -1.e-9;
Seuil += Bb->CoutDeLaMeilleureSolutionEntiere;

ProfondeurRelative      = 0;			
NoeudsAExplorer         = (NOEUD **) malloc( 1 * sizeof( void * ) );
if ( NoeudsAExplorer == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_EliminerLesNoeudsSousOptimaux 1 \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}
NombreDeNoeudsAExplorer = 1;
NoeudsAExplorer[0]      = Bb->NoeudRacine;

Bb->NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux = NombreDeNoeudsAExplorer; /* Pour le nettoyage eventuel */
Bb->Liste1_PNE_EliminerLesNoeudsSousOptimaux    = NoeudsAExplorer;         /* Pour le nettoyage eventuel */

while( 1 ) {
  ProfondeurRelative++;
  NoeudsAExplorerAuProchainEtage = (NOEUD **) malloc( NombreDeNoeudsAExplorer * 2 * sizeof( void * ) );
  if ( NoeudsAExplorerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_EliminerLesNoeudsSousOptimaux 2 \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }

  Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux = 0;                              /* Pour le nettoyage eventuel */
  Bb->Liste2_PNE_EliminerLesNoeudsSousOptimaux    = NoeudsAExplorerAuProchainEtage; /* Pour le nettoyage eventuel */

  NombreDeNoeudsAExplorerAuProchainEtage = 0;

  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {

    NoeudCourant = NoeudsAExplorer[i]; 
    if ( NoeudCourant != 0 ) {
      AnalyserEnDessousDuNoeud = OUI;
      if( NoeudCourant->StatutDuNoeud == EVALUE ) {

	      Marge = Bb->ToleranceDOptimalite * 0.01 * fabs( NoeudCourant->MinorantDuCritereAuNoeud );
        Seuil_1 = Bb->CoutDeLaMeilleureSolutionEntiere - Marge;
				if ( Seuil_1 > Seuil ) Seuil_1 = Seuil;
			
        if( NoeudCourant->MinorantDuCritereAuNoeud >= Seuil_1 /*Seuil*/ ) {
          /* On marque le noeud comme etant A_REJETER, mais on le garde pour etre certain de ne 
             plus jamais lui creer de fils. Par contre on elimine physiquement tous ses fils */
	        /*
            printf("\n Suppression des descendants du noeud %x car son cout est %lf et le meilleur cout est %lf ",
	          NoeudCourant , NoeudCourant->MinorantDuCritereAuNoeud , Bb->CoutDeLaMeilleureSolutionEntiere);
	        */
          Bb->NombreDeNoeudsElagues++;
          Bb->SommeDesProfondeursDElaguage += NoeudCourant->ProfondeurDuNoeud;					
          AnalyserEnDessousDuNoeud = NON;
          BB_SupprimerTousLesDescendantsDUnNoeud( Bb, NoeudCourant );				
          NoeudCourant->StatutDuNoeud = A_REJETER; /* Le noeud courant n'aura plus de fils et est sous-optimal */
        }
      }
    } 
    if( AnalyserEnDessousDuNoeud == OUI ) {
                /* Renseigner la table des noeuds du prochain etage */
      if ( NoeudCourant != 0 ) {
        Noeud = NoeudCourant->NoeudSuivantGauche;
        if ( Noeud != 0 ) {
          if( Noeud->StatutDuNoeud == EVALUE ) {
            Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux++; /* Pour le nettoyage eventuel */
            NombreDeNoeudsAExplorerAuProchainEtage++;
            NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;
          }
        }
        Noeud = NoeudCourant->NoeudSuivantDroit;
        if ( Noeud != 0 ) {
          if( Noeud->StatutDuNoeud == EVALUE ) {
            Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux++; /* Pour le nettoyage eventuel */
            NombreDeNoeudsAExplorerAuProchainEtage++;
            NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;  
          }
        }
      }
    }
  /* fin for */
  } 
                        /* Preparations pour l'etage suivant */
  if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) break; /* Exploration de l'arbre terminee */
  free( NoeudsAExplorer );
  NoeudsAExplorer = (NOEUD **) malloc( NombreDeNoeudsAExplorerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsAExplorer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_EliminerLesNoeudsSousOptimaux 3 \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorer = NombreDeNoeudsAExplorerAuProchainEtage; 

  Bb->NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux = NombreDeNoeudsAExplorer; /* Pour le nettoyage eventuel */
  Bb->Liste1_PNE_EliminerLesNoeudsSousOptimaux    = NoeudsAExplorer;         /* Pour le nettoyage eventuel */

  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudsAExplorer[i] = NoeudsAExplorerAuProchainEtage[i];
  }
  free( NoeudsAExplorerAuProchainEtage );
/* fin while */
}	    

free( NoeudsAExplorer );
free( NoeudsAExplorerAuProchainEtage );

Bb->NbNoeuds1_PNE_EliminerLesNoeudsSousOptimaux = 0; /* Pour le nettoyage eventuel */
Bb->NbNoeuds2_PNE_EliminerLesNoeudsSousOptimaux = 0; /* Pour le nettoyage eventuel */

return;

}











