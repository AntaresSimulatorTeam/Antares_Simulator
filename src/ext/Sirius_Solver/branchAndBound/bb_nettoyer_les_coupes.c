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
/***********************************************************************

   FONCTION: Nettoyage des coupes si l'on en a trop cree.
            
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_fonctions.h"
 
# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/

void BB_NettoyerLesCoupes( BB * Bb, char DemandeExpresse )     
{
NOEUD ** NoeudsAExplorer                    ; NOEUD ** NoeudsAExplorerAuProchainEtage        ; 
NOEUD *  Noeud                              ; NOEUD *  NoeudCourant                          ; 
int     NombreDeNoeudsAExplorer            ; int     NombreDeNoeudsAExplorerAuProchainEtage;
int     i; int j; int  NumeroDeCoupe     ; int     NombreDeCoupesEnlevees                ;
char     UtiliserLaCoupe                    ; int     NumeroDeLaPremiereCoupeSupprimee      ;

COUPE ** Coupe;
char TypeDeCoupe; double * Coefficient; int * IndiceDeLaVariable; char OnInverse; int NbLiftAndProject;

/* Remarque: lorsqu'il faut nettoyer les coupes a cause d'instabilités numeriques, il faut vraiment
le faire sinon ca plante. Donc il ne faut pas shunter le test sur DemandeExpresse */

if ( DemandeExpresse != OUI ) {
  /* Plutot que de nettoyer on arrete d'en calculer */
  Bb->NbMaxDeCoupesCalculeesAtteint = OUI;	
  goto FinNettoyage;
}

#if VERBOSE_BB
  printf("-> Nettoyage des coupes\n");
#endif

Bb->EvaluerLesFilsDuMeilleurMinorant = OUI; 

/* Marquage initial des coupes */
Coupe = Bb->NoeudRacine->CoupesGenereesAuNoeud;  
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {   
  Coupe[NumeroDeCoupe]->UtiliserLaCoupe = NON;
  /* On veut conserver les coupes du noeud racine */
  if ( Coupe[NumeroDeCoupe]->CoupeRacine == OUI ) Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
}

NoeudsAExplorer = (NOEUD **) malloc( 1 * sizeof( void * ) );
if ( NoeudsAExplorer == NULL ) { 
  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_NettoyerLesCoupes 1\n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}
NombreDeNoeudsAExplorer = 1;
NoeudsAExplorer[0]      = Bb->NoeudRacine;

/* Marquage de coupes saturees qui sont presentes dans les noeuds qui restent a evaluer */
while( 1 ) {
  NoeudsAExplorerAuProchainEtage = (NOEUD **) malloc( NombreDeNoeudsAExplorer * 2 * sizeof( void * ) );
  if ( NoeudsAExplorerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_ NettoyerLesCoupes 2\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorerAuProchainEtage = 0;
  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudCourant = NoeudsAExplorer[i];  
    if ( NoeudCourant != 0 ) {
      if ( NoeudCourant->StatutDuNoeud == EVALUE && NoeudCourant->NoeudTerminal != OUI ) {
        Noeud = NoeudCourant->NoeudSuivantGauche;
        if ( Noeud != 0 ) {
	  if ( Noeud->StatutDuNoeud == A_EVALUER ) {
	    /* On recherche les coupes saturees */
            for ( j = 0 ; j < Noeud->NombreDeCoupesExaminees ; j++ ) {
              NumeroDeCoupe = Noeud->NumeroDesCoupesExaminees[j];
              Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = OUI;      
              if ( Noeud->LaCoupeEstSaturee[j] == OUI ) Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
              /* Si la coupe a un code saturation de -1 on la prend en compte */
              if ( Noeud->LaCoupeEstSaturee[j] == -1 ) Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;               	       	    
            }
	  }
	  else {
	    /* Le fils gauche a ete evalue, il faut descendre d'un cran */
            NombreDeNoeudsAExplorerAuProchainEtage++;
            NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;
	  }
	}	
        Noeud = NoeudCourant->NoeudSuivantDroit;
        if ( Noeud != 0 ) {
	  if ( Noeud->StatutDuNoeud == A_EVALUER ) {
	    /* On recherche les coupes saturees */
            for ( j = 0 ; j < Noeud->NombreDeCoupesExaminees ; j++ ) {
              NumeroDeCoupe = Noeud->NumeroDesCoupesExaminees[j];
              Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = OUI;      
              if ( Noeud->LaCoupeEstSaturee[j] == OUI ) Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;
              /* Si la coupe a un code saturation de -1 on la prend en compte */
              if ( Noeud->LaCoupeEstSaturee[j] == -1 ) Coupe[NumeroDeCoupe]->UtiliserLaCoupe = OUI;        	      
            }
	  }
	  else {
	    /* Le fils droit a ete evalue, il faut descendre d'un cran */
            NombreDeNoeudsAExplorerAuProchainEtage++;
            NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage-1] = Noeud;
          }	  
	}
      }     
    }
  } 
                        /* Preparations pour l'etage suivant */
  if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) break; /* 1- Exploration de l'arbre terminee */
  free( NoeudsAExplorer );
  NoeudsAExplorer = (NOEUD **) malloc( NombreDeNoeudsAExplorerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsAExplorer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_NettoyerLesCoupes 3\n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorer = NombreDeNoeudsAExplorerAuProchainEtage; 
  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudsAExplorer[i] = NoeudsAExplorerAuProchainEtage[i];
  }
  free( NoeudsAExplorerAuProchainEtage );
}	    

free( NoeudsAExplorerAuProchainEtage );

/* On enleve toutes les coupes qui ne sont pas saturees ou qui ne sont pas des lift and project */
NombreDeNoeudsAExplorer = 1;
NoeudsAExplorer[0]      = Bb->NoeudRacine;

NombreDeCoupesEnlevees = 0;
while( 1 ) {
  NoeudsAExplorerAuProchainEtage = (NOEUD **) malloc( NombreDeNoeudsAExplorer * 2 * sizeof( void * ) );
  if ( NoeudsAExplorerAuProchainEtage == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_NettoyerLesCoupes \n");
    Bb->AnomalieDetectee = OUI;
    longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
  }
  NombreDeNoeudsAExplorerAuProchainEtage = 0;
  for( i = 0 ; i < NombreDeNoeudsAExplorer ; i++ ) {
    NoeudCourant = NoeudsAExplorer[i]; 
    if ( NoeudCourant != 0 ) {
      /* On enleve tout ce qui concerne les coupes mais on conserve les lift and project, les coupes saturees
         ainsi que celles qui ont ete generees au noeud racine */
      NbLiftAndProject                 = 0;
      NumeroDeLaPremiereCoupeSupprimee = 0;
      if( NoeudCourant->NombreDeCoupesGenereesAuNoeud > 0 ) {
        Coupe = NoeudCourant->CoupesGenereesAuNoeud; /* Pointeur sur le tableau de pointeurs sur les coupes */

	/* On met toutes les lift and project en premier */
        OnInverse = OUI;
 	      while ( OnInverse == OUI ) {
          OnInverse = NON;
          for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud-1 ; NumeroDeCoupe++ ) {
            if ( Coupe[NumeroDeCoupe]->TypeDeCoupe != 'L' && Coupe[NumeroDeCoupe+1]->TypeDeCoupe == 'L' ) {
	            OnInverse = OUI;
              TypeDeCoupe         = Coupe[NumeroDeCoupe]->TypeDeCoupe;
              Coefficient         = Coupe[NumeroDeCoupe]->Coefficient;
              IndiceDeLaVariable  = Coupe[NumeroDeCoupe]->IndiceDeLaVariable;
	            UtiliserLaCoupe     = Coupe[NumeroDeCoupe]->UtiliserLaCoupe;
              /* La coupe NumeroDeCoupe devient de type L */ 
              /*Coupe[NumeroDeCoupe]->VariableCause      = Coupe[NumeroDeCoupe+1]->VariableCause;*/
              Coupe[NumeroDeCoupe]->TypeDeCoupe        = Coupe[NumeroDeCoupe+1]->TypeDeCoupe;
              Coupe[NumeroDeCoupe]->NombreDeTermes     = Coupe[NumeroDeCoupe+1]->NombreDeTermes;
              Coupe[NumeroDeCoupe]->Coefficient        = Coupe[NumeroDeCoupe+1]->Coefficient;
              Coupe[NumeroDeCoupe]->IndiceDeLaVariable = Coupe[NumeroDeCoupe+1]->IndiceDeLaVariable;
              Coupe[NumeroDeCoupe]->SecondMembre       = Coupe[NumeroDeCoupe+1]->SecondMembre;
              Coupe[NumeroDeCoupe]->UtiliserLaCoupe    = Coupe[NumeroDeCoupe+1]->UtiliserLaCoupe;	      
              /* La coupe NumeroDeCoupe + 1 devient de type autre que L */ 
              Coupe[NumeroDeCoupe+1]->TypeDeCoupe        = TypeDeCoupe;
              Coupe[NumeroDeCoupe+1]->Coefficient        = Coefficient;
              Coupe[NumeroDeCoupe+1]->IndiceDeLaVariable = IndiceDeLaVariable;
              Coupe[NumeroDeCoupe+1]->UtiliserLaCoupe    = UtiliserLaCoupe;	      
	          }
	        }	  
	      }
        for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud; NumeroDeCoupe++ ) {
	        if ( Coupe[NumeroDeCoupe]->TypeDeCoupe == 'L' ) NbLiftAndProject++;
	      }
	
        /* On met maintenant toutes les autres coupes a conserver */
	      /* On met toutes les lift and project en premier */
        OnInverse = OUI;
 	      while ( OnInverse == OUI ) {
          OnInverse = NON;
          for ( NumeroDeCoupe = NbLiftAndProject ; NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud-1; NumeroDeCoupe++ ) {
            if ( Coupe[NumeroDeCoupe]->UtiliserLaCoupe != OUI && Coupe[NumeroDeCoupe+1]->UtiliserLaCoupe == OUI ) {
	            OnInverse = OUI;
              TypeDeCoupe         = Coupe[NumeroDeCoupe]->TypeDeCoupe;
              Coefficient         = Coupe[NumeroDeCoupe]->Coefficient;
              IndiceDeLaVariable  = Coupe[NumeroDeCoupe]->IndiceDeLaVariable;
	            UtiliserLaCoupe     = Coupe[NumeroDeCoupe]->UtiliserLaCoupe;
              /* La coupe NumeroDeCoupe devient de type L */ 
              /*Coupe[NumeroDeCoupe]->VariableCause      = Coupe[NumeroDeCoupe+1]->VariableCause;*/
              Coupe[NumeroDeCoupe]->TypeDeCoupe        = Coupe[NumeroDeCoupe+1]->TypeDeCoupe;
              Coupe[NumeroDeCoupe]->NombreDeTermes     = Coupe[NumeroDeCoupe+1]->NombreDeTermes;
              Coupe[NumeroDeCoupe]->Coefficient        = Coupe[NumeroDeCoupe+1]->Coefficient;
              Coupe[NumeroDeCoupe]->IndiceDeLaVariable = Coupe[NumeroDeCoupe+1]->IndiceDeLaVariable;
              Coupe[NumeroDeCoupe]->SecondMembre       = Coupe[NumeroDeCoupe+1]->SecondMembre;
              Coupe[NumeroDeCoupe]->UtiliserLaCoupe    = Coupe[NumeroDeCoupe+1]->UtiliserLaCoupe;	      
              /* La coupe NumeroDeCoupe + 1 devient de type autre que L */ 
              Coupe[NumeroDeCoupe+1]->TypeDeCoupe        = TypeDeCoupe;
              Coupe[NumeroDeCoupe+1]->Coefficient        = Coefficient;
              Coupe[NumeroDeCoupe+1]->IndiceDeLaVariable = IndiceDeLaVariable;
              Coupe[NumeroDeCoupe+1]->UtiliserLaCoupe    = UtiliserLaCoupe;
	          }
	        }
	      }
        for ( NumeroDeCoupe = NbLiftAndProject ; NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud; NumeroDeCoupe++ ) {
	        if ( Coupe[NumeroDeCoupe]->UtiliserLaCoupe != OUI ) break;
	      }

	      NumeroDeLaPremiereCoupeSupprimee = NumeroDeCoupe;
        for ( NumeroDeCoupe = NumeroDeLaPremiereCoupeSupprimee;
	            NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud;
	            NumeroDeCoupe++ ) {
          free( Coupe[NumeroDeCoupe]->Coefficient ); 
          Coupe[NumeroDeCoupe]->Coefficient = NULL;
          free( Coupe[NumeroDeCoupe]->IndiceDeLaVariable );
          Coupe[NumeroDeCoupe]->IndiceDeLaVariable = NULL;
          free( Coupe[NumeroDeCoupe] );
          Coupe[NumeroDeCoupe] = NULL;	 
          NombreDeCoupesEnlevees++;
        }	
	      if ( NumeroDeLaPremiereCoupeSupprimee <= 0 ) {
	        free( NoeudCourant->CoupesGenereesAuNoeud );
          NoeudCourant->CoupesGenereesAuNoeud = NULL;
	      }
	      else {
          NoeudCourant->CoupesGenereesAuNoeud =
	          (COUPE **) realloc( NoeudCourant->CoupesGenereesAuNoeud , NumeroDeLaPremiereCoupeSupprimee * sizeof( void * ) );
	      }
      }
      
      NoeudCourant->NombreDeCoupesGenereesAuNoeud = NumeroDeLaPremiereCoupeSupprimee; /* Et ca sert aussi pour le noeud racine */
      if ( NumeroDeLaPremiereCoupeSupprimee <= 0 ) {      
        free( NoeudCourant->NumeroDesCoupeAjouteeAuProblemeCourant ); 
        NoeudCourant->NumeroDesCoupeAjouteeAuProblemeCourant = NULL;
      }
      else {
        NoeudCourant->NumeroDesCoupeAjouteeAuProblemeCourant =
	      (int *) realloc( NoeudCourant->NumeroDesCoupeAjouteeAuProblemeCourant , NumeroDeLaPremiereCoupeSupprimee * sizeof( int ) );
      }
			/* Decompte des types de coupes */
      NoeudCourant->NombreDeG = 0;
      NoeudCourant->NombreDeK = 0;
      NoeudCourant->NombreDeI = 0;
      for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < NoeudCourant->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
        if ( NoeudCourant->CoupesGenereesAuNoeud[NumeroDeCoupe]->TypeDeCoupe == 'G' ) NoeudCourant->NombreDeG++;
				else if ( NoeudCourant->CoupesGenereesAuNoeud[NumeroDeCoupe]->TypeDeCoupe == 'K' ) NoeudCourant->NombreDeK++;
				else if ( NoeudCourant->CoupesGenereesAuNoeud[NumeroDeCoupe]->TypeDeCoupe == 'I' ) NoeudCourant->NombreDeI++;
			}
			
      if ( NoeudCourant->NombreDeCoupesExaminees > 0 ) {
        free( NoeudCourant->NumeroDesCoupesExaminees ); 
        NoeudCourant->NumeroDesCoupesExaminees = NULL;
        free( NoeudCourant->LaCoupeEstSaturee ); 
        NoeudCourant->LaCoupeEstSaturee = NULL;
        NoeudCourant->NombreDeCoupesExaminees = 0;
      }

      if ( NoeudCourant->NombreDeCoupesViolees > 0 ) {
        free( NoeudCourant->NumeroDesCoupesViolees ); 
        NoeudCourant->NumeroDesCoupesViolees = NULL;
        NoeudCourant->NombreDeCoupesViolees = 0;
      }
            
      if ( NoeudCourant->StatutDuNoeud == A_EVALUER ) {
        /* Du coup, il faut revoir la base de depart de tous les noeuds a evaluer car elle
           peut etre singuliere. On dispose d'une base inversible, c'est la base optimale
           du noeud racine ou il n'y avait pas de coupes. C'est celle qu'on va mettre dans
           tous les noeuds a evaluer */
        memcpy( (char * ) NoeudCourant->PositionDeLaVariable , (char * ) Bb->NoeudRacine->PositionDeLaVariableSansCoupes ,
		          Bb->NombreDeVariablesDuProbleme * sizeof( int ) );
        NoeudCourant->NbVarDeBaseComplementaires = Bb->NoeudRacine->NbVarDeBaseComplementairesSansCoupes;    
        memcpy( (char * ) NoeudCourant->ComplementDeLaBase , (char * ) Bb->NoeudRacine->ComplementDeLaBaseSansCoupes ,
	  	          Bb->NoeudRacine->NbVarDeBaseComplementairesSansCoupes * sizeof( int ) );       
      }
      
      /* Renseigner la table des noeuds du prochain etage */
      if ( NoeudCourant != 0 ) {
        Noeud = NoeudCourant->NoeudSuivantGauche;
        if ( Noeud != 0 ) {
          NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage] = Noeud;
          NombreDeNoeudsAExplorerAuProchainEtage++;
        }
        Noeud = NoeudCourant->NoeudSuivantDroit;
        if ( Noeud != 0 ) {
          NoeudsAExplorerAuProchainEtage[NombreDeNoeudsAExplorerAuProchainEtage] = Noeud;  
          NombreDeNoeudsAExplorerAuProchainEtage++;
        }
      }
    }
  } 
                        /* Preparations pour l'etage suivant */
  if( NombreDeNoeudsAExplorerAuProchainEtage == 0 ) break; /* 2- Exploration de l'arbre terminee */
  free( NoeudsAExplorer );
  NoeudsAExplorer = (NOEUD **) malloc( NombreDeNoeudsAExplorerAuProchainEtage * sizeof( void * ) );
  if ( NoeudsAExplorer == NULL ) { 
    printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_NettoyerLesCoupes \n");
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

#if VERBOSE_BB
  printf("      Nombre de coupes enlevees %d \n", NombreDeCoupesEnlevees);
#endif

/* Si on nettoie vraiment, on recalcule des coupes */
Bb->NbMaxDeCoupesCalculeesAtteint = NON;

/*FinNettoyage:*/

Bb->NombreTotalDeCoupesDuPoolUtilisees = 0;
Bb->NombreTotalDeGDuPoolUtilisees = 0;
Bb->NombreTotalDeIDuPoolUtilisees = 0;
Bb->NombreTotalDeKDuPoolUtilisees = 0;

Bb->NombreDeProblemesResolusDepuisLaRAZDesCoupes = 0;

FinNettoyage:

Bb->EnleverToutesLesCoupesDuPool = NON; 
 
return; 
}

