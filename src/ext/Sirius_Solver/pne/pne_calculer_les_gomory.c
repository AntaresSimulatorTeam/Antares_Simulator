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

   FONCTION: Calcul des coupes 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h" 

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif 

# define NOMBRE_MAX_DE_COUPES_PAR_PROBLEME_RELAXE  1000000 /* Pas de limite */ /* C'est le nombre max de CG qu'on va calculer */

/*----------------------------------------------------------------------------*/

void PNE_Gomory( PROBLEME_PNE * Pne )   
{
int i; int Var; double UTrav; int Count; int Ok; int CountMx; int CountMN;
double Fractionnalite; BB * Bb; int NbCalculs;

NbCalculs = 0;

#if VERBOSE_PNE
  printf("-> Determination des coupes de GOMORY \n"); fflush(stdout);
#endif

if ( Pne->PremFrac >= 0 ) {
  Ok = SPX_PreparerLeCalculDesGomory( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
                                      Pne->NombreDeVariablesTrav , Pne->TypeDeVariableTrav );							        
  if ( Ok != 1 ) goto FinPneGomory;
}

Bb = (BB *) Pne->ProblemeBbDuSolveur;
CountMN = (int) ( 0.5 * Bb->NombreDeVariablesEntieresDuProbleme);
if ( CountMN <= 0 ) CountMN = 1;

CountMN = 200; 

CountMx = (int) ( 0.1 * Pne->NombreDeVariablesAValeurFractionnaire ); 

if ( CountMx < CountMN ) CountMx = CountMN;
  
Count = 0; 
i     = Pne->PremFrac;

while ( i >= 0 && Count < CountMx ) {   
  Var = i;
  if ( Var < 0 || Var >= Pne->NombreDeVariablesTrav ) goto NextVar;
		
  /* Test supplementaire au cas ou on voudrait faire des coupes sur des variables a valeurs 
     presque entieres */	
  UTrav = Pne->UTrav[Var];
	Fractionnalite = fabs( UTrav - floor( UTrav ) );
	if ( fabs( UTrav - ceil ( UTrav ) ) < Fractionnalite ) Fractionnalite = fabs( UTrav - ceil ( UTrav ) );
	
  if( Fractionnalite < SEUIL_FRACTIONNALITE_POUR_FAIRE_UNE_COUPE_DE_GOMORY ) break;

	NbCalculs++;
  PNE_CalculerUneGomoryEnVariablesMixtes( Pne, Var, Fractionnalite );
  if ( Pne->NombreDeCoupesAjoute >= NOMBRE_MAX_DE_COUPES_PAR_PROBLEME_RELAXE ) break;	 
	
  Count++;

  NextVar:
  i = Pne->SuivFrac[i];

}  

FinPneGomory:

#if VERBOSE_PNE
  printf("-> Nombre de coupes de GOMORY ajoutees %d \n",Pne->NombreDeCoupesAjoute); 
#endif
/*
  printf("-> Nombre de coupes de GOMORY ajoutees %d sur %d  NombreDeVariablesAValeurFractionnaire %d\n",
	Pne->NombreDeCoupesAjoute,NbCalculs,Pne->NombreDeVariablesAValeurFractionnaire); 
*/
if ( Pne->PremFrac >= 0 ) SPX_TerminerLeCalculDesGomory( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );

return;
}

/*----------------------------------------------------------------------------*/
/*              Calcul d'une coupe de gomory en variables mixtes              */

void PNE_CalculerUneGomoryEnVariablesMixtes( PROBLEME_PNE * Pne, int VariableFractionnaire,
                                             double Fractionnalite )
{  
int   i; int NombreDeTermes  ; double SecondMembre; double S; double PlusGrandCoeff; double X; double Y; 
int NombreDeVariablesTrav; int * IndiceDeLaVariable; double * Coefficient; double * UTrav; char OnAEcrete;
double RapportMaxDesCoeffs; double ZeroPourCoeffVariablesDEcart; double ZeroPourCoeffVariablesNative;
double RelaxRhsAbs; double RelaxRhsRel;

RapportMaxDesCoeffs = RAPPORT_MAX_COEFF_COUPE_GOMORY;
ZeroPourCoeffVariablesDEcart = ZERO_POUR_COEFF_VARIABLE_DECART_DANS_COUPE_GOMORY_OU_INTERSECTION;
ZeroPourCoeffVariablesNative = ZERO_POUR_COEFF_VARIABLE_NATIVE_DANS_COUPE_GOMORY_OU_INTERSECTION;
RelaxRhsAbs = RELAX_RHS_GOMORY_ABS;
RelaxRhsRel = RELAX_RHS_GOMORY_REL;

IndiceDeLaVariable = Pne->IndiceDeLaVariable_CG;
Coefficient = Pne->Coefficient_CG;

/*
printf("Coupe de Gomory sur la variable %d valeur %lf\n",VariableFractionnaire,Pne->UTrav[VariableFractionnaire]); fflush(stdout);
*/

SPX_CalculerUneCoupeDeGomory(
       (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur,
       VariableFractionnaire,
			 
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
			 
if ( NombreDeTermes <= 0 ) return;

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
UTrav = Pne->UTrav;
PlusGrandCoeff = -LINFINI_PNE;
S = 0.0;
for ( i = 0 ; i < NombreDeTermes ; i++ ) {		
  Y = Coefficient[i];
  if ( IndiceDeLaVariable[i] < NombreDeVariablesTrav ) {
    S += Y * UTrav[IndiceDeLaVariable[i]];
	  if ( fabs( Y ) > PlusGrandCoeff ) PlusGrandCoeff = fabs( Y );
  }
}
  
if ( S > SecondMembre ) {  
  X = fabs( SecondMembre - S );	 
  if ( X > SEUIL_VIOLATION_COUPE_DE_GOMORY ) {
		 
    /* Mise a jour de la matrice pour les coupes d'intersection sauf si la coupe a trop de termes car on perd pas mal de
		   precision dans les coupes d'intersection. Le coeff sur le nombre de variables est totalement arbitraire */
		
		if ( Fractionnalite > SEUIL_FRACTIONNALITE_POUR_COUPE_INTERSECTION /*&& NombreDeTermes < (0.95 * NombreDeVariablesTrav)*/ ) {   
      SPX_MatriceCoupesDIntersection( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );
		}	 
		 	
    /*
    printf("Coupe de gomory Violation %e NombreDeTermes %d (max.: %d)\n",X,NombreDeTermes,Pne->NombreDeVariablesTrav);   
		for ( i = 0 ; i < NombreDeTermes ; i++ ) {
		  printf(" %e (%d) + ",Coefficient[i],IndiceDeLaVariable[i]);
		}
		printf(" < %e\n",SecondMembre);
		fflush( stdout );
    */
		  
    /* On met la Gomory dans le probleme */
		
    PNE_NormaliserUnCoupe( Pne->Coefficient_CG, &SecondMembre, NombreDeTermes, PlusGrandCoeff );
	 
    PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'G', NombreDeTermes, SecondMembre, X, Coefficient, IndiceDeLaVariable );     		
		
    /* Test 2 MIR cuts sur le tableau du simplexe: pas concluant pour l'instant */  
    /*
	 	double ValeurDuZero;
	 	ValeurDuZero = 1.e-11;
		PNE_TwoStepMirSurTableau( Pne, VariableFractionnaire, ValeurDuZero );
		*/
		/* Fin test */  
 

																							 
		# if KNAPSACK_SUR_GOMORY == OUI_SPX
		  /* Il faut le faire apres le stockage de la coupe car la recherche des knapsack modifie
			   Pne->Coefficient_CG et Pne->IndiceDeLaVariable_CG */
      /* On regarde si on peut faire une K sur la coupe */
			if ( X > VIOLATION_MIN_POUR_K_SUR_COUPE && OnAEcrete != OUI_SPX ) {
        PNE_CalculerUneKnapsackSurGomoryOuIntersection( Pne, Coefficient, IndiceDeLaVariable,
										  																  SecondMembre, NombreDeTermes, PlusGrandCoeff );		
		  }
		# endif
																							 
  }  	
  else { 
	  /*
    printf("     -> Coupe refusee car distance au plan trop petite valeur %e valeur variable fractionnaire %e\n",
             X,Pne->UTrav[VariableFractionnaire]);
	  */
  }
	
}
  
else {
  /*
  printf("PNE_CalculerUneGomoryEnVariablesMixtes: attention la solution courante verifie la contrainte S = %lf < SecondMembre = %lf \n",
		      S,SecondMembre);   
 
	*/
}

   
return;
}

