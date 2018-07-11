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

   FONCTION: Stockage des coupes de sac a dos negligees.
	           Remarque: on peut faire qq chose de commun avec les G.
                
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

# define SIZE_ALLOC_COUPES 100 /* Nombre de coupes allouees */
# define SIZE_ALLOC_TERMES_COUPES (SIZE_ALLOC_COUPES*25)

void PNE_AllocCoupesKNegligees( PROBLEME_PNE * );
void PNE_AugmenterNombreDeCoupesKNegligees( PROBLEME_PNE * );
void PNE_AugmenterLaTailleDesCoupesKNegligees( PROBLEME_PNE * );

/*----------------------------------------------------------------------------*/

void PNE_AllocCoupesKNegligees( PROBLEME_PNE * Pne )
{
COUPES_K_NEGLIGEES * CoupesKNegligees;

CoupesKNegligees = (COUPES_K_NEGLIGEES *) malloc( sizeof( COUPES_K_NEGLIGEES ) );
if ( CoupesKNegligees == NULL ) return;

CoupesKNegligees->SecondMembre = (double *) malloc( SIZE_ALLOC_COUPES * sizeof( double ) );
if ( CoupesKNegligees->SecondMembre == NULL ) {
  return;
}
CoupesKNegligees->LaCoupeEstDansLePool = (char *) malloc( SIZE_ALLOC_COUPES * sizeof( char ) );
if ( CoupesKNegligees->LaCoupeEstDansLePool == NULL ) {
  free( CoupesKNegligees->SecondMembre );
	return;
}
CoupesKNegligees->First = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesKNegligees->First == NULL ) {
  free( CoupesKNegligees->SecondMembre );	
	free( CoupesKNegligees->LaCoupeEstDansLePool ); 
  return;
}
CoupesKNegligees->NbElements = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesKNegligees->NbElements == NULL ) {
  free( CoupesKNegligees->SecondMembre );	
	free( CoupesKNegligees->LaCoupeEstDansLePool ); 
	free( CoupesKNegligees->First ); 
  return;
}
CoupesKNegligees->NombreDeCoupesAllouees = SIZE_ALLOC_COUPES;

CoupesKNegligees->Colonne = (int *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( int ) );
if ( CoupesKNegligees->Colonne == NULL ) {
  free( CoupesKNegligees->SecondMembre );	
	free( CoupesKNegligees->LaCoupeEstDansLePool ); 
	free( CoupesKNegligees->First ); 
	free( CoupesKNegligees->NbElements ); 
  return;
}
CoupesKNegligees->Coefficient = (double *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( double ) );
if ( CoupesKNegligees->Coefficient == NULL ) {
  free( CoupesKNegligees->SecondMembre );	
	free( CoupesKNegligees->LaCoupeEstDansLePool ); 
	free( CoupesKNegligees->First ); 
	free( CoupesKNegligees->NbElements ); 
	free( CoupesKNegligees->Colonne ); 
  return;
}
CoupesKNegligees->TailleCoupesAllouee = SIZE_ALLOC_TERMES_COUPES;

CoupesKNegligees->IndexLibre = 0;
CoupesKNegligees->NombreDeCoupes = 0;
CoupesKNegligees->Full = NON_PNE;
Pne->CoupesKNegligees = CoupesKNegligees;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterNombreDeCoupesKNegligees( PROBLEME_PNE * Pne )
{
COUPES_K_NEGLIGEES * CoupesKNegligees; int Size; double * SecondMembre; char * LaCoupeEstDansLePool;
int * First; int * NbElements;

CoupesKNegligees = Pne->CoupesKNegligees;
Size = CoupesKNegligees->NombreDeCoupesAllouees + SIZE_ALLOC_COUPES;

SecondMembre = (double *) realloc( CoupesKNegligees->SecondMembre, Size * sizeof( double ) );
if ( SecondMembre == NULL ) {
	CoupesKNegligees->Full = OUI_PNE;
  return;
}
LaCoupeEstDansLePool = (char *) realloc( CoupesKNegligees->LaCoupeEstDansLePool, Size * sizeof( char ) );
if ( LaCoupeEstDansLePool == NULL ) {
  free( SecondMembre );
	CoupesKNegligees->Full = OUI_PNE;
	return;
}
First = (int *) realloc( CoupesKNegligees->First, Size * sizeof( int ) );
if ( First == NULL ) {
  free( SecondMembre );	
	free( LaCoupeEstDansLePool ); 
	CoupesKNegligees->Full = OUI_PNE;
  return;
}
NbElements = (int *) realloc( CoupesKNegligees->NbElements, Size * sizeof( int ) );
if ( NbElements == NULL ) {
  free( SecondMembre );	
	free( LaCoupeEstDansLePool ); 
	free( First ); 
	CoupesKNegligees->Full = OUI_PNE;
  return;
}
CoupesKNegligees->NombreDeCoupesAllouees = Size;
CoupesKNegligees->SecondMembre = SecondMembre;
CoupesKNegligees->LaCoupeEstDansLePool = LaCoupeEstDansLePool;
CoupesKNegligees->First = First;
CoupesKNegligees->NbElements = NbElements;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLaTailleDesCoupesKNegligees( PROBLEME_PNE * Pne )
{
COUPES_K_NEGLIGEES * CoupesKNegligees; int Size; int * Colonne; double * Coefficient;

CoupesKNegligees = Pne->CoupesKNegligees;
Size = CoupesKNegligees->TailleCoupesAllouee + SIZE_ALLOC_TERMES_COUPES;

Colonne = (int *) realloc( CoupesKNegligees->Colonne, Size * sizeof( int ) );
if ( Colonne == NULL ) {
	CoupesKNegligees->Full = OUI_PNE;
  return;
}
Coefficient = (double *) realloc( CoupesKNegligees->Coefficient, Size * sizeof( double ) );
if ( Coefficient == NULL ) {
	free( Colonne ); 
	CoupesKNegligees->Full = OUI_PNE;
  return;
}
CoupesKNegligees->TailleCoupesAllouee = Size;
CoupesKNegligees->Colonne = Colonne;
CoupesKNegligees->Coefficient = Coefficient;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_CreerUneCoupeKNegligee( PROBLEME_PNE * Pne, int NumeroDeCoupe )
{
COUPES_K_NEGLIGEES * CoupesKNegligees; int il; int NombreDeCoupes;
int Nb; int * First; int * NbElements; int * Colonne; int * Mdeb; int * NbTerm;
int * Nuvar; double * Coefficient; double * A; int il1; int ilMax; 

return;

Mdeb   = Pne->Coupes.Mdeb;
NbTerm = Pne->Coupes.NbTerm;
Nuvar  = Pne->Coupes.Nuvar;
A      = Pne->Coupes.A;

CoupesKNegligees = Pne->CoupesKNegligees;
if ( CoupesKNegligees == NULL ) {
  PNE_AllocCoupesKNegligees( Pne );
  CoupesKNegligees = Pne->CoupesKNegligees;
  if ( CoupesKNegligees == NULL ) return; /* Saturation memoire */
}

NombreDeCoupes = CoupesKNegligees->NombreDeCoupes;

/* Place suffisante */
if ( NombreDeCoupes >= CoupesKNegligees->NombreDeCoupesAllouees ) {
  /* On augmente la taille */
	PNE_AugmenterNombreDeCoupesKNegligees( Pne );
	if ( Pne->CoupesKNegligees == NULL ) return;
	if ( CoupesKNegligees->Full == OUI_PNE ) return;
}

/* Place suffisante */
il1 = CoupesKNegligees->IndexLibre;
while ( il1 + NbTerm[NumeroDeCoupe] + 1 >= CoupesKNegligees->TailleCoupesAllouee ) {
  /* On augmente la taille */
  PNE_AugmenterLaTailleDesCoupesKNegligees( Pne );
	if ( Pne->CoupesKNegligees == NULL ) return;	 
	if ( CoupesKNegligees->Full == OUI_PNE ) return;
}

First = CoupesKNegligees->First;
NbElements = CoupesKNegligees->NbElements;
Coefficient = CoupesKNegligees->Coefficient;
Colonne = CoupesKNegligees->Colonne;

First[NombreDeCoupes] = il1;
Nb = 0;
il = Mdeb[NumeroDeCoupe];
ilMax = il + NbTerm[NumeroDeCoupe];
while ( il < ilMax ) {
  if ( A[il] != 0 ) {
    Coefficient[il1] = A[il];
    Colonne[il1] = Nuvar[il];
	  il1++;
		Nb++;
	}
  il++;
}
NbElements[NombreDeCoupes] = Nb;

CoupesKNegligees->SecondMembre[NombreDeCoupes] = Pne->Coupes.B[NumeroDeCoupe];

# if TRACES == 1
  printf("Coupe de K negligee: %d\n",NombreDeCoupes);
  il = First[NombreDeCoupes];
  ilMax = il + NbElements[NombreDeCoupes];
	while ( il < ilMax ) {
    if ( Pne->TypeDeVariableTrav[Colonne[il]] == ENTIER ) printf("%e (%d I) ",Coefficient[il],Colonne[il]);
		else printf("%e (%d R) ",Coefficient[il],Colonne[il]);		
    il++;
	}
	printf("  SecondMembre %e\n",CoupesKNegligees->SecondMembre[NombreDeCoupes]);
# endif

CoupesKNegligees->IndexLibre = il1;
CoupesKNegligees->LaCoupeEstDansLePool[NombreDeCoupes] = NON_PNE;
CoupesKNegligees->NombreDeCoupes++;

return;
}


