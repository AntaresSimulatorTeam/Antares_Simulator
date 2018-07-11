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

FONCTION: Remplace le malloc
                
AUTEUR: R. GONZALEZ

************************************************************************/

# include "mem_allocateur.h"

/**************************************************************************/

char * MEM_Malloc( void * h, size_t NombreDOctets ) 
{
long NbOctetsAReserver    ; long i; long j; long Taille; long i0; long i1; char Flag; 
MEMOIRE_THREAD * Mem      ; ENTETE * EnteteAAllouer    ; char * AdresseAllouee      ; 
BLOCS_LIBRES * BlocsLibres; long NombreDeBlocsLibres   ; long DernierBloc           ;
char ** AdresseDuBlocLibre; long * TailleDuBlocLibre   ; long TailleRestante        ; 
long PlusGrandBlocLibre   ; long TailleBlocLibre       ;
# ifdef TRACES_MALLOC
  long T; long Ti;
# endif

AdresseAllouee = NULL;
if ( h == NULL ) return( AdresseAllouee );	  

Mem = (MEMOIRE_THREAD *) h;

NbOctetsAReserver = (long ) NombreDOctets + sizeof( ENTETE );

ALIGN( NbOctetsAReserver );

RechercheBlocLibre:
i0   = Mem->PageAllocEnCours;
i1   = Mem->NombreDeSuperTableaux;
Flag = 0;
NewMalloc:
for ( i = i0 ; i < i1 ; i++ ) {

	/* Boucle sur les blocs libres du super tableau */	
  BlocsLibres = Mem->DescriptionDesBlocsLibres[i];
  if ( BlocsLibres->NombreDeBlocsLibres == 0 ) continue;
	
	# ifdef DEBUG
    /* Controle */
    if ( MEM_QuantiteLibre( BlocsLibres ) != BlocsLibres->TailleDisponible ) {
		  printf("Malloc: erreur taille disponible T %ld  TailleDisponible %ld  TailleInitialeDuSuperTableau %ld\n",
			        MEM_QuantiteLibre( BlocsLibres ),BlocsLibres->TailleDisponible,BlocsLibres->TailleInitialeDuSuperTableau);
		}
	# endif
	
	if ( BlocsLibres->TailleDisponible < NbOctetsAReserver ) continue;
	
	if ( BlocsLibres->NombreDeNouveauxBlocsLibres > FREQUENCE_DEFRAG ) {
	  MEM_DefragmenterLEspaceLibre( BlocsLibres );
	}
			 
  if ( BlocsLibres->PlusGrandeTailleDispo < NbOctetsAReserver ) continue; 
	
  NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres; 
	AdresseDuBlocLibre  = BlocsLibres->AdresseDuBlocLibre;
	TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;	
  PlusGrandBlocLibre  = 0;
	
  /* Strategie first fit */
	
  for ( j = 0 ; j < NombreDeBlocsLibres ; j++ ) {
    TailleBlocLibre = TailleDuBlocLibre[j];
		if ( TailleBlocLibre > PlusGrandBlocLibre ) PlusGrandBlocLibre = TailleBlocLibre;    		
    if ( TailleBlocLibre >= NbOctetsAReserver ) {
			/* Mise a jour de l'entete */
		  EnteteAAllouer = (ENTETE *) AdresseDuBlocLibre[j];
      TailleRestante = TailleBlocLibre - NbOctetsAReserver;			
      if ( TailleRestante >= (long) TAILLE_MIN_BLOC ) {					
        /* On peut creer un bloc libre juste apres */				
        EnteteAAllouer->Taille             = NbOctetsAReserver;
        EnteteAAllouer->AdresseBlocsLibres = (char *) BlocsLibres;			
			  EnteteAAllouer->NombreDAllocs = 0;						
				
		    /* Mise a jour des blocs libres */
        AdresseDuBlocLibre[j]+= NbOctetsAReserver;
	      TailleDuBlocLibre [j]-= NbOctetsAReserver;
				BlocsLibres->TailleDisponible-= NbOctetsAReserver;
				if ( NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[0];       				
      }
      else {
	      /* On alloue tout le bloc */				
        EnteteAAllouer->Taille             = TailleBlocLibre;
        EnteteAAllouer->AdresseBlocsLibres = (char *) BlocsLibres;			
			  EnteteAAllouer->NombreDAllocs = 0;				
							
				BlocsLibres->TailleDisponible-= TailleBlocLibre;								
	      /* On supprime le bloc */	
	      DernierBloc = NombreDeBlocsLibres - 1;
        AdresseDuBlocLibre[j] = AdresseDuBlocLibre[DernierBloc];
      	TailleDuBlocLibre [j] = TailleDuBlocLibre[DernierBloc];
				if ( NombreDeBlocsLibres == 1 ) BlocsLibres->PlusGrandeTailleDispo = TailleDuBlocLibre[0];       				
	      BlocsLibres->NombreDeBlocsLibres--;
				BlocsLibres->NombreDeNouveauxBlocsLibres--;				
      }
      AdresseAllouee = (char *) EnteteAAllouer + sizeof( ENTETE );
			Mem->PageAllocEnCours = i;
		  /*printf("Adresse allouee %X taille utile %ld taille reservee %d\n",AdresseAllouee,NombreDOctets,NbOctetsAReserver);*/
      return( AdresseAllouee );	  		 			
		}
  }
  BlocsLibres->PlusGrandeTailleDispo = PlusGrandBlocLibre;
}
if ( Flag == 0 ) {
  Flag = 1;
	i0 = 0;
	i1 = Mem->PageAllocEnCours;
	goto NewMalloc;  
}

/* Avant de creer un nouveau super tableau, on vire tous les tableaux vides dans lesquels on n'a rien pu mettre */

/* Un gros tableau inutilise est vire */
for ( i = 0 ; i < Mem->NombreDeSuperTableaux ; ) {
  BlocsLibres = Mem->DescriptionDesBlocsLibres[i];
	if ( BlocsLibres->NombreDeBlocsLibres == 1 ) {
	  if ( BlocsLibres->TailleInitialeDuSuperTableau == BlocsLibres->TailleDuBlocLibre[0] ) {
      # ifdef TRACES_MALLOC
			  printf("On vire un super tableau de taille %ld \n",BlocsLibres->TailleInitialeDuSuperTableau);
      # endif

			if ( BlocsLibres->SuperTableauStandard == 1 ) Mem->NombreDeSuperTableauxStandards--;

      free( BlocsLibres->AdresseDuBlocLibre );
      free( BlocsLibres->TailleDuBlocLibre );
      free( BlocsLibres );
      free( Mem->AdresseSuperTableau[i] );
			
		  j = Mem->NombreDeSuperTableaux-1;
      Mem->DescriptionDesBlocsLibres[i] = Mem->DescriptionDesBlocsLibres[j];
			Mem->AdresseSuperTableau      [i] = Mem->AdresseSuperTableau[j];
			Mem->NombreDeSuperTableaux--;
						
			continue;
	  }
  }
	i++;
}
Mem->PageAllocEnCours = 0; /* Mise a jour a la creation du prochain super tableau */

# ifdef TRACES_MALLOC
  T = 0;
  Ti = 0;
  for ( i = 0 ; i < Mem->NombreDeSuperTableaux ; i++ ) {
    BlocsLibres = Mem->DescriptionDesBlocsLibres[i];
	  T+= BlocsLibres->TailleInitialeDuSuperTableau;
    NombreDeBlocsLibres = BlocsLibres->NombreDeBlocsLibres;
	  TailleDuBlocLibre   = BlocsLibres->TailleDuBlocLibre;
    for ( j = 0 ; j < NombreDeBlocsLibres ; j++ ) Ti+= TailleDuBlocLibre[j];	
  }
  printf("Nombre de tableaux deja alloues %ld Taille %ld Mo  taille inutilisee %ld\n",
          Mem->NombreDeSuperTableaux,T/(1024*1024),Ti/(1024*1024));
# endif

Taille = Mem->TailleStandard;
if ( NbOctetsAReserver > Taille ) {
  Taille = NbOctetsAReserver + Mem->TailleStandard /*(long) ceil( 1.5 * NbOctetsAReserver )*/;
  # ifdef TRACES_MALLOC
	  printf("Alloc gros tableau %ld\n",Taille);
	# endif
}
/* Allocation d'un super tableau */
if ( MEM_AllocSuperTableau( (void *) Mem, Taille ) == 0 ) return( NULL );

goto RechercheBlocLibre; /* Comme ce nouveau super tableau est adapter, c'est celui qui va etre utilise */

/* On ne passe donc jamais la */
return( AdresseAllouee );
}



