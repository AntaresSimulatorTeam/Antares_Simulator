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

   FONCTION: Recherche de couverture de sac a dos sur combinaisons simples
	           de contraintes.

   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif
   
# define ZERO_K 1.e-9

/*----------------------------------------------------------------------------*/

void PNE_KnapsackSurCombinaisonsDeContraintes( PROBLEME_PNE * Pne, int Cnt0, int NombreDeTermes,
                                               int * Variable, double * Coeff, double SecondMembre,
																							 char * ContrainteDejaUtilisee,
																							 char * VariableSupprimee )
{ 
int Var; int ic; int il; int ilMax; int Var1; double X0; double X; int Cnt1; int il1; int il1Max; 
int i; char CouvertureTrouvee; int NbK; int NbConcat; int NombreDeVariablesTrav;
double Alpha; int * MdebTrav; int * NbTermTrav; char RendreLesCoeffsEntiers;
int * NuvarTrav; int * TypeDeBorneTrav; int * TypeDeVariableTrav; int * CdebTrav;
int * CsuiTrav; int * NumContrainteTrav; double * BTrav; double * ATrav; double * UTrav;
double * UminTrav; double * UmaxTrav; char * SensContrainteTrav;
double * V; double Gamma; int NombreDeTermes0; 
char Mixed_0_1_Knapsack;

/*
printf("KnapsackSurCombinaisonsDeContraintes NombreDeTermes %d\n",NombreDeTermes);
*/

Mixed_0_1_Knapsack = NON_PNE;

NbK = 0;
 
NombreDeTermes0 = NombreDeTermes;
NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;

memset( (char *) ContrainteDejaUtilisee, NON_PNE, Pne->NombreDeContraintesTrav * sizeof( char ) );
ContrainteDejaUtilisee[Cnt0] = OUI_PNE;

memset( (char *) VariableSupprimee, NON_PNE, NombreDeVariablesTrav * sizeof( char ) );

MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
BTrav = Pne->BTrav;   
ATrav = Pne->ATrav;
NuvarTrav = Pne->NuvarTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
UTrav = Pne->UTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;  
CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
SensContrainteTrav = Pne->SensContrainteTrav;

/* On etale la contrainte */
V = Pne->Coefficient_CG;
memset( (char *) V , 0 , Pne->NombreDeVariablesTrav * sizeof( double ) );
for ( i = 0 ; i < NombreDeTermes ; i++ ) V[Variable[i]] = Coeff[i];

/* On essaie de remplacer chaque variable entiere de la contrainte par d'autres variables
   dont la valeur est plus grande que la variable remplacee. On espere ainsi augmenter le
	 membre de gauche pour trouver plus facilement une knapsack violee */
NbConcat = 0;
il = MdebTrav[Cnt0];
ilMax = il + NbTermTrav[Cnt0];
while ( il < ilMax ) { 
  if ( ATrav[il] == 0.0 ) goto NextIl;
  Var = NuvarTrav[il];
	if ( TypeDeVariableTrav[Var] != ENTIER ) goto NextIl;	
	if ( VariableSupprimee[Var] == OUI_PNE ) goto NextIl;	
	Gamma = V[Var];
	if ( Gamma == 0.0 ) goto NextIl;
	
	/* Si la variable est sur une borne peu prometteuse on ne la remplace pas */	
	if ( Gamma > 0 ) {	  
	  if ( fabs( UTrav[Var] - UminTrav[Var] ) < ZERO_K ) goto NextIl;
	}  
	else {
	  if ( fabs( UmaxTrav[Var] - UTrav[Var] ) < ZERO_K ) goto NextIl;
	}
	
	X0 = fabs( Gamma * UTrav[Var] );
	X = 0.0;
	/* Pour pouvoir remplacer cette variable, il faut trouver une contrainte ou la variable a un
	   signe oppose */
	ic = CdebTrav[Var];
	while ( ic >= 0 ) {
    Cnt1 = NumContrainteTrav[ic];
		if ( ContrainteDejaUtilisee[Cnt1] == OUI_PNE ) goto NextIc;
		if ( fabs( ATrav[ic] ) < ZERO_K ) goto NextIc;
		if ( SensContrainteTrav[Cnt1] == '<' ) {
		  if ( ATrav[ic] * Gamma > 0.0 ) goto NextIc;
		}
		/* On s'interesse a la contrainte */
		Alpha = -Gamma / ATrav[ic];
    il1 = MdebTrav[Cnt1];
    il1Max = il1 + NbTermTrav[Cnt1]; 
    while ( il1 < il1Max ) { 
      Var1 = NuvarTrav[il1];
			if ( Var1 == Var ) goto NextIl1;
			if ( VariableSupprimee[Var1] == OUI_PNE ) goto NextIc;
			if ( TypeDeVariableTrav[Var1] == ENTIER ) {
			  X += fabs( Alpha * ATrav[il1] * UTrav[Var1] );
			}
			else {				
		    if ( Alpha * ATrav[il1] < 0.0 ) {
		      /* Il faudrait mettre la variable au max: on ne pourra pas utiliser la contrainte */
				  if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES ||
					     TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
            X += fabs( Alpha * ATrav[il1] * UmaxTrav[Var1] );
					}									
				  else {
					  /*printf("probleme de borne\n");*/
					  goto NextIc;
					}
				}
				else if ( Alpha * ATrav[il1] > 0.0 ) {
		      /* Il faudrait mettre la variable au min: on ne pourra pas utiliser la contrainte */
				  if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES ||
					     TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_INFERIEUREMENT ) {
            X += fabs( Alpha * ATrav[il1] * UminTrav[Var1] );							 
					}															
					else {
					  /*printf("probleme de borne\n");*/
					  goto NextIc;
					}
				}									
			}
			NextIl1:
			il1++;		
		}
		if ( X >= X0 ) {
		  /* On peut utiliser la contrainte */			
      il1 = MdebTrav[Cnt1];
      il1Max = il1 + NbTermTrav[Cnt1];
		  while ( il1 < il1Max ) {
        Var1 = NuvarTrav[il1];
			  X = Alpha * ATrav[il1];
			  if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE ) SecondMembre -= X * UTrav[Var1];
				/* On ajoute la variable mais on ne place pas les variables continues sur borne car
				   le signe peut encore changer */
			  else V[Var1] += X;
		    il1++;
		  }
		  SecondMembre += Alpha * BTrav[Cnt1];
			if ( fabs( V[Var] ) > ZERO_K ) goto NextIl; /* Manque de precision */  
      V[Var] = 0.0;
		  VariableSupprimee[Var] = OUI_PNE;
      ContrainteDejaUtilisee[Cnt1] = OUI_PNE;
		  /*printf("Concatenation de la contrainte %d  nbtermes %d\n",Cnt1,NbTermTrav[Cnt1]);*/
		  NbConcat++;
		  goto NextIl;
		}
		NextIc:
    ic = CsuiTrav[ic];
  }
	NextIl:	
  il++;
}
/*printf("NbConcat %d NombreDeTermes0 %d\n",NbConcat,NombreDeTermes0);*/
if ( NbConcat == 0 ) goto FinConcat;

/* On reconstitue le vecteur de la contrainte sur laquelle on va chercher la knapsack */
NombreDeTermes = 0;
for ( Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) {
  if ( V[Var] != 0.0 ) {	
	  if ( TypeDeVariableTrav[Var] == ENTIER ) {	
	    Coeff[NombreDeTermes] = V[Var];
		  Variable[NombreDeTermes] = Var;
		  NombreDeTermes++;
		}
		else {
		  if ( V[Var] < 0.0 ) {
		    /* Il faut monter la variable au max */
			  if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
				     TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    SecondMembre -= V[Var] * UmaxTrav[Var];								
        }				
				else {
				  NombreDeTermes = 0;
					break;
				}				
			}
			else {
		    /* Il faut baisser la variable au min */
			  if ( TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ||
					   TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    SecondMembre -= V[Var] * UminTrav[Var];				
        }									        				
				else {
				  NombreDeTermes = 0;
					break;
				}										
			}
		}
	}
}
/*printf(" NombreDeTermes %d  SecondMembre %e\n",NombreDeTermes,SecondMembre);*/
if ( NombreDeTermes == 0 ) goto FinConcat;
/*
printf(" Tentative recherche K NombreDeTermes0 %d NombreDeTermes %d NbConcat %d\n",NombreDeTermes0,NombreDeTermes,NbConcat);
*/
if ( NombreDeTermes >= MIN_TERMES_POUR_KNAPSACK ) {
  /*
	printf(" recherche K NombreDeTermes0 %d NombreDeTermes %d  NbConcat %d\n",NombreDeTermes0,NombreDeTermes,NbConcat);	
  */
	RendreLesCoeffsEntiers = OUI_PNE;
	CouvertureTrouvee = NON_PNE;			
  PNE_GreedyCoverKnapsack( Pne, 0, NombreDeTermes, Variable, Coeff, SecondMembre, RendreLesCoeffsEntiers, &CouvertureTrouvee,
													 Mixed_0_1_Knapsack, 0.0, 0, NULL, NULL, NULL );							  
	if ( CouvertureTrouvee == OUI_PNE ) {
    /*printf("---------------- Knapsack trouvee sur combinaison de contraintes  NbConcat=%d\n",NbConcat);*/
    NbK++;
	}
	
}
 
FinConcat:

/*
if ( NbK > 0 || 1 ) printf("     -> %d Knapsack trouvees sur combinaison \n",NbK);
*/

return;
}
