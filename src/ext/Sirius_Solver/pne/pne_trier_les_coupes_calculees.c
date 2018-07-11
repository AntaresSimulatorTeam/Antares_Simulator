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

   FONCTION: Calcul des coupes 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/
/* Tri des coupes: apres la reoptimisation, on examine la position des la 
   variable d'ecart. Si elle est basique alors la coupe n'est pas saturee: 
   dans ce cas on ne la conserve pas */

void PNE_TrierLesCoupesCalculees( PROBLEME_PNE * Pne,
                                  int   YaUneSolution,
                                  int   BasesFilsDisponibles,
				                          char   ProblemeReoptimiseSurLesCoupes,
                  		            char * CoupeSaturee,
                                  char * CoupeSatureeAGauche, 
                                  char * CoupeSatureeADroite
                                 )
{
int i; int j  ; int jMax; int IndiceDeCoupe; int * ConserverLaContrainte; int IndexCnt; 
int NbCoupesConservees; char ArchiverToutesLesCoupes; 

int  NombreDeTermes; double * Coefficient_CG; int * IndiceDeLaVariable_CG; double SecondMembre;
COUPE_CALCULEE ** Coupe; char Type; BB * Bb;

if ( YaUneSolution == NON ) goto FinDuTri;

Pne->NbGDuCycle = 0;
Pne->NbIDuCycle = 0;
Pne->NbKDuCycle = 0;
Pne->NbGInsere = 0; 
Pne->NbIInsere = 0;
Pne->NbKInsere = 0;

Bb = (BB *) Pne->ProblemeBbDuSolveur;

ArchiverToutesLesCoupes = BB_ArchiverToutesLesCoupesLorsDuTri( Bb );

/* Si on ne reoptimise pas le noeud on ne peut pas se contenter d'archiver que les coupes
   saturees car on ne les connait pas */
if ( ProblemeReoptimiseSurLesCoupes == NON_PNE ) ArchiverToutesLesCoupes = OUI_PNE;

ConserverLaContrainte = (int *) malloc( Pne->Coupes.NombreDeContraintes * sizeof( int ) );
if ( ConserverLaContrainte == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_TrierLesCoupesCalculees \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {
  ConserverLaContrainte[j] = 3;
}

/* Pour le nettoyage des coupes preexistantes */
/* Si on n'archive pas toutes les coupes, on conserve celles qui sont saturees au noeud resolu
   ainsi que celles qui sont saturees en au moins 1 des 2 noeuds fils du strong branching */
jMax = Pne->Coupes.NombreDeContraintes - Pne->NombreDeCoupesCalculees;

for ( j = 0 ; j < jMax ; j++ ) {
  CoupeSaturee       [j] = OUI_PNE;
  CoupeSatureeAGauche[j] = OUI_PNE;
  CoupeSatureeADroite[j] = OUI_PNE;
}

if ( BasesFilsDisponibles == NON_PNE ) {
  /* Noeud resolu */
  for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {
    if ( Pne->Coupes.PositionDeLaVariableDEcart[j] == EN_BASE ) {
      /* La contrainte n'est pas saturee, on ne la conserve pas */
      if ( ArchiverToutesLesCoupes == NON_PNE ) ConserverLaContrainte[j] = 0;     
      if ( j < jMax ) CoupeSaturee[j] = NON_PNE;
    }
  } 
}
else {  
  /* Noeud resolu */		
  for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {
    if ( Pne->Coupes.PositionDeLaVariableDEcart[j] == EN_BASE ) {
      /* La contrainte n'est pas saturee, on ne la conserve pas */
      if ( ArchiverToutesLesCoupes == NON_PNE ) ConserverLaContrainte[j]-=1;     
    }		
  }  
  /* Noeud fils Gauche */
  for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {        
    if ( Pne->Coupes.PositionDeLaVariableDEcartAGauche[j] == EN_BASE ) {
      /* La contrainte n'est pas saturee, on ne la conserve pas */
      if (  ArchiverToutesLesCoupes == NON_PNE ) ConserverLaContrainte[j]-= 1; 
      if ( j < jMax ) CoupeSatureeAGauche[j] = NON_PNE;
    }						
  }
  /* Noeud fils Droit */
  for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {       
    if ( Pne->Coupes.PositionDeLaVariableDEcartADroite[j] == EN_BASE ) {
      /* La contrainte n'est pas saturee, on ne la conserve pas */
      if ( ArchiverToutesLesCoupes == NON_PNE ) ConserverLaContrainte[j]-= 1; 
      if ( j < jMax ) CoupeSatureeADroite[j] = NON_PNE;
    }						
  }
}

for ( j = 0 ; j < Pne->Coupes.NombreDeContraintes ; j++ ) {
  if ( Pne->Coupes.NbTerm[j] <= 0 ) {
    ConserverLaContrainte[j] = 0;
    Pne->Coupes.PositionDeLaVariableDEcart[j] = EN_BASE;
		Pne->Coupes.PositionDeLaVariableDEcartAGauche[j] = EN_BASE;
		Pne->Coupes.PositionDeLaVariableDEcartADroite[j] = EN_BASE;
    /* Pour les coupes qui proviennent des noeuds peres: si on a mis NbTerm = 0, il faut que la
       coupe soit non saturee a gauche et a droite sinon cela entraine une confusion dans la
       constitution des bases de depart des noeuds suivants */
    if ( j < jMax ) {
		  CoupeSaturee[j] = NON_PNE;
      CoupeSatureeAGauche[j] = NON_PNE;
		  CoupeSatureeADroite[j] = NON_PNE;
    }		
  }
}

/* Maintenant on procede au stockage des coupes au noeud */

IndexCnt = jMax - 1;
NbCoupesConservees = 0;
if ( Pne->ResolutionDuNoeudReussie == OUI_PNE ) {
  for ( IndiceDeCoupe = 0 ; IndiceDeCoupe < Pne->NombreDeCoupesCalculees ; IndiceDeCoupe++ ) {

    Type                  = Pne->CoupesCalculees[IndiceDeCoupe]->Type;			
    NombreDeTermes        = Pne->CoupesCalculees[IndiceDeCoupe]->NombreDeTermes;
    Coefficient_CG        = Pne->CoupesCalculees[IndiceDeCoupe]->Coefficient;
    IndiceDeLaVariable_CG = Pne->CoupesCalculees[IndiceDeCoupe]->IndiceDeLaVariable; 
    SecondMembre          = Pne->CoupesCalculees[IndiceDeCoupe]->SecondMembre;
		
    if ( Type == 'G' ) Pne->NbGDuCycle++;
		else if ( Type == 'K' ) Pne->NbKDuCycle++;
		else if ( Type == 'I' ) Pne->NbIDuCycle++;

    j = Pne->CoupesCalculees[IndiceDeCoupe]->NumeroDeLaContrainte;		

		if ( Pne->Coupes.NbTerm[j] > 0 ) IndexCnt++;
		
    if ( ConserverLaContrainte[j] == 0 ) {
      /* S'il ne s'agit pas d'une coupe negligee qu'on a essaye de remettre dans le cicrcuit, on la stocke
			   dans les coupes negligees */
			/*
      if ( Type == 'K' ) {
        if ( Pne->CoupesKNegligees != NULL ) {
		      if ( Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansKNegligees < 0 ) PNE_CreerUneCoupeKNegligee( Pne, j );
				}
		  }		
      else if ( Type == 'G' ) {
        if ( Pne->CoupesGNegligees != NULL ) {
		      if ( Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansGNegligees < 0 ) PNE_CreerUneCoupeGNegligee( Pne, j );
				}
		  }
			*/
		  continue;
		}
		
    if ( Type == 'G' ) Pne->NbGInsere++;
		else if ( Type == 'K' ) Pne->NbKInsere++;
		else if ( Type == 'I' ) Pne->NbIInsere++;

    # if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE
		  if ( Pne->Cliques != NULL ) {
		    i = Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansCliques;
		    if ( i >= 0 && i < Pne->Cliques->NombreDeCliques ) Pne->Cliques->LaCliqueEstDansLePool[i] = OUI_PNE;
			}
      if ( Pne->CoupesDeProbing != NULL ) {
		    i = Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansCoupesDeProbing;
		    if ( i >= 0 && i < Pne->CoupesDeProbing->NombreDeCoupesDeProbing ) Pne->CoupesDeProbing->LaCoupDeProbingEstDansLePool[i] = OUI_PNE;
      }
		 
      # if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
      if ( Pne->ContraintesDeBorneVariable != NULL ) {
		    i = Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansContraintesDeBorneVariable;				
		    if ( i >= 0 && i < Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne ) Pne->ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool[i] = OUI_PNE;
      }
			# endif
			
    # endif
		
    if ( Pne->CoupesKNegligees != NULL ) {
		  i = Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansKNegligees;
		  if ( i >= 0 && i <  Pne->CoupesKNegligees->NombreDeCoupes ) Pne->CoupesKNegligees->LaCoupeEstDansLePool[i] = OUI_PNE;
    }	
    if ( Pne->CoupesGNegligees != NULL ) {
		  i = Pne->CoupesCalculees[IndiceDeCoupe]->IndexDansGNegligees;
		  if ( i >= 0 && i <  Pne->CoupesGNegligees->NombreDeCoupes ) Pne->CoupesGNegligees->LaCoupeEstDansLePool[i] = OUI_PNE;
    }		
		  
    BB_StockerUneCoupeGenereeAuNoeud( Bb,
                                      NombreDeTermes, 
                                      Coefficient_CG, 
                                      IndiceDeLaVariable_CG, 
                                      SecondMembre, 
                                      Type );
		
    if ( BasesFilsDisponibles == NON_PNE ) {
      CoupeSaturee[jMax + NbCoupesConservees] = OUI_PNE;
      if ( Pne->Coupes.PositionDeLaVariableDEcart[j] == EN_BASE ) CoupeSaturee[jMax + NbCoupesConservees] = NON_PNE;

      if ( ArchiverToutesLesCoupes == OUI_PNE ) {
        if ( ProblemeReoptimiseSurLesCoupes == NON_PNE ) CoupeSaturee[jMax + NbCoupesConservees] = -1;
	      else if ( Pne->Coupes.PositionDeLaVariableDEcart[j] == EN_BASE ) CoupeSaturee[jMax + NbCoupesConservees] = -1;
      }
      
    }
    else {
      CoupeSatureeAGauche[jMax + NbCoupesConservees] = OUI_PNE;
      if ( Pne->Coupes.PositionDeLaVariableDEcartAGauche[j] == EN_BASE ) CoupeSatureeAGauche[jMax + NbCoupesConservees] = NON_PNE;

      if ( ArchiverToutesLesCoupes == OUI_PNE ) {
        if ( ProblemeReoptimiseSurLesCoupes == NON_PNE ) CoupeSatureeAGauche[jMax + NbCoupesConservees] = -1;
	      else if ( Pne->Coupes.PositionDeLaVariableDEcartAGauche[j] == EN_BASE ) CoupeSatureeAGauche[jMax + NbCoupesConservees] = -1;
      }
      
      CoupeSatureeADroite[jMax + NbCoupesConservees] = OUI_PNE;
      if ( Pne->Coupes.PositionDeLaVariableDEcartADroite[j] == EN_BASE ) CoupeSatureeADroite[jMax + NbCoupesConservees] = NON_PNE;

      if ( ArchiverToutesLesCoupes == OUI_PNE ) {
        if ( ProblemeReoptimiseSurLesCoupes == NON_PNE ) CoupeSatureeADroite[jMax + NbCoupesConservees] = -1;
	      else if ( Pne->Coupes.PositionDeLaVariableDEcartADroite[j] == EN_BASE ) CoupeSatureeADroite[jMax + NbCoupesConservees] = -1;
      }
      
    }

    NbCoupesConservees++;    
  }
}

#if VERBOSE_PNE
  printf("-> Nombre de coupes calculees %d  Nombre de coupes conservees %d\n",Pne->NombreDeCoupesCalculees,NbCoupesConservees); 
#endif
	
if ( Bb->AffichageDesTraces == OUI && VERBOSE_PNE == 1 ) {
  if ( Pne->NbGDuCycle != 0 || Pne->NbIDuCycle != 0 || Pne->NbKDuCycle != 0 ) {
    printf("Generating new cuts (family cut: computed/kept)   ");
    if ( Pne->NbGDuCycle != 0 ) {	
      printf("G: %3d/%3d   ",Pne->NbGDuCycle,Pne->NbGInsere);
    }
    if ( Pne->NbIDuCycle != 0 ) {	
      printf("I: %3d/%3d   ",Pne->NbIDuCycle,Pne->NbIInsere);
    }
    if ( Pne->NbKDuCycle != 0 ) {	
      printf("K: %3d/%3d   ",Pne->NbKDuCycle,Pne->NbKInsere);
    }
    printf("\n");	
    /* Pour forcer le reaffichage de la legende */
	  Bb->NombreDAffichages = CYCLE_DAFFICHAGE_LEGENDE;
	}
}

free( ConserverLaContrainte );

FinDuTri:
/* Liberation de la structure COUPE_CALCULEE */
if( Pne->NombreDeCoupesCalculees > 0 ) {
  Coupe = Pne->CoupesCalculees; /* Pointeur sur le tableau de pointeurs sur les coupes */  
  for ( i = 0 ; i < Pne->NombreDeCoupesCalculees ; i++ ) {
    free( Coupe[i]->Coefficient ); 
    free( Coupe[i]->IndiceDeLaVariable );
    free( Coupe[i] );	
  }
  free( Pne->CoupesCalculees ); 
  Pne->NombreDeCoupesCalculees = 0;
}

/*Pne->Coupes.NombreDeContraintes = 0;*/ /* Remis a 0 dans PNE_RecupererLeProblemeInitial */

return;
}

/*----------------------------------------------------------------------------*/
/* Routine appelee par la partie branch and bound. Elle inhibe, au sens de
   la partie PNE les coupes non saturees afin que lors d'une nouvelle evaluation
   on ne mette pas trop de temps de calcul inutilement. Pour inhiber, on
   met a 0 le nombre de termes de la coupe.
   Attention cela n'est prevu que pour fonctionner au noeud racine car il
   n'y a que la qu'on un grand nombre de round de coupes. */

void PNE_ActualiserLesCoupesAPrendreEnCompte( PROBLEME_PNE * Pne )
{
int j; int jMax; int NbSat; int Seuil; int NbConserve;

jMax = Pne->Coupes.NombreDeContraintes - Pne->NombreDeCoupesCalculeesNonEvaluees;

/* En dessous de x% de coupes, on ne fait rien */
if ( jMax < 0.02 * Pne->NombreDeContraintesTrav || jMax <= 10 /*100*/ ) {
  return;
}

NbSat = 0;
Seuil = (int) ceil( 0.5 * jMax );

if ( Seuil <= 10 ) Seuil = 10;
for ( j = 0 ; j < jMax ; j++ ) {
  if ( Pne->Coupes.NbTerm[j] <= 0 ) continue;
  if ( Pne->Coupes.PositionDeLaVariableDEcart[j] != EN_BASE ) {
    /* Contrainte saturee */
    NbSat++;
  }
}
NbConserve = NbSat;

/* On ne conserve que les coupes saturees */
/* Il n'est pas judicieux de conserver des coupes non saturees car on risque de s'en servir pour calculer
   des gomory supplementaires */
if ( NbSat > Seuil ) {
  /* Il y a deja beaucoup de contraintes saturee => on enleve les non saturee */
  for ( j = 0 ; j < jMax ; j++ ) {
    if ( Pne->Coupes.NbTerm[j] <= 0 ) continue;
    /* Si la contrainte n'est pas saturee, on ne la conserve pas */		
    if ( Pne->Coupes.PositionDeLaVariableDEcart[j] != EN_BASE ) continue;	
    Pne->Coupes.NbTerm[j] = 0;

		/* Les coupes qui ont ete mises ici ne sont jamais des coupes negligees qu'on a essaye de remettre dans le circuit */
    if ( Pne->Coupes.TypeDeCoupe[j] == 'K' ) PNE_CreerUneCoupeKNegligee( Pne, j );			
    else if ( Pne->Coupes.TypeDeCoupe[j] == 'G' ) PNE_CreerUneCoupeGNegligee( Pne, j );
		
  }  
}
else {
  /* Il n'y a pas beaucoup de contraintes saturee => on conserve les contraintes
     jusqu'a la valeur Seuil */
  for ( j = 0 ; j < jMax ; j++ ) {
    if ( Pne->Coupes.NbTerm[j] <= 0 ) continue;	  
    if ( Pne->Coupes.PositionDeLaVariableDEcart[j] != EN_BASE ) continue;
    /* La contrainte n'est pas saturee */
    if ( NbConserve > Seuil ) {
      /* On ne la conserve pas */
      Pne->Coupes.NbTerm[j] = 0;

		  /* Les coupes qui ont ete mises ici ne sont jamais des coupes negligees qu'on a essaye de remettre dans le circuit */
      if ( Pne->Coupes.TypeDeCoupe[j] == 'K' ) PNE_CreerUneCoupeKNegligee( Pne, j );			
      else if ( Pne->Coupes.TypeDeCoupe[j] == 'G' ) PNE_CreerUneCoupeGNegligee( Pne, j );
						
    }
    else NbConserve++;
  }
}

return;
}

