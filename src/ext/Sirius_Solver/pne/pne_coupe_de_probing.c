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

   FONCTION: Creation des coupes de probing. Une coupe est cree lorsque
	           l'instanciation d'une variable rend une contrainte toujours
						 satisfaite.
                
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

# define TRACES 0

# define CONTRAINTE_NATIVE 1
# define COUPE_DE_PROBING 2

# define RMAX 1.e+5
# define ZERO_NOUVEAU_COEFF 1.e-15

# define SIZE_ALLOC_COUPES 100 /* Nombre de coupes de probing allouees */
# define SIZE_ALLOC_TERMES_COUPES (SIZE_ALLOC_COUPES*25)

void PNE_AllocCoupesDeProbing( PROBLEME_PNE * );
void PNE_AugmenterNombreDeCoupesDeProbing( PROBLEME_PNE * );
void PNE_AugmenterLaTailleDesCoupesDeProbing( PROBLEME_PNE * );

/*----------------------------------------------------------------------------*/

void PNE_AllocCoupesDeProbing( PROBLEME_PNE * Pne )
{
COUPES_DE_PROBING * CoupesDeProbing;

CoupesDeProbing = (COUPES_DE_PROBING *) malloc( sizeof( COUPES_DE_PROBING ) );
if ( CoupesDeProbing == NULL ) return;

CoupesDeProbing->SecondMembre = (double *) malloc( SIZE_ALLOC_COUPES * sizeof( double ) );
if ( CoupesDeProbing->SecondMembre == NULL ) {
  return;
}
CoupesDeProbing->LaCoupDeProbingEstDansLePool = (char *) malloc( SIZE_ALLOC_COUPES * sizeof( char ) );
if ( CoupesDeProbing->LaCoupDeProbingEstDansLePool == NULL ) {
  free( CoupesDeProbing->SecondMembre );
	return;
}
CoupesDeProbing->First = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesDeProbing->First == NULL ) {
  free( CoupesDeProbing->SecondMembre );	
	free( CoupesDeProbing->LaCoupDeProbingEstDansLePool ); 
  return;
}
CoupesDeProbing->NbElements = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesDeProbing->NbElements == NULL ) {
  free( CoupesDeProbing->SecondMembre );	
	free( CoupesDeProbing->LaCoupDeProbingEstDansLePool ); 
	free( CoupesDeProbing->First ); 
  return;
}
CoupesDeProbing->NombreDeCoupesDeProbingAlloue = SIZE_ALLOC_COUPES;

CoupesDeProbing->Colonne = (int *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( int ) );
if ( CoupesDeProbing->Colonne == NULL ) {
  free( CoupesDeProbing->SecondMembre );	
	free( CoupesDeProbing->LaCoupDeProbingEstDansLePool ); 
	free( CoupesDeProbing->First ); 
	free( CoupesDeProbing->NbElements ); 
  return;
}
CoupesDeProbing->Coefficient = (double *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( double ) );
if ( CoupesDeProbing->Coefficient == NULL ) {
  free( CoupesDeProbing->SecondMembre );	
	free( CoupesDeProbing->LaCoupDeProbingEstDansLePool ); 
	free( CoupesDeProbing->First ); 
	free( CoupesDeProbing->NbElements ); 
	free( CoupesDeProbing->Colonne ); 
  return;
}
CoupesDeProbing->TailleCoupesDeProbingAllouee = SIZE_ALLOC_TERMES_COUPES;

CoupesDeProbing->IndexLibre = 0;
CoupesDeProbing->NombreDeCoupesDeProbing = 0;
CoupesDeProbing->Full = NON_PNE;
Pne->CoupesDeProbing = CoupesDeProbing;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterNombreDeCoupesDeProbing( PROBLEME_PNE * Pne )
{
COUPES_DE_PROBING * CoupesDeProbing; int Size; double * SecondMembre; char * LaCoupDeProbingEstDansLePool;
int * First; int * NbElements;

CoupesDeProbing = Pne->CoupesDeProbing;
Size = CoupesDeProbing->NombreDeCoupesDeProbingAlloue + SIZE_ALLOC_COUPES;

SecondMembre = (double *) realloc( CoupesDeProbing->SecondMembre, Size * sizeof( double ) );
if ( SecondMembre == NULL ) {
	CoupesDeProbing->Full = OUI_PNE;
  return;
}
LaCoupDeProbingEstDansLePool = (char *) realloc( CoupesDeProbing->LaCoupDeProbingEstDansLePool, Size * sizeof( char ) );
if ( LaCoupDeProbingEstDansLePool == NULL ) {
  free( SecondMembre );
	CoupesDeProbing->Full = OUI_PNE;
	return;
}
First = (int *) realloc( CoupesDeProbing->First, Size * sizeof( int ) );
if ( First == NULL ) {
  free( SecondMembre );	
	free( LaCoupDeProbingEstDansLePool ); 
	CoupesDeProbing->Full = OUI_PNE;
  return;
}
NbElements = (int *) realloc( CoupesDeProbing->NbElements, Size * sizeof( int ) );
if ( NbElements == NULL ) {
  free( SecondMembre );	
	free( LaCoupDeProbingEstDansLePool ); 
	free( First ); 
	CoupesDeProbing->Full = OUI_PNE;
  return;
}
CoupesDeProbing->NombreDeCoupesDeProbingAlloue = Size;
CoupesDeProbing->SecondMembre = SecondMembre;
CoupesDeProbing->LaCoupDeProbingEstDansLePool = LaCoupDeProbingEstDansLePool;
CoupesDeProbing->First = First;
CoupesDeProbing->NbElements = NbElements;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLaTailleDesCoupesDeProbing( PROBLEME_PNE * Pne )
{
COUPES_DE_PROBING * CoupesDeProbing; int Size; int * Colonne; double * Coefficient;

CoupesDeProbing = Pne->CoupesDeProbing;
Size = CoupesDeProbing->TailleCoupesDeProbingAllouee + SIZE_ALLOC_TERMES_COUPES;

Colonne = (int *) realloc( CoupesDeProbing->Colonne, Size * sizeof( int ) );
if ( Colonne == NULL ) {
	CoupesDeProbing->Full = OUI_PNE;
  return;
}
Coefficient = (double *) realloc( CoupesDeProbing->Coefficient, Size * sizeof( double ) );
if ( Coefficient == NULL ) {
	free( Colonne ); 
	CoupesDeProbing->Full = OUI_PNE;
  return;
}
CoupesDeProbing->TailleCoupesDeProbingAllouee = Size;
CoupesDeProbing->Colonne = Colonne;
CoupesDeProbing->Coefficient = Coefficient;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_CreerUneCoupeDeProbing( PROBLEME_PNE * Pne, int Var1, double CoeffDeVar1, int ContrainteSource,
                                 char TypeContrainteSource, int ContrainteNative, double ValeurDeVar1 )
{
COUPES_DE_PROBING * CoupesDeProbing; int il; int NombreDeCoupesDeProbing;
int Nb; int * First; int * NbElements; int * Colonne; int * Mdeb; int * NbTerm;
int * Nuvar; double * Coefficient; double * A; int il1; int ilMax; double Amin;
int ilVar1; double CoeffTest; int NbTrm; double Sec;

CoupesDeProbing = Pne->CoupesDeProbing;
if ( CoupesDeProbing == NULL ) {
  PNE_AllocCoupesDeProbing( Pne );
  CoupesDeProbing = Pne->CoupesDeProbing;
  if ( CoupesDeProbing == NULL ) return; /* Saturation memoire */
}

NombreDeCoupesDeProbing = CoupesDeProbing->NombreDeCoupesDeProbing;
First = CoupesDeProbing->First;
NbElements = CoupesDeProbing->NbElements;
Coefficient = CoupesDeProbing->Coefficient;
Colonne = CoupesDeProbing->Colonne;

/* Place suffisante */
if ( NombreDeCoupesDeProbing >= CoupesDeProbing->NombreDeCoupesDeProbingAlloue ) {
  /* On augmente la taille */
	PNE_AugmenterNombreDeCoupesDeProbing( Pne );
	if ( Pne->CoupesDeProbing == NULL ) return;
	if ( CoupesDeProbing->Full == OUI_PNE ) return;
  First = CoupesDeProbing->First;
  NbElements = CoupesDeProbing->NbElements;	
}

if ( TypeContrainteSource == CONTRAINTE_NATIVE ) NbTerm = Pne->NbTermTrav;
else if ( TypeContrainteSource == COUPE_DE_PROBING ) NbTerm = CoupesDeProbing->NbElements; 
else return;

NbTrm = NbTerm[ContrainteSource];
/* Place suffisante */
il1 = CoupesDeProbing->IndexLibre;
while ( il1 + NbTrm + 1 >= CoupesDeProbing->TailleCoupesDeProbingAllouee ) {
  /* On augmente la taille */
  PNE_AugmenterLaTailleDesCoupesDeProbing( Pne );	
	if ( Pne->CoupesDeProbing == NULL ) return;	 
	if ( CoupesDeProbing->Full == OUI_PNE ) return;
  Coefficient = CoupesDeProbing->Coefficient;
  Colonne = CoupesDeProbing->Colonne;	
}

if ( TypeContrainteSource == CONTRAINTE_NATIVE ) {
  Mdeb = Pne->MdebTrav;
  Nuvar = Pne->NuvarTrav;
  A = Pne->ATrav;
	Sec = Pne->BTrav[ContrainteSource];
}
else {
  Mdeb = CoupesDeProbing->First; 
  Nuvar = CoupesDeProbing->Colonne; 
  A = CoupesDeProbing->Coefficient;
	Sec = CoupesDeProbing->SecondMembre[ContrainteSource];
}

First[NombreDeCoupesDeProbing] = il1;
Nb = 0;
il = Mdeb[ContrainteSource];
ilMax = il + NbTerm[ContrainteSource];
Amin = LINFINI_PNE;
ilVar1 = -1;
CoeffTest = 0;
while ( il < ilMax ) {
  if ( A[il] != 0 ) {
	  if ( Nuvar[il] == Var1 ) {
		  ilVar1 = il1;
		}
		if ( fabs( A[il] ) < Amin ) Amin = fabs( A[il] );
    Coefficient[il1] = A[il];
    Colonne[il1] = Nuvar[il];
	  il1++;
		Nb++;
	}
  il++;
}
NbElements[NombreDeCoupesDeProbing] = Nb;

CoupesDeProbing->SecondMembre[NombreDeCoupesDeProbing] = Sec;

if ( ilVar1 >= 0 ) {	
  /* On modifie le coefficient de la variable */
  if ( ValeurDeVar1 == 1 ) {
    /* La variable Var1 a ete instanciee a 1 dans le probing */
    Coefficient[ilVar1] += CoeffDeVar1;
		if ( fabs( Coefficient[ilVar1] ) < ZERO_NOUVEAU_COEFF ) Coefficient[ilVar1] = 0.0;
  }
  else {
    /* La variable Var1 a ete instanciee a 0 dans le probing */	
    Coefficient[ilVar1] -= CoeffDeVar1;
		if ( fabs( Coefficient[ilVar1] ) < ZERO_NOUVEAU_COEFF ) Coefficient[ilVar1] = 0.0;
    CoupesDeProbing->SecondMembre[NombreDeCoupesDeProbing] -= CoeffDeVar1;
  }
	CoeffTest = Coefficient[ilVar1];
	if ( CoeffTest == 0.0 ) {
    /* On enleve la variable */
    Coefficient[ilVar1] = Coefficient[il1-1];
    Colonne[ilVar1] = Colonne[il1-1];
    NbElements[NombreDeCoupesDeProbing]--;
		if ( NbElements[NombreDeCoupesDeProbing] <= 0 ) return;
	}
}
else {
  /* On ajoute la variable */
  Colonne[il1] = Var1;
  if ( ValeurDeVar1 == 1 ) {
    /* La variable Var1 a ete instanciee a 1 dans le probing */
    Coefficient[il1] = CoeffDeVar1;
  }
  else {
    /* La variable Var1 a ete instanciee a 0 dans le probing */	
    Coefficient[il1] = -CoeffDeVar1;
    CoupesDeProbing->SecondMembre[NombreDeCoupesDeProbing] -= CoeffDeVar1;
  }
	CoeffTest = Coefficient[il1];	
  NbElements[NombreDeCoupesDeProbing]++;
  il1++;
}

/* Problemes de conditionnement ? */
if ( CoeffTest != 0.0 ) {
  if ( Pne->PlusGrandTerme / Pne->PlusPetitTerme < 1.e+5 ) {
    if ( fabs( CoeffTest ) > RMAX * Amin ) {
		  # if TRACES == 1
	      printf("Coupe de probing refusee a cause du conditionnement  Amin = %e\n",Amin);
 	    # endif
			return;
    }
  }
  else if ( fabs( CoeffTest ) > 1.01 * Pne->PlusGrandTerme || fabs( CoeffTest ) < 0.99 * Pne->PlusPetitTerme ) {	  
		# if TRACES == 1
	    printf("Coupe de probing refusee a cause du conditionnement CoeffTest = %e PlusPetitTerme = %e PlusGrandTerme = %e\n",
	            CoeffTest,Pne->PlusPetitTerme,Pne->PlusGrandTerme);						  
 	  # endif
		return;
  }
}
else if ( NbElements[NombreDeCoupesDeProbing] <= 1 ) {
  /* 1 seul element et en plus il est nul ! */
	return;
}
if ( fabs( CoeffTest ) < 0.1 ) return;

# if TRACES == 1
  printf("Coupe de probing: %d\n",NombreDeCoupesDeProbing);
  il = First[NombreDeCoupesDeProbing];
  ilMax = il + NbElements[NombreDeCoupesDeProbing];
	while ( il < ilMax ) {
    if ( Pne->TypeDeVariableTrav[Colonne[il]] == ENTIER ) printf("%e (%d I) ",Coefficient[il],Colonne[il]);
		else printf("%e (%d R) ",Coefficient[il],Colonne[il]);		
    il++;
	}
	printf("  SecondMembre %e\n",CoupesDeProbing->SecondMembre[NombreDeCoupesDeProbing]);
# endif

/* Si la contrainte source est deja une coupe de probing on invalide l'ancienne coupe de probing */
if ( TypeContrainteSource == COUPE_DE_PROBING ){
  /* Si la contrainte source est deja une coupe de probing on invalide l'ancienne coupe de probing */
  First[ContrainteSource] = -1;
}

Pne->ProbingOuNodePresolve->NumeroDeCoupeDeProbing[ContrainteNative] = NombreDeCoupesDeProbing;

CoupesDeProbing->IndexLibre = il1;
CoupesDeProbing->LaCoupDeProbingEstDansLePool[NombreDeCoupesDeProbing] = NON_PNE;
CoupesDeProbing->NombreDeCoupesDeProbing++;

return;
}
 
/*----------------------------------------------------------------------------------------------------*/
/* Si des contraintes son devenues non activables on cree les coupes de probing correspondantes */
void PNE_CreerLesCoupesDeProbing( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int NombreDeContraintes; double * Bmax; double * B; double CoeffCandidat; int Cnt; double * BmaxSv;
int * NumeroDeCoupeDeProbing; int * First; int * NbElements; int * Colonne; double * SecondMembre;
double * Coefficient; int NumCoupe; COUPES_DE_PROBING * CoupesDeProbing; char * BorneInfConnueSv;
double * ValeurDeBorneInfSv; double * ValeurDeBorneSupSv; int il; int Var; double A; int NbCntCoupesDeProbing;
char BrnInfConnueSv; int ilMax; double SmaxSv; double Sec; char TypeContrainteSource;
int NumContrainteSource; double Smax; char BrnInfConnue; char * BorneInfConnue; double * ValeurDeBorneInf;
double * ValeurDeBorneSup; int i; int * NumCntCoupesDeProbing; char * FlagCntCoupesDeProbing;

# if UTILISER_LES_COUPES_DE_PROBING == NON_PNE
  return;
# endif

if ( ProbingOuNodePresolve->VariableInstanciee < 0 ) return;

if ( ProbingOuNodePresolve->NbCntCoupesDeProbing <= 0 ) return;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
B = Pne->BTrav;
Bmax = ProbingOuNodePresolve->Bmax;
BmaxSv = ProbingOuNodePresolve->BmaxSv;
NumeroDeCoupeDeProbing = ProbingOuNodePresolve->NumeroDeCoupeDeProbing;

CoupesDeProbing = Pne->CoupesDeProbing;

BorneInfConnueSv = ProbingOuNodePresolve->BorneInfConnueSv;
ValeurDeBorneInfSv = ProbingOuNodePresolve->ValeurDeBorneInfSv;
ValeurDeBorneSupSv = ProbingOuNodePresolve->ValeurDeBorneSupSv;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;

NumCntCoupesDeProbing = ProbingOuNodePresolve->NumCntCoupesDeProbing;
FlagCntCoupesDeProbing = ProbingOuNodePresolve->FlagCntCoupesDeProbing;

NbCntCoupesDeProbing = ProbingOuNodePresolve->NbCntCoupesDeProbing;
ProbingOuNodePresolve->NbCntCoupesDeProbing = 0;

for ( i = 0 ; i < NbCntCoupesDeProbing ; i++ ) {

  Cnt = NumCntCoupesDeProbing[i];
	FlagCntCoupesDeProbing[Cnt] = 0;

  SmaxSv = BmaxSv[Cnt];
  Smax = Bmax[Cnt];	
	Sec = B[Cnt];
	TypeContrainteSource = CONTRAINTE_NATIVE;
	NumContrainteSource = Cnt;
		
	/* Si la contrainte est deja reliee a une coupe de probing on regarde si elle est violee puis relaxee */	
  if ( NumeroDeCoupeDeProbing[Cnt] >= 0 && CoupesDeProbing != NULL ) {
    /* Car a pu etre augmente */
    First = CoupesDeProbing->First;
    NbElements = CoupesDeProbing->NbElements;
    SecondMembre = CoupesDeProbing->SecondMembre;
    Colonne = CoupesDeProbing->Colonne;
    Coefficient = CoupesDeProbing->Coefficient;	
	  NumCoupe = NumeroDeCoupeDeProbing[Cnt];
	  TypeContrainteSource = COUPE_DE_PROBING;
	  NumContrainteSource = NumCoupe;
    il = First[NumCoupe];
		if ( il < 0 ) goto TestSeuil;
	  ilMax = il + NbElements[NumCoupe];
	  SmaxSv = 0;
		Smax = 0;
	  while ( il < ilMax ) {
      Var = Colonne[il];		
	    A = Coefficient[il];
      BrnInfConnueSv = BorneInfConnueSv[Var];
      if ( BrnInfConnueSv == FIXE_AU_DEPART || BrnInfConnueSv == FIXATION_SUR_BORNE_INF ||
	         BrnInfConnueSv == FIXATION_SUR_BORNE_SUP || BrnInfConnueSv == FIXATION_A_UNE_VALEUR ) {
	      SmaxSv += A * ValeurDeBorneInfSv[Var];						
	    }
			else {
	      if ( A > 0.0 ) SmaxSv += A * ValeurDeBorneSupSv[Var];			
	      else SmaxSv += A * ValeurDeBorneInfSv[Var];
			}
      BrnInfConnue = BorneInfConnue[Var];
      if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	         BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
	      Smax += A * ValeurDeBorneInf[Var];						
	    }
			else {
	      if ( A > 0.0 ) Smax += A * ValeurDeBorneSup[Var];			
	      else Smax += A * ValeurDeBorneInf[Var];
			}
			
		  il++;
	  }
		Sec = SecondMembre[NumCoupe];
  }
  TestSeuil:
	if ( SmaxSv <= Sec ) continue;	
	if ( Smax >= Sec ) continue;
	
  CoeffCandidat = Sec - Smax;
	if ( CoeffCandidat < SEUIL_DADMISSIBILITE /*1.e-2*/ ) continue;
	
  PNE_CreerUneCoupeDeProbing( Pne, ProbingOuNodePresolve->VariableInstanciee, CoeffCandidat, NumContrainteSource,
	                            TypeContrainteSource, Cnt, Pne->ProbingOuNodePresolve->ValeurDeLaVariableInstanciee );	
}

/*
if ( Pne->CoupesDeProbing != NULL ) printf("CoupesDeProbing->NombreDeCoupesDeProbing %d\n",Pne->CoupesDeProbing->NombreDeCoupesDeProbing);
*/

return;
}

