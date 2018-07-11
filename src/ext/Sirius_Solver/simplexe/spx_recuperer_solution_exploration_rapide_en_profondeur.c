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

   FONCTION: Recuperation de la solution lorsqu'elle est entiere pendant
             l'exploration rapide en profondeur.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

#include "pne_define.h"

#include "bb_define.h"

/*----------------------------------------------------------------------------*/

void SPX_RecupererLaSolutionSiExplorationRapide( PROBLEME_SPX * Spx,
                                                 void * PneProb,
						 void * BbProb,
						 void * Noeud,
						 int * TypeInstanciation )  
{
PROBLEME_PNE * Pne; BB * Bb; NOEUD * NoeudEnExamen; int VarPne; int i;
int VarSpx; double X; int NombreDeVariablesBinairesPne; int * NumeroDesVariablesBinairesPne;
int * CorrespondanceVarEntreeVarSimplexe;

if ( Spx->YaUneSolution != OUI_SPX ) return;

Pne = (PROBLEME_PNE *) PneProb;
Bb  = (BB *) BbProb;

/* Le calcul du cout est fait dans SPX_ChoisirLaVariableAInstancier */
SPX_UnScaling( Spx );

/* On met dans X la valeur d'instanciation */
NombreDeVariablesBinairesPne  = Bb->NombreDeVariablesEntieresDuProbleme;
NumeroDesVariablesBinairesPne = Bb->NumerosDesVariablesEntieresDuProbleme;

CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;

for ( i = 0 ; i < NombreDeVariablesBinairesPne ; i++ ) {
  VarPne = NumeroDesVariablesBinairesPne[i];
  if ( TypeInstanciation[VarPne] == SORT_PAS ) continue;
  /* La variable a ete instanciee */
  VarSpx = CorrespondanceVarEntreeVarSimplexe[VarPne];
  if ( VarSpx < 0 || VarSpx >= Spx->NombreDeVariables ) continue;
  
  Spx->X[VarSpx] = 0.;
  if ( TypeInstanciation[VarPne] == SORT_SUR_XMAX ) Spx->X[VarSpx] = 1.;
  
}

NoeudEnExamen = (NOEUD *) Noeud;

Pne->NombreDeVariablesAValeurFractionnaire = 0;
Pne->NormeDeFractionnalite = 0.0;

Bb->NombreDeSolutionsEntieresTrouvees++;
NoeudEnExamen->LaSolutionRelaxeeEstEntiere = OUI;
NoeudEnExamen->MinorantDuCritereAuNoeud = Spx->Cout;

/*printf("Cout %e CoutMax %e\n",Spx->Cout,Spx->CoutMax);*/

/* Il faut mettre a jour Pne->UTrav car c'est utilise pour stocker la solution optimale
   entiere courante quand on passe dans NettoyerLArbre */

for ( VarPne = 0 ; VarPne < Pne->NombreDeVariablesTrav ; VarPne++ ) {
  if ( Pne->TypeDeBorneTrav[VarPne] == VARIABLE_FIXE ) continue;
  VarSpx = Spx->CorrespondanceVarEntreeVarSimplexe[VarPne];    
  X = Spx->X[VarSpx];
    
  if ( Spx->TypeDeVariable[VarSpx] == BORNEE || Spx->TypeDeVariable[VarSpx] == BORNEE_INFERIEUREMENT ) {
    Pne->UTrav[VarPne] = X + Spx->XminEntree[VarSpx]; 
  }
  else Pne->UTrav[VarPne] = X;
  
  if ( Pne->TypeDeBorneTrav[VarPne] == VARIABLE_BORNEE_SUPERIEUREMENT ) Pne->UTrav[VarPne] = -Pne->UTrav[VarPne]; 

}

return;
}
