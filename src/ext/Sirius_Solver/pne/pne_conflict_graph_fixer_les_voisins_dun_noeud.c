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

   FONCTION: On fixe des variables du conflict graph.  
                 
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

/*----------------------------------------------------------------------------*/

void PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( PROBLEME_PNE * Pne, int Noeud )
{
int * Adjacent; int * Next; int * First; int Edge; int Nv; int Var; double * UminTrav;
double * UmaxTrav; double * UTrav; int Pivot; int Complement; char BorneMiseAJour;
char UneVariableAEteFixee; double ValeurDeVar;

if ( Pne->ConflictGraph == NULL ) return;

/*printf("ConflictGraphFixerLesNoeudsVoisinsDunNoeud Noeud %d\n",Noeud);*/

Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next; 				
First = Pne->ConflictGraph->First;
Pivot = Pne->ConflictGraph->Pivot;

UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
UTrav = Pne->UTrav;

if ( Noeud < Pivot ) {
  /* C'est a valeur Max qui est imposee */
  Var = Noeud;
  ValeurDeVar = UmaxTrav[Var];
	UminTrav[Var] = ValeurDeVar;
	UTrav[Var] = ValeurDeVar;
  UneVariableAEteFixee = FIXE_AU_DEPART;
}
else {
  /* C'est a valeur Min qui est imposee */
  Var = Noeud - Pivot;
  ValeurDeVar = UminTrav[Var];	
	UmaxTrav[Var] = ValeurDeVar ;
	UTrav[Var] = ValeurDeVar;
  UneVariableAEteFixee = FIXE_AU_DEPART;
}

BorneMiseAJour = NON_PNE;

PNE_ProbingMajBminBmax( Pne, Var, UTrav[Var], BorneMiseAJour );
if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;

PNE_MajIndicateursDeBornes( Pne, Pne->ProbingOuNodePresolve->ValeurDeBorneInf, Pne->ProbingOuNodePresolve->ValeurDeBorneSup,
                            Pne->ProbingOuNodePresolve->BorneInfConnue, Pne->ProbingOuNodePresolve->BorneSupConnue,
                            ValeurDeVar, Var, UneVariableAEteFixee, BorneMiseAJour );
														
if ( Noeud < Pivot ) Complement = Pivot + Noeud;
else Complement = Noeud - Pivot;
/* S'il y a des implications pour l'instanciation, on les applique */
Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	/* Attention a ne pas prendre le complement */
	if ( Nv == Complement ) goto NextEdge;
  if ( Nv < Pivot ) {
	  Var = Nv;		
		if ( UminTrav[Var] == UmaxTrav[Var] ) goto NextEdge;
    /* La valeur borne sup est interdite pour la variable */
		/* On doit donc fixer la variable a Umin et fixer les voisins de ce noeud */
	  Nv = Pivot + Nv;
		/* On fixe la variable correspondant a Nv */
    PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( Pne, Nv );
		/* On redemarre au debut de Noeud car le chainage a pu changer */
		Edge = First[Noeud];
		continue; /* Pour ne pas faire Edge = Next[Edge] */		
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Nv = Nv - Pivot;
	  Var = Nv;		
		if ( UminTrav[Var] == UmaxTrav[Var] ) goto NextEdge;		
		/* On fixe la variable correspondant a Nv */
    PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( Pne, Nv );
		Edge = First[Noeud];
		continue; /* Pour ne pas faire Edge = Next[Edge] */		
	}
	NextEdge:
  Edge = Next[Edge];
}			
/* On elimine l'entree du noeud dans le graphe */
PNE_ConflictGraphSupprimerUnNoeud( Noeud, First, Adjacent, Next );

/* On elimine l'entree du complement */
PNE_ConflictGraphSupprimerUnNoeud( Complement, First, Adjacent, Next );

return;
}










