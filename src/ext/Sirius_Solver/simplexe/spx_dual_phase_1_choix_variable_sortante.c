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

   FONCTION: Phase 1 de l'algorithme dual, choix de la variable 
            sortante.
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"  

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif   
  
/*----------------------------------------------------------------------------*/

void SPX_DualPhase1ChoixDeLaVariableSortante( PROBLEME_SPX * Spx )

{
int Cnt; int Var; double AbsV; double MxAbsV; double * V; double * DualPoids;
char * TypeDeVariable; int * VariableEnBaseDeLaContrainte; 
int VariableSortante; int SortSurXmaxOuSurXmin;
int NombreDeContraintes;
 
Spx->VariableSortante = -1;
MxAbsV    = -1.;
V         = Spx->V;
DualPoids = Spx->DualPoids;
TypeDeVariable               = Spx->TypeDeVariable;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
NombreDeContraintes          = Spx->NombreDeContraintes;

VariableSortante     = -1;
SortSurXmaxOuSurXmin = SORT_PAS;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {

  Var = VariableEnBaseDeLaContrainte[Cnt];
	
	/* Une variable non bornee ne sort pas de la base */
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue; 
	

  /* Si une variable est non bornee superieurement, elle ne peut pas sortir de la base 
     si Spx->V est negatif. En effet cela conduirait a un cout reduit negatif pour 
     cette variable et il faudrait qu'elle sorte sur borne max. ce qui n'est pas possible */
  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) { 
    if ( V[Cnt] <= 0. ) continue;	
  }

  /* La variable est donc bornee ( des 2 cotes ou seulement bornee inferieurement ) */
  AbsV  = fabs( V[Cnt] );
  AbsV  = AbsV * AbsV / DualPoids[Cnt];     
  if ( AbsV > MxAbsV ) {
    VariableSortante = Var;
    MxAbsV           = AbsV;
    if ( V[Cnt] >= 0. ) SortSurXmaxOuSurXmin = SORT_SUR_XMIN;	 
    else                SortSurXmaxOuSurXmin = SORT_SUR_XMAX;
  }   

}

Spx->VariableSortante     = VariableSortante;
Spx->SortSurXmaxOuSurXmin = SortSurXmaxOuSurXmin;

/* Traces */
/*
if ( Spx->VariableSortante >= 0 ) {
  if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {     
    printf("  -> Iter %d variable sortante %d poids %lf contrainte associee %d V %lf SORT_SUR_XMIN",
           Spx->Iteration,
           Spx->VariableSortante,
	   Spx->DualPoids[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]],
           Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante],
           MxAbsV );     
  }
  else if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) {
        printf("  -> Iter %d variable sortante %d poids %lf contrainte associee %d V %lf SORT_SUR_XMAX",
           Spx->Iteration,
           Spx->VariableSortante,
	   Spx->DualPoids[Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante]],
           Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante], 
           -MxAbsV );     
  }
  else {
    printf("Bug dans l algorithme dual, sous-programme SPX_DualChoixDeLaVariableKiKitLaBase\n");
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx, Spx->AnomalieDetectee ); 
  } 
  if ( Spx->TypeDeVariable[Spx->VariableSortante] == BORNEE )
    printf(" type variable sortante: BORNEE min %e max %e\n",Spx->Xmin[Spx->VariableSortante],Spx->Xmax[Spx->VariableSortante]); 
  if ( Spx->TypeDeVariable[Spx->VariableSortante] == BORNEE_INFERIEUREMENT ) 
    printf(" type de la variable sortante: BORNEE_INFERIEUREMENT\n"); 
  fflush(stdout);
}
*/
/* Fin traces */

return; 

}

/*----------------------------------------------------------------------------*/

void SPX_DualPhase1ChoixDeLaVariableSortanteAuHasard( PROBLEME_SPX * Spx )

{
int Cnt; int Var; int NombreDeVariablesCandidates; int Nombre; double X;
int * VariableCandidate;

VariableCandidate = (int *) malloc( Spx->NombreDeContraintes * sizeof( int ) ); 
if ( VariableCandidate == NULL ) {
  printf(" Simplexe: memoire insuffisante dans SPX_DualPhase1ChoixDeLaVariableSortanteAuHasard\n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}
    
Spx->VariableSortante        = -1;
NombreDeVariablesCandidates = 0;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Var = Spx->VariableEnBaseDeLaContrainte[Cnt]; 
  
  if ( Spx->TypeDeVariable[Var] == NON_BORNEE ) continue; /* Une variable libre ne sort pas de la base */

  /* Si une variable est non bornee superieurement, elle ne peut pas sortir de la base 
     si Spx->V est negatif. En effet cela conduirait a un cout reduit negatif pour 
     cette variable et il faudrait qu'elle sorte sur borne max. ce qui n'est pas possible */
  if ( Spx->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) { 
    if ( Spx->V[Cnt] <= 0. ) continue;	
  }

  if ( fabs( Spx->V[Cnt] ) < 1.e-7/*6*/ ) continue;

  VariableCandidate[NombreDeVariablesCandidates] = Var;
  NombreDeVariablesCandidates++;
}

#if VERBOSE_SPX
  printf("SPX_DualPhase1ChoixDeLaVariableSortanteAuHasard NombreDeVariablesCandidates %d\n",NombreDeVariablesCandidates);
#endif

if ( NombreDeVariablesCandidates > 0 ) {
  /* On tire un nombre au hasard compris entre 0 et NombreDeVariablesCandidates - 1 */

  # if UTILISER_PNE_RAND == OUI_SPX
    Spx->A1 = PNE_Rand( Spx->A1 );
    X = Spx->A1 * (NombreDeVariablesCandidates - 1);
	# else	
    X = rand() * Spx->UnSurRAND_MAX * (NombreDeVariablesCandidates - 1);
  # endif
	
  Nombre = (int) X;
  if ( Nombre >= NombreDeVariablesCandidates - 1 ) Nombre = NombreDeVariablesCandidates - 1; 

  Spx->VariableSortante = VariableCandidate[Nombre];

  Cnt = Spx->ContrainteDeLaVariableEnBase[Spx->VariableSortante];      
  if ( Spx->V[Cnt] >= 0. ) Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMIN;	 
  else                    Spx->SortSurXmaxOuSurXmin = SORT_SUR_XMAX;  
}
/*
printf("Nouveau choix de variable sortante %d Spx->V[Cnt] %lf \n",Spx->VariableSortante,Spx->V[Cnt]);
*/
free( VariableCandidate );

return; 

}


