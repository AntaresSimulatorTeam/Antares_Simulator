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

   FONCTION: Resolution du systeme transpose pour obtenir une ligne
             de l'inverse de la base    

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"

# define DEBUG NON_SPX

/*----------------------------------------------------------------------------*/
/* Calcul de la ligne de B^{-1} de la variable sortante */
void SPX_CalculerErBMoins1AvecBaseReduite( PROBLEME_SPX * Spx, char CalculEnHyperCreux ) 
{
double * ErBMoinsUn; int * IndexTermesNonNulsDeErBMoinsUn; int i; int Cnt;
char TypeDEntree; char ResoudreLeSystemeReduit; char TypeDeSortie; char SecondMembreCreux;
char * PositionDeLaVariable; double * AReduit; int * IndexAReduit;
int RangDeLaMatriceFactorisee; int NombreDeTermesNonNulsDuVecteurReduit;
int * OrdreColonneDeLaBaseFactorisee; int * Mdeb; int * NbTerm; int * Indcol;
double * A; int r; int CntVarSor; int * ContrainteDeLaVariableEnBase;
int NbTermesNonNulsDeErBMoinsUn; int * LigneDeLaBaseFactorisee;

ErBMoinsUn = Spx->ErBMoinsUn;
IndexTermesNonNulsDeErBMoinsUn = Spx->IndexTermesNonNulsDeErBMoinsUn;
NbTermesNonNulsDeErBMoinsUn = 0;

ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
PositionDeLaVariable = Spx->PositionDeLaVariable;
Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A = Spx->A;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;
AReduit = Spx->AReduit;
IndexAReduit = Spx->IndexAReduit;
OrdreColonneDeLaBaseFactorisee = Spx->OrdreColonneDeLaBaseFactorisee;
LigneDeLaBaseFactorisee = Spx->LigneDeLaBaseFactorisee;

CntVarSor = ContrainteDeLaVariableEnBase[Spx->VariableSortante];
ResoudreLeSystemeReduit = NON_SPX;

/* Remarque: a ce stade toutes les AReduit composantes de sont nulles */

if ( CalculEnHyperCreux != OUI_SPX ) {
  TypeDEntree = VECTEUR_LU;		 
	TypeDeSortie = VECTEUR_LU;
  SecondMembreCreux = NON_SPX;
	
  r = OrdreColonneDeLaBaseFactorisee[CntVarSor];
	if ( r < RangDeLaMatriceFactorisee ) {	
	  ResoudreLeSystemeReduit	= OUI_SPX;						
		AReduit[r] = 1;
  }
	else {
		/* Ca ne peut pas arriver */
		printf("CalculerErBMoins1AvecBaseReduite bug: variable sortante hors base reduite impossible\n");
		exit(0);
  }
}
else {
  TypeDEntree  = COMPACT_LU;
	TypeDeSortie = COMPACT_LU;
  SecondMembreCreux = OUI_SPX;
	NombreDeTermesNonNulsDuVecteurReduit = 0;
	
  r = OrdreColonneDeLaBaseFactorisee[CntVarSor];
	if ( r < RangDeLaMatriceFactorisee ) {	
	  ResoudreLeSystemeReduit	= OUI_SPX;						
		AReduit[NombreDeTermesNonNulsDuVecteurReduit] = 1;
		IndexAReduit[NombreDeTermesNonNulsDuVecteurReduit] = r;
    NombreDeTermesNonNulsDuVecteurReduit++;		
  }
	else {
		/* Ca ne peut pas arriver */
		printf("CalculerErBMoins1AvecBaseReduite bug: variable sortante hors base reduite impossible\n");
		exit(0);			    			
	}
}   

/* Eventuellement les Eta vecteurs */
/* Attention il faut pas faire comme ca mais resoudre tout le systeme avec les eta */
/*
if ( Spx->UtiliserLaLuUpdate == NON_SPX ) {
  SPX_AppliquerLesEtaVecteursTransposee( Spx, Pi, IndexDesTermesNonNuls, NombreDeTermesNonNuls,
	                                       CalculEnHyperCreux, TypeDEntree );	
}
*/

SPX_ResolutionDeSystemeTransposee( Spx, TypeDEntree, AReduit, IndexAReduit, &NombreDeTermesNonNulsDuVecteurReduit,																		
                                   &TypeDeSortie, CalculEnHyperCreux, SecondMembreCreux );
																	 
if ( TypeDeSortie == VECTEUR_LU ) {
	for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) ErBMoinsUn[Cnt] = 0.0;	
	/* On complete avec la partie de la base reduite */
  for ( r = 0 ; r < RangDeLaMatriceFactorisee ; r++ ) {
	  ErBMoinsUn[LigneDeLaBaseFactorisee[r]] = AReduit[r];
	  AReduit[r] = 0;			  
	}
}
else {
  /* TypeDeSortie = COMPACT_LU */
	/* Le type d'entree est forcement COMPACT_LU */
  for ( i = 0 ; i < NombreDeTermesNonNulsDuVecteurReduit ; i++ ) {
		ErBMoinsUn[NbTermesNonNulsDeErBMoinsUn] = AReduit[i];
		AReduit[i] = 0;
		IndexTermesNonNulsDeErBMoinsUn[NbTermesNonNulsDeErBMoinsUn] = LigneDeLaBaseFactorisee[IndexAReduit[i]];
    NbTermesNonNulsDeErBMoinsUn++;
  }
	Spx->NbTermesNonNulsDeErBMoinsUn = NbTermesNonNulsDeErBMoinsUn;
}								 
										 
if ( CalculEnHyperCreux == OUI_SPX ) {
  if ( TypeDeSortie != TypeDEntree ) {
    /* Ca s'est pas bien passe et on s'est forcement retrouve en VECTEUR_LU */
    Spx->TypeDeStockageDeErBMoinsUn = VECTEUR_SPX;		
		Spx->NbEchecsErBMoins++;
		/*
		printf("SPX_CalculerErBMoins1 echec hyper creux ErBMoins1 iteration %d\n",Spx->Iteration);
		*/
		if ( Spx->NbEchecsErBMoins >= SEUIL_ECHEC_CREUX ) {
      # if VERBOSE_SPX
		    printf("Arret de l'hyper creux pour le calcul de la ligne pivot, iteration %d\n",Spx->Iteration);
      # endif			
		  Spx->CalculErBMoinsUnEnHyperCreux = NON_SPX;
      Spx->CountEchecsErBMoins = 0;
		}		
	}
	else Spx->NbEchecsErBMoins = 0;
}

# if VERIFICATION_ERBMOINS1 == OUI_SPX
printf("------------- CalculerErBMoins1 Spx->NombreDeChangementsDeBase %d  Iteration %d ---\n",Spx->NombreDeChangementsDeBase,Spx->Iteration);
if ( TypeDEntree == VECTEUR_LU ) printf("TypeDEntree = VECTEUR_LU\n");
if ( TypeDEntree == COMPACT_LU ) printf("TypeDEntree = COMPACT_LU\n");
if ( TypeDeSortie == VECTEUR_LU ) printf("TypeDeSortie = VECTEUR_LU\n");
if ( TypeDeSortie == COMPACT_LU ) printf("TypeDeSortie = COMPACT_LU\n");
{
double * Buff; int i; int Var; int ic; int icMx; double S; double * Sortie; char Arret;
Buff = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
Sortie = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
if ( TypeDeSortie == COMPACT_LU ) {
  for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = 0;
  for ( i = 0 ; i < NbTermesNonNulsDeErBMoinsUn ; i++ ) Sortie[IndexTermesNonNulsDeErBMoinsUn[i]] = ErBMoinsUn[i];   
}
else {
  for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Sortie[i] = Spx->ErBMoinsUn[i];
}
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) Buff[i] = 0;
Buff[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]] = 1.;
Arret = NON_SPX;
for ( i = 0 ; i < Spx->NombreDeContraintes ; i++ ) {
  Var = Spx->VariableEnBaseDeLaContrainte[i];
	ic = Spx->Cdeb[Var];
	icMx = ic + Spx->CNbTerm[Var];
	S = 0;  
	while ( ic < icMx ) {		
	  S += Spx->ACol[ic] * Sortie[Spx->NumeroDeContrainte[ic]];
	  ic++;
	}
	if ( fabs( S - Buff[i] ) > 1.e-7 ) {
	  printf("i = %d  S %e Buff %e  ecart %e\n",i,S,Buff[i],fabs( S - Buff[i] ));
		printf("Var = %d\n",Var);
	  ic = Spx->Cdeb[Var];
	  icMx = ic + Spx->CNbTerm[Var];
	  while ( ic < icMx ) {
	    printf("NumeroDeContrainte[%d] = %d  Sortie = %e  ACol = %e\n",ic,Spx->NumeroDeContrainte[ic],Sortie[Spx->NumeroDeContrainte[ic]],Spx->ACol[ic]);
	    ic++;
	  }		
    Arret = OUI_SPX;
	}
}
if ( Arret == OUI_SPX ) {
  printf("RangDeLaMatriceFactorisee %d   NombreDeContraintes %d\n",Spx->RangDeLaMatriceFactorisee,Spx->NombreDeContraintes);
  exit(0);
}
printf("Fin verif erbmoins1  OK\n");
free( Buff );
free( Sortie );

SPX_VerifierLesVecteursDeTravail( Spx );

}
# endif
										 
return;
}
