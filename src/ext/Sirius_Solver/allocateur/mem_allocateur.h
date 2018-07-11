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
# ifdef __CPLUSPLUS
  extern "C"
	{
# endif
# ifndef DONNEES_INTERNES_MEMOIRE_DEJA_DEFINIES
/*****************************************************************/

# define DEBUG  
# undef  DEBUG

# define TRACES_MALLOC
# undef TRACES_MALLOC     

# define TRACES_ALLOC_SUPERTABLEAU
# undef TRACES_ALLOC_SUPERTABLEAU

# define TRACES_DEFRAG     
# undef TRACES_DEFRAG 

# define TRACES_QUIT
# undef TRACES_QUIT

# define FREQUENCE_DEFRAG             100
# define CHUNK_CELLULES_DESCRIPITIVES 1000

# define ALIGN(x) x=x>>3;x=x<<3;x+=8

# include "mem_sys.h"

typedef struct {
long   Taille; /* En octets */
char * AdresseBlocsLibres; 
long   NombreDAllocs;
long   PourMutipleDe8;
} ENTETE;

/* Blocs libres d'un super tableau */
typedef struct {
long    NombreDeBlocsLibres;
long *  TailleDuBlocLibre;
char ** AdresseDuBlocLibre;
long    NombreDeCellulesDescriptivesAllouees;
long    NombreDeNouveauxBlocsLibres;
long    PlusGrandeTailleDispo; /* C'est approximatif */
long    TailleDisponible;
long    TailleInitialeDuSuperTableau;
long    SuperTableauStandard; /* 1 si oui, 0 si non */
} BLOCS_LIBRES;

# include "mem_fonctions.h"

typedef struct {  
long PageAllocEnCours;
/* Pour chaque super tableau */
long            NombreDeSuperTableaux;
BLOCS_LIBRES ** DescriptionDesBlocsLibres;
char **         AdresseSuperTableau; 

long NombreDeSuperTableauxStandards;
long TailleStandardDeDepart;
long TailleStandard;

} MEMOIRE_THREAD;

# define CYCLE_SHIFT       5
# define TAILLE_STANDARD  (1024*1024*2) 
# define TAILLE_MIN_USER  (1024) /* Ce qu'il doit rester au minimum pour creer un bloc */
# define TAILLE_MIN_BLOC  ( sizeof( ENTETE ) + TAILLE_MIN_USER )

long MEM_ClassementTriRapide( char ** , long * , long , long );
void MEM_Classement( char ** , long * , long , long );
void MEM_DefragmenterLEspaceLibre( BLOCS_LIBRES * );
char MEM_AllocSuperTableau( void * , long ); 

/*****************************************************************/	
# define DONNEES_INTERNES_MEMOIRE_DEJA_DEFINIES
# endif
# ifdef __CPLUSPLUS
  }
# endif		
