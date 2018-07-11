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

   FONCTION: Variable probing
                
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

# define VERBOSE_CREATION_GRAPHE NON_PNE
# define AJOUTER_COMPLEMENT 1 /* Vaut 0 ou 1 */

/*----------------------------------------------------------------------------*/
void PNE_AllocConflictGraph( PROBLEME_PNE * Pne )
{
int TailleAllouee_1; int TailleAllouee_2; int * First; int i;
CONFLICT_GRAPH * ConflictGraph;
Pne->ConflictGraph = (CONFLICT_GRAPH *) malloc( sizeof( CONFLICT_GRAPH ) );
if ( Pne->ConflictGraph == NULL ) return;
ConflictGraph = Pne->ConflictGraph;

TailleAllouee_1 = Pne->NombreDeVariablesTrav << 1;
TailleAllouee_2 = Pne->NombreDeVariablesTrav + Pne->NombreDeContraintesTrav;
ConflictGraph->First = (int *) malloc( TailleAllouee_1 * sizeof( int ) );
if ( ConflictGraph->First == NULL ) {
  free( Pne->ConflictGraph ); Pne->ConflictGraph = NULL;
	return;
}  
ConflictGraph->Adjacent = (int *) malloc( TailleAllouee_2 * sizeof( int ) );
if ( ConflictGraph->Adjacent == NULL ) {
  free( ConflictGraph->First ); ConflictGraph->First = NULL;
  free( Pne->ConflictGraph ); Pne->ConflictGraph = NULL;
	return;
}
ConflictGraph->Next = (int *) malloc( TailleAllouee_2 * sizeof( int ) );
if ( ConflictGraph->Next == NULL ) {
  free( ConflictGraph->First ); ConflictGraph->First = NULL;
  free( ConflictGraph-> Adjacent); ConflictGraph->Adjacent = NULL;
  free( Pne->ConflictGraph ); Pne->ConflictGraph = NULL;
	return;
}
First = ConflictGraph->First;
for ( i = 0 ; i < TailleAllouee_1 ; i++ ) First[i] = -1;

ConflictGraph->NbNoeudsDuGraphe = Pne->NombreDeVariablesTrav << 1;
ConflictGraph->Pivot = Pne->NombreDeVariablesTrav;
ConflictGraph->NbEdges = 0;
ConflictGraph->NbEdgesLast = 0;
ConflictGraph->TailleAllouee = TailleAllouee_2;
ConflictGraph->IncrementDAllocation = TailleAllouee_2;
ConflictGraph->Full = NON_PNE;
return;
}
/*----------------------------------------------------------------------------*/
/* A revoir pour controler un peu mieux la taille  */
void PNE_ReallocConflictGraph( PROBLEME_PNE * Pne )
{
int * pt; CONFLICT_GRAPH * ConflictGraph;
ConflictGraph = Pne->ConflictGraph;
ConflictGraph->TailleAllouee += ConflictGraph->IncrementDAllocation;
pt = (int *) realloc( ConflictGraph->Adjacent, ConflictGraph->TailleAllouee * sizeof( int ) );
if ( pt == NULL ) {
  ConflictGraph->Full = OUI_PNE;
	return;
}
ConflictGraph->Adjacent = pt;
pt = (int *) realloc( ConflictGraph->Next, ConflictGraph->TailleAllouee * sizeof( int ) );
if ( ConflictGraph->Next == NULL ) {
  ConflictGraph->Full = OUI_PNE;
	return;
}
ConflictGraph->Next = pt;
return;
}
















