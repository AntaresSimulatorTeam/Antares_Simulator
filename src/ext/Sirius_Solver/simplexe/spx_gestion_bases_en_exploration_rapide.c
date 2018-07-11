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

   FONCTION: Sauvegarde et restitution des bases en exploration rapide
             en profondeur.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

#include "bb_define.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void SPX_SauvegarderLaBaseDeDepart( PROBLEME_SPX * Spx , void * Noeud ) 
{
BASE_SIMPLEXE * BaseDuNoeud; NOEUD * NoeudEnCours;

NoeudEnCours = (NOEUD *) Noeud;

/* Initialisation a partir de la base simplexe */
NoeudEnCours->BaseSimplexeDuNoeud = (BASE_SIMPLEXE *) malloc( sizeof( BASE_SIMPLEXE ) );
if ( NoeudEnCours->BaseSimplexeDuNoeud == NULL ) {
  /* Completer la gestion de la saturation memoire */
  printf("Simplexe -> memoire insuffisante pour l allocation de l espace de travail \n");
  return;
}
BaseDuNoeud = NoeudEnCours->BaseSimplexeDuNoeud;

BaseDuNoeud->PositionDeLaVariable         = (char *)   malloc( Spx->NombreDeVariables   * sizeof( char   ) );  
BaseDuNoeud->InDualFramework              = (char *)   malloc( Spx->NombreDeVariables   * sizeof( char   ) );  
BaseDuNoeud->ContrainteDeLaVariableEnBase = (int *)   malloc( Spx->NombreDeVariables   * sizeof( int   ) );  
BaseDuNoeud->DualPoids                    = (double *) malloc( Spx->NombreDeContraintes * sizeof( double ) );
BaseDuNoeud->VariableEnBaseDeLaContrainte = (int *)   malloc( Spx->NombreDeContraintes * sizeof( int   ) );

if ( BaseDuNoeud->PositionDeLaVariable         == NULL || BaseDuNoeud->InDualFramework == NULL ||  
     BaseDuNoeud->ContrainteDeLaVariableEnBase == NULL || BaseDuNoeud->DualPoids       == NULL ||
     BaseDuNoeud->VariableEnBaseDeLaContrainte == NULL ) {
  /* Completer la gestion de la saturation memoire */
  printf("Simplexe -> memoire insuffisante pour l allocation de l espace de travail \n");
  return;
}

memcpy( (char *) BaseDuNoeud->DualPoids, 
        (char *) Spx->DualPoids, Spx->NombreDeContraintes * sizeof( double ) );
	
memcpy( (char *) BaseDuNoeud->VariableEnBaseDeLaContrainte,
        (char *) Spx->VariableEnBaseDeLaContrainte, Spx->NombreDeContraintes * sizeof( int ) );

memcpy( (char *) BaseDuNoeud->PositionDeLaVariable,
        (char *) Spx->PositionDeLaVariable, Spx->NombreDeVariables * sizeof( char ) );
	
memcpy( (char *) BaseDuNoeud->InDualFramework,
        (char *) Spx->InDualFramework, Spx->NombreDeVariables * sizeof( char ) );
	
memcpy( (char *) BaseDuNoeud->ContrainteDeLaVariableEnBase,
        (char *) Spx->ContrainteDeLaVariableEnBase, Spx->NombreDeVariables * sizeof( int ) );

return;
}

/*----------------------------------------------------------------------------*/

void SPX_InitialiserLaBaseDeDepart( PROBLEME_SPX * Spx , void * Noeud ) 
{
NOEUD * NoeudEnCours; BASE_SIMPLEXE * BaseDuNoeud;

/* Initialisation a partir de la base simplexe */
NoeudEnCours = (NOEUD *) Noeud;
BaseDuNoeud = NoeudEnCours->BaseSimplexeDuNoeud;
  
memcpy( (char *) Spx->DualPoids,  
        (char *) BaseDuNoeud->DualPoids, Spx->NombreDeContraintes * sizeof( double ) );
	
memcpy( (char *) Spx->VariableEnBaseDeLaContrainte,
        (char *) BaseDuNoeud->VariableEnBaseDeLaContrainte, Spx->NombreDeContraintes * sizeof( int ) );

memcpy( (char *) Spx->PositionDeLaVariable,
        (char *) BaseDuNoeud->PositionDeLaVariable, Spx->NombreDeVariables * sizeof( char ) );
	
memcpy( (char *) Spx->InDualFramework, 
        (char *) BaseDuNoeud->InDualFramework, Spx->NombreDeVariables * sizeof( char ) );
	
memcpy( (char *) Spx->ContrainteDeLaVariableEnBase,
        (char *) BaseDuNoeud->ContrainteDeLaVariableEnBase, Spx->NombreDeVariables * sizeof( int ) );

return;
}
