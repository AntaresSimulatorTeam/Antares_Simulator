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

   FONCTION: Detection des contraintes de borne variable de probing violees.
	           Les contraintes de borne variable sont crees a la creation du conflict graph.
                 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"
 
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define PROFONDEUR_LIMITE_CONTRAINTES_DE_BORNE_VARIABLE 1000000  /*10*/ 
# define MAX_COUPES_DE_BORNE_VARIABLE 1000

# define TESTER_LE_CONDITIONNEMENT OUI_PNE
# define RAPPORT_MAX 1.e+4 

# define TRACES NON_PNE

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE

/*----------------------------------------------------------------------------*/

void PNE_DetectionDesContraintesDeBorneVariableViolees( PROBLEME_PNE * Pne )
{
double S; int Cnt; double E; int * First; int NbV; double NormeV; int il; int ilMax; double * Coeff;
int * Indice; double B; int NbT; double * X; int * Colonne; double * SecondMembre; int * TypeDeBorne;
BB * Bb; double * Coefficient; char * LaContrainteDeBorneVariableEstDansLePool;
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int NombreDeContraintesDeBorne; double Seuil;
# if TESTER_LE_CONDITIONNEMENT == OUI_PNE 			
  PROBLEME_SPX * Spx; double PlusPetitTermeDeLaMatrice; double PlusGrandTermeDeLaMatrice;
  double * ScaleX; int * CorrespondanceVarEntreeVarSimplexe; int VarSpx; double ValeurMin;
	double ValeurMax; double RapportDeScaling; double Valeur; double RapportCoupe;
# endif

# if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE
  # if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE
    return;
  # endif
# endif

if ( Pne->ContraintesDeBorneVariable == NULL ) return;

Bb = Pne->ProblemeBbDuSolveur;
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > PROFONDEUR_LIMITE_CONTRAINTES_DE_BORNE_VARIABLE ) return;

# if TESTER_LE_CONDITIONNEMENT == OUI_PNE 			
  Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
  if ( Spx == NULL ) return;
  CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
  PlusPetitTermeDeLaMatrice = Spx->PlusPetitTermeDeLaMatrice;
  PlusGrandTermeDeLaMatrice = Spx->PlusGrandTermeDeLaMatrice;	
  ScaleX = Spx->ScaleX;
	RapportDeScaling = Spx->RapportDeScaling;
	if ( RapportDeScaling < RAPPORT_MAX ) RapportDeScaling = RAPPORT_MAX;	
# endif

ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;

PNE_MiseAJourSeuilCoupes( Pne, COUPE_DE_BORNE_VARIABLE, &Seuil );

TypeDeBorne = Pne->TypeDeBorneTrav;
X = Pne->UTrav;
Coeff = Pne->Coefficient_CG;
Indice = Pne->IndiceDeLaVariable_CG;

First = ContraintesDeBorneVariable->First;
LaContrainteDeBorneVariableEstDansLePool = ContraintesDeBorneVariable->LaContrainteDeBorneVariableEstDansLePool;
SecondMembre = ContraintesDeBorneVariable->SecondMembre;
Colonne = ContraintesDeBorneVariable->Colonne;
Coefficient = ContraintesDeBorneVariable->Coefficient;

/* Attention: la partie simplexe considere que les variables intervenant dans les coupes ne
	 sont jamais de type fixe, c'est a dire qu'il y a toujours une correspondance des
	 les variables du simplexe. Il faut donc ne pas mettre ces coupes. */
	 
NbV = 0;
NormeV = 0.0;
NombreDeContraintesDeBorne = ContraintesDeBorneVariable->NombreDeContraintesDeBorne;

for ( Cnt = 0 ; Cnt < NombreDeContraintesDeBorne ; Cnt++ ) {
  if ( NbV >= MAX_COUPES_DE_BORNE_VARIABLE ) break;
  if ( LaContrainteDeBorneVariableEstDansLePool[Cnt] == OUI_PNE ) continue;
  if ( First[Cnt] < 0 ) continue;	
  il = First[Cnt];
	ilMax = il + 2;
	S = 0;
	NbT = 0;
	B = SecondMembre[Cnt];
  # if TESTER_LE_CONDITIONNEMENT == OUI_PNE 			
    ValeurMin = LINFINI_PNE;
	  ValeurMax = -LINFINI_PNE;
	# endif
	while ( il < ilMax ) {
	  if ( TypeDeBorne[Colonne[il]] != VARIABLE_FIXE ) {
      /* Car sinon il n'y a pas de variable correspondante dans le simplexe */
			/* Test de conditionnement */
      # if TESTER_LE_CONDITIONNEMENT == OUI_PNE 			
			  VarSpx = CorrespondanceVarEntreeVarSimplexe[Colonne[il]];
			  if ( VarSpx < 0 ) {
				  NbT = 0;
					break;
				}
			  Valeur = fabs( Coefficient[il] * ScaleX[VarSpx] );
				if ( Valeur > ValeurMax ) ValeurMax = Valeur;
				if ( Valeur < ValeurMin ) ValeurMin = Valeur;		    
			# endif
			/*  */		 
      S += Coefficient[il] * X[Colonne[il]];
		  Coeff[NbT] = Coefficient[il];		 
	    Indice[NbT] = Colonne[il];				
		  NbT++;
		}
		else {
      NbT = 0; /* Pour qu'elle ne soit pas violee */
      break;			
		}		
		il++;
	}
	if ( NbT != 2 ) continue;

  if ( S - B > Seuil ) {
    Pne->SommeViolationsBornesVariables += S - B;
    Pne->NombreDeBornesVariables++;
  }
	
  if ( S - B > Pne->SeuilDeViolationBornesVariables ) {			 
	  E = S - B;

    # if TESTER_LE_CONDITIONNEMENT == OUI_PNE
	    RapportCoupe = ValeurMax / ValeurMin;
		  if ( RapportCoupe > RapportDeScaling ) continue;
			RapportCoupe = 1.;
      # ifdef ON_COMPILE			
      if ( ValeurMax > PlusGrandTermeDeLaMatrice ) {
        /* On fait une homothetie vers PlusGrandTermeDeLaMatrice */
			  RapportCoupe = PlusGrandTermeDeLaMatrice / ValeurMax;
			}
			else if ( ValeurMin < PlusPetitTermeDeLaMatrice ) {
        /* On fait une homothetie vers PlusPetitTermeDeLaMatrice */
			  RapportCoupe = PlusPetitTermeDeLaMatrice / ValeurMin;
			}
			# endif
			
      if ( ValeurMax > PlusGrandTermeDeLaMatrice ) {
        /* On fait une homothetie vers PlusGrandTermeDeLaMatrice */
			  RapportCoupe = PlusGrandTermeDeLaMatrice / ValeurMax;
			}
			
			if ( RapportCoupe != 1. ) {
			  /*SPX_ArrondiEnPuissanceDe2( &RapportCoupe );*/				
			  for ( NbT = 0 ; NbT < 2 ; NbT++ ) Coeff[NbT] *= RapportCoupe;				
			  B *= RapportCoupe;				
			}
    # endif
	
	  NormeV += E;
    /* On Stocke la coupe */
		NbV++;
    # if TRACES == OUI_PNE
		  printf("Ajout de la contrainte de borne variable %d : \n",Cnt);
			printf("valeur des variables: var. continue %e (%d) var. binaire %e (%d)\n",X[Indice[0]],Indice[0],X[Indice[1]],Indice[1]);
		  printf("%e (%d) + %e (%d) < %e  violation: %e\n",Coeff[0],Indice[0],Coeff[1],Indice[1],B,E);
		# endif
    PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbT, B, E, Coeff, Indice );
    Pne->CoupesCalculees[Pne->NombreDeCoupesCalculees-1]->IndexDansContraintesDeBorneVariable = Cnt;
	}
}

if ( Pne->AffichageDesTraces == OUI_PNE &&  NbV > 0 ) {
  /*
  printf("Adding %d variable bound constraint violated by %e\n",NbV,NormeV);
	fflush( stdout );
	*/
}

return;
}

/*----------------------------------------------------------------------------*/

# endif
