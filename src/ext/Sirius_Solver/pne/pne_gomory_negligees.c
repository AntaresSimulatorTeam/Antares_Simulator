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

   FONCTION: Stockage des coupes de Gomory negligees.
	           Remarque: on peut faire qq chose de commun avec les K.
                
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

void PNE_AllocCoupesGNegligees( PROBLEME_PNE * );
void PNE_AugmenterNombreDeCoupesGNegligees( PROBLEME_PNE * );
void PNE_AugmenterLaTailleDesCoupesGNegligees( PROBLEME_PNE * );

/*----------------------------------------------------------------------------*/

void PNE_AllocCoupesGNegligees( PROBLEME_PNE * Pne )
{
COUPES_G_NEGLIGEES * CoupesGNegligees;

CoupesGNegligees = (COUPES_G_NEGLIGEES *) malloc( sizeof( COUPES_G_NEGLIGEES ) );
if ( CoupesGNegligees == NULL ) return;

CoupesGNegligees->SecondMembre = (double *) malloc( SIZE_ALLOC_COUPES * sizeof( double ) );
if ( CoupesGNegligees->SecondMembre == NULL ) {
  return;
}
CoupesGNegligees->LaCoupeEstDansLePool = (char *) malloc( SIZE_ALLOC_COUPES * sizeof( char ) );
if ( CoupesGNegligees->LaCoupeEstDansLePool == NULL ) {
  free( CoupesGNegligees->SecondMembre );
	return;
}
CoupesGNegligees->First = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesGNegligees->First == NULL ) {
  free( CoupesGNegligees->SecondMembre );	
	free( CoupesGNegligees->LaCoupeEstDansLePool ); 
  return;
}
CoupesGNegligees->NbElements = (int *) malloc( SIZE_ALLOC_COUPES * sizeof( int ) );
if ( CoupesGNegligees->NbElements == NULL ) {
  free( CoupesGNegligees->SecondMembre );	
	free( CoupesGNegligees->LaCoupeEstDansLePool ); 
	free( CoupesGNegligees->First ); 
  return;
}
CoupesGNegligees->NombreDeCoupesAllouees = SIZE_ALLOC_COUPES;

CoupesGNegligees->Colonne = (int *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( int ) );
if ( CoupesGNegligees->Colonne == NULL ) {
  free( CoupesGNegligees->SecondMembre );	
	free( CoupesGNegligees->LaCoupeEstDansLePool ); 
	free( CoupesGNegligees->First ); 
	free( CoupesGNegligees->NbElements ); 
  return;
}
CoupesGNegligees->Coefficient = (double *) malloc( SIZE_ALLOC_TERMES_COUPES * sizeof( double ) );
if ( CoupesGNegligees->Coefficient == NULL ) {
  free( CoupesGNegligees->SecondMembre );	
	free( CoupesGNegligees->LaCoupeEstDansLePool ); 
	free( CoupesGNegligees->First ); 
	free( CoupesGNegligees->NbElements ); 
	free( CoupesGNegligees->Colonne ); 
  return;
}
CoupesGNegligees->TailleCoupesAllouee = SIZE_ALLOC_TERMES_COUPES;

CoupesGNegligees->IndexLibre = 0;
CoupesGNegligees->NombreDeCoupes = 0;
CoupesGNegligees->Full = NON_PNE;
Pne->CoupesGNegligees = CoupesGNegligees;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterNombreDeCoupesGNegligees( PROBLEME_PNE * Pne )
{
COUPES_G_NEGLIGEES * CoupesGNegligees; int Size; double * SecondMembre; char * LaCoupeEstDansLePool;
int * First; int * NbElements;

CoupesGNegligees = Pne->CoupesGNegligees;
Size = CoupesGNegligees->NombreDeCoupesAllouees + SIZE_ALLOC_COUPES;

SecondMembre = (double *) realloc( CoupesGNegligees->SecondMembre, Size * sizeof( double ) );
if ( SecondMembre == NULL ) {
	CoupesGNegligees->Full = OUI_PNE;
  return;
}
LaCoupeEstDansLePool = (char *) realloc( CoupesGNegligees->LaCoupeEstDansLePool, Size * sizeof( char ) );
if ( LaCoupeEstDansLePool == NULL ) {
  free( SecondMembre );
	CoupesGNegligees->Full = OUI_PNE;
	return;
}
First = (int *) realloc( CoupesGNegligees->First, Size * sizeof( int ) );
if ( First == NULL ) {
  free( SecondMembre );	
	free( LaCoupeEstDansLePool ); 
	CoupesGNegligees->Full = OUI_PNE;
  return;
}
NbElements = (int *) realloc( CoupesGNegligees->NbElements, Size * sizeof( int ) );
if ( NbElements == NULL ) {
  free( SecondMembre );	
	free( LaCoupeEstDansLePool ); 
	free( First ); 
	CoupesGNegligees->Full = OUI_PNE;
  return;
}
CoupesGNegligees->NombreDeCoupesAllouees = Size;
CoupesGNegligees->SecondMembre = SecondMembre;
CoupesGNegligees->LaCoupeEstDansLePool = LaCoupeEstDansLePool;
CoupesGNegligees->First = First;
CoupesGNegligees->NbElements = NbElements;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AugmenterLaTailleDesCoupesGNegligees( PROBLEME_PNE * Pne )
{
COUPES_G_NEGLIGEES * CoupesGNegligees; int Size; int * Colonne; double * Coefficient;

CoupesGNegligees = Pne->CoupesGNegligees;
Size = CoupesGNegligees->TailleCoupesAllouee + SIZE_ALLOC_TERMES_COUPES;

Colonne = (int *) realloc( CoupesGNegligees->Colonne, Size * sizeof( int ) );
if ( Colonne == NULL ) {
	CoupesGNegligees->Full = OUI_PNE;
  return;
}
Coefficient = (double *) realloc( CoupesGNegligees->Coefficient, Size * sizeof( double ) );
if ( Coefficient == NULL ) {
	free( Colonne ); 
	CoupesGNegligees->Full = OUI_PNE;
  return;
}
CoupesGNegligees->TailleCoupesAllouee = Size;
CoupesGNegligees->Colonne = Colonne;
CoupesGNegligees->Coefficient = Coefficient;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_CreerUneCoupeGNegligee( PROBLEME_PNE * Pne, int NumeroDeCoupe )
{
COUPES_G_NEGLIGEES * CoupesGNegligees; int il; int NombreDeCoupes;
int Nb; int * First; int * NbElements; int * Colonne; int * Mdeb; int * NbTerm;
int * Nuvar; double * Coefficient; double * A; int il1; int ilMax; 

return;

Mdeb   = Pne->Coupes.Mdeb;
NbTerm = Pne->Coupes.NbTerm;
Nuvar  = Pne->Coupes.Nuvar;
A      = Pne->Coupes.A;

CoupesGNegligees = Pne->CoupesGNegligees;
if ( CoupesGNegligees == NULL ) {
  PNE_AllocCoupesGNegligees( Pne );
  CoupesGNegligees = Pne->CoupesGNegligees;
  if ( CoupesGNegligees == NULL ) return; /* Saturation memoire */
}

NombreDeCoupes = CoupesGNegligees->NombreDeCoupes;

/* Place suffisante */
if ( NombreDeCoupes >= CoupesGNegligees->NombreDeCoupesAllouees ) {
  /* On augmente la taille */
	PNE_AugmenterNombreDeCoupesGNegligees( Pne );
	if ( Pne->CoupesGNegligees == NULL ) return;
	if ( CoupesGNegligees->Full == OUI_PNE ) return;
}

/* Place suffisante */
il1 = CoupesGNegligees->IndexLibre;
while ( il1 + NbTerm[NumeroDeCoupe] + 1 >= CoupesGNegligees->TailleCoupesAllouee ) {
  /* On augmente la taille */
  PNE_AugmenterLaTailleDesCoupesGNegligees( Pne );
	if ( Pne->CoupesGNegligees == NULL ) return;	 
	if ( CoupesGNegligees->Full == OUI_PNE ) return;
}

First = CoupesGNegligees->First;
NbElements = CoupesGNegligees->NbElements;
Coefficient = CoupesGNegligees->Coefficient;
Colonne = CoupesGNegligees->Colonne;

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

CoupesGNegligees->SecondMembre[NombreDeCoupes] = Pne->Coupes.B[NumeroDeCoupe];

# if TRACES == 1
  printf("Coupe de G negligee: %d\n",NombreDeCoupes);
  il = First[NombreDeCoupes];
  ilMax = il + NbElements[NombreDeCoupes];
	while ( il < ilMax ) {
    if ( Pne->TypeDeVariableTrav[Colonne[il]] == ENTIER ) printf("%e (%d I) ",Coefficient[il],Colonne[il]);
		else printf("%e (%d R) ",Coefficient[il],Colonne[il]);		
    il++;
	}
	printf("  SecondMembre %e\n",CoupesGNegligees->SecondMembre[NombreDeCoupes]);
# endif

CoupesGNegligees->IndexLibre = il1;
CoupesGNegligees->LaCoupeEstDansLePool[NombreDeCoupes] = NON_PNE;
CoupesGNegligees->NombreDeCoupes++;

return;
}


