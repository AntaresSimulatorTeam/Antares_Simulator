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

FONCTION: Remplace le free

AUTEUR: R. GONZALEZ

************************************************************************/

# include "mem_allocateur.h"

/**************************************************************************/

void MEM_Free( void * Adresse )
{   
long j; char * AdresseALibere; long Taille             ; long * TailleDuBlocLibre  ; 
BLOCS_LIBRES * BlocsLibres   ; long NombreDeBlocsLibres; char ** AdresseDuBlocLibre;

if ( Adresse == NULL ) return;
 
AdresseALibere = (char *) Adresse - sizeof( ENTETE );

Taille = ((ENTETE *) AdresseALibere)->Taille;

BlocsLibres = (BLOCS_LIBRES *) ((ENTETE *) AdresseALibere)->AdresseBlocsLibres;

NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres;
AdresseDuBlocLibre  = BlocsLibres->AdresseDuBlocLibre;
TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;

/* On regarde si on peut merger la nouvelle zone avec la derniere libre */
if ( NombreDeBlocsLibres > 0 ) {
  j = NombreDeBlocsLibres - 1;
  if ( AdresseALibere + Taille == AdresseDuBlocLibre[j] ) {
	  AdresseDuBlocLibre[j] = AdresseALibere;
		TailleDuBlocLibre [j]+= Taille;
		if ( TailleDuBlocLibre[j] > BlocsLibres->PlusGrandeTailleDispo ) {
		  BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[j];
		}
		BlocsLibres->TailleDisponible+= Taille;
    if ( NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[0];       				
    BlocsLibres->NombreDeNouveauxBlocsLibres++;
  	return;
  }
  if ( AdresseDuBlocLibre[j] + TailleDuBlocLibre[j] == AdresseALibere ) {
		TailleDuBlocLibre[j]+= Taille;
		if ( TailleDuBlocLibre[j] > BlocsLibres->PlusGrandeTailleDispo ) {
		  BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[j];
		}		
		BlocsLibres->TailleDisponible+= Taille;		
    if ( NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[0];       				
    BlocsLibres->NombreDeNouveauxBlocsLibres++;
    return;	
  }
}
 
/* On place le bloc a la fin */

if ( NombreDeBlocsLibres >= BlocsLibres->NombreDeCellulesDescriptivesAllouees ) {
  BlocsLibres->NombreDeCellulesDescriptivesAllouees+= CHUNK_CELLULES_DESCRIPITIVES;
  j = BlocsLibres->NombreDeCellulesDescriptivesAllouees;

  BlocsLibres->AdresseDuBlocLibre = (char **) realloc( BlocsLibres->AdresseDuBlocLibre, j * sizeof( unsigned long ) );
  if ( BlocsLibres->AdresseDuBlocLibre == NULL ) {
	  printf("Saturation memoire dans l'allocateur de memoire\n");
		return;
	}	
  BlocsLibres->TailleDuBlocLibre  = (long *) realloc( BlocsLibres->TailleDuBlocLibre , j * sizeof( long ) );
  if ( BlocsLibres->TailleDuBlocLibre == NULL ) {
	  printf("Saturation memoire dans l'allocateur de memoire\n");
		return;
	}	
}

BlocsLibres->AdresseDuBlocLibre[NombreDeBlocsLibres] = AdresseALibere;
BlocsLibres->TailleDuBlocLibre [NombreDeBlocsLibres] = Taille;

if ( Taille > BlocsLibres->PlusGrandeTailleDispo ) BlocsLibres->PlusGrandeTailleDispo = Taille;
	 	
BlocsLibres->TailleDisponible+= Taille;
BlocsLibres->NombreDeBlocsLibres++;
BlocsLibres->NombreDeNouveauxBlocsLibres++;
if ( BlocsLibres->NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = BlocsLibres->TailleDuBlocLibre[0];       				

return;			
}
 
