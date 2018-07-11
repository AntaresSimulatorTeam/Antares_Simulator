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

   FONCTION: Determination des variables a valeur fractionnaire
                            
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
							     
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# define UTILISER_LES_POIDS_DU_SIMPLEXE NON_PNE

void PNE_ClasserLesVariableFractionnairesSteepestEdge( PROBLEME_PNE * , int , double , double * );

/*----------------------------------------------------------------------------*/

void PNE_DeterminerLesVariablesFractionnaires( PROBLEME_PNE * Pne, int * PositionDeLaVariable )
{
int Var; int LaValeurEstEntiere; double Sigma; double U1; double U2; double UTrav;
double Milieu; double Moyenne; double ValeurDeFractionnaliteNulle; 
# if UTILISER_LES_POIDS_DU_SIMPLEXE == OUI_PNE
  PROBLEME_SPX * Spx; double * ScaleX; int * CorrespondanceVarEntreeVarSimplexe;
	int * ContrainteDeLaVariableEnBase; int VarSpx; int CntSpx; double * DualPoids;
	double * Fractionnalite; double Frac;
# endif

# if UTILISER_LES_POIDS_DU_SIMPLEXE == OUI_PNE
  Spx = NULL;
  if ( Pne->ProblemeSpxDuSolveur != NULL ) {
    Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
  }
  else {
    printf("Bug dans PNE_DeterminerLesVariablesFractionnaires. On ne dispose pas du probleme Simplexe associe au probleme en nombres entiers etudie\n");
  }
	ScaleX  = Spx->ScaleX;
	CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;    
  ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
	DualPoids = Spx->DualPoids;
	Fractionnalite = Pne->UStrongBranching;
# endif

Pne->CestTermine                           = OUI_PNE;
Pne->VariableLaPlusFractionnaire           = -1;
Milieu                                     = 0.5;
Pne->NombreDeVariablesAValeurFractionnaire = 0;
Pne->NormeDeFractionnalite                 = 0.0; 
Pne->PremFrac                              = -1;

Moyenne = 0.0;
Sigma   = 0.0;

ValeurDeFractionnaliteNulle = VALEUR_DE_FRACTIONNALITE_NULLE;

/* Verification des valeurs entieres et choix de branchement. Pour finioler on peut boucler sur les seules variables entieres
   car il y existe maintenant une liste de ces variables. On peut acceder au type de variable par TypeDeVariable. Ceci dit
	 il n'y a pas grande difference. */
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  Pne->LaVariableAUneValeurFractionnaire[Var] = NON_PNE;
	if ( Pne->UminTrav[Var] == Pne->UmaxTrav[Var] ) continue;
  if ( Pne->TypeDeVariableTrav[Var] == ENTIER && Pne->TypeDeBorneTrav[Var] != VARIABLE_FIXE ) {
    LaValeurEstEntiere = OUI_PNE;
    UTrav = Pne->UTrav[Var];
		ValeurDeFractionnaliteNulle = Pne->SeuilDeFractionnalite[Var];
    if ( Pne->SolveurPourLeProblemeRelaxe == SIMPLEXE ) {
      /* Si la variable est basique sa valeur est potentiellement fractionnaire */
      if ( PositionDeLaVariable[Var] == EN_BASE ) {
        /* Ceci permet de resserer la tolerance */
				U1 = UTrav - floor( UTrav );
				U2 = ceil( UTrav ) - UTrav;				
        if( U1 > ValeurDeFractionnaliteNulle && U2 > ValeurDeFractionnaliteNulle ) {
          /*
	        if ( U1 < U2 ) printf("%d ValeurDeFractionnaliteNulle %e fractionnalite %e  \n",Var,ValeurDeFractionnaliteNulle,U1);
          else printf("%d ValeurDeFractionnaliteNulle %10.17e fractionnalite %10.17e  \n",Var,ValeurDeFractionnaliteNulle,U2);
				  */
          Pne->CestTermine   = NON_PNE;
          LaValeurEstEntiere = NON_PNE;
          Pne->LaVariableAUneValeurFractionnaire[Var] = OUI_PNE;
          Pne->NombreDeVariablesAValeurFractionnaire++;
	        if ( U1 < U2 ) { 
					  Pne->NormeDeFractionnalite+= U1;
						Sigma+= U1 * U1;
					}
	        else {
					  Pne->NormeDeFractionnalite+= U2;
						Sigma+= U2 * U2;
					}

          # if UTILISER_LES_POIDS_DU_SIMPLEXE == OUI_PNE
					  /* Avant classement on prend en compte les poids du steepest edge */
				    VarSpx = CorrespondanceVarEntreeVarSimplexe[Var];
					  if ( VarSpx < 0 ) printf("Bug dans PNE_DeterminerLesVariablesFractionnaires. Table Spx->CorrespondanceVarEntreeVarSimplexe fausse.\n");					  
					  CntSpx = ContrainteDeLaVariableEnBase[VarSpx];
					  if ( CntSpx < 0 ) printf("Bug dans PNE_DeterminerLesVariablesFractionnaires. Table Spx->ContrainteDeLaVariableEnBase fausse.\n");									
					  /* On scale pour se ramener au simplexe et on applique les poids du steepest edge */
				    U1 = U1 / ScaleX[VarSpx]; U1 = U1 * U1 / DualPoids[CntSpx];
				    U2 = U2 / ScaleX[VarSpx]; U2 = U2 * U2 / DualPoids[CntSpx];
            Frac = U1;
						if ( U1 < U2 ) Frac = U2;						
	          PNE_ClasserLesVariableFractionnairesSteepestEdge( Pne, Var, Frac, Fractionnalite );																
					# else			
            /* Classement de la variable en fonction de sa position par rapport au milieu */	  	  
            PNE_ClasserLesVariableFractionnaires( Pne, Var, Milieu );
					# endif
					
        }
      }      
    }
    else if( fabs( UTrav - floor( UTrav ) ) > ValeurDeFractionnaliteNulle && fabs( UTrav - ceil ( UTrav ) ) > ValeurDeFractionnaliteNulle ) {      
			Pne->CestTermine = NON_PNE;
      LaValeurEstEntiere = NON_PNE;
      Pne->LaVariableAUneValeurFractionnaire[Var] = OUI_PNE;
      Pne->NombreDeVariablesAValeurFractionnaire++;
      PNE_ClasserLesVariableFractionnaires( Pne, Var, Milieu );			
		}
  }
}					        

Pne->VariableLaPlusFractionnaire = Pne->PremFrac;

#if VERBOSE_PNE
  printf(" Nombre de variables a valeur fractionnaire: %d \n",Pne->NombreDeVariablesAValeurFractionnaire); fflush(stdout);
#endif
 /*
 {
  BB * Bb; 
  Bb = (BB *) Pne->ProblemeBbDuSolveur;
  printf(" Nombre de variables a valeur fractionnaire: %d profondeur du noeud: %d\n",
	         Pne->NombreDeVariablesAValeurFractionnaire,
					 Bb->NoeudEnExamen->ProfondeurDuNoeud);					 
 }
 */

/* Si on a trouve une solution entiere, on place les valeurs optimales des variables entieres sur la bonne borne
   pour ne pas avoir de probleme par la suite */
if ( Pne->NombreDeVariablesAValeurFractionnaire == 0 ) {
  for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
    if ( Pne->TypeDeVariableTrav[Var] == ENTIER && Pne->TypeDeBorneTrav[Var] != VARIABLE_FIXE ) {
	    U1 = fabs( Pne->UmaxTrav[Var] - Pne->UTrav[Var] );
	    U2 = fabs( Pne->UTrav[Var] - Pne->UminTrav[Var] );
			if ( U1 < U2 ) Pne->UTrav[Var] = Pne->UmaxTrav[Var];
			else Pne->UTrav[Var] = Pne->UminTrav[Var];
    }		    
  }
}					        

Pne->FaireDuStrongBranching = OUI_PNE;

return;
}

/*----------------------------------------------------------------------------*/
/*                Classe les variables entieres en fonction 
                  de leur valeur fractionnaire                                */

void PNE_ClasserLesVariableFractionnaires( PROBLEME_PNE * Pne, int i , double Milieu )
{
int ik; int ikPrec;

if ( Pne->PremFrac == -1 ) { /* C'est la premiere variable */
  Pne->PremFrac    =  i;
  Pne->SuivFrac[i] = -1;
  return;
}

/* C'est pas la premiere variable: on lui cherche un emplacement */
ik = Pne->PremFrac;
if ( fabs( Pne->UTrav[i ] - ( Milieu * Pne->UmaxTrav[i ] ) ) < 
     fabs( Pne->UTrav[ik] - ( Milieu * Pne->UmaxTrav[ik] ) ) ) {
  Pne->PremFrac    = i;
  Pne->SuivFrac[i] = ik; 
  return; 
}

/* C'est pas le meilleur */
ikPrec = ik;  
ik     = Pne->SuivFrac[ik];
while ( ik >= 0 ) {
  if ( fabs( Pne->UTrav[i ] - ( Milieu * Pne->UmaxTrav[i ] ) ) < 
       fabs( Pne->UTrav[ik] - ( Milieu * Pne->UmaxTrav[ik] ) ) ) {
    /* Emplacement trouve */
    Pne->SuivFrac[ikPrec] = i;
    Pne->SuivFrac[i]      = ik;
    return; 
  }
  ikPrec = ik;
  ik     = Pne->SuivFrac[ik];
}

/* C'est la plus mauvaise: classement de la variable a la fin de la liste */
Pne->SuivFrac[ikPrec] =  i;
Pne->SuivFrac[i]      = -1;

return;
}

/*----------------------------------------------------------------------------*/
/*                Classe les variables entieres en fonction                   */
/*                de leur valeur fractionnaire ponderee par                   */
/*								le steepest edge du simplexe                                */

void PNE_ClasserLesVariableFractionnairesSteepestEdge( PROBLEME_PNE * Pne, int Var,
                                                      double Frac,
                                                      double * Fractionnalite )
{
int ik; int ikPrec;

Fractionnalite[Var] = Frac;
if ( Pne->PremFrac == -1 ) { /* C'est la premiere variable */
  Pne->PremFrac    =  Var;
  Pne->SuivFrac[Var] = -1;
  return;
}

/* C'est pas la premiere variable: on lui cherche un emplacement */
ik = Pne->PremFrac;
if ( Frac > Fractionnalite[ik] ) {
  Pne->PremFrac    = Var;
  Pne->SuivFrac[Var] = ik;	
  return; 
}

/* C'est pas le meilleur */
ikPrec = ik;  
ik     = Pne->SuivFrac[ik];
while ( ik >= 0 ) {
  if ( Frac >  Fractionnalite[ik] ) {
    /* Emplacement trouve */
    Pne->SuivFrac[ikPrec] = Var;
    Pne->SuivFrac[Var] = ik;
    return; 
  }
  ikPrec = ik;
  ik = Pne->SuivFrac[ik];
}

/* C'est la plus mauvaise: classement de la variable a la fin de la liste */
Pne->SuivFrac[ikPrec] =  Var;
Pne->SuivFrac[Var] = -1;

return;
}
