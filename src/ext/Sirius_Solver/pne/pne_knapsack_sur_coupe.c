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

   FONCTION: Calcul d'une couope de knapsack sur une coupe de Gomory
	           ou sur une coupe d'intersection.
                
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

# define RAPPORT_MAX 1.e+5
# define ZERO_COEFFMN 1.e-4

# define MAX_TERMES_POUR_KNAPSACK_DANS_COUPE 100

/*----------------------------------------------------------------------------*/

void PNE_CalculerUneKnapsackSurGomoryOuIntersection( PROBLEME_PNE * Pne,  
                                                     double * Coefficient_CG,
																										 int * IndiceDeLaVariable_CG,
																										 double SecondMembre,
																										 int NombreDeTermes,
																										 double PlusGrandCoeff )
{
int i; char Kpossible; int Var; char CouvertureTrouvee; int NbT; double CoeffMx; double CoeffMn;
double * Coeff; int * Variable; int * TypeDeBorneTrav; double * UminTrav; double Coefficient;
int * TypeDeVariableTrav; double * UmaxTrav; char Trouve; char RendreLesCoeffsEntiers;
char Mixed_0_1_Knapsack; 

int Cnt1; double bBorne; int VarBin; double u; double l; char Found; int i1;

# if NORMALISER_LES_COUPES_SUR_LES_G_ET_I == OUI_PNE
  double Normalisation;
# endif

# if NORMALISER_LES_COUPES_SUR_LES_G_ET_I != OUI_PNE
  PlusGrandCoeff = 1.; /* Pour ne pas avoir de warning a a compilation */
# endif

Mixed_0_1_Knapsack = NON_PNE;

Kpossible = OUI_PNE;
NbT = 0;
Coeff = Pne->ValeurLocale;
Variable = Pne->IndiceLocal;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;

/* Si les Gomory et les coupes d'intersection sont normalisees, on revient aux valeurs initiales */
# if NORMALISER_LES_COUPES_SUR_LES_G_ET_I == OUI_PNE
  Normalisation = PlusGrandCoeff;  
  SPX_ArrondiEnPuissanceDe2( &Normalisation );
  for ( i = 0 ; i < NombreDeTermes ; i++ ) Coefficient_CG[i] *= Normalisation;          
  SecondMembre *= Normalisation;
# endif

CoeffMx = -LINFINI_PNE;
CoeffMn =  LINFINI_PNE;

for ( i = 0 ; i < NombreDeTermes ; i++ ) {		
  Var = IndiceDeLaVariable_CG[i];
	Coefficient = Coefficient_CG[i];
  if ( TypeDeVariableTrav[Var] == ENTIER ) {
	
    /* Attention a cause des bornes variables il faut verifier que la variable n'y est pas deja. Peut etre ameliore */
	  Found = 0;
		/*
		for ( i1 = 0 ; i1 < NbT ; i1++ ) {
		  if ( Variable[i1] == Var ) {
			  Found = 1;
        Coeff[i1] += Coefficient;
		    if ( fabs( Coeff[i1] ) > CoeffMx ) CoeffMx = fabs( Coeff[i1] );
		    else if ( fabs( Coeff[i1] ) < CoeffMn ) CoeffMn = fabs( Coeff[i1] );				
				break;
			}
		}
		*/
    /* Fin borne sup variable */
		
	 	if ( Found == 0 ) {	
      Coeff[NbT] = Coefficient;
		  if ( fabs( Coefficient ) > CoeffMx ) CoeffMx = fabs( Coefficient );
		  else if ( fabs( Coefficient ) < CoeffMn ) CoeffMn = fabs( Coefficient );
		  Variable[NbT] = Var;
		  NbT++;
	  }	
		continue;
  }
	
  if ( Coefficient < 0.0 ) {
    /* Il faut monter la variable au max */

 

        /* S'il y a une borne sup variable on remplace la variable par sa borne sup variable */
	      if ( Pne->CntDeBorneSupVariable != NULL && 0 ) {
          if ( Pne->CntDeBorneSupVariable[Var] >= 0 ) {
		        Cnt1 = Pne->CntDeBorneSupVariable[Var];
	          i1 = Pne->MdebTrav[Cnt1];
			      bBorne = Pne->BTrav[Cnt1];
			      SecondMembre -= Coefficient * bBorne ;								
		        VarBin = Pne->NuvarTrav[i1];
						
	          u = -Pne->ATrav[i1];
						/* Peut etre ameliore */
						Found = 0;
						for ( i1 = 0 ; i1 < NbT ; i1++ ) {
						  if ( Variable[i1] == VarBin ) {
							  Found = 1;
                Coeff[i1] += Coefficient * u;
		            if ( fabs( Coeff[i1] ) > CoeffMx ) CoeffMx = fabs( Coeff[i1] );
		            else if ( fabs( Coeff[i1] ) < CoeffMn ) CoeffMn = fabs( Coeff[i1] );									
								break;
							}
						}
						if ( Found == 0 ) {
              Coeff[NbT] = Coefficient * u;														
			        Variable[NbT] = VarBin;
		          if ( fabs( Coeff[NbT] ) > CoeffMx ) CoeffMx = fabs( Coeff[NbT] );
		          else if ( fabs( Coeff[NbT] ) < CoeffMn ) CoeffMn = fabs( Coeff[NbT] );											
				      NbT++;							
            }
						/*printf("Variable bound ajoute\n");*/						
				    continue;				
	        }
	      }
        /* Fin borne sup variable */
		
    if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
		  SecondMembre -= Coefficient * UmaxTrav[Var];			
    }				
		else {
		  Kpossible = NON_PNE;			
			break;
    }
	}
  else {
	  /* Il faut baisser la variable au min */


        /* S'il y a une borne sup variable on remplace la variable par sa borne sup variable */
        if ( Pne->CntDeBorneInfVariable != NULL && 0 ) {
          if ( Pne->CntDeBorneInfVariable[Var] >= 0 ) {
					  Cnt1 = Pne->CntDeBorneInfVariable[Var];
		        i1 = Pne->MdebTrav[Cnt1];
			      bBorne = -Pne->BTrav[Cnt1];
			      SecondMembre -= Coefficient * bBorne ;								
		        VarBin = Pne->NuvarTrav[i1];
	          l = Pne->ATrav[i1];
						/* Peut etre ameliore */
						Found = 0;
						for ( i1 = 0 ; i1 < NbT ; i1++ ) {
						  if ( Variable[i1] == VarBin ) {
							  Found = 1;
                Coeff[i1] += Coefficient * l;
								break;
							}
						}						
						if ( Found == 0 ) {
              Coeff[NbT] = Coefficient * l;
			        Variable[NbT] = VarBin;
				      NbT++;
            }
						/*printf("Variable bound ajoute\n");*/						
				    continue;			
	        } 		
        }
        /* Fin borne inf variable */
		
	  if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
		  SecondMembre -= Coefficient * UminTrav[Var];			
		}		
		else {
		  Kpossible = NON_PNE;		
		  break;
		}
  }											
}

/* Si le second membre est positif et que tous les coeff sont negatifs alors la contrainte ne sera jamais
   violee */
if ( SecondMembre >= 0.0 ) {  
  Trouve = NON_PNE;
	for ( i = 0 ; i < NbT ; i++ ) {
	  if ( Coeff[i] > 0.0 ) {
      Trouve = OUI_PNE;
			break;
		}
	}
	if ( Trouve == NON_PNE ) Kpossible = NON_PNE;
}

if ( Kpossible == OUI_PNE && NbT >= MIN_TERMES_POUR_KNAPSACK && NbT <= MAX_TERMES_POUR_KNAPSACK_DANS_COUPE ) {
  /*
	printf("On peut tenter une K sur la Gomory  NbT %d \n",NbT);			
	for ( i = 0 ; i < NbT ; i++ ) {
	  printf(" %e (%d)",Coeff[i],Variable[i]);
	}
	printf(" < %e\n",SecondMembre); fflush(stdout);  
  */
	
	if ( fabs( SecondMembre ) > CoeffMx ) CoeffMx = fabs( SecondMembre );
	else if ( fabs( SecondMembre ) < CoeffMn ) CoeffMn = fabs( SecondMembre );
	
	RendreLesCoeffsEntiers = NON_PNE;
	if ( CoeffMn > ZERO_COEFFMN ) {
	  if ( CoeffMx / CoeffMn < RAPPORT_MAX ) RendreLesCoeffsEntiers = OUI_PNE;
	} 
	
	CouvertureTrouvee = NON_PNE;						  
  PNE_GreedyCoverKnapsack( Pne, 0, NbT, Variable, Coeff, SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
													 Mixed_0_1_Knapsack, 0.0, 0, NULL, NULL, NULL );							
  /*
	if ( CouvertureTrouvee == OUI_PNE ) {
	  printf("   K trouvee sur Gomory ou coupe d'intersection !!!!!!!!!!!!!!\n");
	}
	*/
}
  
return;
}

