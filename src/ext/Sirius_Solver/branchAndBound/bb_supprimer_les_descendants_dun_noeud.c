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

/*-----------------------------------------------------------------------------------------------------------------*/
/* 
  Suppression physique d'un sous-arbre a partir de NoeudDeDepart. Le noeud NoeudDeDepart n'est 
  pas supprime, il est marque comme noeud terminal.
*/

void BB_SupprimerTousLesDescendantsDUnNoeud( BB * Bb, NOEUD *  NoeudDeDepart )
{
int i                             ; int     ProfondeurRelative                     ;
NOEUD ** NoeudsASupprimer          ; NOEUD ** NoeudsASupprimerAuProchainEtage        ; 
int     NombreDeNoeudsASupprimer  ; int     NombreDeNoeudsASupprimerAuProchainEtage;
NOEUD  * NoeudCourant              ; NOEUD  * Noeud                                  ;
int Fois                          ; int     OnSupprime                             ;

Fois       = 1;
OnSupprime = OUI;

Debut:

if ( Fois == 2 ) NoeudDeDepart->NoeudTerminal = OUI;

ProfondeurRelative = 1;
NoeudsASupprimer   =  (NOEUD **) malloc( 2 * sizeof( void * ) );
if ( NoeudsASupprimer == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_SupprimerTousLesDescendantsDUnNoeud 1\n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}
NombreDeNoeudsASupprimer = 2;
NoeudsASupprimer[0] = NoeudDeDepart->NoeudSuivantGauche;
NoeudsASupprimer[1] = NoeudDeDepart->NoeudSuivantDroit; 
/* Pour indiquer que NoeudDeDepart n'a plus de fils apres suppression de ses fils */

if ( Fois == 2 ) {
  NoeudDeDepart->NoeudSuivantGauche = 0; 
  NoeudDeDepart->NoeudSuivantDroit  = 0;  
}

Bb->NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud = NombreDeNoeudsASupprimer; /* Pour le nettoyage eventuel */
Bb->Liste1_PNE_SupprimerTousLesDescendantsDUnNoeud    = NoeudsASupprimer;         /* Pour le nettoyage eventuel */

while( 1 ) {
  ProfondeurRelative++;
  NoeudsASupprimerAuProchainEtage = (NOEUD **) malloc( NombreDeNoeudsASupprimer * 2 * sizeof( void * ) );
  if ( NoeudsASupprimerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_SupprimerTousLesDescendantsDUnNoeud 2\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsASupprimerAuProchainEtage = 0;

  Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud = 0;                               /* Pour le nettoyage eventuel */
  Bb->Liste2_PNE_SupprimerTousLesDescendantsDUnNoeud    = NoeudsASupprimerAuProchainEtage; /* Pour le nettoyage eventuel */

  for( i = 0 ; i < NombreDeNoeudsASupprimer ; i++ ) {
    NoeudCourant = NoeudsASupprimer[i];    
    if( NoeudCourant != 0 ) {
      if ( Fois == 1 ) {
        if ( NoeudCourant == Bb->NoeudEnExamen ) { OnSupprime = NON; goto FinSuppression; }
      }         
                /* Renseigner la table des noeuds du prochain etage */
      Noeud = NoeudCourant->NoeudSuivantGauche;
      if( Noeud != 0 ) {
        if ( Fois == 1 ) {
          if ( Noeud == Bb->NoeudEnExamen ) { OnSupprime = NON; goto FinSuppression; }
        }       
        Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud++; /* Pour le nettoyage eventuel */
        NombreDeNoeudsASupprimerAuProchainEtage++;
        NoeudsASupprimerAuProchainEtage[NombreDeNoeudsASupprimerAuProchainEtage-1] = Noeud;
      }
      Noeud = NoeudCourant->NoeudSuivantDroit;
      if( Noeud != 0 ) {
        if ( Fois == 1 ) {
          if ( Noeud == Bb->NoeudEnExamen ) { OnSupprime = NON; goto FinSuppression; }
        }      
        Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud++; /* Pour le nettoyage eventuel */
        NombreDeNoeudsASupprimerAuProchainEtage++;
        NoeudsASupprimerAuProchainEtage[NombreDeNoeudsASupprimerAuProchainEtage-1] = Noeud;  
      }
      /* Liberation de la structure du noeud pere */
      if ( Fois == 2 ) {
        NoeudCourant->NoeudSuivantGauche = 0;
        NoeudCourant->NoeudSuivantDroit  = 0;            
        BB_DesallouerUnNoeud( Bb, NoeudCourant );
      }
    }
  /* fin for */
  }
                        /* Preparations pour l'etage suivant */
  if( NombreDeNoeudsASupprimerAuProchainEtage == 0 ) break; /* Exploration de l'arbre terminee */

  free( NoeudsASupprimer );

  NoeudsASupprimer = (NOEUD **) malloc( NombreDeNoeudsASupprimerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsASupprimer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_SupprimerTousLesDescendantsDUnNoeud 3\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsASupprimer = NombreDeNoeudsASupprimerAuProchainEtage; 

  Bb->NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud = NombreDeNoeudsASupprimer; /* Pour le nettoyage eventuel */
  Bb->Liste1_PNE_SupprimerTousLesDescendantsDUnNoeud    = NoeudsASupprimer;	   /* Pour le nettoyage eventuel */

  for( i = 0 ; i < NombreDeNoeudsASupprimer ; i++ ) {
    NoeudsASupprimer[i] = NoeudsASupprimerAuProchainEtage[i];
  }

  free( NoeudsASupprimerAuProchainEtage );

/* fin while */
}

FinSuppression:

free( NoeudsASupprimer );
free( NoeudsASupprimerAuProchainEtage );

Bb->NbNoeuds1_PNE_SupprimerTousLesDescendantsDUnNoeud = 0; /* Pour le nettoyage eventuel */
Bb->NbNoeuds2_PNE_SupprimerTousLesDescendantsDUnNoeud = 0; /* Pour le nettoyage eventuel */

if ( Fois == 1 ) {
  if ( OnSupprime == NON ) {
    /* Alors on part du noeud en examen */
    NoeudDeDepart = Bb->NoeudEnExamen; 
  }
  Fois = 2;
  goto Debut;
}

return;
}











