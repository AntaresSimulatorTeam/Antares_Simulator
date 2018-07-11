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

   FONCTION: Exploration de l'arbre en largeur a partir du noeud. Le noeud  
   de depart est lui aussi examine, mais il est le seul de son etage a     
   etre examine. Attention, le noeud de depart doit deja exister. 

   AUTEUR: R. GONZALEZ

**************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/

void BB_BalayageEnLargeur( BB * Bb , NOEUD * NoeudDeDepart , int ProfondeurDuNoeudDeDepart )
{
int     i                                  ; int     YaUneSolution                         ; 
int     SolutionEntiereTrouvee             ; int     ProfondeurDuNoeud                     ;
NOEUD ** NoeudsAExplorer                    ; NOEUD ** NoeudsAExplorerAuProchainEtage        ; 
NOEUD *  NoeudCourant                          ; 
int     NombreDeNoeudsAExplorer            ; int     NombreDeNoeudsAExplorerAuProchainEtage;
int     ProfondeurRelative                 ;  
int     NombreDeProblemesEvalues           ; int     PremierIndice                         ;
int     DernierIndice                      ; 




#if VERBOSE_BB
  printf("************************************************\n");
  printf("           Balayage En Largeur \n");
  printf("           ------------------- \n");
#endif

                     /* Initialisation */

ProfondeurDuNoeud       = ProfondeurDuNoeudDeDepart;			
ProfondeurRelative      = 0;
NoeudsAExplorer         = (NOEUD **) malloc( 1 * sizeof( void * ) );
if ( NoeudsAExplorer == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_BalayageEnLargeur \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

NombreDeNoeudsAExplorer = 1;
NoeudsAExplorer[0]      = NoeudDeDepart;

Bb->NbNoeuds1_PNE_BalayageEnLargeur = NombreDeNoeudsAExplorer; /* Pour le nettoyage eventuel */
Bb->Liste1_PNE_BalayageEnLargeur    = NoeudsAExplorer;         /* Pour le nettoyage eventuel */

while ( 1 ) {

  ProfondeurRelative++;

  Bb->TailleTableau = NombreDeNoeudsAExplorer * 2 * sizeof( void * );

  NoeudsAExplorerAuProchainEtage         = (NOEUD **) malloc( NombreDeNoeudsAExplorer * 2 * sizeof( void * ) );
  if ( NoeudsAExplorerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_BalayageEnLargeur \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
  Bb->NbNoeuds2_PNE_BalayageEnLargeur = 0;                              /* Pour le nettoyage eventuel */
  Bb->Liste2_PNE_BalayageEnLargeur    = NoeudsAExplorerAuProchainEtage; /* Pour le nettoyage eventuel */

  NombreDeNoeudsAExplorerAuProchainEtage = 0;
  NombreDeProblemesEvalues               = 0;
  PremierIndice                          = 0;

  for ( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {

    Bb->TypeDExplorationEnCours = LARGEUR_TOUT_SEUL;
    /* Lorsqu'on est dans un balayage en largeur on ne calcule pas de coupes */
    Bb->CalculerDesCoupes = NON_PNE;
    
    NoeudCourant = NoeudsAExplorer[i]; 
    if ( NoeudCourant != 0 ) {
      if ( NoeudCourant->StatutDuNoeud == A_EVALUER && 
           NoeudCourant->NoeudTerminal != OUI       && 
           NoeudCourant->StatutDuNoeud != A_REJETER ) {
      
        #if VERBOSE_BB
          printf("\n************************************************\n");
          printf("\n Balayage en largeur: Noeud %x , Profondeur en cours %d , numero du noeud dans la profondeur %d , nb de noeuds dans la largeur %d\n ",
                 NoeudCourant,ProfondeurDuNoeud,i,NombreDeNoeudsAExplorer);
        #endif

        Bb->NoeudEnExamen = NoeudCourant;
			     /* Exploration du noeud */
        NombreDeProblemesEvalues++; 
        YaUneSolution = BB_ResoudreLeProblemeRelaxe( Bb , NoeudCourant , &SolutionEntiereTrouvee ); 

        BB_NettoyerLArbre( Bb , &YaUneSolution , NoeudCourant );  /* Fait aussi la mise a jour du statut */

        BB_CreerLesNoeudsFils( Bb , NoeudCourant );    

      }
                    /* Renseigner la table des noeuds du prochain etage */
      Bb->NbNoeuds2_PNE_BalayageEnLargeur++; /* Pour le nettoyage eventuel */
      NombreDeNoeudsAExplorerAuProchainEtage++;
      NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = NoeudCourant->NoeudSuivantGauche;

      Bb->NbNoeuds2_PNE_BalayageEnLargeur++; /* Pour le nettoyage eventuel */
      NombreDeNoeudsAExplorerAuProchainEtage++;
      NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = NoeudCourant->NoeudSuivantDroit;

    }

    if ( NombreDeProblemesEvalues == CYCLE_POUR_RECHERCHE_EN_PROFONDEUR ) {  
      DernierIndice = i + 1;
      BB_FaireUneRechercheEnProfondeurDansUneRechercheEnLargeur( Bb ); 
      PremierIndice = DernierIndice;
      NombreDeProblemesEvalues = 0; 
    }

    /* Fin for */  
  }
                        /* C'est termine ? */
  if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) { 
    #if VERBOSE_BB
      printf(" Plus de noeud a explorer \n"); 
    #endif
    if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) break; /* Exploration de l'arbre terminee */
  }

  /* Exploration en profondeur du noeud de plus petit minorant a partir d'une certaine profondeur relative */

  if ( ProfondeurRelative >= 0 /*3*/ ) {
    #if VERBOSE_BB
      printf(" Exploration en profondeur dans une exploration en largeur \n"); 
    #endif    
    BB_FaireUneRechercheEnProfondeurDansUneRechercheEnLargeur( Bb );   
  }

  /* Temps maximum depasse ?*/
  if ( Bb->ArreterLesCalculs == OUI ) break;

                        /* Nettoyage des coupes */
  /*
  BB_NettoyerLesCoupes( Bb , NombreDeNoeudsAExplorer , NoeudsAExplorer );      
  */
                        /* Preparations pour l'etage suivant */         
  free( NoeudsAExplorer );
  ProfondeurDuNoeud++;
  NoeudsAExplorer = (NOEUD **) malloc( NombreDeNoeudsAExplorerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsAExplorer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_BalayageEnLargeur \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorer = NombreDeNoeudsAExplorerAuProchainEtage; 
  
  Bb->NbNoeuds1_PNE_BalayageEnLargeur = NombreDeNoeudsAExplorer; /* Pour le nettoyage eventuel */
  Bb->Liste1_PNE_BalayageEnLargeur    = NoeudsAExplorer;	        /* Pour le nettoyage eventuel */
  for ( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudsAExplorer[i] = NoeudsAExplorerAuProchainEtage[i];
  }
  free( NoeudsAExplorerAuProchainEtage );
/* Fin while */
}

free( NoeudsAExplorer );
free( NoeudsAExplorerAuProchainEtage );

Bb->NbNoeuds1_PNE_BalayageEnLargeur = 0;
Bb->NbNoeuds2_PNE_BalayageEnLargeur = 0;

return;
}

/*----------------------------------------------------------------------------------*/
/*      Fait une recherche en profondeur jusqu'a trouver une solution entiere       */

void BB_FaireUneRechercheEnProfondeurDansUneRechercheEnLargeur( BB * Bb )
{
char SolutionEntiereTrouveeGauche;

/*
printf("Recherche en profondeur dans un balayage en largeur\n"); fflush(stdout);
*/

Bb->TypeDExplorationEnCours = PROFONDEUR_DANS_LARGEUR;

if ( Bb->ArreterLesCalculs == NON ) BB_BestFirst( Bb );
  
SolutionEntiereTrouveeGauche = NON;

if ( SolutionEntiereTrouveeGauche == NON && Bb->ArreterLesCalculs == NON ) {

  /* Si l'ecart borne inf est trop grand on prend RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL
	   sinon on prend RECHERCHER_LE_PLUS_PETIT */
	/*
  if ( Bb->EcartBorneInf > 50. ) {
    BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
	}
	else {
    BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL );
	}
	*/
	
	BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL );

  if ( Bb->NoeudDuMeilleurMinorant != 0 ) {
    Bb->SolutionAmelioranteTrouvee = NON;
    SolutionEntiereTrouveeGauche = BB_BalayageEnProfondeur( Bb,
							                                              Bb->NoeudDuMeilleurMinorant, 
                                                            Bb->NoeudDuMeilleurMinorant->ProfondeurDuNoeud );
							    
    if ( Bb->ArreterLesCalculs == NON ) BB_BestFirst( Bb ); 
    
  }
}

return;
}








