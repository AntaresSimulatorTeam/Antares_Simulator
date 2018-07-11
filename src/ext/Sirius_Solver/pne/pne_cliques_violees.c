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

   FONCTION: Detection des cliques violees.
	           Les cliques ont ete determinees a partir du conflict graph.
                 
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

# define PROFONDEUR_LIMITE_CLIQUES 1000000 /*10*/ 

/*----------------------------------------------------------------------------*/

void PNE_DetectionDesCliquesViolees( PROBLEME_PNE * Pne )
{
double S; int NombreDeVariablesTrav; double * UTrav; CLIQUES * Cliques; int c;
int * First; int * NbElements; double NormeV; int il; int ilMax;
double * Coeff; int * Indice; double B; int NbT; double X; char ControleCliques;
char ControleImplications; CONFLICT_GRAPH * ConflictGraph; int Noeud; 
int NbNoeudsDuGraphe; int Pivot; int * Adjacent; int * Next;  double S0; double B0;
int Var; int Complement; int Edge; int Nv; char * LaCliqueEstDansLePool; double Sec;
int * NoeudDeClique; int * TypeDeBorne; BB * Bb; int NbCliquesViolees; char Ok;
char * CliqueDeTypeEgalite; int NbImplicationsViolees; double Seuil; 

if ( Pne->ConflictGraph == NULL ) return;
if ( Pne->Cliques == NULL ) return;

Bb = Pne->ProblemeBbDuSolveur;
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > PROFONDEUR_LIMITE_CLIQUES ) return;

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
UTrav = Pne->UTrav;
Cliques = Pne->Cliques;
First = Cliques->First;  
NbElements = Cliques->NbElements;
NoeudDeClique = Cliques->Noeud;
LaCliqueEstDansLePool = Cliques->LaCliqueEstDansLePool;
CliqueDeTypeEgalite = Cliques->CliqueDeTypeEgalite;
Pivot = Pne->ConflictGraph->Pivot;

Coeff = Pne->Coefficient_CG;
Indice = Pne->IndiceDeLaVariable_CG;

ControleCliques = OUI_PNE /*OUI_PNE*/;
ControleImplications = NON_PNE;
if ( Pne->Cliques->Full == OUI_PNE ) ControleImplications = OUI_PNE;

ControleImplications = OUI_PNE;

if ( ControleCliques != OUI_PNE ) goto Implications;

/* Attention: la partie simplexe considere que les variables intervenant dans les coupes ne
	 sont jamais de type fixe, c'est a dire qu'il y a toujours une correspondance des
	 les variables du simplexe. Il faut donc ne pas mettre ces coupes. */
NormeV = 0.0;
NbCliquesViolees = 0;

PNE_MiseAJourSeuilCoupes( Pne, COUPE_CLIQUE, &Seuil );

for ( c = 0 ; c < Cliques->NombreDeCliques ; c++ ) {
  if ( LaCliqueEstDansLePool[c] == OUI_PNE ) continue;  
  il = First[c];
	if ( il < 0 ) continue; /* On ne sait jamais ... */	
  ilMax = il + NbElements[c];
	S = 0;
  Sec = 1;
	Ok = OUI_PNE;
  while ( il < ilMax ) {
	  if ( NoeudDeClique[il] < Pivot ) {
	    if ( TypeDeBorne[NoeudDeClique[il]] == VARIABLE_FIXE ) {				
				Ok = NON_PNE;
			  break;
			}			
		  S += UTrav[NoeudDeClique[il]];
		}
		else {
	    if ( TypeDeBorne[NoeudDeClique[il]-Pivot] == VARIABLE_FIXE ) {				
				Ok = NON_PNE;
			  break;
			}			
		  S -= UTrav[NoeudDeClique[il]-Pivot];
			Sec -= 1;
		}
	  il++;
	}
	if ( Ok == OUI_PNE ) {

    if ( S - Sec > Seuil ) {
      Pne->SommeViolationsCliques += S - Sec;
      Pne->NombreDeCliques++;
    }
	
    if ( S - Sec > Pne->SeuilDeViolationCliques ) {			 
	    X = fabs( S - Sec );
	    NormeV += X;
      /* On Stocke la coupe */
      il = First[c];
		  NbT = 0;
      while ( il < ilMax ) {		
	      if ( NoeudDeClique[il] < Pivot ) {
		      Coeff[NbT] = 1;
			    Indice[NbT] = NoeudDeClique[il];			
			  }
	 	    else {
		      Coeff[NbT] = -1;
			    Indice[NbT] = NoeudDeClique[il] - Pivot;		
		    }			
			  NbT++;
	      il++;
	    }
		  NbCliquesViolees++;
      B = Sec;			
      PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbT, B, X, Coeff , Indice );
      Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees-1]->IndexDansCliques = c;
      if ( CliqueDeTypeEgalite[c] == OUI_PNE ) {        
				for ( il = 0 ; il < NbT ; il++ ) Coeff[il] *= -1;			 
			  B *= -1;       
				PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbT, B, X, Coeff , Indice );
        Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees-1]->IndexDansCliques = c;
				/*printf("ajout clique egalite %d\n",c);*/
		  }			
	  }
	}
}

if ( NbCliquesViolees <= 0 ) ControleImplications = OUI_PNE;

Implications:

if ( ControleImplications != OUI_PNE )  goto FinControles;

/* Maintenant on regarde les implications du graphe */

PNE_MiseAJourSeuilCoupes( Pne, COUPE_IMPLICATION, &Seuil );

NbImplicationsViolees = 0;
NormeV = 0.0;
ConflictGraph = Pne->ConflictGraph;
if ( ConflictGraph == NULL ) return;

NbNoeudsDuGraphe = ConflictGraph->NbNoeudsDuGraphe;
Pivot = ConflictGraph->Pivot;
Adjacent = ConflictGraph->Adjacent;
Next = ConflictGraph->Next;
First = ConflictGraph->First;

/* Attention: la partie simplexe considere que les variables intervenant dans les coupes ne
	 sont jamais de type fixe, c'est a dire qu'il y a toujours une correspondance des
	 les variables du simplexe. Il faut donc ne pas mettre ces coupes. */
	 
for ( Noeud = 0 ; Noeud < NbNoeudsDuGraphe ; Noeud++ ) {
  if ( First[Noeud] < 0 ) continue;
	if ( Noeud < Pivot ) {
		Var = Noeud;
	  Complement = Pivot + Noeud;
		S0 = UTrav[Var];
		Coeff[0] = 1;
		B0 = 1.;
	}
	else {
	  Var = Noeud - Pivot;
	  Complement = Noeud - Pivot;
		S0 = -UTrav[Var];
		B0 = 0.0;
		Coeff[0] = -1;
	}
	if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;	
	Indice[0] = Var;
	NbT = 1;
	
  Edge = First[Noeud];
	while ( Edge >= 0 ) {
    Nv = Adjacent[Edge];
		if ( Nv == Complement ) goto NextEdge;				
		if ( Nv < Pivot ) {
		  Var = Nv;
      S = S0 + UTrav[Var];
		  Coeff[1] = 1;
      B = B0;			
		}   
		else {
		  Var = Nv - Pivot;
      S = S0 - UTrav[Var];
			B = B0 - 1;
		  Coeff[1] = -1;
		}
	  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) goto NextEdge; 
	  Indice[1] = Var;
		NbT = 2;

    if ( S - B > Seuil ) {
      Pne->SommeViolationsImplications += S - B;
      Pne->NombreDImplications++;
    }

    if ( S - B > Pne->SeuilDeViolationCliques ) {			 
			/* 
		  printf("Noeud=%d Implication violee %e (%d=%e) %e (%d=%e) < %e\n",
			        Noeud,
			        Coeff[0],Indice[0],UTrav[Indice[0]],
			        Coeff[1],Indice[1],UTrav[Indice[1]],
							B);
			*/				
	    X = S - B;
	    NormeV += X;
      /* On Stocke la coupe */						
      PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbT, B, X, Coeff , Indice );
			NbImplicationsViolees++;
		}
		NextEdge:
	  Edge = Next[Edge];
	}
}

FinControles:
return;
}   















