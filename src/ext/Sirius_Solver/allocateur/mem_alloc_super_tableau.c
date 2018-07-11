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

   FONCTION: Allocation d'un super tableau 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "mem_allocateur.h"

/**************************************************************************/

char MEM_AllocSuperTableau( void * h, long Taille ) 
{    
long i; long j; MEMOIRE_THREAD * Mem; BLOCS_LIBRES * BlocsLibres; long Shift;   

Mem = (MEMOIRE_THREAD *) h;

i = Mem->NombreDeSuperTableaux;
Mem->NombreDeSuperTableaux++;
if ( i == 0 ) {
  Mem->AdresseSuperTableau = (char **) malloc( Mem->NombreDeSuperTableaux * sizeof( unsigned long ) );
	if ( Mem->AdresseSuperTableau == NULL ) return( 0 );
  Mem->DescriptionDesBlocsLibres = (BLOCS_LIBRES **) malloc( Mem->NombreDeSuperTableaux * sizeof( unsigned long ) );
	if ( Mem->DescriptionDesBlocsLibres == NULL ) return( 0 );	
}
else {
  Mem->AdresseSuperTableau = (char **) realloc( Mem->AdresseSuperTableau, Mem->NombreDeSuperTableaux * sizeof( unsigned long ) );
	if ( Mem->AdresseSuperTableau == NULL ) return( 0 );
  Mem->DescriptionDesBlocsLibres = (BLOCS_LIBRES **) realloc( Mem->DescriptionDesBlocsLibres, Mem->NombreDeSuperTableaux * sizeof( unsigned long ) );
	if ( Mem->DescriptionDesBlocsLibres == NULL ) return( 0 );
}


Mem->AdresseSuperTableau[i] = (char *)  malloc( (size_t) Taille );
if ( Mem->AdresseSuperTableau[i] == NULL ) return( 0 );
Mem->DescriptionDesBlocsLibres[i] = (BLOCS_LIBRES *) malloc( sizeof( BLOCS_LIBRES ) );

/* On cree l'entete */
BlocsLibres = Mem->DescriptionDesBlocsLibres[i];

j = CHUNK_CELLULES_DESCRIPITIVES;
BlocsLibres->AdresseDuBlocLibre = (char **) malloc( j * sizeof( unsigned long ) );
if ( BlocsLibres->AdresseDuBlocLibre == NULL ) return( 0 );
BlocsLibres->TailleDuBlocLibre = (long *) malloc( j * sizeof( long ) );
if ( BlocsLibres->TailleDuBlocLibre == NULL ) return( 0 );

BlocsLibres->NombreDeCellulesDescriptivesAllouees = j;

BlocsLibres->NombreDeBlocsLibres = 1;
BlocsLibres->NombreDeNouveauxBlocsLibres = 0;
BlocsLibres->AdresseDuBlocLibre[0] = Mem->AdresseSuperTableau[i];
BlocsLibres->TailleDuBlocLibre [0] = Taille;
BlocsLibres->PlusGrandeTailleDispo = Taille;
BlocsLibres->TailleInitialeDuSuperTableau = Taille;
BlocsLibres->TailleDisponible = Taille;

if ( Taille != Mem->TailleStandard ) {
  BlocsLibres->SuperTableauStandard = 0;
}
else {
  BlocsLibres->SuperTableauStandard = 1;
  Mem->NombreDeSuperTableauxStandards++;
	/* Mise a jour eventuelle de la taille standard */
	Shift = Mem->NombreDeSuperTableauxStandards / CYCLE_SHIFT;
	/* On limite le shift a 4 ce qui est equivalent a limiter la taille standard a 16 fois la
	   taille de depart */
	if ( Shift > 4 ) Shift = 4;	
	Mem->TailleStandard = Mem->TailleStandardDeDepart << Shift;

  # ifdef TRACES_ALLOC_SUPERTABLEAU
	  if ( Shift != 0 ) {
	    printf("SuperTableauxStandards %ld  NombreDeSuperTableaux %d taille standard %ld  taille initiale %ld shift %ld\n",
			        Mem->NombreDeSuperTableauxStandards, Mem->NombreDeSuperTableaux, Mem->TailleStandard,
							(long) TAILLE_STANDARD, Shift);
		}
	# endif
	
}

/* S'il n'y a aucun tableau standard alors on dit que le tableau qui vient d'etre cree est standard */
/* Ceci permet d'ajuster la taille standard de depart si la taille parametree ne convient pas */
if ( Mem->NombreDeSuperTableauxStandards == 0 ) {
  BlocsLibres->SuperTableauStandard   = 1;
  Mem->NombreDeSuperTableauxStandards = 1;
	Mem->TailleStandard                 = Taille;
	Mem->TailleStandardDeDepart         = Taille;
}

Mem->PageAllocEnCours = i;

return( 1 );
}


