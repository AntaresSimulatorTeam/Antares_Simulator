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

   FONCTION: Resolution avec la base reduite.
	           On ne calcule Tau que pour les contraintes de la base reduite.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"  

# include "lu_define.h"

# define DEBUG NON_SPX

/*----------------------------------------------------------------------------*/

void SPX_DualSteepestEdgeResolutionBaseReduite( PROBLEME_SPX * Spx, double * BetaPReturn,
                                                char ResolutionEnHyperCreux, char * ResetRefSpace,																							
																								int * NbTermesNonNulsDeTau, char * StockageDeTau,
																								char * LeSystemeAEteResolu )
{									  																	 
int Var; int il ; int ilMax; char Save; double BetaP; double X; char ResoudreLeSysteme;
char SecondMembreCreux; int * CdebProblemeReduit; int * CNbTermProblemeReduit; double * NBarreR;
char * InDualFramework; int * NumerosDesVariablesHorsBase; int * IndicesDeLigneDesTermesDuProblemeReduit;
int i;  double * ValeurDesTermesDesColonnesDuProblemeReduit; double * Tau; char TypeDEntree;
char TypeDeSortie; int j; int * NumVarNBarreRNonNul; int * T; int NbTermesNonNuls;
char HyperCreuxInitial; int * OrdreLigneDeLaBaseFactorisee; int RangDeLaMatriceFactorisee;
int r; int * IndexTermesNonNulsDeTau;

*ResetRefSpace = NON_SPX;
BetaP = *BetaPReturn;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

OrdreLigneDeLaBaseFactorisee = Spx->OrdreLigneDeLaBaseFactorisee;

InDualFramework = Spx->InDualFramework;   
NumerosDesVariablesHorsBase  = Spx->NumerosDesVariablesHorsBase;
NBarreR = Spx->NBarreR;

Tau = Spx->Tau;
IndexTermesNonNulsDeTau = (int *) Spx->ErBMoinsUn;  	

CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;

T = Spx->T;

ResoudreLeSysteme = NON_SPX;
*LeSystemeAEteResolu = ResoudreLeSysteme;

NbTermesNonNuls = 0;

if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {		
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  if ( ResolutionEnHyperCreux == OUI_SPX ) {	
 	  *StockageDeTau = ADRESSAGE_INDIRECT_SPX;								
    for ( j = 0 ; j < Spx->NombreDeValeursNonNullesDeNBarreR ; j++ ) {
      Var = NumVarNBarreRNonNul[j];
      if ( InDualFramework[Var] == NON_SPX ) continue;			
      X = NBarreR[Var];
		  if ( X == 0.0 ) continue;
      /* Calcul sur les variables hors base qui sont dans le framework */
      BetaP += X * X; 
      il    = CdebProblemeReduit[Var];
      ilMax = il + CNbTermProblemeReduit[Var];
      while ( il < ilMax ) {
		    r = IndicesDeLigneDesTermesDuProblemeReduit[il];
				if ( T[r] == -1 ) {
					T[r] = 1;
			    IndexTermesNonNulsDeTau[NbTermesNonNuls] = r;   
          NbTermesNonNuls++;
        }										
        Tau[r] += X * ValeurDesTermesDesColonnesDuProblemeReduit[il];																	
	      il++;
      }				  
    }
	  if ( NbTermesNonNuls != 0 ) ResoudreLeSysteme = OUI_SPX;
    /* Raz de T */
    for ( j = 0 ; j < NbTermesNonNuls ; j++ ) T[IndexTermesNonNulsDeTau[j]] = -1;
		
	  TypeDEntree  = ADRESSAGE_INDIRECT_LU;
	  TypeDeSortie = ADRESSAGE_INDIRECT_LU;
		
  }
	else {	
    /* Pas de resolution hyper creux */
 	  *StockageDeTau = VECTEUR_SPX;	
	  ResolutionEnHyperCreux = NON_SPX;				
		/* Base reduite */
    for ( j = 0 ; j < Spx->NombreDeValeursNonNullesDeNBarreR ; j++ ) {
      Var = NumVarNBarreRNonNul[j];
      if ( InDualFramework[Var] == NON_SPX ) continue;			
      X = NBarreR[Var];
		  if ( X == 0.0 ) continue;
      /* Calcul sur les variables hors base qui sont dans le framework */
      ResoudreLeSysteme = OUI_SPX;
      BetaP += X * X; 
      il    = CdebProblemeReduit[Var];
      ilMax = il + CNbTermProblemeReduit[Var];
      while ( il < ilMax ) {
		    Tau[IndicesDeLigneDesTermesDuProblemeReduit[il]] += X * ValeurDesTermesDesColonnesDuProblemeReduit[il];
				il++;
      }
    }		
	  TypeDEntree  = VECTEUR_LU;
	  TypeDeSortie = VECTEUR_LU;		
	}
}
else {
  /* Pas de resolution hyper creux */
 	*StockageDeTau = VECTEUR_SPX;
	ResolutionEnHyperCreux = NON_SPX;
	/* Base reduite */
  for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
    Var = NumerosDesVariablesHorsBase[i];	
    if ( InDualFramework[Var] == NON_SPX ) continue;
    if ( NBarreR[Var] == 0.0 ) continue;	
    X = NBarreR[Var];		
		if ( X == 0.0 ) continue;
    /* Calcul sur les variables hors base qui sont dans le framework */
    ResoudreLeSysteme = OUI_SPX;
    BetaP += X * X; 
    /*        */
    il    = CdebProblemeReduit[Var];
    ilMax = il + CNbTermProblemeReduit[Var];
    while ( il < ilMax ) {
		  Tau[IndicesDeLigneDesTermesDuProblemeReduit[il]] += X * ValeurDesTermesDesColonnesDuProblemeReduit[il];
			il++;
		}
	}		
	TypeDEntree  = VECTEUR_LU;
	TypeDeSortie = VECTEUR_LU;	  
}

*LeSystemeAEteResolu = ResoudreLeSysteme;
HyperCreuxInitial = ResolutionEnHyperCreux;

SPX_DualSteepestControleDuPoidsDeCntBase( Spx, BetaP, ResetRefSpace );
if ( *ResetRefSpace == OUI_SPX ) {
  return;
}

/* Dynamic steepest edge: si la variable sortante est une variable basique artificielle on l'enleve de 
   l'espace de reference */
SPX_DualSteepestVariablesDeBornesIdentiques( Spx, &BetaP ); 

*BetaPReturn = BetaP;

/* Resolution du systeme */

if ( ResoudreLeSysteme == OUI_SPX ) {
  Save = NON_LU;
  SecondMembreCreux = OUI_LU;
	
  if ( ResolutionEnHyperCreux == OUI_SPX ) {
	  if ( NbTermesNonNuls >= TAUX_DE_REMPLISSAGE_POUR_VECTEUR_HYPER_CREUX * RangDeLaMatriceFactorisee ) {			
		  ResolutionEnHyperCreux = NON_SPX;
 	    *StockageDeTau = VECTEUR_SPX;
	    TypeDeSortie = VECTEUR_LU;				
		}
	}

  SPX_ResolutionDeSysteme( Spx, TypeDEntree, Tau, IndexTermesNonNulsDeTau, &NbTermesNonNuls,
												   &TypeDeSortie, ResolutionEnHyperCreux, Save, SecondMembreCreux );
												   
}

/* Eventuellement forme produit de l'inverse */
if ( ResoudreLeSysteme == OUI_SPX ) {
  if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
    SPX_AppliquerLesEtaVecteurs( Spx, Tau, IndexTermesNonNulsDeTau, &NbTermesNonNuls, ResolutionEnHyperCreux, TypeDeSortie );
  }		

	
  SPX_DualSteepestGestionIndicateursHyperCreux( Spx, ResolutionEnHyperCreux, HyperCreuxInitial, TypeDeSortie,
	                                              StockageDeTau, ResetRefSpace );
	if ( *ResetRefSpace == OUI_SPX ) return;
	
}

*NbTermesNonNulsDeTau = NbTermesNonNuls;

if ( TypeDeSortie == ADRESSAGE_INDIRECT_LU ) *StockageDeTau = ADRESSAGE_INDIRECT_SPX;
else if ( TypeDeSortie == VECTEUR_LU ) *StockageDeTau = VECTEUR_SPX;
else {
  printf("BUG dans SPX_DualSteepestEdgeResolutionAvecBaseReduite TypeDeSortie = %d est incorrect\n",TypeDeSortie);
  exit(0);
}

# if VERIFICATION_STEEPEST == OUI_SPX
printf("---------------- DualSteepestEdgeResolutionBaseReduite  Spx->NombreDeChangementsDeBase %d -------------\n",Spx->NombreDeChangementsDeBase);
if ( TypeDEntree == VECTEUR_LU ) printf("apres resolution TypeDEntree = VECTEUR_LU");
if ( TypeDEntree == ADRESSAGE_INDIRECT_LU ) printf("apres resolution TypeDEntree = ADRESSAGE_INDIRECT_LU");
if ( TypeDeSortie == VECTEUR_LU ) printf(" TypeDeSortie = VECTEUR_LU\n");
if ( TypeDeSortie == ADRESSAGE_INDIRECT_LU ) printf(" TypeDeSortie = ADRESSAGE_INDIRECT_LU\n");	
{
double * Buff; int Var; int ic; int icMx; double * Sortie; char Arret; int rr;
Buff = (double *) malloc( RangDeLaMatriceFactorisee * sizeof( double ) );
Sortie = (double *) malloc( RangDeLaMatriceFactorisee * sizeof( double ) );
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Sortie[r]= 0;
if ( TypeDeSortie == ADRESSAGE_INDIRECT_LU ) {
  for ( i = 0 ; i < NbTermesNonNuls ; i++ ) {
	  Sortie[IndexTermesNonNulsDeTau[i]] = Tau[IndexTermesNonNulsDeTau[i]];
	}
}
else if ( TypeDeSortie == VECTEUR_LU ) {
  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Sortie[r] = Tau[r];	
}
else {
  printf("Bug dans DualSteepestEdgeResolutionBaseReduite\n");
	exit(0);
}

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) Buff[r]= 0;
if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  for ( j = 0 ; j < Spx->NombreDeValeursNonNullesDeNBarreR ; j++ ) {
    Var = NumVarNBarreRNonNul[j];
    if ( InDualFramework[Var] == NON_SPX ) continue;			
    X = NBarreR[Var];
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
		  r = OrdreLigneDeLaBaseFactorisee[NumeroDeContrainte[il]];
		  if ( r < RangDeLaMatriceFactorisee ) {		
        Buff[r] += X * ACol[il];
			}
	    il++;
    }				  
  }		
}
else {
  for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
    Var = NumerosDesVariablesHorsBase[i];
    if ( InDualFramework[Var] == NON_SPX ) continue;	
    X = NBarreR[Var];
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
		  r = OrdreLigneDeLaBaseFactorisee[NumeroDeContrainte[il]];
		  if ( r < RangDeLaMatriceFactorisee ) {			
        Buff[r]+= X * ACol[il];
			}
      il++;
    }
  }	
}     

for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
  if ( Sortie[r] == 0 ) continue;
  Var = Spx->VariableEnBaseDeLaContrainte[Spx->ColonneDeLaBaseFactorisee[r]];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	while ( ic < icMx ) {
		rr = OrdreLigneDeLaBaseFactorisee[NumeroDeContrainte[ic]];
		if ( rr < RangDeLaMatriceFactorisee ) {		
	    Buff[rr] -= ACol[ic] * Sortie[r];
		}
	  ic++;
	}
}
Arret = NON_SPX;
for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
	if ( fabs( Buff[r] ) > 1.e-7 ) {
	  printf("r = %d   ecart %e\n",r,Buff[r]);
    Var = Spx->VariableEnBaseDeLaContrainte[Spx->ColonneDeLaBaseFactorisee[r]];
		if ( Spx->OrigineDeLaVariable[Var] != NATIVE ) printf(" variable non native\n");
		else printf(" variable native\n");		
		Arret = OUI_SPX;
	}
}
if ( Arret == OUI_SPX ) {
 printf("Verif Tau  not OK\n");
 exit(0);
}
printf("Fin verif Tau   OK\n");
free( Buff );
free( Sortie );

}

SPX_VerifierLesVecteursDeTravail( Spx );

printf("VerifierLesVecteursDeTravail    OK\n");

# endif


return;
}


   
  
  
