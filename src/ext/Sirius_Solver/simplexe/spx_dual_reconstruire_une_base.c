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

   FONCTION: On reconstruit une base de depart a l'aide de la base optimale
	           du noeud racine sans coupes.
						 Pour les coupes du probleme en cours, on met toutes les
						 variables d'ecart en base.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        

# include "pne_define.h"		        

/*----------------------------------------------------------------------------*/

void SPX_DualReconstruireUneBase( PROBLEME_SPX * Spx , char * Echec )
{
PROBLEME_PNE * Pne; BB * Bb; NOEUD * Noeud; int * VariableEnBaseDeLaContrainte;
int * ContrainteDeLaVariableEnBase; int Cnt; int Var; int il;
char * PositionDeLaVariable; int * Mdeb; int * NbTerm; int * Indcol;
# ifdef UTILISER_BORNES_AUXILIAIRES   
  char * StatutBorneSupCourante;
# endif

*Echec = OUI_PNE;

Pne = (PROBLEME_PNE *) Spx->ProblemePneDeSpx;
if ( Pne != NULL ) {
  Bb = (BB *) Pne->ProblemeBbDuSolveur;
} 
else return;      

if ( Pne->YaDesVariablesEntieres != OUI_PNE ) return;
/* En particulier il n'y a pas de base dispo a la premiere resolution du noeud racine */
if ( Bb->BaseDisponibleAuNoeudRacine == NON ) return;

Noeud = Bb->NoeudRacine;

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
PositionDeLaVariable = Spx->PositionDeLaVariable;
Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) VariableEnBaseDeLaContrainte[Cnt] = -1;
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ )   ContrainteDeLaVariableEnBase[Var] = -1;

# ifdef UTILISER_BORNES_AUXILIAIRES   
  /* On enleve toutes les bornes auxiliaires car elles seront retablies si necessaire */
	StatutBorneSupCourante = Spx->StatutBorneSupCourante;
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
    if ( StatutBorneSupCourante[Var] != BORNE_NATIVE ) SPX_SupprimerUneBorneAuxiliaire( Spx, Var );					
  }
  if ( Spx->NombreDeBornesAuxiliairesUtilisees != 0 ) {
    #if VERBOSE_SPX
      printf("Attention probleme dans SPX_DualReconstruireUneBase: le nombre de bornes auxiliaires n'est pas nul (valeur = %d)\n",
				      Spx->NombreDeBornesAuxiliairesUtilisees);
		#endif
		return;
	}			
# endif

/* Construction de la base de depart (hors coupes) */
SPX_ConstruireLaBaseDuProblemeModifie( Spx, Bb->NombreDeVariablesDuProbleme,
                                       Noeud->PositionDeLaVariableSansCoupes,
                                       Noeud->NbVarDeBaseComplementairesSansCoupes,
                                       Noeud->ComplementDeLaBaseSansCoupes );
																						 
SPX_CompleterLaBaseDuProblemeModifie( Spx, Bb->NombreDeVariablesDuProbleme,
                                      Bb->NombreDeContraintesDuProbleme,
																			Noeud->PositionDeLaVariableSansCoupes );
/* On ajoute les variables basiques pour les coupes */
for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  /* Recherche de la variable d'ecart */
  il = Mdeb[Cnt] + NbTerm[Cnt] - 1;
	Var = Indcol[il];
	PositionDeLaVariable[Var] = EN_BASE_LIBRE;
	ContrainteDeLaVariableEnBase[Var] = Cnt;
  VariableEnBaseDeLaContrainte[Cnt] = Var;	
}

*Echec = NON_PNE;

/* Il est important de le mettre la car utilise dans SPX_InitialiserLeTableauDesVariablesHorsBase */
Spx->LeSteepestEdgeEstInitilise = NON_SPX;

return;
}
