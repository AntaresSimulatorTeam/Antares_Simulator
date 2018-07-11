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

   FONCTION: En exploration rapide (pilotee par le branch and bound),
             creation des noeuds fils d'un noeud qui vient d'etre
	     evalue.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

#include "pne_define.h"

#include "bb_define.h"
#include "bb_fonctions.h"

/*----------------------------------------------------------------------------*/

/* On connait le noeud courant, c'est celui qui vient d'être examiné */
void Spx_CreationNoeudsFils( PROBLEME_SPX * Spx, void * PneProb, void * BbProb,
                             int VariablesPneAInstancier )
{ 
NOEUD * NoeudPere; NOEUD * Noeud; NOEUD * NoeudEnExamen; /*int CodeRetour ;*/ int FilsACreer; int ValeurDInstanciation;
int NombreDeVariablesAInstancier; int NumerosDesVariablesAInstancier[1]; int Var; int VariableDEcart;
int NbVarDeBaseComplementaires; int NbT; int il; int ilMax; int Cnt; int Cnt_E; int NumCoupe;
int NombreDeVariablesPNE; int VarPne; int YaUneSolution; double ValeurDuCritereAuNoeud;

PROBLEME_PNE * Pne; BB * Bb;

Pne = (PROBLEME_PNE *) PneProb;
Bb  = (BB *) BbProb;

NombreDeVariablesPNE = Pne->NombreDeVariablesTrav;

NoeudEnExamen = Bb->NoeudEnExamen;

ValeurDuCritereAuNoeud = Spx->Cout;

/* La base de depart des fils sera la base courante */

NombreDeVariablesAInstancier = 1;
NumerosDesVariablesAInstancier[0] = VariablesPneAInstancier;

YaUneSolution = OUI;

/* Pour eviter de supprimer ce noeud lors du nettoyage de l'arbre, on diminue la
   valeur du minorant par rapport au cout de la solution entiere */ 
/* Il y a un test dans NettoyerArbre ou on enleve 1.e-9 a la meilleure solution entiere */
if ( ValeurDuCritereAuNoeud >= Bb->CoutDeLaMeilleureSolutionEntiere - 1.e-9 ) {
  ValeurDuCritereAuNoeud = Bb->CoutDeLaMeilleureSolutionEntiere - 2.e-9 ; 
}
NoeudEnExamen->MinorantDuCritereAuNoeud = ValeurDuCritereAuNoeud;

/* printf("Cout %e CoutMax %e\n",Spx->Cout,Spx->CoutMax); */

BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudEnExamen ); /* Fait aussi la mise a jour du statut */

Bb->BasesFilsDisponibles = NON_PNE;

for ( VarPne = 0 ; VarPne < NombreDeVariablesPNE ; VarPne++ ) NoeudEnExamen->PositionDeLaVariable[VarPne] = -1;

for ( VarPne = 0 ; VarPne < NombreDeVariablesPNE ; VarPne++ ) {
  Var = Spx->CorrespondanceVarEntreeVarSimplexe[VarPne];
  if ( Var >= 0 ) { 
    if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {
      NoeudEnExamen->PositionDeLaVariable[VarPne] = HORS_BASE_SUR_BORNE_SUP;
    }
    else if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
      NoeudEnExamen->PositionDeLaVariable[VarPne] = HORS_BASE_SUR_BORNE_INF;
    }
    else if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_A_ZERO ) {
      NoeudEnExamen->PositionDeLaVariable[VarPne] = HORS_BASE_A_ZERO;
    }
    else {
      /* Une variable native est basique. Si la contrainte de cette variable en base 
         est une coupe, on va considerer que cette coupe est saturee. En principe, elle 
         sera conservee. */ 
      NoeudEnExamen->PositionDeLaVariable[VarPne] = EN_BASE;
    }
  } 
}

NbVarDeBaseComplementaires = 0;
for ( Var = 0 ; Var < Spx->NombreDeVariablesDuProblemeSansCoupes ; Var++ ) {

  if ( Spx->OrigineDeLaVariable[Var] == NATIVE ) continue;

  if ( Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ||
       Spx->PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF || 
       Spx->PositionDeLaVariable[Var] == HORS_BASE_A_ZERO ) continue;
       
  /* La variable est en base, la colonne de la variable doit avoir un seul terme */
  NbT   = 0;
  il    = Spx->Cdeb[Var];
  ilMax = il + Spx->CNbTerm[Var];
  while ( il < ilMax ) {
    NbT++;
    Cnt = Spx->NumeroDeContrainte[il];
    il++;
  }
  if ( NbT != 1 ) {
    printf("Bug dans SPX_RecupererLaSolution nombre de termes dans la colonne de la variable %d incorrect, %d\n",Var,NbT); 
    Spx->AnomalieDetectee = OUI_SPX;
    longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
  Cnt_E = Spx->CorrespondanceCntSimplexeCntEntree[Cnt];
  if ( Cnt_E >= 0 ) {
    NoeudEnExamen->ComplementDeLaBase[NbVarDeBaseComplementaires] = Cnt_E;	
    NbVarDeBaseComplementaires = NbVarDeBaseComplementaires + 1;
  }
}
NoeudEnExamen->NbVarDeBaseComplementaires = NbVarDeBaseComplementaires;

/* Il faut determiner l'etat de saturation des coupes */

NumCoupe = 0;
if ( Spx->NombreDeContraintes - Spx->NombreDeContraintesDuProblemeSansCoupes != Bb->NombreDeCoupesAjoutees ) {
  printf("pb Bb->NombreDeCoupesAjoutees dans SPX\n");
}
for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  /* On cherche la variable Cnt_E correspondante */
  /* while ( NbTermCoupes[Cnt_E] <= 0 ) NumCoupe; attention je crois que dans certains car il peut y avoir 0 termes dans une coupe
     oui mais alors elle ne sont pas rpise en compte dans modifier probleme mais NumCoupe il y a probleme dans la
     numeortation . Il peut y avoir 0 termes car il peut y avoir appel a une fonction qui met ca a
     O pour le noeud racine dans PNE trier les coupes */
  /* Variable d'ecart */
  VariableDEcart = Spx->Indcol[Spx->Mdeb[Cnt] + Spx->NbTerm[Cnt] - 1];
  if ( Spx->PositionDeLaVariable[VariableDEcart] == HORS_BASE_SUR_BORNE_INF ) { 
    /* La coupe est saturee */
    Bb->CoupeSaturee[NumCoupe] = OUI_PNE;
  }
  else {
    /* La coupe n'est pas saturee */
    Bb->CoupeSaturee[NumCoupe] = NON_PNE;
  }
  NumCoupe++;
}
		   
NoeudPere = NoeudEnExamen;

FilsACreer = FILS_GAUCHE;
ValeurDInstanciation = 0;
Noeud = BB_AllouerUnNoeud( Bb,
			   NoeudPere,                         
                           NoeudPere->ProfondeurDuNoeud + 1,
			   FilsACreer,
                           ValeurDInstanciation,
			   NombreDeVariablesAInstancier,
			   NumerosDesVariablesAInstancier,
			   ValeurDuCritereAuNoeud );
NoeudPere->NoeudSuivantGauche = Noeud;

SPX_SauvegarderLaBaseDeDepart( Spx , (void *) Noeud ); 

FilsACreer = FILS_DROIT;
ValeurDInstanciation = 1;
Noeud = BB_AllouerUnNoeud( Bb,
			   NoeudPere,                         
                           NoeudPere->ProfondeurDuNoeud + 1,
			   FilsACreer,
                           ValeurDInstanciation,
			   NombreDeVariablesAInstancier,
			   NumerosDesVariablesAInstancier,
			   ValeurDuCritereAuNoeud );
NoeudPere->NoeudSuivantDroit = Noeud;

SPX_SauvegarderLaBaseDeDepart( Spx , (void *) Noeud ); 
      
return;
}
