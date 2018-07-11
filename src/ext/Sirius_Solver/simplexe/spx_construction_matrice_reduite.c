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

   FONCTION: Construction du stockage de la matrice reduite par colonne
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define TRACES 0

/*----------------------------------------------------------------------------*/

void SPX_ConstructionDeLaMatriceReduite( PROBLEME_SPX * Spx )						    
{
int Var; int NbTrmCol; int * Cdeb; int * CNbTerm; int rr; int * NumeroDeContrainte;
double * ACol; int ic; int icMx; int icNew; int * OrdreLigneDeLaBaseFactorisee;
int RangDeLaMatriceFactorisee; int NbTermesTotal; int * CdebProblemeReduit;
int * CNbTermProblemeReduit; int * IndicesDeLigneDesTermesDuProblemeReduit;
double * ValeurDesTermesDesColonnesDuProblemeReduit; int NbH; int NbRaz;

Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;   
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol = Spx->ACol;

RangDeLaMatriceFactorisee = Spx->RangDeLaMatriceFactorisee;

OrdreLigneDeLaBaseFactorisee = Spx->OrdreLigneDeLaBaseFactorisee;
	
CdebProblemeReduit = Spx->CdebProblemeReduit;
CNbTermProblemeReduit = Spx->CNbTermProblemeReduit;
ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;
IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;

/* Construction de la matrice du probleme reduit stockee par colonne */
icNew = 0;
NbTermesTotal = 0;
NbH = 0;
NbRaz = 0;
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  CdebProblemeReduit[Var] = icNew;		
	NbTrmCol = 0;	  
	ic = Cdeb[Var];
	icMx = ic + CNbTerm[Var];
  if ( NbTermesTotal + CNbTerm[Var] >= Spx->NbElementsAllouesPourLeProblemeReduit ) {
    Spx->NbElementsAllouesPourLeProblemeReduit += CNbTerm[Var] * 10;
    Spx->ValeurDesTermesDesColonnesDuProblemeReduit = (double *) realloc( Spx->ValeurDesTermesDesColonnesDuProblemeReduit, Spx->NbElementsAllouesPourLeProblemeReduit * sizeof( double ) );
    Spx->IndicesDeLigneDesTermesDuProblemeReduit = (int *) realloc( Spx->IndicesDeLigneDesTermesDuProblemeReduit, Spx->NbElementsAllouesPourLeProblemeReduit * sizeof( int ) );
    if ( Spx->ValeurDesTermesDesColonnesDuProblemeReduit == NULL || Spx->IndicesDeLigneDesTermesDuProblemeReduit == NULL ) {
      printf("Simplexe, sous-programme SPX_FactoriserLaBase: \n");
      printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
      Spx->AnomalieDetectee = OUI_SPX;
      longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
    }
    ValeurDesTermesDesColonnesDuProblemeReduit = Spx->ValeurDesTermesDesColonnesDuProblemeReduit;
    IndicesDeLigneDesTermesDuProblemeReduit = Spx->IndicesDeLigneDesTermesDuProblemeReduit;
	}
		
	while ( ic < icMx ) {
		rr = OrdreLigneDeLaBaseFactorisee[NumeroDeContrainte[ic]];
		if ( rr < RangDeLaMatriceFactorisee ) {			  						
		  ValeurDesTermesDesColonnesDuProblemeReduit[icNew] = ACol[ic];				
		  IndicesDeLigneDesTermesDuProblemeReduit[icNew] = rr;  
		  NbTrmCol++;
			icNew++;
		}
    ic++;
	}
	CNbTermProblemeReduit[Var] = NbTrmCol;

  /* Pour la partie hors systeme reduit, on remet tous les couts a leur valeur d'origine */
	if ( NbTrmCol == 0 && 0 ) {
	  if ( Spx->C[Var] != Spx->Csv[Var] ) NbRaz++;
	  Spx->C[Var] = Spx->Csv[Var];
    Spx->CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;
		NbH++;
  }
	
	NbTermesTotal += NbTrmCol;
	
}

# if TRACES == 1
  printf("\n Nombre de variables hors du probleme reduit %d sur %d  restent %d / nombre de couts remis a zero: %d\n",
             NbH,Spx->NombreDeVariables,Spx->NombreDeVariables-NbH,NbRaz);
  printf("\n Nombre de termes de la matrice reduite des contraintes %d\n",NbTermesTotal);						 
# endif

return;

# ifdef COMPILER_LA_SUITE

/* Test: on regarde si on peut fixer des variables */
char * SensContrainte; int Cnt; int il; char Signe; int NbFix; int NbIneg; double * XminSv; double * XmaxSv; int NbRlx; double Smin; double Smax; int ilMax; int NbForcing; char Rebouclage;
SensContrainte = (char *) malloc( Spx->NombreDeContraintes * sizeof( char ) );
XminSv = (double *) malloc( Spx->NombreDeVariables * sizeof( double ) );
XmaxSv = (double *) malloc( Spx->NombreDeVariables * sizeof( double ) );
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  XminSv[Var] = Spx->Xmin[Var];
  XmaxSv[Var] = Spx->Xmax[Var];
}

NbIneg = 0;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  il = Spx->Mdeb[Cnt] + Spx->NbTerm[Cnt] - 1;
  Var = Spx->Indcol[il];
	if ( Spx->OrigineDeLaVariable[Var] == ECART ) { SensContrainte[Cnt] = '<'; NbIneg++; }
	else SensContrainte[Cnt] = '=';
}

NbFix = 0;
for ( Var = 0 ; Var < Spx->NombreDeVariablesNatives ; Var++ ) {
  
  if ( Spx->C[Var] > 0 ) Signe = '+';
	else if ( Spx->C[Var] < 0 ) Signe = '-';
	else Signe = '0';
	
  ic = CdebProblemeReduit[Var];
  icMx = ic + CNbTermProblemeReduit[Var];
	while ( ic < icMx ) {
	  rr = IndicesDeLigneDesTermesDuProblemeReduit[ic];
		Cnt = Spx->LigneDeLaBaseFactorisee[rr];
		if ( SensContrainte[Cnt] == '=' ) { Signe = '!'; break; }
		else {
      if ( ValeurDesTermesDesColonnesDuProblemeReduit[ic] > 0 ) {
        if ( Signe == '-' ) { Signe = '!'; break; }
				else if ( Signe == '0' ) Signe = '+';
			}
			else if ( ValeurDesTermesDesColonnesDuProblemeReduit[ic] < 0 ) {
        if ( Signe == '+' ) { Signe = '!'; break; }
				else if ( Signe == '0' ) Signe = '-';			
      }
		}
    ic++;
	}
	if ( Signe == '!' ) continue;
	else if ( Signe == '+' ) {
	 Spx->Xmax[Var] = Spx->Xmin[Var];
  }
	else if ( Signe == '-' ){
	  Spx->Xmin[Var] = Spx->Xmax[Var];
	}
	else {
	 Spx->Xmax[Var] = Spx->Xmin[Var];
	}
	NbFix++;
}

NbRlx = 0;
NbForcing = 0;
REBOUCLAGE:
Rebouclage = NON_SPX;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  if ( SensContrainte[Cnt] == 'R' ) continue;
  il = Spx->Mdeb[Cnt];
	ilMax = il + Spx->NbTerm[Cnt];
	Smin = 0;
	Smax = 0;
	while ( il < ilMax ) {
    Var = Spx->Indcol[il];
		if ( Spx->A[il] > 0 ) {
		  Smin += Spx->A[il] * Spx->Xmin[Var];
			Smax += Spx->A[il] * Spx->Xmax[Var];
		}
		else {
		  Smin += Spx->A[il] * Spx->Xmax[Var];
			Smax += Spx->A[il] * Spx->Xmin[Var];
		}
    il++;
	}
	if ( SensContrainte[Cnt] == '<' ) {
    if ( Smax <= Spx->B[Cnt] ) {
      NbRlx++;
			SensContrainte[Cnt] = 'R';
		}
		else if ( Smin >= Spx->B[Cnt] ) {
      NbForcing++;
			SensContrainte[Cnt] = 'R';
			Rebouclage = OUI_SPX;
      il = Spx->Mdeb[Cnt];
	    ilMax = il + Spx->NbTerm[Cnt];
	    while ( il < ilMax ) {
        Var = Spx->Indcol[il];
				if ( Spx->Xmax[Var] != Spx->Xmin[Var] ) NbFix++;
		    if ( Spx->A[il] > 0 ) Spx->Xmax[Var] = Spx->Xmin[Var];
		    else Spx->Xmin[Var] = Spx->Xmax[Var];
	      il++;			
			}
		}
	}
	else {
    if ( fabs( Smax - Spx->B[Cnt] ) < 1.e-8 ) {
      NbForcing++;
			SensContrainte[Cnt] = 'R';
			Rebouclage = OUI_SPX;
      il = Spx->Mdeb[Cnt];
	    ilMax = il + Spx->NbTerm[Cnt];
	    while ( il < ilMax ) {
        Var = Spx->Indcol[il];
				if ( Spx->Xmax[Var] != Spx->Xmin[Var] ) NbFix++;
		    if ( Spx->A[il] > 0 ) Spx->Xmin[Var] = Spx->Xmax[Var];
		    else Spx->Xmax[Var] = Spx->Xmin[Var];
	      il++;			
			}
			
		}
    else if ( fabs( Smin - Spx->B[Cnt] ) < 1.e-8 ) {
      NbForcing++;
			SensContrainte[Cnt] = 'R';
			Rebouclage = OUI_SPX;
      il = Spx->Mdeb[Cnt];
	    ilMax = il + Spx->NbTerm[Cnt];
	    while ( il < ilMax ) {
        Var = Spx->Indcol[il];
				if ( Spx->Xmax[Var] != Spx->Xmin[Var] ) NbFix++;
		    if ( Spx->A[il] > 0 ) Spx->Xmax[Var] = Spx->Xmin[Var];
		    else Spx->Xmin[Var] = Spx->Xmax[Var];
	      il++;			
			}			
		}
	}
}
if ( Rebouclage == OUI_SPX ) goto REBOUCLAGE;

free( SensContrainte );
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  Spx->Xmin[Var] = XminSv[Var];
  Spx->Xmax[Var] = XmaxSv[Var];
}
free( XminSv );
free( XmaxSv );

printf("NbRlx %d NbForcing %d\n",NbRlx,NbForcing);

printf("NbFix %d  sur %d  restent %d\n",NbFix,Spx->NombreDeContraintes,Spx->NombreDeContraintes-NbFix);
printf("Nombre de contraintes d'inegalite %d\n",NbIneg);
/* Fin test */

# endif

return;
}

