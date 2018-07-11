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

   FONCTION: Detection des coupes de knapsack negliges violees.
                 
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

# define PROFONDEUR_LIMITE_COUPES_DE_K 10

/*----------------------------------------------------------------------------*/

void PNE_DetectionKnapsackNegligeesViolees( PROBLEME_PNE * Pne )
{
double S; int Cnt; double E; int * First; int * NbElements; int NbV; double NormeV;
int il; int ilMax; double * Coeff; int * Indice; double B; int NbT; double * X;
int * Colonne; char * LaCoupeEstDansLePool; double * SecondMembre;
double * Coefficient; COUPES_K_NEGLIGEES * CoupesKNegligees; double Marge;
int * TypeDeBorne; BB * Bb; 

Marge = 0.01;
   
Bb = Pne->ProblemeBbDuSolveur;
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > PROFONDEUR_LIMITE_COUPES_DE_K ) return;

if ( Pne->CoupesKNegligees == NULL ) return;
CoupesKNegligees = Pne->CoupesKNegligees;

TypeDeBorne = Pne->TypeDeBorneTrav;
X = Pne->UTrav;
Coeff = Pne->Coefficient_CG;
Indice = Pne->IndiceDeLaVariable_CG;

First = CoupesKNegligees->First;
LaCoupeEstDansLePool = CoupesKNegligees->LaCoupeEstDansLePool;
NbElements = CoupesKNegligees->NbElements;
SecondMembre = CoupesKNegligees->SecondMembre;
Colonne = CoupesKNegligees->Colonne;
Coefficient = CoupesKNegligees->Coefficient;
	 
NbV = 0;
NormeV = 0.0;
for ( Cnt = 0 ; Cnt < CoupesKNegligees->NombreDeCoupes ; Cnt++ ) {
  if ( First[Cnt] < 0 ) continue;	
  if ( LaCoupeEstDansLePool[Cnt] == OUI_PNE ) continue;
  il = First[Cnt];
	ilMax = il + NbElements[Cnt];
	S = 0;
	NbT = 0;
	B = CoupesKNegligees->SecondMembre[Cnt];
	while ( il < ilMax ) {
	  if ( TypeDeBorne[Colonne[il]] != VARIABLE_FIXE ) {
      /* Car sinon il n'y a pas de variable correspondante dans le simplexe */	
      S += Coefficient[il] * X[Colonne[il]];
		  Coeff[NbT] = Coefficient[il];		 
	    Indice[NbT] = Colonne[il];				
		  NbT++;
		}
		else {
      B -= Coefficient[il] * X[Colonne[il]];
		}		
		il++;
	}
  if ( S > B + Marge && NbT > 0 ) {
    /*
    printf("NbT %d depassement %e\n",NbT,S-B);
		for ( il = 0 ; il < NbT ; il++ ) {
		  printf(" %e (%d) ",Coeff[il],Indice[il]);
		}
		printf("< %e\n",B);
	  */
	  E = S - B;
	  NormeV += E;
    /* On Stocke la coupe */
		NbV++;
    PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbT, B, E, Coeff, Indice );
    Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees-1]->IndexDansKNegligees = Cnt; 
	}
}

if ( Pne->AffichageDesTraces == OUI_PNE &&  NbV > 0 ) {
  printf("Adding %d initially neglected Knapsack cuts violated by %e\n",NbV,NormeV);
	fflush( stdout );
}

return;
}   















