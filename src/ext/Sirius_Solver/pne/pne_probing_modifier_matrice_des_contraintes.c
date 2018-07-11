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

   FONCTION: On modifie la matrice des contraintes dans le cas ou le
	           probing a conduit a creer des coupes de probing.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif
 																									
/*----------------------------------------------------------------------------*/

void PNE_ProbingModifierLaMatriceDesContraintes( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int il; int Cnt; int NumCoupe; int il0; int il0Max; int NombreDeContraintes; int Nb;
int * Mdeb0; int * NbTerm0; int * Nuvar0; int * Mdeb; int * NbTerm; int * Nuvar; int * First;
int * NbElements; int * Colonne; int * NumeroDeCoupeDeProbing; double Amn; double Amx; int ilMax;
double * B0; double * A0; double * B; double * A; double * SecondMembre; double * Coefficient;
char * Sens0; char * SensContrainte; COUPES_DE_PROBING * CoupesDeProbing; double a;
int NbCmod;

CoupesDeProbing = Pne->CoupesDeProbing;
if ( CoupesDeProbing == NULL ) return;
if ( CoupesDeProbing->NombreDeCoupesDeProbing <= 0 ) return;
 
NombreDeContraintes = Pne->NombreDeContraintesTrav;
Mdeb0 = Pne->MdebTrav;
NbTerm0 = Pne->NbTermTrav;
B0 = Pne->BTrav;
Sens0 = Pne->SensContrainteTrav;
Nuvar0 = Pne->NuvarTrav;
A0 = Pne->ATrav;

if ( CoupesDeProbing->NombreDeCoupesDeProbing <= 0 ) return;

First = CoupesDeProbing->First;
NbElements = CoupesDeProbing->NbElements;
SecondMembre = CoupesDeProbing->SecondMembre;
Colonne = CoupesDeProbing->Colonne;
Coefficient = CoupesDeProbing->Coefficient;

NumeroDeCoupeDeProbing = ProbingOuNodePresolve->NumeroDeCoupeDeProbing;

Mdeb = (int *) malloc( NombreDeContraintes * sizeof( int ) );
NbTerm = (int *) malloc( NombreDeContraintes * sizeof( int ) );
B = (double *) malloc( NombreDeContraintes * sizeof( double ) );
SensContrainte = (char *) malloc( NombreDeContraintes * sizeof( char ) );

/* Determination de la nouvelle taille de la matrice des contraintes */
il = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
	if ( NumeroDeCoupeDeProbing[Cnt] < 0 ) il += NbTerm0[Cnt];
	else il += NbElements[NumeroDeCoupeDeProbing[Cnt]];
	il += MARGE_EN_FIN_DE_CONTRAINTE;
}

Nuvar = (int *) malloc( il * sizeof( int ) );
A = (double *) malloc( il * sizeof( double ) );

Pne->TailleAlloueePourLaMatriceDesContraintes = il;

if ( Mdeb           == NULL || NbTerm == NULL || B == NULL || 
     SensContrainte == NULL || Nuvar  == NULL || A == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_ProbingModifierLaMatriceDesContraintes \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

/* Reconstruction de la matrice des contraintes */
NbCmod = 0;
il = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  Mdeb[Cnt] = il;
	SensContrainte[Cnt] = Sens0[Cnt]; 
	Nb = 0;
	if ( NumeroDeCoupeDeProbing[Cnt] < 0 ) {
	  B[Cnt] = B0[Cnt];
    il0    = Mdeb0[Cnt];
    il0Max = il0 + NbTerm0[Cnt];				
    while ( il0 < il0Max) {
      if ( A0[il0] != 0.0 ) {     
        A    [il] = A0[il0];			
        Nuvar[il] = Nuvar0[il0];
			  Nb++;
        il++;  
      }
      il0++;
    }
	}
	else {
    /* On remplace la contrainte par la coupe de probing */
    NumCoupe = NumeroDeCoupeDeProbing[Cnt];
		NbCmod++;
	  B[Cnt] = SecondMembre[NumCoupe];
    il0 = First[NumCoupe];
		if ( il0 < 0 ) {
		  printf("Bug dans PNE_ProbingModifierLaMatriceDesContraintes\n");
			exit(0);
		}
	  il0Max = il0 + NbElements[NumCoupe];
	  while ( il0 < il0Max ) {
	    if ( Coefficient[il0] != 0.0 ) {
        A    [il] = Coefficient[il0];				
        Nuvar[il] = Colonne[il0];				
			  Nb++;
        il++;  
      }
      il0++;
	  }						
	}
	
  for ( il0 = 0 ; il0 < MARGE_EN_FIN_DE_CONTRAINTE ; il0++ ) {
    A[il] = 0.0;
    il++; 
  }
	
	NbTerm[Cnt] = Nb;	
		
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbCmod > 0 ) printf("Contraints matrix was modified %d time(s) due to probing constraints\n",NbCmod);
}

Pne->ChainageTransposeeExploitable = NON_PNE;

Amn =  LINFINI_PNE;
Amx = -LINFINI_PNE;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    a = fabs ( A[il] ); 
    if ( a > 0 ) {
      if ( a < Amn ) Amn = a;
      else if ( a > Amx ) Amx = a; 
    }
    il++;
  }
}
Pne->PlusGrandTerme = Amx;  
Pne->PlusPetitTerme = Amn;

Pne->MdebTrav = Mdeb;
Pne->NbTermTrav = NbTerm;
Pne->BTrav = B;
Pne->SensContrainteTrav = SensContrainte;
Pne->NuvarTrav = Nuvar;
Pne->ATrav = A;

free( Mdeb0 );
free( NbTerm0 );
free( B0 );
free( Sens0 );
free( Nuvar0 );
free( A0 );

free( Pne->CsuiTrav );
free( Pne->NumContrainteTrav );
Pne->CsuiTrav          = (int *) malloc( Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( int ) );
Pne->NumContrainteTrav = (int *) malloc( Pne->TailleAlloueePourLaMatriceDesContraintes * sizeof( int ) );

/* Liberation des coupes de probing */
if ( CoupesDeProbing != NULL ) {
  free( CoupesDeProbing->SecondMembre );
  CoupesDeProbing->SecondMembre = NULL;	
  free( CoupesDeProbing->First );
  CoupesDeProbing->First = NULL;	
  free( CoupesDeProbing->NbElements );
  CoupesDeProbing->NbElements = NULL;	
  free( CoupesDeProbing->Colonne );
  CoupesDeProbing->Colonne = NULL;	
  free( CoupesDeProbing->Coefficient );
  CoupesDeProbing->Coefficient = NULL;	
  free( CoupesDeProbing->LaCoupDeProbingEstDansLePool );
  CoupesDeProbing->LaCoupDeProbingEstDansLePool = NULL;	
  free( Pne->CoupesDeProbing );
	Pne->CoupesDeProbing = NULL;
}

return;
}
