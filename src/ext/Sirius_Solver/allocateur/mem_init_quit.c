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
FONCTION: Init et Quit de l'allocateur et utilitaires
                
AUTEUR: R. GONZALEZ

************************************************************************/

# include "mem_allocateur.h"

/**************************************************************************/

void * MEM_Init() 
{   
MEMOIRE_THREAD * h; 

h = (MEMOIRE_THREAD *) malloc( sizeof( MEMOIRE_THREAD ) );
if ( h != NULL ) {
  h->NombreDeSuperTableaux          = 0;
  h->PageAllocEnCours               = 0;
	h->NombreDeSuperTableauxStandards = 0;
  h->TailleStandard                 = (long) TAILLE_STANDARD;
  h->TailleStandardDeDepart         = (long) TAILLE_STANDARD;
}

/* Allocation d'un super petit tableau */
if ( MEM_AllocSuperTableau( (void *)h, (long) (TAILLE_STANDARD >> 1) ) == 0 ) return( NULL ); 

/* Allocation d'un super tableau taille standard: il en faut au moins 1 de taille standard au depart */
if ( MEM_AllocSuperTableau( (void *)h, (long) TAILLE_STANDARD ) == 0 ) return( NULL );

return( (void *) h );
}

/**************************************************************************/

void MEM_Quit( void * h ) 
{
long i; MEMOIRE_THREAD * Mem; BLOCS_LIBRES * BlocsLibres; 

if ( h == NULL ) return;
Mem = (MEMOIRE_THREAD *) h;

# ifdef TRACES_QUIT
  printf("Nombre de super tableaux alloues %ld\n",Mem->NombreDeSuperTableaux);
# endif

for ( i = 0 ; i < Mem->NombreDeSuperTableaux ; i++ ) {
  BlocsLibres = Mem->DescriptionDesBlocsLibres[i];
  free( Mem->AdresseSuperTableau[i] );
  free( BlocsLibres->AdresseDuBlocLibre );
  free( BlocsLibres->TailleDuBlocLibre );
  free( BlocsLibres );		
}

/*printf("Liberation du tas terminee nombre d'allocations %ld\n",Mem->Allocation);*/

free( Mem->AdresseSuperTableau );   
free( Mem->DescriptionDesBlocsLibres );
free( Mem );

return;
} 

/**************************************************************************/

long MEM_QuantiteLibre( BLOCS_LIBRES * BlocsLibres )
{ long k; long T; long * TailleDuBlocLibre; 
  T = 0;
  TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;	
  for ( k = 0 ; k <  BlocsLibres->NombreDeBlocsLibres; k++ ) {
    T+= TailleDuBlocLibre[k];
  }
	return( T );
}

/**************************************************************************/

long MEM_ClassementTriRapide( char ** AdresseDuBlocLibre, long *  TailleDuBlocLibre,
                              long Deb, long Fin )
{
long Compt; char * Pivot; long i; long DebPlus1; char * Adresse; long Taille;

DebPlus1 = Deb + 1;
Compt    = Deb;
Pivot = AdresseDuBlocLibre[Deb];

for ( i = DebPlus1 ; i <= Fin ; i++) {  	
  if ( AdresseDuBlocLibre[i] < Pivot) {
    Compt++;
    Adresse = AdresseDuBlocLibre[Compt];				
		AdresseDuBlocLibre[Compt] = AdresseDuBlocLibre[i];		
		AdresseDuBlocLibre[i] = Adresse;
		Taille = TailleDuBlocLibre[Compt];
		TailleDuBlocLibre[Compt] = TailleDuBlocLibre[i];
		TailleDuBlocLibre[i] = Taille;		
  }
}

Adresse = AdresseDuBlocLibre[Compt];
AdresseDuBlocLibre[Compt] = AdresseDuBlocLibre[Deb];
AdresseDuBlocLibre[Deb] = Adresse;
Taille = TailleDuBlocLibre[Compt];
TailleDuBlocLibre[Compt] = TailleDuBlocLibre[Deb];
TailleDuBlocLibre[Deb] = Taille;		

return(Compt);
}

/**************************************************************************/
void MEM_Classement( char ** AdresseDuBlocLibre, long *  TailleDuBlocLibre,
                     long Debut, long Fin )
{
long Pivot;
if ( Debut < Fin ) {
  Pivot = MEM_ClassementTriRapide( AdresseDuBlocLibre, TailleDuBlocLibre, Debut, Fin );
  MEM_Classement( AdresseDuBlocLibre, TailleDuBlocLibre, Debut  , Pivot-1 );
  MEM_Classement( AdresseDuBlocLibre, TailleDuBlocLibre, Pivot+1, Fin );
}
return;
}
/**************************************************************************/

void MEM_DefragmenterLEspaceLibre( BLOCS_LIBRES * BlocsLibres )
{
long *  TailleDuBlocLibre; char ** AdresseDuBlocLibre; long NombreDeBlocsLibres; long N1;
char * A0; char * A1; long T1; long i; long j; long PlusGrandeTailleDispo;

NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres;
AdresseDuBlocLibre  = BlocsLibres->AdresseDuBlocLibre;
TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;

# ifdef TRACES_DEFRAG
  printf("Defragmentation d'un super tableau:\n");
  printf("  Avant:\n");
	printf("         Taille du super tableau %ld\n", BlocsLibres->TailleInitialeDuSuperTableau);
	printf("         Taille disponible       %ld\n", BlocsLibres->TailleDisponible);
	printf("         NombreDeBlocsLibres     %ld\n", BlocsLibres->NombreDeBlocsLibres);
	printf("         Plus grand bloc libre   %ld (approx)\n", BlocsLibres->PlusGrandeTailleDispo);
	BlocsLibres->PlusGrandeTailleDispo = 0;
  for ( i = 0 ; i < NombreDeBlocsLibres ; i++ ) {
	  if ( TailleDuBlocLibre[i] > BlocsLibres->PlusGrandeTailleDispo ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[i];
	}
	printf("         Plus grand bloc libre   %ld (calcule)\n", BlocsLibres->PlusGrandeTailleDispo);
# endif

/* On reclasse les blocs dans l'ordre croissant des adresses */
MEM_Classement( AdresseDuBlocLibre, TailleDuBlocLibre,0 ,NombreDeBlocsLibres-1 );

PlusGrandeTailleDispo = 0;

/* On fusionne */
N1 = 0;
for ( i = 0 ; i < NombreDeBlocsLibres ; ) {
  A0 = AdresseDuBlocLibre[i];
	T1 = TailleDuBlocLibre[i];
	A1 = A0 + T1;
  for ( j = i+1 ; j < NombreDeBlocsLibres ; j++ ) {
    if ( A1 == AdresseDuBlocLibre[j] ) {
	    A1+= TailleDuBlocLibre[j];
			T1+= TailleDuBlocLibre[j];
	  }
		else break;
	}
	AdresseDuBlocLibre[N1] = A0;
	TailleDuBlocLibre [N1] = T1;
	if ( T1 > PlusGrandeTailleDispo ) PlusGrandeTailleDispo = T1;
  N1++;
	i = j;
}

BlocsLibres->NombreDeBlocsLibres = N1;
BlocsLibres->NombreDeNouveauxBlocsLibres = 0;
BlocsLibres->PlusGrandeTailleDispo = PlusGrandeTailleDispo;

# ifdef TRACES_DEFRAG
  printf("  Apres:\n");
	printf("         NombreDeBlocsLibres     %ld\n", BlocsLibres->NombreDeBlocsLibres);
	printf("         Plus grand bloc libre   %ld\n", BlocsLibres->PlusGrandeTailleDispo);
# endif

return;
}
