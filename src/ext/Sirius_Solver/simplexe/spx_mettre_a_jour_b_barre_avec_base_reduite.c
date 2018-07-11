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

   FONCTION: Mise de BBarre = B^{-1} * b 

                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"  

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h" 
# include "lu_fonctions.h"

# define DEBUG NON_SPX

void SPX_CalculSecondMembrePourABarreSAvecBaseReduite( PROBLEME_SPX * , char * , int * , int * );
																											 
/*----------------------------------------------------------------------------*/

void SPX_CalculSecondMembrePourABarreSAvecBaseReduite( PROBLEME_SPX * Spx, char * ResoudreLeSysteme,
                                                       int * IndexTermesNonNuls, int * NbTNonNuls )		 							 
{
int il; int ilMax; int i; int Var; double Xmx; char Flag; int * BoundFlip; double * Xmax;
int * IndicesDeLigneDesTermesDuProblemeReduit; double * ValeurDesTermesDesColonnesDuProblemeReduit;
int * CdebProblemeReduit; int * CNbTermProblemeReduit; int RangDeLaMatriceFactorisee;
int r; int * T; double * Bs; int NbTermesNonNuls;

BoundFlip = Spx->BoundFlip;
Xmax = Spx->Xmax;
Bs = Spx->Bs;

CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

T = Spx->T; /* Toujours a -1 quand on a fini de s'en servir */

NbTermesNonNuls = 0;	

Flag = NON_SPX;

for ( i = 0 ; i < Spx->NbBoundFlip; i++ ) { 
  Var = BoundFlip[i];
  if ( BoundFlip[i] > 0 ) {
    Var = BoundFlip[i] - 1;
	  Xmx = Xmax[Var];	   
  }
  else {
    Var = -BoundFlip[i] - 1;
  	Xmx = -Xmax[Var];	
  }
  if ( Xmx == 0.0 ) continue;
  il    = CdebProblemeReduit[Var];
  ilMax = il + CNbTermProblemeReduit[Var];
  while ( il < ilMax ) {
	  r = IndicesDeLigneDesTermesDuProblemeReduit[il];
	  if ( T[r] == -1 ) {
			Flag  = OUI_SPX;
      T[r] = 1;
	    IndexTermesNonNuls[NbTermesNonNuls] = r;   
      NbTermesNonNuls++;
	  }
	  Bs[r] += Xmx * ValeurDesTermesDesColonnesDuProblemeReduit[il];			    	   		  
    il++;
  }
}

/* Raz de T */
for ( i = 0 ; i < NbTermesNonNuls ; i++ ) T[IndexTermesNonNuls[i]] = -1;	 		

/* On a interet a compacter AReduit et peut etre meme aussi pour la partie hors base reduite */

for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
	if ( Bs[IndexTermesNonNuls[i]] == 0 ) {
		IndexTermesNonNuls[i] = IndexTermesNonNuls[NbTermesNonNuls-1];
		i--;
		NbTermesNonNuls--;
	}
}
  
*ResoudreLeSysteme = Flag;
*NbTNonNuls        = NbTermesNonNuls;

return;
}

/*----------------------------------------------------------------------------*/
/*                    Cas de la mise a jour de BBarre                         */

void SPX_MettreAJourBBarreAvecBaseReduite( PROBLEME_SPX * Spx, double * Bs, int * IndexTermesNonNuls,
                                           int * NbTermesNonNuls, char * StockageDeBs )
{
char Save; char SecondMembreCreux; char ResoudreLeSysteme; char TypeDEntree; char TypeDeSortie;
char ResolutionEnHyperCreux;

if ( Spx->CalculABarreSEnHyperCreux == OUI_SPX ) ResolutionEnHyperCreux = OUI_SPX;
else ResolutionEnHyperCreux = NON_SPX;

SPX_CalculSecondMembrePourABarreSAvecBaseReduite( Spx, &ResoudreLeSysteme, IndexTermesNonNuls, NbTermesNonNuls );

if ( Spx->TypeDeCreuxDeLaBase == BASE_HYPER_CREUSE && ResolutionEnHyperCreux == OUI_SPX ) {
	TypeDEntree = ADRESSAGE_INDIRECT_LU;
	TypeDeSortie = COMPACT_LU;      
	*StockageDeBs = COMPACT_SPX;	
}
else {	
	*StockageDeBs = VECTEUR_SPX;
	ResolutionEnHyperCreux = NON_SPX; 
	TypeDEntree = VECTEUR_LU;
	TypeDeSortie = VECTEUR_LU;      
}

if ( ResoudreLeSysteme == OUI_SPX ) {
  Save = NON_LU;
  SecondMembreCreux = OUI_LU;
	
	if ( ResolutionEnHyperCreux == OUI_SPX ) {			
	  if ( *NbTermesNonNuls >= ceil(  TAUX_DE_REMPLISSAGE_POUR_VECTEUR_HYPER_CREUX * Spx->RangDeLaMatriceFactorisee ) ) {
		  ResolutionEnHyperCreux = NON_SPX;
	    *StockageDeBs = VECTEUR_SPX;
	    TypeDEntree = VECTEUR_LU;
	    TypeDeSortie = VECTEUR_LU;
		}	 		
	}

	/* TypeDEntree est egal soit a ADRESSAGE_INDIRECT_SPX soit a COMPACT_LU */
	/* TypeDeSortie (demande) est egal a COMPACT_LU */
		
  SPX_ResolutionDeSysteme( Spx, TypeDEntree, Bs, IndexTermesNonNuls, NbTermesNonNuls,
												   &TypeDeSortie, ResolutionEnHyperCreux, Save, SecondMembreCreux );
  													 										 												 
  if ( ResolutionEnHyperCreux == OUI_SPX ) {
    if ( TypeDeSortie != COMPACT_LU ) {		
      *StockageDeBs = VECTEUR_SPX;
    }		
  }

}
else {
  *NbTermesNonNuls = 0; /* Pour le cas ou on ne resout pas le systeme */
}

return;

/* A completer par la forme produit de l'inverse si necessaire */


# if VERIFICATION_MAJ_BBARRE == OUI_SPX
printf("------------- MettreAJourBBarreAvecBaseReduite Spx->NombreDeChangementsDeBase %d  Iteration %d ---\n",Spx->NombreDeChangementsDeBase,Spx->Iteration);
if ( TypeDEntree == VECTEUR_LU ) printf("TypeDEntree = VECTEUR_LU\n");
if ( TypeDEntree == COMPACT_LU ) printf("TypeDEntree = COMPACT_LU\n");
if ( TypeDEntree == ADRESSAGE_INDIRECT_LU ) printf("TypeDEntree = ADRESSAGE_INDIRECT_LU\n");
if ( TypeDeSortie == VECTEUR_LU ) printf("TypeDeSortie = VECTEUR_LU\n");
if ( TypeDeSortie == COMPACT_LU ) printf("TypeDeSortie = COMPACT_LU\n");
if ( TypeDeSortie == ADRESSAGE_INDIRECT_LU ) printf("TypeDeSortie = ADRESSAGE_INDIRECT_LU\n");
if ( *StockageDeBs == VECTEUR_LU ) printf("StockageDeBs = VECTEUR_LU\n");
if ( *StockageDeBs == COMPACT_LU ) printf("StockageDeBs = COMPACT_LU\n");
if ( *StockageDeBs == ADRESSAGE_INDIRECT_LU ) printf("StockageDeBs = ADRESSAGE_INDIRECT_LU\n");

{
double * Buff; int i; int Var; int ic; int icMx; double * Sortie; char Arret; double Xmx;
Buff = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
Sortie = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = 0;
if ( *StockageDeBs == COMPACT_LU ) {
  for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) Sortie[IndexTermesNonNuls[i]] = Bs[i];   
}
else if ( *StockageDeBs == ADRESSAGE_INDIRECT_LU ) { /* EN realite c'est a prendre comme une forme compacte */
  for ( i = 0 ; i < *NbTermesNonNuls ; i++ ) {
	  Sortie[IndexTermesNonNuls[i]] = Bs[i];
	}
}
else if ( *StockageDeBs == VECTEUR_LU ) {
  for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = Bs[i];
}
else {
  printf("BUG StockageDeBs incorrect \n");
	exit(0);
}

for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Buff[i] = 0;
for ( i = 0 ; i < Spx->NbBoundFlip; i++ ) { 
  Var = Spx->BoundFlip[i];
	if ( Spx->BoundFlip[i] > 0 ) {
	  Var = Spx->BoundFlip[i] - 1;
		Xmx = Xmax[Var];	   
	}
	else {
	  Var = -Spx->BoundFlip[i] - 1;
		Xmx = -Xmax[Var];	
	}
  if ( Xmx == 0.0 ) continue;
  il    = Spx->Cdeb[Var];
  ilMax = il + Spx->CNbTerm[Var];
  while ( il < ilMax ) {
		Buff[Spx->NumeroDeContrainte[il]] += Xmx * Spx->ACol[il];		
    il++;
	}
}

for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  Var = Spx->VariableEnBaseDeLaContrainte[i];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	while ( ic < icMx ) {
	  Buff[NumeroDeContrainte[ic]] -= ACol[ic] * Sortie[i];
	  ic++;
	}
}

Arret = NON_SPX;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
	if ( fabs( Buff[Cnt] ) > 1.e-7 ) {
	  printf("Cnt = %d   ecart %e\n",Cnt,Buff[Cnt]);

    il = Spx->Mdeb[Cnt];
		ilMax = il + Spx->NbTerm[Cnt];
    while ( il < ilMax ) {
		  Var = Spx->Indcol[il];
			if ( Spx->PositionDeLaVariable[Var] == EN_BASE_LIBRE ) {
        printf("A %e Colonne %d  valeur %e\n",Spx->A[il],Spx->ContrainteDeLaVariableEnBase[Var],Bs[Spx->ContrainteDeLaVariableEnBase[Var]]);
		  }
			il++;
		}
    printf("\n");
		
		Arret = OUI_SPX;
	}
}

free( Buff );
free( Sortie );

SPX_VerifierLesVecteursDeTravail( Spx );
printf("Verif des vecteurs auxiliaires OK\n");

if ( Arret == OUI_SPX ) {
  printf("RangDeLaMatriceFactorisee %d   NombreDeContraintes %d\n",Spx->RangDeLaMatriceFactorisee,Spx->NombreDeContraintes);
  exit(0);
}
printf("Fin verif MettreAJourBBarreAvecBaseReduite  OK\n");

}

# endif

return;
}   

