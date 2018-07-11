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

FONCTION: Remplace le realloc

AUTEUR: R. GONZALEZ

************************************************************************/

# include "mem_allocateur.h"

/**************************************************************************/

char * MEM_Realloc( void * h, void * Pt, size_t NombreDOctets )
{  
long i;long j;long Taille ; long NouvelleTaille     ; long TailleDispo ; 
char * NouvelleAdresse    ; MEMOIRE_THREAD * Mem    ; ENTETE * Entete  ;  
BLOCS_LIBRES * BlocsLibres; long NombreDeBlocsLibres; long Supplement  ;  
char ** AdresseDuBlocLibre; long * TailleDuBlocLibre; long Nboct       ;
long TaillePageRecuperee  ; long NombreDAllocs      ; char * AdresseFin;
long NboctPossible        ; long TailleRestante     ; long T2          ;

NouvelleAdresse = NULL;

if ( h == NULL ) return( NouvelleAdresse );
 
Mem = (MEMOIRE_THREAD *) h;

/* Si la taille demandee est plus petite on ne fait rien */
Entete = (ENTETE *) ((char *) Pt - sizeof( ENTETE ));
Taille      = Entete->Taille;
BlocsLibres = (BLOCS_LIBRES *) Entete->AdresseBlocsLibres;
Entete->NombreDAllocs++;

TailleDispo   = Taille - sizeof(ENTETE);
NombreDAllocs = Entete->NombreDAllocs;

Nboct = (long) NombreDOctets + sizeof( ENTETE );
ALIGN( Nboct );

/* Si l'utilisateur veut retailler la zone en beaucoup plus petit, on le fait */
if ( Nboct < (long) (0.1 * Taille) ) {
	/*printf("On retaille une zone allouee en plus petit ancienne taille %ld  nouvelle taille %ld\n",Taille,Nboct);*/		
  TaillePageRecuperee = Taille - Nboct;
	if ( TaillePageRecuperee >= (long) TAILLE_MIN_BLOC ) {
    NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres;
    AdresseDuBlocLibre  = BlocsLibres->AdresseDuBlocLibre;
    TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;	
    /* On peut creer un bloc libre juste apres */
		/* On retaille le bloc */
    Entete->Taille = Nboct;
		
    /* On cree un nouveeau bloc libre derriere entete derriere */				
    if ( NombreDeBlocsLibres >= BlocsLibres->NombreDeCellulesDescriptivesAllouees ) {
      BlocsLibres->NombreDeCellulesDescriptivesAllouees+= CHUNK_CELLULES_DESCRIPITIVES;
      j = BlocsLibres->NombreDeCellulesDescriptivesAllouees;
      BlocsLibres->AdresseDuBlocLibre = (char **) realloc( BlocsLibres->AdresseDuBlocLibre, j * sizeof( unsigned long ) );
      if ( BlocsLibres->AdresseDuBlocLibre == NULL ) {
	      printf("Saturation memoire dans l'allocateur de memoire\n");
		    return( NULL );
	    }	
      BlocsLibres->TailleDuBlocLibre  = (long *) realloc( BlocsLibres->TailleDuBlocLibre , j * sizeof( long ) );
      if ( BlocsLibres->TailleDuBlocLibre == NULL ) {
	      printf("Saturation memoire dans l'allocateur de memoire\n");
		    return( NULL );
	    }	
    }
		
    BlocsLibres->AdresseDuBlocLibre[NombreDeBlocsLibres] = (char *) Entete + Nboct;
    BlocsLibres->TailleDuBlocLibre [NombreDeBlocsLibres] = TaillePageRecuperee;

    if ( TaillePageRecuperee > BlocsLibres->PlusGrandeTailleDispo ) BlocsLibres->PlusGrandeTailleDispo = TaillePageRecuperee;
	 	
    BlocsLibres->TailleDisponible+= TaillePageRecuperee;
    BlocsLibres->NombreDeBlocsLibres++;
    BlocsLibres->NombreDeNouveauxBlocsLibres++;
    if ( BlocsLibres->NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = BlocsLibres->TailleDuBlocLibre[0];       				
  }		
  return( (char *) Pt );				
}

/* S'il y a la place necessaire derriere, on ne fait rien */
if ( TailleDispo >= (long) NombreDOctets ) {
  return( (char *) Pt );
}

/* S'il n' a qu'une seule zone libre et qu'elle est derriere et que ca fait le supertableau alors on la donne */
NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres;
AdresseDuBlocLibre  = BlocsLibres->AdresseDuBlocLibre;
TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;

AdresseFin = (char *) Entete + Taille;

for ( i = 0 ; i < NombreDeBlocsLibres ; i++ ) {
  if ( AdresseDuBlocLibre[i] == AdresseFin ) {
		T2 = TailleDuBlocLibre[i]; 
	  NboctPossible = Taille + T2; 
	  if ( NboctPossible >= Nboct ) {					
			TailleRestante = NboctPossible - Nboct;
      if ( TailleRestante >= (long) TAILLE_MIN_BLOC ) {					
        /* On peut conserver un bloc libre juste apres */
        Entete->Taille = Nboct;				
				
		    /* Mise a jour des blocs libres */
        AdresseDuBlocLibre[i] = (char *) Entete + Nboct;
	      TailleDuBlocLibre [i] = TailleRestante;
		  	BlocsLibres->TailleDisponible-= T2 - TailleRestante;												
      }
      else {
	      /* On alloue tout le bloc */
        Entete->Taille = Taille + T2;
				
        j = NombreDeBlocsLibres - 1;
        AdresseDuBlocLibre[i] = AdresseDuBlocLibre[j];
        TailleDuBlocLibre[i] = TailleDuBlocLibre[j];
		  	BlocsLibres->TailleDisponible-= T2;				
				BlocsLibres->NombreDeBlocsLibres--;						
      }
      if ( NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[0];       							
      return( (char *) Pt );
    }
  }
}

/* On est contraints de faire un realloc donc on prend encore plus de marge */

Supplement = NombreDAllocs * ( (long) NombreDOctets - TailleDispo );

if ( Supplement > (long) NombreDOctets ) Supplement = (long) NombreDOctets;

NouvelleAdresse = MEM_Malloc( h, NombreDOctets + Supplement );
if ( NouvelleAdresse == NULL ) return( NULL );

Entete = (ENTETE *) ((char *) NouvelleAdresse - sizeof( ENTETE ));
NouvelleTaille = Entete->Taille;
Entete->NombreDAllocs = NombreDAllocs;

if ( NouvelleTaille < Taille ) Taille = NouvelleTaille;
NombreDOctets = Taille - sizeof( ENTETE );

memcpy( (char *) NouvelleAdresse, (char *) Pt, NombreDOctets * sizeof( char ) );
/*
char * pt1; char * pt2;
pt2 = NouvelleAdresse; pt1 = Pt;
for ( i = 0 ; i < NombreDOctets ; i++ ) {
  *pt2 = *pt1; pt1++; pt2++;  
}
*/
MEM_Free( Pt );

# ifdef DEBUG
  if ( MEM_QuantiteLibre( BlocsLibres ) != BlocsLibres->TailleDisponible ) {
	  printf("Realloc: erreur taille disponible T %ld  TailleDisponible %ld  TailleInitialeDuSuperTableau %ld\n",
			      MEM_QuantiteLibre( BlocsLibres ),BlocsLibres->TailleDisponible,BlocsLibres->TailleInitialeDuSuperTableau);
  }						
# endif		
	
return( NouvelleAdresse );

}


