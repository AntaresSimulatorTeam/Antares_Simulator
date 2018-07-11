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

   FONCTION: Construction de la liste des variables a instancier
	           dans le variable probing.
                 
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

int PNE_PartitionVarInstTriRapide( int * , int * , int , int );
void PNE_VarInstTriRapide( int * , int * , int , int );

/*----------------------------------------------------------------------------*/

int PNE_PartitionVarInstTriRapide( int * NumVarAInstancier, int * NbContraintesDeVar, int Deb, int Fin )
{
int Compt; double Pivot; int i; int Var;
Compt = Deb;
Pivot = NbContraintesDeVar[NumVarAInstancier[Deb]];
/* Ordre decroissant */
for ( i = Deb + 1 ; i <= Fin ; i++) {		
  if ( NbContraintesDeVar[NumVarAInstancier[i]] > Pivot) {
    Compt++;		
    Var = NumVarAInstancier[Compt];
    NumVarAInstancier[Compt] = NumVarAInstancier[i];
    NumVarAInstancier[i] = Var; 			
  }
}	
Var = NumVarAInstancier[Compt];
NumVarAInstancier[Compt] = NumVarAInstancier[Deb];
NumVarAInstancier[Deb] = Var;
return(Compt);
}

/*----------------------------------------------------------------------------*/

void PNE_VarInstTriRapide( int * NumVarAInstancier, int * NbContraintesDeVar, int Debut, int Fin )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = PNE_PartitionVarInstTriRapide( NumVarAInstancier, NbContraintesDeVar, Debut, Fin );	
  PNE_VarInstTriRapide( NumVarAInstancier, NbContraintesDeVar, Debut  , Pivot-1 );
  PNE_VarInstTriRapide( NumVarAInstancier, NbContraintesDeVar, Pivot+1, Fin );
}
return;
}

/*----------------------------------------------------------------------------*/
/* On construit la liste des variables a tester en prenant toutes les variables
   entieres et en les classant dans l'ordre decroissant du nombre de contraintes
	 dans lequel elles interviennent */
void PNE_ProbingInitVariablesAInstancierApresLePresolve( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int Var; int NombreDeVariables; int * TypeDeBorne; int * TypeDeVariable; double * Umin;
double * Umax; char * FlagVarAInstancier; int NbVarAInstancier; 

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;

FlagVarAInstancier = ProbingOuNodePresolve->FlagVarAInstancier;

NbVarAInstancier = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  FlagVarAInstancier[Var] = 0;
	if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;
	if ( TypeDeVariable[Var] != ENTIER ) continue;
	if ( Umin[Var] == Umax[Var] ) continue;
  FlagVarAInstancier[Var] = 1;
 	NbVarAInstancier++;
}
ProbingOuNodePresolve->NbVarAInstancier = NbVarAInstancier;

return;
}

/*----------------------------------------------------------------------------*/
/* On construit la liste des variables a tester en prenant les variables
   entieres dont la valeur est fractionnaire et en les classant dans l'ordre
	 decroissant du nombre de contraintes dans lequel elles interviennent */
void PNE_ProbingInitVariablesAInstancier( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int Var; int NombreDeVariables; char * FlagVarAInstancier; int NbVarAInstancier; int * SuivFrac;

NombreDeVariables = Pne->NombreDeVariablesTrav;

FlagVarAInstancier = ProbingOuNodePresolve->FlagVarAInstancier;
memset( (char *) FlagVarAInstancier, 0 , NombreDeVariables * sizeof( char ) );

NbVarAInstancier = 0;
SuivFrac = Pne->SuivFrac;
Var = Pne->PremFrac;
while ( Var >= 0 ) {
  FlagVarAInstancier[Var] = 1;
 	NbVarAInstancier++;
  Var = SuivFrac[Var];		
}

ProbingOuNodePresolve->NbVarAInstancier = NbVarAInstancier;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_ProbingMajVariablesAInstancier( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int Var; int NombreDeVariables; int * NumVarAInstancier; char * FlagVarAInstancier; int * NbContraintesDeVar;
int NbVarAInstancier; int Nb; int ic; int * Cdeb; int * Csui; int Debut; int Fin;

NombreDeVariables = Pne->NombreDeVariablesTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;

NumVarAInstancier = ProbingOuNodePresolve->NumVarAInstancier;
FlagVarAInstancier = ProbingOuNodePresolve->FlagVarAInstancier;
NbContraintesDeVar = ProbingOuNodePresolve->NumeroDeVariableModifiee;

NbVarAInstancier = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( FlagVarAInstancier[Var] == 0 ) continue;
	FlagVarAInstancier[Var] = 0;
  NumVarAInstancier[NbVarAInstancier] = Var;
 	NbVarAInstancier++;
	/* Decompte du nombre de contraintes */
	Nb = 0;
	ic = Cdeb[Var];
	while ( ic >= 0 ) {
	  Nb++;		
    ic = Csui[ic];
	}
	NbContraintesDeVar[Var] = Nb;	
}

ProbingOuNodePresolve->NbVarAInstancier = NbVarAInstancier;

/* Classement */
Debut = 0;
Fin = NbVarAInstancier - 1;
if ( NbVarAInstancier > 1 ) PNE_VarInstTriRapide( NumVarAInstancier, NbContraintesDeVar, Debut, Fin );

return;
}

/*----------------------------------------------------------------------------*/
/* On initialise le flag pour les variables qui impliquent ValeurDeVar pour Var */

void PNE_ProbingMajFlagVariablesAInstancier( PROBLEME_PNE * Pne, int Var, double ValeurDeVar )
{

int Edge; int Noeud; int Complement; int Nv; int Pivot; int * First; int * Adjacent;
int * Next; char * FlagVarAInstancier;

if ( Var < 0 ) return;

FlagVarAInstancier = Pne->ProbingOuNodePresolve->FlagVarAInstancier;
Pivot = Pne->ConflictGraph->Pivot;

if ( ValeurDeVar == 1.0 ) { Noeud = Pivot + Var; Complement = Var; }
else { Noeud = Var; Complement = Pivot + Var; }

First = Pne->ConflictGraph->First;
Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next;

Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	/* Attention a ne pas prendre le complement */
	if ( Nv == Complement ) goto NextEdge;
  if ( Nv < Pivot ) {
	  Var = Nv;	
  	FlagVarAInstancier[Var] = 1;
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Var = Nv - Pivot;
  	FlagVarAInstancier[Var] = 1;
	}
	NextEdge:
  Edge = Next[Edge];
}

return;
}
