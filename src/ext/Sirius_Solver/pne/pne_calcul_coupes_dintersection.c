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

   FONCTION: Calcul des coupes d'intersection (necessite d'avoir
             calcule les gomory avant)
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

/*----------------------------------------------------------------------------*/

void PNE_CalculerLesCoupesDIntersection( PROBLEME_PNE * Pne )
{
int i; int j; int NbPotentielDeCoupes; int NombreDeTermes; double S; double X;
double SecondMembre; double PlusGrandCoeff; char OnAEcrete; double Y; int NombreDeVariablesTrav;
double RapportMaxDesCoeffs; double ZeroPourCoeffVariablesDEcart; double ZeroPourCoeffVariablesNative;
double RelaxRhsAbs; double RelaxRhsRel; int * IndiceDeLaVariable; double * Coefficient;
double * UTrav;

RapportMaxDesCoeffs = RAPPORT_MAX_COEFF_COUPE_INTERSECTION;
ZeroPourCoeffVariablesDEcart = ZERO_POUR_COEFF_VARIABLE_DECART_DANS_COUPE_GOMORY_OU_INTERSECTION;
ZeroPourCoeffVariablesNative = ZERO_POUR_COEFF_VARIABLE_NATIVE_DANS_COUPE_GOMORY_OU_INTERSECTION;
RelaxRhsAbs = RELAX_RHS_INTERSECTION_ABS;
RelaxRhsRel = RELAX_RHS_INTERSECTION_REL;

IndiceDeLaVariable = Pne->IndiceDeLaVariable_CG;
Coefficient = Pne->Coefficient_CG;
NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
UTrav = Pne->UTrav;

/* C'est la reduction des normes qui prend le plus de temps et de loin */
SPX_ReductionDesNormesPourCoupesDIntersection( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );

NbPotentielDeCoupes = SPX_NombrePotentielDeCoupesDIntersection( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );

/* NbPotentielDeCoupes ne joue pas sur le temps car SPX_CalculerUneCoupeDIntersection est tres rapide */

# if CALCULER_COUPES_DINTERSECTION == NON_PNE
  SPX_TerminerLeCalculDesCoupesDIntersection( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );
  return;	
# endif

for ( i = 0 ; i < NbPotentielDeCoupes ; i++ ) {
  SPX_CalculerUneCoupeDIntersection(
              (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,  
              i, 
			 
              RapportMaxDesCoeffs,
			        ZeroPourCoeffVariablesDEcart,
			        ZeroPourCoeffVariablesNative,
              RelaxRhsAbs,
              RelaxRhsRel,
			 							
              &NombreDeTermes,  
              Coefficient,  
              IndiceDeLaVariable,
              &SecondMembre,
							&OnAEcrete );
							
  if ( NombreDeTermes <= 0 ) continue;
  	    
  PlusGrandCoeff = -LINFINI_PNE;  
  S = 0.0;
  for ( j = 0 ; j < NombreDeTermes ; j++ ) {
    Y = Coefficient[j];
    if ( IndiceDeLaVariable[j] < NombreDeVariablesTrav ) {
      S += Y * UTrav[IndiceDeLaVariable[j]];
	    if ( fabs( Y ) > PlusGrandCoeff ) PlusGrandCoeff = fabs( Y );
    }   
  }
	
  if ( S > SecondMembre ) {  
    X = fabs( SecondMembre - S );	 
    if ( X > SEUIL_VIOLATION_COUPE_DINTERSECTION ) {
		
		  /*
      printf("Coupe d'Intersection Violation %e NombreDeTermes %d (max.: %d)\n",X,NombreDeTermes,Pne->NombreDeVariablesTrav);   
		  for ( i = 0 ; i < NombreDeTermes ; i++ ) {
		    printf(" %e (%d) + ",Coefficient[i],IndiceDeLaVariable[i]);
		  }
		  printf(" < %e\n",SecondMembre);
		  fflush( stdout );			
			*/
			
      PNE_NormaliserUnCoupe( Pne->Coefficient_CG, &SecondMembre, NombreDeTermes, PlusGrandCoeff );
			
			PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'I', NombreDeTermes, SecondMembre, X,
                                                 Coefficient, IndiceDeLaVariable );

		  # if KNAPSACK_SUR_COUPE_DINTERSECTION == OUI_SPX
				/* Il faut le faire apres le stockage de la coupe car la recherche des knapsack modifie
			     Pne->Coefficient_CG et Pne->IndiceDeLaVariable_CG */
        /* On regarde si on peut faire une K sur la coupe */
			  if ( X > VIOLATION_MIN_POUR_K_SUR_COUPE && OnAEcrete != OUI_SPX ) {
          PNE_CalculerUneKnapsackSurGomoryOuIntersection( Pne, Coefficient, IndiceDeLaVariable,
																										      SecondMembre, NombreDeTermes, PlusGrandCoeff );		
		    }
			# endif
																								 
    }
  }
		
}

SPX_TerminerLeCalculDesCoupesDIntersection( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );

return;
}

