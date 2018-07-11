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

   FONCTION: Resolution de Min c x sous contrainte Ax = b par un  
             simplexe (forme revisee du simplexe) en matrices  
             creuses : appel de la routine principale.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

void SPX_InitSetJmp( jmp_buf );

/*---------------------------------------------------------------------------------------------------------*/

void SPX_InitSetJmp( jmp_buf BufferDeStockage )
{
setjmp( BufferDeStockage ); 
return;
}

/*---------------------------------------------------------------------------------------------------------*/

PROBLEME_SPX * SPX_Simplexe( PROBLEME_SIMPLEXE * Probleme , PROBLEME_SPX * Spx )
{
void * Tas;

if ( Spx == NULL ) {
  # ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
    Tas = MEM_Init(); 
    Spx = (PROBLEME_SPX *) MEM_Malloc( Tas, sizeof( PROBLEME_SPX ) );
    if ( Spx == NULL ) {
      printf("Saturation memoire, impossible d'allouer un objet PROBLEME_SPX\n");    
      Probleme->ExistenceDUneSolution = SPX_ERREUR_INTERNE;
      return( Spx );
    }
		memset( (char *) Spx, 0, sizeof( PROBLEME_SPX ) );
	  Spx->Tas = Tas;
  # else
	  Tas = NULL;
    Spx = (PROBLEME_SPX *) malloc( sizeof( PROBLEME_SPX ) );
    if ( Spx == NULL ) {
      printf("Saturation memoire, impossible d'allouer un objet PROBLEME_SPX\n");    
      Probleme->ExistenceDUneSolution = SPX_ERREUR_INTERNE;
      return( Spx );
    }
		memset( (char *) Spx, 0, sizeof( PROBLEME_SPX ) );
	  Spx->Tas = Tas;
	# endif
}

Spx->AnomalieDetectee = NON_SPX;

setjmp( Spx->EnvSpx );

/* Pour ne pas avoir de warning a la compilation */
/* Attention, il ne faut pas faire appel à a une autre routine pour faire le setjmp
   car lorsque le longjmp arrive, au return de la routine en question on se retrouve
	 n'importe ou et ça plante */
/*SPX_InitSetJmp( Spx->EnvSpx );*/

if ( Spx->AnomalieDetectee != NON_SPX ) {
  /* Liberation du probleme */ 
   /* Meme si une anomalie a ete detectee il est preferable de ne pas liberer le probleme 
      ici. Le probleme est de toute facon libere en fin de PNE . */ 
   /* SPX_LibererProbleme( Spx ); */     
  Probleme->ExistenceDUneSolution = SPX_ERREUR_INTERNE;
  if ( Spx->AnomalieDetectee == SPX_MATRICE_DE_BASE_SINGULIERE ) {
    Probleme->ExistenceDUneSolution = SPX_MATRICE_DE_BASE_SINGULIERE;
    /*printf("Trace simplexe: Matrice de base singuliere\n");*/
  }
  return( Spx );     
} 
else {
  /* Optimisation */
 SPX_SimplexeCalculs( Probleme , Spx );
 /* On ne renvoie pas de pointeur a la structure si sa desallocation 
    a ete demandee par l'appelant */
 if ( Probleme->LibererMemoireALaFin == OUI_SPX ) Spx = NULL;
}
 
return( Spx );
}  











