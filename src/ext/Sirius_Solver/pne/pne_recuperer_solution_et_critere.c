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

   FONCTION: Recuperation des resultats
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_define.h"

# define TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES  1.e-2 /*1.e-3*/

/*----------------------------------------------------------------------------*/

void PNE_RecupererLaSolutionEtCalculerLeCritere( PROBLEME_PNE * Pne,
                                                 PROBLEME_A_RESOUDRE * Probleme )
{
int i; int Var; int VarE; double Critere; int CntE; int Cnt; double S; int il; int ilMax;     

double * UTrav; double * UminEntree; double * UmaxEntree; double * BTrav; double * ATrav;
double * VariablesDualesDesContraintesTrav; double * LTrav; int * TypeDeVariableTrav;
int * TypeDeBorneTrav; char * VariableAInverser; int * CorrespondanceVarEntreeVarNouvelle;
double ToleranceViolation; char * SensContrainteTrav; int * MdebTrav; int * NbTermTrav;
int * NuvarTrav; int * CorrespondanceCntPneCntEntree; int NumeroDeLaContrainteDeCoutMax;
int * NumeroDesContraintesInactives; int * TypeDeBorneE; double * UminE; double * UmaxE;
char * SensE; int * MdebE; int * NbtermE; double * AE; int * NuvarE; int ilE; int ilMaxE;
double CoutE; double C1; double C2; double C3; double C4; double Zborne; double a; double u;
int NombreDeVariablesE; int NombreDeContraintesE; int * TypeDeBorneTravE;   
double * UE; double * LE; double * VariablesDualesDesContraintesE;

NombreDeVariablesE = Probleme->NombreDeVariables;
NombreDeContraintesE = Probleme->NombreDeContraintes;
TypeDeBorneTravE = Probleme->TypeDeBorneDeLaVariable;
UE = Probleme->X;
LE = Probleme->CoutLineaire;              
VariablesDualesDesContraintesE = Probleme->VariablesDualesDesContraintes;

LTrav = Pne->LTrav;
UTrav = Pne->UTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
UminEntree = Pne->UminEntree;
UmaxEntree = Pne->UmaxEntree;
VariableAInverser = Pne->VariableAInverser;
CorrespondanceVarEntreeVarNouvelle = Pne->CorrespondanceVarEntreeVarNouvelle;  

SensContrainteTrav = Pne->SensContrainteTrav;
BTrav = Pne->BTrav;
MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
ATrav = Pne->ATrav;
NuvarTrav = Pne->NuvarTrav;
CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;
VariablesDualesDesContraintesTrav = Pne->VariablesDualesDesContraintesTrav;

NumeroDesContraintesInactives = Pne->NumeroDesContraintesInactives;

if ( Pne->YaUneSolution == SOLUTION_OPTIMALE_TROUVEE ) {

  /* NumeroDeLaContrainteDeCoutMax = -1 s'il n'y a pas de variables entieres */
  NumeroDeLaContrainteDeCoutMax = Pne->NumeroDeLaContrainteDeCoutMax;

  for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
    /* En presence de variables entieres, il peut y avoir une contrainte de coutmax. A ce niveau de l'execution
	     cette contrainte peut etre violee par la solution optimale (alors que ce n'est pas le cas dans le simplexe.
		   Pourquoi ? Parce que le second membre de cette contrainte est mis a jour a chaque fois que l'on trouve
		   une solution entiere ameliorante. Or ici on testerait la violation de cette contrainte avec le nouveau
		   second membre. Comme la solution est ameliorante on la detecterait comme etant violee par la solution
		   optimale ce qui serait une conclusion fausse. */
	  if ( Cnt == NumeroDeLaContrainteDeCoutMax ) continue;
    /* Verifier les violations de contraintes */
    S = 0.;
    il    = MdebTrav[Cnt];
    ilMax = il + NbTermTrav[Cnt];
    while ( il < ilMax ) {
      S+= ATrav[il] * UTrav[NuvarTrav[il]];
      il++;
    }
		ToleranceViolation = ( 1. + fabs( BTrav[Cnt] ) ) * 0.01 * TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES;
    if ( SensContrainteTrav[Cnt] == '=' ) {
      if ( fabs( S - BTrav[Cnt] ) > ToleranceViolation ) {
        Pne->YaUneSolution = SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES;
				# if VERBOSE_PNE == 1
          printf("Contrainte d'egalite %d violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					        BTrav[Cnt],fabs( S - BTrav[Cnt] ));
				# endif
      }
    }
    else if ( SensContrainteTrav[Cnt] == '<' ) {
      if ( S > BTrav[Cnt] + ToleranceViolation ) {
        Pne->YaUneSolution = SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES;
				# if VERBOSE_PNE == 1
          printf("Contrainte %d de type < ou = violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					        BTrav[Cnt],fabs( S - BTrav[Cnt] )); 
				# endif				
      }
    }
    else if ( SensContrainteTrav[Cnt] == '>' ) {
      if ( S < BTrav[Cnt] - ToleranceViolation ) {
        Pne->YaUneSolution = SOLUTION_OPTIMALE_TROUVEE_MAIS_QUELQUES_CONTRAINTES_SONT_VIOLEES;
				# if VERBOSE_PNE == 1
          printf("Contrainte %d de type > ou = violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					        BTrav[Cnt],fabs( S - BTrav[Cnt] )); 
				# endif	
				
      }
    }
  }
}

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  if ( TypeDeVariableTrav[i] == ENTIER ) {
    if ( fabs( UTrav[i] - floor( UTrav[i] ) ) <= TOLERANCE_SUR_LES_ENTIERS ) {
      UTrav[i] = floor( UTrav[i] );
    }
    else UTrav[i] = ceil( UTrav[i] );
  }
}						      

/* Restitution du resultat dans le contexte d'appel et calcul du critere */

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {	
  if ( TypeDeBorneTrav[i] == VARIABLE_BORNEE_DES_DEUX_COTES || 
       TypeDeBorneTrav[i] == VARIABLE_BORNEE_INFERIEUREMENT )  
    UTrav[i]+= UminEntree[i]; 
  else if ( TypeDeBorneTrav[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) 
    UTrav[i]+= UmaxEntree[i]; 
 
  if ( VariableAInverser[i] == OUI_PNE ) UTrav[i] = -UTrav[i];
	
}

if ( VariablesDualesDesContraintesE != NULL ) {
  /* 22/10/2015: suppression de la boucle qui suit et recalcul des variables duales juste apres */
	/*
  for ( i = 0 ; i < Pne->NombreDeContraintesInactives ; i++ ) {	
    CntE = NumeroDesContraintesInactives[i];
    if ( CntE >= 0 && CntE < NombreDeContraintesE ) {
	    VariablesDualesDesContraintesE[CntE] = 0.0;
    }
  }
	*/
  for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
    CntE = CorrespondanceCntPneCntEntree[Cnt];
    if ( CntE >= 0 && CntE < NombreDeContraintesE ) {
      VariablesDualesDesContraintesE[CntE] = VariablesDualesDesContraintesTrav[Cnt];			
    }
  }	
}

/* Recuperation des substitutions de variables et des variables colineaire ainsi que
   des variables duales */
PNE_PostSolve( Pne, Probleme );

for ( Critere = 0. , VarE = 0 ; VarE < NombreDeVariablesE ; VarE++ ) {
  Var = CorrespondanceVarEntreeVarNouvelle[VarE];
  if ( Var >= 0 ) {
		UE[VarE] = UTrav[Var];
	}
  Critere += LE[VarE] * UE[VarE];
}

/* Pour les contraintes inactives dont la variable duale reste non initialisee on se sert des couts des
   variables qui la composent */
	 
if ( VariablesDualesDesContraintesE != NULL ) {
  TypeDeBorneE = Probleme->TypeDeBorneDeLaVariable;
  UmaxE = Probleme->Xmax;
  UminE = Probleme->Xmin;
  SensE = Probleme->Sens;
  MdebE = Probleme->IndicesDebutDeLigne;
  NbtermE = Probleme->NombreDeTermesDesLignes;
  AE = Probleme->CoefficientsDeLaMatriceDesContraintes;
  NuvarE = Probleme->IndicesColonnes;
	Zborne = 1.e-6;
  for ( CntE = 0 ; CntE < NombreDeContraintesE ; CntE++ ) {
    if ( VariablesDualesDesContraintesE[CntE] < VALEUR_NON_INITIALISEE ) continue;		
    VariablesDualesDesContraintesE[CntE] = 0;

    if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
      /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	       duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		     faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		     recalculer exactement les variables duales. */
      continue;
    }
		
		if ( SensE[CntE] != '=' ) continue; 
		/* Contrainte de type = :
			 C1 = le plus petit cout des variable a coefficient positif qui ne sont pas sur Xmax
			 C2 = le plus petit cout change de signe des variable a coefficient positif qui ne sont pas sur Xmin
			 C3 = le plus petit cout des variables a coefficient negatif qui ne sont pas sur Xmax
			 C4 = le plus petit cout change de signe des variables a coefficient negatif qui ne sont pas su Xmin
			 Et on affecte le min de C1, C2, C3, C4 
     */		 
    C1 = 2 * VALEUR_NON_INITIALISEE;
    C2 = 2 * VALEUR_NON_INITIALISEE;
    C3 = 2 * VALEUR_NON_INITIALISEE;
    C4 = 2 * VALEUR_NON_INITIALISEE;
		S = 0;
    ilE = MdebE[CntE];
		ilMaxE = ilE + NbtermE[CntE];
		while ( ilE < ilMaxE ) {
		  a = AE[ilE];
		  if ( a == 0.0 ) goto NextIlE;
      VarE = NuvarE[ilE];

			S += a * UE[VarE];
			
			CoutE = LE[VarE];			
			if ( TypeDeBorneE[VarE] == VARIABLE_FIXE ) goto NextIlE;					
			if ( TypeDeBorneE[VarE] == VARIABLE_NON_BORNEE ) {
        if ( a > 0 ) {
				  if ( C1 > CoutE ) C1 = CoutE;
          if ( C2 > -CoutE ) C2 = -CoutE;
			  }
				else {
          if ( C3 > CoutE ) C3 = CoutE;
          if ( C4 > -CoutE ) C4 = -CoutE;
				}
			}
			else if ( TypeDeBorneE[VarE] == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  if ( a > 0 ) {
          if ( C1 > CoutE ) C1 = CoutE;
          if ( fabs ( UE[VarE] - UminE[VarE] ) > Zborne ) {
            if ( C2 > -CoutE ) C2 = -CoutE;						
				  }
				}
				else {
          if ( C3 > CoutE ) C3 = CoutE;
          if ( fabs ( UE[VarE] - UminE[VarE] ) > Zborne ) {
            if ( C4 > -CoutE ) C4 = -CoutE;
					}
				}
			}
			else if ( TypeDeBorneE[VarE] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  if ( a > 0 ) {
          if ( fabs ( UE[VarE] - UmaxE[VarE] ) > Zborne ) {
            if ( C1 > CoutE ) C1 = CoutE;
					}
          if ( C2 > -CoutE ) C2 = -CoutE;				  
				}
				else {
          if ( fabs ( UE[VarE] - UmaxE[VarE] ) > Zborne ) {
            if ( C3 > CoutE ) C3 = CoutE;
					}
          if ( C4 > -CoutE ) C4 = -CoutE;
				}
			}
			else {
			  if ( a > 0 ) {
          if ( fabs ( UE[VarE] - UmaxE[VarE] ) > Zborne ) {
            if ( C1 > CoutE ) C1 = CoutE;						
					}
          if ( fabs ( UE[VarE] - UminE[VarE] ) > Zborne ) {
            if ( C2 > -CoutE ) C2 = -CoutE;					
				  }					
				}
				else {
          if ( fabs ( UE[VarE] - UmaxE[VarE] ) > Zborne ) {
            if ( C3 > CoutE ) C3 = CoutE;					
					}
					
          if ( fabs ( UE[VarE] - UminE[VarE] ) > Zborne ) {
            if ( C4 > -CoutE ) C4 = -CoutE;						
					}
				}
			}
      NextIlE:
		  ilE++;
		}	
	  u = 4 * VALEUR_NON_INITIALISEE;    
		if ( u > C1 ) u = C1;
    if ( u > C2 ) u = C2;
    if ( u > C3 ) u = C3;
    if ( u > C4 ) u = C4;
						
	  if ( u > VALEUR_NON_INITIALISEE ) u = 0;
		
		/* Il faut changer le signe pour se remettre dans le contexte d'un simplexe */
		u = -u;
				
    /* Ici on n'a jamais une contrainte d'egalite */
    if ( Probleme->Sens[CntE]	== '<' ) {
      if ( S < Probleme->SecondMembre[CntE] + 1.e-7 ) u = 0;			
		}
		else if ( Probleme->Sens[CntE]	== '>' ) {
      if ( S > Probleme->SecondMembre[CntE] - 1.e-7 ) u = 0;
		}
		
    VariablesDualesDesContraintesE[CntE] = u;
		
	}
}
  
#if VERBOSE_PNE
  if ( Pne->YaUneSolution != PAS_DE_SOLUTION_TROUVEE ) {
    printf("  Valeur du critere: %lf \n",Critere ); 
    printf("(attention c'est une valeur interne au solveur, en particulier elle ne tient\n"); 
    printf(" pas compte de la contribution de variables fixes au critere)\n");
	}
#endif

/* Controle d'existence de valeurs non initialisees */
if ( VariablesDualesDesContraintesE != NULL ) {
  i = 0;
  for ( CntE = 0 ; CntE < NombreDeContraintesE ; CntE++ ) {
    if ( VariablesDualesDesContraintesE[CntE] > VALEUR_NON_INITIALISEE ) {
	    printf("Fin du PostSolve: variable duale de la contrainte %d pas initialisee\n",CntE);
			i++;
	  }
  }
	if ( i != 0 ) printf("Nombre de variables duales non initialisees %d sur %d\n",i,NombreDeContraintesE);
}

for ( VarE = 0 ; VarE < NombreDeVariablesE ; VarE++ ) {
  if ( UE[VarE] > VALEUR_NON_INITIALISEE ) {
	  printf("Fin du PostSolve: valeur de la variable %d pas initialisee\n",VarE);
	}
}

return;
}

/*----------------------------------------------------------------------------*/

