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

   FONCTION: Heuristique Marchand-Wolsey pour faire des MIR sur des
	           contraintes natives agregees.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TROP_DE_VARIABLES_ENTIERES  100

# define MAX_FOIS_SUCCESS_MIR 2

# define Z0 1.e-7
# define Z1 0.9999999

# define VALEUR_NULLE_DUN_COEFF 1.e-15 /*1.e-12*/

# define MAX_AGREG 3 /*5*/ /*3*/

# define MAX_CYCLES_SUR_CONTRAINTES 5 /*10*/
# define MAX_CYCLES_SUR_VARIABLES 5 /*10*/
		
# define LOW_VARBIN   1
# define HIGH_VARBIN  2
# define LOW   3
# define HIGH  4

/*----------------------------------------------------------------------------*/
double PNE_G_de_D( double d, double f )
{ double X; double fd;
X = floor( d );
/*fd = d - floor( d );*/
fd = d - X;
if ( fd > f ) {
  X += ( fd - f ) / ( 1. - f ) ;
}
return( X );
}
/*----------------------------------------------------------------------------*/
double PNE_TesterUneMIR( int NombreDeVariablesBinaires, int * NumeroDeLaVariableBinaire,
                         double * CoeffDeLaVariableBinaire, double * UTrav,
												 double SecondMembreDeLaMIR, double Delta, double f, double SeuilPourC,
												 double * bMIR, char ModeTest )
{
int i; int Var; double ViolationMIR; double MembreDeGauche; double CoeffMir; double U;

MembreDeGauche = 0.0;
/* Calcul de chaque coeff de la MIR et de la violation */
goto NouveauCode;
for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
  Var = NumeroDeLaVariableBinaire[i];			
  if ( UTrav[Var] > SeuilPourC ) {
		/* La variable est dans C */
		CoeffMir = PNE_G_de_D( -CoeffDeLaVariableBinaire[i] / Delta, f );
	  MembreDeGauche += CoeffMir * ( 1. - UTrav[Var] );
	  if ( ModeTest == NON_PNE ) {
		  CoeffDeLaVariableBinaire[i] = -CoeffMir;
			*bMIR -= CoeffMir;
		}		
  }
	else {
	  CoeffMir = PNE_G_de_D( CoeffDeLaVariableBinaire[i] / Delta, f );
	  MembreDeGauche += CoeffMir * UTrav[Var];
	  if ( ModeTest == NON_PNE ) CoeffDeLaVariableBinaire[i] = CoeffMir;
	}
}
NouveauCode:
if ( ModeTest == NON_PNE ) {
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    Var = NumeroDeLaVariableBinaire[i];
		U = UTrav[Var];
    if ( U > SeuilPourC ) {
		  /* La variable est dans C */
		  CoeffMir = PNE_G_de_D( -CoeffDeLaVariableBinaire[i] / Delta, f );
	    MembreDeGauche += CoeffMir * ( 1. - U );
		  CoeffDeLaVariableBinaire[i] = -CoeffMir;
			*bMIR -= CoeffMir;
    }
	  else {
	    CoeffMir = PNE_G_de_D( CoeffDeLaVariableBinaire[i] / Delta, f );
	    MembreDeGauche += CoeffMir * U;
	    CoeffDeLaVariableBinaire[i] = CoeffMir;
	  }
  }
}
else {
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    Var = NumeroDeLaVariableBinaire[i];			
		U = UTrav[Var];
    if ( U > SeuilPourC ) {
		  /* La variable est dans C */
		  CoeffMir = PNE_G_de_D( -CoeffDeLaVariableBinaire[i] / Delta, f );
	    MembreDeGauche += CoeffMir * ( 1. - U );
    }
 	  else {
	    CoeffMir = PNE_G_de_D( CoeffDeLaVariableBinaire[i] / Delta, f );
	    MembreDeGauche += CoeffMir * U;
	  }
  }
}

ViolationMIR = MembreDeGauche - SecondMembreDeLaMIR;

if ( ViolationMIR > 0 && 0 ) {
	printf("Pendant tests ViolationMIR = %e  MembreDeGauche %e SecondMembreDeLaMIR %e ",ViolationMIR,MembreDeGauche,SecondMembreDeLaMIR);
	if ( ModeTest == NON_PNE ) printf("Final bMIR %e\n",*bMIR);
	printf("\n");
}

return( ViolationMIR );
}
/*----------------------------------------------------------------------------*/
void PNE_TesterUneMIRpourUneValeurDeDelta( PROBLEME_PNE * Pne,
                                           double Delta, int NombreDeVariablesBinaires,
                                           double SeuilPourC, int * NumeroDeLaVariableBinaire,
                                           double * CoeffDeLaVariableBinaire, double b,
																					 double * CoeffDeLaVariableContinue,
								                           double ValeurDeLaVariableContinue, 
																					 double * UTrav, double * ViolationMaxMIR,
																					 double * DeltaOpt, char * MIRviolee,
																					 double * bMIR, char ModeTest )
{
double CoeffVarCont; double Beta; double f; double SecondMembreDeLaMIR;
double ViolationMIR; double Seuil;

if ( Delta < Z0 ) return;
Beta = b / Delta;
f = Beta - floor( Beta );

if ( f > Z1 ) return;

/* On fait une MIR sur la contrainte divisee par Delta */
CoeffVarCont = 1 / ( Delta * ( 1. - f ) );
SecondMembreDeLaMIR = floor( Beta ) + ( CoeffVarCont * ValeurDeLaVariableContinue );

if ( ModeTest == NON_PNE ) {
  *CoeffDeLaVariableContinue = CoeffVarCont;
	*bMIR = floor( Beta );
}

ViolationMIR = PNE_TesterUneMIR( NombreDeVariablesBinaires, NumeroDeLaVariableBinaire,
                                 CoeffDeLaVariableBinaire, UTrav, SecondMembreDeLaMIR,
																 Delta, f, SeuilPourC, bMIR ,ModeTest );

PNE_MiseAJourSeuilCoupes( Pne, COUPE_MIR_MARCHAND_WOLSEY, &Seuil );

if ( ViolationMIR > Seuil ) {
  Pne->SommeViolationsMIR_MARCHAND_WOLSEY += ViolationMIR;
  Pne->NombreDeMIR_MARCHAND_WOLSEY++;
}
																 
if ( ViolationMIR > Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY && ViolationMIR > *ViolationMaxMIR ) {
	*MIRviolee = OUI_PNE; 
  *ViolationMaxMIR = ViolationMIR;
	*DeltaOpt = Delta;	
	/*printf("Violation MIR    ViolationMIR = %e\n",ViolationMIR); */ 	
}

return;
}

/*----------------------------------------------------------------------------*/
/* La contrainte mixte recue est du type Somme ai * yi <= b + s ou les yi
   sont des variables binaires et s est une variable continue >= 0 */
/* On suppose que les variables entieres sont des binaires donc leur borne
   sup est 1. */

char PNE_C_MIR( PROBLEME_PNE * Pne, int NombreDeVariablesBinaires, int * NumeroDeLaVariableBinaire,
                double * CoeffDeLaVariableBinaire, double * b, double * CoeffDeLaVariableContinue,
								double ValeurDeLaVariableContinue )
{
int i; int Var; double * UTrav; double Delta; double SeuilPourC; char MIRviolee;
double ViolationMaxMIR; double DeltaOpt; double Delta0; char ModeTest; double bTest;
double bMIR; double SeuilPourCRetenu; double Beta; double f; double S; 

SeuilPourC = 0.5;
MIRviolee = NON_PNE;
ViolationMaxMIR = -1.0;
UTrav = Pne->UTrav;

/* Au depart, on a toujours CoeffDeLaVariableContinue = 1 */
/* Recherche de l'ensemble C que l'on va complementer */
bTest = *b;
for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
  if ( UTrav[NumeroDeLaVariableBinaire[i]] > SeuilPourC ) {
    /* On complemente la variable */
		bTest -= CoeffDeLaVariableBinaire[i]; /* Borne sur des variables binaires = 1 */
	}
}

/* Choix des valeurs de Delta */
ModeTest = OUI_PNE;

for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
  Var = NumeroDeLaVariableBinaire[i];
  if ( UTrav[Var] < Z0 || UTrav[Var] > Z1 ) continue;
	Delta = fabs( CoeffDeLaVariableBinaire[i] );
	
  Beta = bTest / Delta;
  f = Beta - floor( Beta );
	
  if ( f < Z0 ) {
	  Delta *= 0.9 /*1.5*/;
	}
		
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest );																			
}
if ( MIRviolee == OUI_PNE ) {
  
  Delta0 = DeltaOpt;
  /* On essaie d'ameliorer la violation */
	Delta = Delta0 / 2;	
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest );																					
	Delta = Delta0 / 4;
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest ); 	
	Delta = Delta0 / 8;
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest );																					
}

/* Il manque la derniere etape d'amelioration de la mir decrite dans l'article */

if ( MIRviolee == OUI_PNE ) {	
  SeuilPourCRetenu = SeuilPourC;	
  /* On a trouve un DeltaOpt */
	/* On essaie d'augmenter la violation de la MIR en essayant SeuilPourC a 0.4 puis SeuilPourC 0.6 */
	Delta = DeltaOpt;
  SeuilPourC = 0.4;	
	BaisseSeuilC:
  bTest = *b;
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    if ( UTrav[NumeroDeLaVariableBinaire[i]] > SeuilPourC ) {
      /* On complemente la variable */
		  bTest -= CoeffDeLaVariableBinaire[i];
	  }
  }
	MIRviolee = NON_PNE;
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest ); 
	if ( MIRviolee == OUI_PNE ) {
	  /*printf("MIR amelioree en modifiant SeuilPourC a la baisse\n");*/
	  SeuilPourCRetenu = SeuilPourC;
		SeuilPourC = SeuilPourC - 0.1;
		if ( SeuilPourC > 0.0 && 0 ) goto BaisseSeuilC;
	}
	
	Delta = DeltaOpt;
  SeuilPourC = 0.6;
	HausseeSeuilC:	
  bTest = *b;
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    if ( UTrav[NumeroDeLaVariableBinaire[i]] > SeuilPourC ) {
      /* On complemente la variable */
		  bTest -= CoeffDeLaVariableBinaire[i];
	  }
  }
	MIRviolee = NON_PNE;
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, &bMIR, ModeTest ); 	
	if ( MIRviolee == OUI_PNE ) {
	  /*printf("MIR amelioree en modifiant SeuilPourC a la hausse\n");*/
	  SeuilPourCRetenu = SeuilPourC;
		SeuilPourC = SeuilPourC + 0.1;
		if ( SeuilPourC < 1.0 && 0 ) goto HausseeSeuilC;		
	}
	
  /* Stockage de la MIR */
	Delta = DeltaOpt;
  SeuilPourC = SeuilPourCRetenu;
  bTest = *b;
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    if ( UTrav[NumeroDeLaVariableBinaire[i]] > SeuilPourC ) {
      /* On complemente la variable */
		  bTest -= CoeffDeLaVariableBinaire[i];
	  }
  }	
	ModeTest = NON_PNE;
  PNE_TesterUneMIRpourUneValeurDeDelta( Pne, Delta, NombreDeVariablesBinaires, SeuilPourC,
	                                      NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
																				bTest, CoeffDeLaVariableContinue,
								                        ValeurDeLaVariableContinue, UTrav, &ViolationMaxMIR,
																				&DeltaOpt, &MIRviolee, b, ModeTest );																					
  MIRviolee = OUI_PNE;
	/*
	printf("b de la MIR %e  CoeffDeLaVariableContinue %e ValeurDeLaVariableContinue %e\n",*b,*CoeffDeLaVariableContinue,ValeurDeLaVariableContinue);
	*/
	S = -(*CoeffDeLaVariableContinue) * ValeurDeLaVariableContinue;
	/*printf("La MIR avant reconstitution de S\n");*/
  for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
    S += CoeffDeLaVariableBinaire[i] * Pne->UTrav[NumeroDeLaVariableBinaire[i]];
		/*printf(" %e (%d) ",CoeffDeLaVariableBinaire[i],NumeroDeLaVariableBinaire[i]);*/
  }
	/*printf("\n");*/
	if ( S < *b ) {
	  /*printf("Erreur des la fin de la MIR  S = %e  < b = %e\n",S,*b);*/
	}
	else {
	  /*printf("violation de cette mir S %e b %e violation %e\n",S,*b,S-*b);*/
	}
	/*printf("On va maintenant revenir aux variables qui constituent S\n");*/
	
}

return( MIRviolee );
}
/*----------------------------------------------------------------------------*/
void PNE_SyntheseEtStockageMIR( PROBLEME_PNE * Pne, int NombreDeVariablesBinaires, int * NumeroDeLaVariableBinaire,
                                double * CoeffDeLaVariableBinaire, double b, double CoeffDeLaVariableContinue,
																int NombreDeVariablesSubstituees, int * NumeroDesVariablesSubstituees,
														    char * TypeDeSubsitution, double * CoefficientDeLaVariableSubstituee )																
{
int i; int Var; int il; int VarBin; int * NuVarCoupe; double * CoeffCoupe; int NbTermes; char TypeSubst;
double Violation; double * Umax; double * Umin; int * Mdeb; double * A; int * Nuvar; double l; double u;
int * CntDeBorneInfVariable; int * CntDeBorneSupVariable; double * U; double bBorne; double S; double * B;
int * TypeDeBorne;

/*
printf(" Synthese de la MIR \n");
printf(" b = %e  CoeffDeLaVariableContinue = %e\n",b,CoeffDeLaVariableContinue);
*/

Mdeb = Pne->MdebTrav;
B = Pne->BTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
Umax = Pne->UmaxTravSv;
Umin = Pne->UminTravSv;
U = Pne->UTrav;
CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;
CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;
TypeDeBorne = Pne->TypeDeBorneTrav;

NuVarCoupe = Pne->IndiceDeLaVariable_CG;
CoeffCoupe = Pne->Coefficient_CG;

NbTermes = 0;
memset( (char *) CoeffCoupe, 0, Pne->NombreDeVariablesTrav * sizeof( double) );
for ( i = 0 ; i < NombreDeVariablesBinaires ; i++ ) {
	CoeffCoupe[NumeroDeLaVariableBinaire[i]] = CoeffDeLaVariableBinaire[i];
}
for ( i = 0 ; i < NombreDeVariablesSubstituees ; i++ ) {
  Var = NumeroDesVariablesSubstituees[i];
	/* On reconstitue Umin Umax */	
	TypeSubst = TypeDeSubsitution[i];
	if ( TypeDeSubsitution[i] == LOW ) {
	  CoeffCoupe[Var] = CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i];
    b += CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * Umin[Var];
	}	
	else if ( TypeDeSubsitution[i] == LOW_VARBIN ) {
	  CoeffCoupe[Var] = CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i];		
	  il = Mdeb[CntDeBorneInfVariable[Var]];				
		VarBin = Nuvar[il];
	  l = A[il];
	  CoeffCoupe[VarBin] -= CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * l;
		
		bBorne = -B[CntDeBorneInfVariable[Var]];
    b += CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * bBorne;
		
	}
	
	else if ( TypeDeSubsitution[i] == HIGH ) {
	  CoeffCoupe[Var] = -CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i];
    b -= CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * Umax[Var];	
	}		
	else if ( TypeDeSubsitution[i] == HIGH_VARBIN ) {
	  CoeffCoupe[Var] = -CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i];
		il = Mdeb[CntDeBorneSupVariable[Var]];		
		VarBin = Nuvar[il];
	  u = -A[il];
	  CoeffCoupe[VarBin] += CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * u;

		bBorne = B[CntDeBorneSupVariable[Var]];
    b -= CoeffDeLaVariableContinue * CoefficientDeLaVariableSubstituee[i] * bBorne;
		
	}
	else {
	  printf("BUG: TypeDeSubsitution %d inconnu i %d\n",TypeDeSubsitution[i],i);
		exit(0);
	}
}

S = 0.;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( CoeffCoupe[Var] != 0.0 ) {
	
	  S+= CoeffCoupe[Var] * U[Var];
		
	  NuVarCoupe[NbTermes] = Var;
	  CoeffCoupe[NbTermes] = CoeffCoupe[Var];
		NbTermes++;
		/*printf(" %e (%d) ",CoeffCoupe[Var],Var);*/
	}
}
/*printf(" < %e  NbTermes %d\n",b,NbTermes);*/

/*
if ( S < b ) {
  printf("MIR Marchand Wolsey la coupe respecte la contrainte ... bizarre ... S = %e respecte inferieur b = %e\n",S,b);
}
else {
  printf("Apres synthese de la coupe Violation = %e  S = %e  b = %e\n\n\n\n",S-b,S,b); 
}
*/

Violation = S-b;

if ( Violation < Pne->SeuilDeViolationMIR_MARCHAND_WOLSEY ) {
  goto FinSyntheseEtStockageMIR;
}

/* printf("MIR Marchand Wolsey apres synthese de la coupe Violation = %e NbTermes = %d\n",Violation,NbTermes); */

PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NbTermes, b, Violation, CoeffCoupe, NuVarCoupe );

FinSyntheseEtStockageMIR:

memset( (char *) CoeffCoupe, 0, Pne->NombreDeVariablesTrav * sizeof( double ) );
memset( (char *) NuVarCoupe, 0, Pne->NombreDeVariablesTrav * sizeof( int ) );

return;
}
/*----------------------------------------------------------------------------*/
char PNE_BoundSubstitution( PROBLEME_PNE * Pne,int NbVarContinues, int NbVarEntieres,
                            int * NumeroDesVariables, double * CoeffDesVariables,
														double SecondMembreContrainteAgregee,
														/* En sortie, la contrainte pour y faire une MIR */                            
														int * NombreDeVariablesBinaires, int * NumeroDeLaVariableBinaire,
                            double * CoeffDeLaVariableBinaire, double * b, double * CoeffDeLaVariableContinue,
								            double * ValeurDeLaVariableContinue,
														int * NombreDeVariablesSubstituees, int * NumeroDesVariablesSubstituees,
														char * TypeDeSubsitution, double * CoefficientDeLaVariableSubstituee,
														char * YaUneVariableS )
{
int i; int Var; int il; double Beta; int * CntDeBorneInfVariable; int * CntDeBorneSupVariable;
char l_valide; char u_valide; char VarEstSurBorneInf; char VarEstSurBorneSup; int VarBin;
 double AlphaJ; int * Mdeb; int * Nuvar; int Nb; int * T; double bBorne; char CodeRet;
double * A; double * U; double * Umin; double * Umax; double l; double u;
 int TypeBorne; int * TypeDeBorne; int Index; int IndexFin; double ValeurDeS; double * B;
double DeltaJ; double ValeurDeTJ; int NbVarB; int NbVarBinaires; int NombreDeVariables;

CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;
CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;
Mdeb = Pne->MdebTrav;
B = Pne->BTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
U = Pne->UTrav;
Umin = Pne->UminTravSv;
Umax = Pne->UmaxTravSv;
TypeDeBorne = Pne->TypeDeBorneTrav;
ValeurDeS = 0.0;
*NombreDeVariablesSubstituees = 0;
*YaUneVariableS = NON_PNE;

NombreDeVariables = Pne->NombreDeVariablesTrav;

/* A ce stade Coefficient_CG est nul et IndiceDeLaVariable_CG est nul */
T = Pne->IndiceDeLaVariable_CG;

/* Si la variable n'a pas de borne sup ou est sur sa borne inf on remplace par l'expression de sa borne inf */
/* Si la variable est sur sa borne sup on remplace par l'expression de sa borne sup */
/* Si AlphJ < 0 on remplace par l'expression de sa borne sup */
/* Si AlphJ > 0 on remplace par l'expression de sa borne inf */

Beta = SecondMembreContrainteAgregee;
NbVarB = 0;
CodeRet = NON_PNE;
l = 0.0;
u = 0.0;

NbVarBinaires = 0;
for ( i = 0 ; i < NbVarContinues ; i++ ) {
  Var = NumeroDesVariables[i];
	AlphaJ = CoeffDesVariables[i];
  TypeBorne	= TypeDeBorne[Var];

	if ( TypeDeBorne[Var] == VARIABLE_FIXE ) {
	  /* La variable continue est fixe: on la passe dans le second membre */
    Beta -= CoeffDesVariables[i] * U[Var]; 
		continue;		
	}
	 
	/* A ce stade on n'a jamais de variables bornees superieurement */
	if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT) {
	  printf("Attention BUG: a ce stade du calcul (les MIR) il ne doit pas exister de VARIABLE_BORNEE_SUPERIEUREMENT\n");
	}
	if ( TypeBorne == VARIABLE_NON_BORNEE ) {
    /* Substitution impossible et on ne peut pas non plus la passer dans s */
		/*printf("Bound substitution impossible car variable non bornee\n");*/
		goto FinBoundSubstitution;	
	}

	/* Presence d'une borne inf ou sup variable */	
	l_valide = NON_PNE;
	u_valide = NON_PNE;
	VarEstSurBorneInf = NON_PNE;
	VarEstSurBorneSup = NON_PNE;
	VarBin = -1;
	bBorne = 0.0;
  if ( CntDeBorneInfVariable != NULL ) {
    if ( CntDeBorneInfVariable[Var] >= 0 ) {				
		  il = Mdeb[CntDeBorneInfVariable[Var]];
			bBorne = -B[CntDeBorneInfVariable[Var]];
		  VarBin = Nuvar[il];
	    l = A[il];
		  l_valide = OUI_PNE;
		  if ( fabs( U[Var] - ( l * U[VarBin] ) ) < 1.e-9 ) VarEstSurBorneInf = OUI_PNE;
			if ( TypeDeBorne[VarBin] == VARIABLE_FIXE ) {			
			  l_valide = NON_PNE; 
				l = bBorne + ( l * U[VarBin] );
			}
	  }
	}
	else if ( fabs( U[Var] - Umin[Var] ) < 1.e-9 ) VarEstSurBorneInf = OUI_PNE;  
	
  if ( CntDeBorneSupVariable != NULL ) {
    if ( CntDeBorneSupVariable[Var] >= 0 ) {		
		  il = Mdeb[CntDeBorneSupVariable[Var]];
			bBorne = B[CntDeBorneSupVariable[Var]];
		  VarBin = Nuvar[il];
	    u = -A[il];
		  u_valide = OUI_PNE;
		  if ( fabs( U[Var] - ( u * U[VarBin] ) ) < 1.e-9 ) VarEstSurBorneSup = OUI_PNE;
			if ( TypeDeBorne[VarBin] == VARIABLE_FIXE ) {			
			  u_valide = NON_PNE; 
				u = bBorne + ( u * U[VarBin] );
			}			
	  }
	}
	else if ( fabs( U[Var] - Umax[Var] ) < 1.e-9 ) VarEstSurBorneSup = OUI_PNE;
	
  /* Attention aux variables binaires de substitution qui sont fixes */
	
  /* Si la variable n'a pas de borne sup ou est sur sa borne inf on remplace par l'expression de sa borne inf */
  if ( (TypeDeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT && u_valide == NON_PNE ) || VarEstSurBorneInf == OUI_PNE ) {
    if ( l_valide == OUI_PNE ) {
		  /* On fait x = l * y + t */
						
      if ( T[VarBin] == 0 ) {
			  NumeroDeLaVariableBinaire[NbVarBinaires] = VarBin;
		    CoeffDeLaVariableBinaire[NbVarBinaires] = AlphaJ * l;
        T[VarBin] = NombreDeVariables + NbVarBinaires;
        NbVarBinaires++;
			}
      else {
			  Nb = T[VarBin] - NombreDeVariables;
		    CoeffDeLaVariableBinaire[Nb] += AlphaJ * l;
			}

			Beta -= AlphaJ * bBorne;
			
		  ValeurDeTJ = U[Var] - ( l * U[VarBin] ) - bBorne;						
		}
		else {
		  /* Pas de borne inf variable */
			Beta -= AlphaJ * Umin[Var];
		  ValeurDeTJ = U[Var] - Umin[Var];
		}
		DeltaJ = AlphaJ;
		if ( DeltaJ < 0 ) {
		  ValeurDeS += -DeltaJ * ValeurDeTJ;
      *YaUneVariableS = OUI_PNE;			 
			NumeroDesVariablesSubstituees[*NombreDeVariablesSubstituees] = Var;
			if ( l_valide == OUI_PNE ) TypeDeSubsitution[*NombreDeVariablesSubstituees] = LOW_VARBIN;
			else TypeDeSubsitution[*NombreDeVariablesSubstituees] = LOW;
			CoefficientDeLaVariableSubstituee[*NombreDeVariablesSubstituees] = DeltaJ;
			*NombreDeVariablesSubstituees = *NombreDeVariablesSubstituees + 1;
		}		
		continue;
	}
  /* Si la variable est sur sa borne sup on remplace par l'expression de sa borne sup */
  if ( VarEstSurBorneSup == OUI_PNE ) {
    if ( u_valide == OUI_PNE ) {
		  /* On fait x = u * y - t */
						
      if ( T[VarBin] == 0 ) {
			  NumeroDeLaVariableBinaire[NbVarBinaires] = VarBin;
		    CoeffDeLaVariableBinaire[NbVarBinaires] = AlphaJ * u;
        T[VarBin] = NombreDeVariables + NbVarBinaires;
        NbVarBinaires++;
			}
      else {
			  Nb = T[VarBin] - NombreDeVariables;
		    CoeffDeLaVariableBinaire[Nb] += AlphaJ * u;
			}
			
			Beta -= AlphaJ * bBorne;
									
		  ValeurDeTJ = bBorne + ( u * U[VarBin] ) - U[Var];			
		}
		else {
		  /* Pas de borne sup variable */
			Beta -= AlphaJ * Umax[Var];
		  ValeurDeTJ = Umax[Var] - U[Var];
		}
		DeltaJ = -AlphaJ;
		if ( DeltaJ < 0 ) {
		  ValeurDeS += -DeltaJ * ValeurDeTJ;
      *YaUneVariableS = OUI_PNE;			 			
			NumeroDesVariablesSubstituees[*NombreDeVariablesSubstituees] = Var;
			if ( u_valide == OUI_PNE ) TypeDeSubsitution[*NombreDeVariablesSubstituees] = HIGH_VARBIN;
			else TypeDeSubsitution[*NombreDeVariablesSubstituees] = HIGH;			
			CoefficientDeLaVariableSubstituee[*NombreDeVariablesSubstituees] = DeltaJ;
			*NombreDeVariablesSubstituees = *NombreDeVariablesSubstituees + 1;			
		}
		continue;
	}
  /* Si AlphaJ < 0 on remplace par l'expression de sa borne sup */
	if ( AlphaJ < 0.0 ) {
    if ( u_valide == OUI_PNE ) {
		  /* On fait x = u * y - t */
						
      if ( T[VarBin] == 0 ) {
			  NumeroDeLaVariableBinaire[NbVarBinaires] = VarBin;
		    CoeffDeLaVariableBinaire[NbVarBinaires] = AlphaJ * u;
        T[VarBin] = NombreDeVariables + NbVarBinaires;
        NbVarBinaires++;
			}
      else {
			  Nb = T[VarBin] - NombreDeVariables;
		    CoeffDeLaVariableBinaire[Nb] += AlphaJ * u;
			}
			
			Beta -= AlphaJ * bBorne;
			
		  ValeurDeTJ = bBorne + ( u * U[VarBin] ) - U[Var];						
		}
		else {  
		  /* Pas de borne sup variable */
			Beta -= AlphaJ * Umax[Var];
		  ValeurDeTJ = Umax[Var] - U[Var];			
		}
		DeltaJ = -AlphaJ;
		if ( DeltaJ < 0 ) {
		  ValeurDeS += -DeltaJ * ValeurDeTJ;
      *YaUneVariableS = OUI_PNE;			 						
			NumeroDesVariablesSubstituees[*NombreDeVariablesSubstituees] = Var;
			if ( u_valide == OUI_PNE ) TypeDeSubsitution[*NombreDeVariablesSubstituees] = HIGH_VARBIN;
			else TypeDeSubsitution[*NombreDeVariablesSubstituees] = HIGH;				
			CoefficientDeLaVariableSubstituee[*NombreDeVariablesSubstituees] = DeltaJ;
			*NombreDeVariablesSubstituees = *NombreDeVariablesSubstituees + 1;						
		}
		continue;
	}
  else {
    /* Si AlphaJ < 0 on remplace par l'expression de sa borne inf */
    if ( l_valide == OUI_PNE ) {
		  /* On fait x = l * y + t */
						
      if ( T[VarBin] == 0 ) {
			  NumeroDeLaVariableBinaire[NbVarBinaires] = VarBin;
		    CoeffDeLaVariableBinaire[NbVarBinaires] = AlphaJ * l;
        T[VarBin] = NombreDeVariables + NbVarBinaires;
        NbVarBinaires++;
			}
      else {
			  Nb = T[VarBin] - NombreDeVariables;
		    CoeffDeLaVariableBinaire[Nb] += AlphaJ * l;
			}
			
			Beta -= AlphaJ * bBorne;
					 					
		  ValeurDeTJ = U[Var] - ( l * U[VarBin] ) - bBorne;
		}
		else {
		  /* Pas de borne inf variable */
			Beta -= AlphaJ * Umin[Var];
		  ValeurDeTJ = U[Var] - Umin[Var];			
		}
		DeltaJ = AlphaJ;
		if ( DeltaJ < 0 ) {
		  ValeurDeS += -DeltaJ * ValeurDeTJ;
      *YaUneVariableS = OUI_PNE;			 									
			NumeroDesVariablesSubstituees[*NombreDeVariablesSubstituees] = Var;
			if ( l_valide == OUI_PNE ) TypeDeSubsitution[*NombreDeVariablesSubstituees] = LOW_VARBIN;
			else TypeDeSubsitution[*NombreDeVariablesSubstituees] = LOW;							
			CoefficientDeLaVariableSubstituee[*NombreDeVariablesSubstituees] = DeltaJ;
			*NombreDeVariablesSubstituees = *NombreDeVariablesSubstituees + 1;					
		}
		continue;
	}
}

*b = Beta;
*CoeffDeLaVariableContinue = 1.0;
*ValeurDeLaVariableContinue = ValeurDeS;

/* On complete avec les variables binaires existantes */
IndexFin = NombreDeVariables - 1;
for ( i = 0 , Index = IndexFin ; i < NbVarEntieres ; i++ , Index-- ) {
  VarBin = NumeroDesVariables[Index];
  if ( T[VarBin] == 0 ) {
		NumeroDeLaVariableBinaire[NbVarBinaires] = VarBin;
		CoeffDeLaVariableBinaire[NbVarBinaires] = CoeffDesVariables[Index];
    T[VarBin] = NombreDeVariables + NbVarBinaires;
    NbVarBinaires++;
	}
  else {
		Nb = T[VarBin] - NombreDeVariables;
		CoeffDeLaVariableBinaire[Nb] += CoeffDesVariables[Index];
	}			
}

*NombreDeVariablesBinaires = NbVarBinaires;
CodeRet = OUI_PNE;

FinBoundSubstitution:

/* RAZ de T */
for ( i = 0 ; i < NbVarBinaires ; i++ ) T[NumeroDeLaVariableBinaire[i]] = 0;

return( CodeRet );
}

/*----------------------------------------------------------------------------*/

char PNE_Agregation( PROBLEME_PNE * Pne, char * VariableDansContrainteMixte, int * NbVarCont, int * NbVarEnt,
                     int * NumeroDesVariables, double * CoeffDesVariables,
										 double * SecondMembreContrainteAgregee, char * ContrainteAgregee,
										 char * VariableSelectionable, int * NbV, int * NumV, int * NbC, int * NumC	)
{
int IndexFin; int NombreDeVariablesTrav; int i; int Var; int * CntDeBorneSupVariable; int * CntDeBorneInfVariable;
double * UTrav; char * ContrainteMixte; double B; double * BTrav; int NbCSv;
int Cont; int * MdebTrav; int * NbTermTrav; double * ATrav; double * UminTrav; double * UmaxTrav;
int * NuvarTrav; double f; int VarBin; int il; int ilMax; double Fmax; int Kappa; double AlfaKappa;
int * CdebTrav; int * CsuiTrav; int * NumContrainteTrav; int ic; int Cnt; double AlfaJ;
double Gamma; double * V; int * T; int Index; int * TypeDeVariableTrav; double l; double u; char l_valide; char u_valide;
int * TypeDeBorneTrav; int TypeBorne; char * SensContrainteTrav; double bBorne; 
int NbVarContinues; int NbVarEntieres; int Nn; int j; int NbTestAggregCnt; int NbTestAggregVar;
int Nb; int * ContrainteSaturee; char NUtiliserQueLesContraintesSaturees;

NbVarContinues = *NbVarCont;
NbVarEntieres = *NbVarEnt;

if ( NbVarEntieres == 0 && 0 ) return( NON_PNE ); /* J'ai rajoute ca le 24/10/2014 */

/* Attention a la restriction: pour l'instant on a 1 seule variable bound par variable */

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
IndexFin = NombreDeVariablesTrav - 1;
CntDeBorneSupVariable = Pne->CntDeBorneSupVariable;
CntDeBorneInfVariable = Pne->CntDeBorneInfVariable;
UTrav = Pne->UTrav;
UminTrav = Pne->UminTravSv;
UmaxTrav = Pne->UmaxTravSv;
SensContrainteTrav = Pne->SensContrainteTrav;

BTrav = Pne->BTrav;
MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
ATrav = Pne->ATrav;   
NuvarTrav = Pne->NuvarTrav;
CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
ContrainteMixte = Pne->ContrainteMixte;

ContrainteSaturee = Pne->ContrainteSaturee;

if ( Pne->NombreDeContraintesTrav > Pne->NombreDeVariablesTrav && Pne->NombreDeContraintesTrav > 20000 ) NUtiliserQueLesContraintesSaturees = OUI_PNE;
else NUtiliserQueLesContraintesSaturees = NON_PNE;

/* La contrainte est rangee dans les vecteurs NumeroDesVariables CoeffDesVariables.
   Pour les variables continues on commence a 0. Pour les variables entieres on commence
	 a NombreDeVariablesTrav - 1 */

NbTestAggregVar = 0;


/* A essayer:
balayer les variables du graphe de conflit reliees aux variables entieres de la contrainte
on ne choisi les contraintes a concatener que parmi ces contraintes
*/

VARTEST:
	 
Fmax = -1.;
Kappa = -1;
AlfaJ = 0.0;
l = 0.0;
u = 0.0;
f = 0.0;

/* Choix de la variable continue P* et de la contrainte a ajouter */

for ( i = 0 ; i < NbVarContinues ; i++ ) {
  Var = NumeroDesVariables[i];

  if ( VariableSelectionable[Var] == NON_PNE ) continue;
	
	/* Remarque: la seule contrainte mixte de la variable a pu etre agregee mais comme
	   c'est long a tester on ne le fait pas */
	if ( VariableDansContrainteMixte[Var] == 0 ) continue;

  if ( UminTrav[Var] == UmaxTrav[Var] ) continue;	
	
	/* Presence d'une borne inf ou sup variable */
	TypeBorne = TypeDeBorneTrav[Var];		
	l_valide = NON_PNE;
	u_valide = NON_PNE;  
	bBorne = 0.0;	
	if ( CntDeBorneInfVariable != NULL ) {
    if ( CntDeBorneInfVariable[Var] >= 0 ) {
		  Cnt = CntDeBorneInfVariable[Var];			
			bBorne = -BTrav[Cnt];			
		  il = MdebTrav[Cnt];
		  VarBin = NuvarTrav[il]; /* Ne sert plus a rien */
	    l = ATrav[il];
		  l_valide = OUI_PNE;
	  }
	}
	if ( CntDeBorneSupVariable != NULL ) {
    if ( CntDeBorneSupVariable[Var] >= 0 ) {
		  Cnt = CntDeBorneSupVariable[Var];			
			bBorne = BTrav[Cnt];
		  il = MdebTrav[Cnt];			
	    u = -ATrav[il];
		  u_valide = OUI_PNE;
	  }  
	}
  if ( l_valide == NON_PNE ) {
    if ( TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT || TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) {
	    l = UminTrav[Var];
		  l_valide = OUI_PNE;
		}
	}
  if ( u_valide == NON_PNE ) {
    if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT || TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES ) {
	    u = UmaxTrav[Var];
		  u_valide = OUI_PNE;
	  }
  }
	
	/* Remarque: les contraintes d'egalite sont classees en premier */

	if ( u_valide == NON_PNE && l_valide == NON_PNE ) {
    f = LINFINI_PNE;
  }	
	else if ( u_valide == OUI_PNE ) {
	  f = fabs( u - UTrav[Var] );
    if ( l_valide ) {
	    if ( f > fabs( UTrav[Var] - l ) ) f = fabs( UTrav[Var] - l );
		}
	}
	else if ( l_valide == OUI_PNE ) f = fabs( UTrav[Var] - l );
	
	/* La variable ne doit pas etre sur borne */
	if ( f < Z0 ) {
	  if ( VariableSelectionable[Var] == OUI_PNE ) {
	    VariableSelectionable[Var] = NON_PNE;
		  NumV[*NbV] = Var;
		  *NbV = *NbV + 1;
		}
		continue;
	}
	if ( f > Fmax ) {
    Fmax = f;
		Kappa = Var;
		AlfaJ = CoeffDesVariables[i];		
	}
}

if ( Kappa < 0 ) {
  /* Rien a agreger */
	return( NON_PNE );
}

/* Choix de la contrainte */

NbTestAggregCnt = 0;
NbCSv = *NbC;

CNTTEST:

Cnt = -1;
Nb = -1;
Nn = 0;

/* Les contraintes d'egalite sont classees en premier */

AlfaKappa = 0.;
ic = CdebTrav[Kappa];
while ( ic >= 0 ) {
	Cont = NumContrainteTrav[ic];	
	if ( SensContrainteTrav[Cont] != '=' ) break;
	if ( ATrav[ic] != 0.0 ) {
    if ( ContrainteMixte[Cont] == OUI_PNE && ContrainteAgregee[Cont] == NON && fabs( AlfaJ / ATrav[ic] ) < 1.e+9 ) {
	    Cnt = Cont;
      AlfaKappa = ATrav[ic];			
		  break;
    }
	}
  ic = CsuiTrav[ic];
}
if ( Cnt >= 0 ) goto ContrainteChoisie;

/* Le cas echeant on continue et alors il s'agit de contraintes d'inegalite */
/* Peut-etre serait-il plus rapide de ne prendre en compte que les contraintes d'inegalite qui sont saturees */
if ( NUtiliserQueLesContraintesSaturees == OUI_PNE ) {
  while ( ic >= 0 ) {
	  Cont = NumContrainteTrav[ic];		
	  if ( ContrainteSaturee[Cont] == OUI_PNE ) {
	    if ( ATrav[ic] != 0.0 ) {
        if ( ContrainteMixte[Cont] == OUI_PNE && ContrainteAgregee[Cont] == NON && fabs( AlfaJ / ATrav[ic] ) < 1.e+9 ) {	
	        Cnt = Cont;
          AlfaKappa = ATrav[ic];			
		      break;
        }
      }
	  }
    ic = CsuiTrav[ic];
	}
}
else {
  while ( ic >= 0 ) {
	  Cont = NumContrainteTrav[ic];		
	  if ( ATrav[ic] != 0.0 ) {
      if ( ContrainteMixte[Cont] == OUI_PNE && ContrainteAgregee[Cont] == NON && fabs( AlfaJ / ATrav[ic] ) < 1.e+9 ) {	
	      Cnt = Cont;
        AlfaKappa = ATrav[ic];			
		    break;
      }
    }
    ic = CsuiTrav[ic];
  }
}

ContrainteChoisie:
if ( Cnt < 0 ) {
	NbTestAggregVar++;
  if ( NbTestAggregVar <= MAX_CYCLES_SUR_VARIABLES ) {
	  if ( VariableSelectionable[Kappa] == OUI_PNE ) {
	    VariableSelectionable[Kappa] = NON_PNE;
		  NumV[*NbV] = Kappa;
		  *NbV = *NbV + 1;
		}
		goto VARTEST;
	}	
  return( NON_PNE );
}

if ( ContrainteAgregee[Cnt] == NON_PNE ) {
  ContrainteAgregee[Cnt] = OUI_PNE;	
  NumC[*NbC] = Cnt;
  *NbC = *NbC + 1;
}

if ( AlfaKappa == 0. ) return( NON_PNE );

Gamma = -( AlfaJ / AlfaKappa );

/* S'il decoule de la nouvelle contrainte agregee une variable d'ecart qui prend un coeff negatif on
   refuse l'agregation */
if ( SensContrainteTrav[Cnt] == '<' ) {
  if ( Gamma < 0.0 ) {   
	  NbTestAggregCnt++;
	  if ( NbTestAggregCnt <= MAX_CYCLES_SUR_CONTRAINTES ) {
      /*ContrainteAgregee[Cnt] = NON_PNE;*/ /* Non: il ne faut plus la prendre en compte pour la variable Kappa */		
		  goto CNTTEST;
		}
	  NbTestAggregVar++;
	  if ( NbTestAggregVar <= MAX_CYCLES_SUR_VARIABLES ) {
		  if ( VariableSelectionable[Kappa] == OUI_PNE ) {			
		    VariableSelectionable[Kappa] = NON_PNE;
		    NumV[*NbV] = Kappa;
		    *NbV = *NbV + 1;
			}
      for ( i = NbCSv; i < *NbC ; i++ ) ContrainteAgregee[NumC[i]] = NON_PNE;				
			*NbC = NbCSv;
			goto VARTEST;
		}						
		return( NON_PNE );
  }
}

V = (double *) Pne->Coefficient_CG;
T = (int *) Pne->IndiceDeLaVariable_CG;

/* Expand de la contrainte dans V */
B = BTrav[Cnt];
il = MdebTrav[Cnt];
ilMax = il + NbTermTrav[Cnt];
while ( il < ilMax ) {
  Var = NuvarTrav[il];
  if ( TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
    B -= ATrav[il] * UTrav[Var];
	}	
  else {
	  if ( ATrav[il] != 0.0 ) {
      V[Var] = Gamma * ATrav[il];
	    T[Var] = 1;
		}
	}	
  il++;
}
B *= Gamma;

/* Combinaision avec l'existant */
for ( i = 0 ; i < NbVarContinues ; i++ ) {
  Var = NumeroDesVariables[i];
	if ( T[Var] == 1 ) {
	  CoeffDesVariables[i] += V[Var];		
		V[Var] = 0.0;
		T[Var] = 0;
	}	
}
for ( i = 0 , Index = IndexFin ; i < NbVarEntieres ; i++ , Index-- ) {
  Var = NumeroDesVariables[Index];	
	if ( T[Var] == 1 ) {
    CoeffDesVariables[Index] += V[Var];
		V[Var] = 0.0;
		T[Var] = 0;			
  }	
}

/* On complete avec la partie de la contrainte ajoutee qui n'a pas pu etre prise en compte */
il = MdebTrav[Cnt];
ilMax = il + NbTermTrav[Cnt];
while ( il < ilMax ) {
  Var = NuvarTrav[il];		
	if ( T[Var] != 0 ) {
    if ( TypeDeVariableTrav[Var] == ENTIER ) {
      CoeffDesVariables[Index] = V[Var];
		  NumeroDesVariables[Index] = Var;
      Index--;
			NbVarEntieres++;
		}
		else {
	    CoeffDesVariables[NbVarContinues] = V[Var];
		  NumeroDesVariables[NbVarContinues] = Var;
		  NbVarContinues++;	
		}
		V[Var] = 0.0;
		T[Var] = 0;		
	}
  il++;
}

/* On enleve tous les coeff de variables continues qui sont a 0 et au passage la 
   variable continue Kappa dont le coefficient est devenu nul */
Nn = 0;
for ( i = 0 ; i < NbVarContinues ; i++ ) {
	if ( fabs( CoeffDesVariables[i] ) < VALEUR_NULLE_DUN_COEFF ) {	
	  for ( j = i + 1 ; j < NbVarContinues ; j++ ) {
		  if ( fabs( CoeffDesVariables[j] ) >= VALEUR_NULLE_DUN_COEFF ) {
		    CoeffDesVariables[i] = CoeffDesVariables[j];
				CoeffDesVariables[j] = 0.0;
			  NumeroDesVariables[i] = NumeroDesVariables[j];
				Nn++;
				break;
			}
		}
	}
	else Nn++;
} 
NbVarContinues = Nn;

/* On enleve tous les coeff de variables entieres qui sont a 0 */
Nn = 0;
for ( i = 0 , Index = IndexFin ; i < NbVarEntieres ; i++ , Index-- ) {
	if ( fabs( CoeffDesVariables[Index] ) < VALEUR_NULLE_DUN_COEFF ) {	
	  for ( j = Index - 1 ; j >= IndexFin - NbVarEntieres + 1 ; j-- ) {		
	    if ( fabs( CoeffDesVariables[j] ) >= VALEUR_NULLE_DUN_COEFF ) {
		    CoeffDesVariables[Index] = CoeffDesVariables[j];
				CoeffDesVariables[j] = 0.0;
			  NumeroDesVariables[Index] = NumeroDesVariables[j];
				Nn++;
				break;
	 		}
    }
	}
	else Nn++;
}
NbVarEntieres = Nn;

*SecondMembreContrainteAgregee = *SecondMembreContrainteAgregee + B;  

*NbVarCont = NbVarContinues;
*NbVarEnt = NbVarEntieres;

/* Construction de la constrainte sur laquelle on va faire la MIR */
return( OUI_PNE );
}

/*----------------------------------------------------------------------------*/
  
void PNE_MIRMarchandWolsey( PROBLEME_PNE * Pne )
{
int Cnt; int Var; char * ContrainteMixte; int il; int ilMax; int * Mdeb; int * NbTerm;
int * NumeroDesVariables; double * CoeffDesVariables; int NbVarContinues; int NbVarEntieres;
double * A; int * Nuvar; int IndexFin; int Index; char * ContrainteAgregee;
int * TypeDeVariable; char Agreg; char OK; double * B; double * X;
int NombreDeVariablesBinaires; int * NumeroDeLaVariableBinaire; double * CoeffDeLaVariableBinaire;
double b; double CoeffDeLaVariableContinue; double ValeurDeLaVariableContinue;
double SecondMembreContrainteAgregee; int NbAgr; char MIRviolee; int NombreDeVariablesSubstituees;
int * NumeroDesVariablesSubstituees; double * CoefficientDeLaVariableSubstituee;
char * TypeDeSubsitution; int NombreDeVariables; int NombreDeContraintes;
int * TypeDeBorne; char * TasPourKnapsack; char * pt; int LallocTas; 
char * VariableDansContrainteMixte; int NbMIRCreees; char YaUneVariableS; double * L; BB * Bb;
char * VariableSelectionable; double * Xmin; double * Xmax;
int NbV; int * NumV; int NbC; int * NumC;

if ( Pne->ContrainteMixte == NULL ) return;

/* L'agregation est couteuse en temps */
/*
if ( Pne->CalculDeMIRmarchandWolsey == NON_PNE ) {
  Pne->NbEvitementsDeCalculsMIRmarchandWolsey++;
	if ( Pne->NbEvitementsDeCalculsMIRmarchandWolsey >= SEUIL_EVITEMENT_MIR_MARCHAND_WOLSEY ) { 
    Pne->NbEvitementsDeCalculsMIRmarchandWolsey = 0;
	  Pne->CalculDeMIRmarchandWolsey = OUI_PNE;
		Pne->NbEchecsConsecutifsDeCalculsMIRmarchandWolsey = 0;
	}
	else {
	  return;
	}
}
*/

Bb = (BB *) Pne->ProblemeBbDuSolveur;

if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > 20 ) return;

if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > Pne->ProfondeurMirMarchandWolseTrouvees ) return;

NbMIRCreees = 0;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
L = Pne->LTrav;

CoeffDesVariables  = Pne->ValeurLocale;
NumeroDesVariables = Pne->IndiceLocal;

LallocTas  = 0; 
LallocTas += NombreDeVariables * sizeof( int ); /* Pour NumeroDesVariablesSubstituees */
LallocTas += NombreDeVariables * sizeof( double ); /* Pour CoefficientDeLaVariableSubstituee */
LallocTas += NombreDeVariables * sizeof( int ); /* Pour NumeroDeLaVariableBinaire */
LallocTas += NombreDeVariables * sizeof( double ); /* Pour CoeffDeLaVariableBinaire */
LallocTas += NombreDeVariables * sizeof( char ); /* Pour TypeDeSubsitution */
LallocTas += NombreDeContraintes * sizeof( char ); /* Pour ContrainteAgregee */
LallocTas += NombreDeVariables * sizeof( char ); /* Pour VariableDansContrainteMixte */
LallocTas += NombreDeVariables * sizeof( char ); /* Pour VariableSelectionable */

LallocTas += NombreDeVariables * sizeof( int ); /* Pour NumV */
LallocTas += NombreDeContraintes * sizeof( int ); /* Pour NumC */

TasPourKnapsack = (char *) malloc( LallocTas );
if ( TasPourKnapsack == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_MIRMarchandWolsey \n");
	return;
}

pt = TasPourKnapsack;
NumeroDesVariablesSubstituees = (int *) pt;
pt += NombreDeVariables * sizeof( int );
CoefficientDeLaVariableSubstituee = (double *) pt;
pt += NombreDeVariables * sizeof( double );
NumeroDeLaVariableBinaire = (int *) pt;
pt += NombreDeVariables * sizeof( int );
CoeffDeLaVariableBinaire = (double *) pt;
pt += NombreDeVariables * sizeof( double );
TypeDeSubsitution = (char *) pt;
pt += NombreDeVariables * sizeof( char );
ContrainteAgregee = (char *) pt;
pt += NombreDeContraintes * sizeof( char );
VariableDansContrainteMixte = (char *) pt;
pt += NombreDeVariables * sizeof( char );
VariableSelectionable = (char *) pt;
pt += NombreDeVariables * sizeof( char );

NumV = (int *) pt;
pt += NombreDeVariables * sizeof( int );
NumC = (int *) pt;
pt += NombreDeContraintes * sizeof( int );

ContrainteMixte = Pne->ContrainteMixte;
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
X = Pne->UTrav;
Xmin = Pne->UminTravSv;
Xmax = Pne->UmaxTravSv;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
IndexFin = Pne->NombreDeVariablesTrav - 1;

memset( (char *) VariableDansContrainteMixte, 0, NombreDeVariables * sizeof( char ) );

memset( (char *) ContrainteAgregee, NON_PNE, NombreDeContraintes * sizeof( char ) );
memset( (char *) VariableSelectionable, OUI_PNE, NombreDeVariables * sizeof( char ) );

/* RAZ de Coefficient_CG et IndiceDeLaVariable_CG car utilises et razes ensuite
   dans la phase d'agregation */
memset( (char *) Pne->Coefficient_CG, 0, NombreDeVariables * sizeof( double ) );
memset( (char *) Pne->IndiceDeLaVariable_CG, 0, NombreDeVariables * sizeof( int ) );

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteMixte[Cnt] == NON_PNE ) continue;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  VariableDansContrainteMixte[Nuvar[il]] = 1;
		il++;
  }	
}

/*
goto TestGrapheDeConflits;
*/

/* Choix d'une contrainte mixte pour demarrer le processus d'agregation */

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) { 

  if ( ContrainteMixte[Cnt] == NON_PNE && 0 ) continue;

  if ( Pne->FoisCntSuccesMirMarchandWolseyTrouvees[Cnt] > MAX_FOIS_SUCCESS_MIR ) continue;
	
	/* On tente Marchand-Wolsey */
  /*
	printf(" Tentative Marchand Wolsey sur Cnt %d \n",Cnt);  	
	*/
	NbVarContinues = 0;
	NbVarEntieres = 0; 
	il = Mdeb[Cnt];
	
	if ( NbTerm[Cnt] > 10000 ) { 
	  /*printf(" Tentative Marchand Wolsey rejetee car NbTermTrav %d \n",NbTermTrav[Cnt]);*/
	  continue;
	}
	
	SecondMembreContrainteAgregee = B[Cnt];
  ilMax = il + NbTerm[Cnt];
	Index = IndexFin;
  while ( il < ilMax ) {
	  if ( A[il] == 0.0 ) goto NextIl;
	  Var = Nuvar[il];
    if ( TypeDeBorne[Var] == VARIABLE_FIXE ) {
      SecondMembreContrainteAgregee -= A[il] * X[Var];
			goto NextIl;
		}
		else if ( Xmin[Var] == Xmax[Var] ) {
      SecondMembreContrainteAgregee -= A[il] * Xmin[Var];
			goto NextIl;
		}		
    if ( TypeDeVariable[Var] == ENTIER ) {			
	    CoeffDesVariables[Index] = A[il];
		  NumeroDesVariables[Index] = Var;
		  Index--;
		  NbVarEntieres++;
    }
	  else {
	    CoeffDesVariables[NbVarContinues] = A[il];
		  NumeroDesVariables[NbVarContinues] = Var;
		  NbVarContinues++;	
	  }
		NextIl:
		il++;
	}
	 
	NbAgr = 1;
	NbC = 0;
	NbV = 0;

  ContrainteAgregee[Cnt] = OUI_PNE;
  NumC[NbC] = Cnt;
  NbC = NbC + 1;

  DEBUT_MIR:
	
  /*printf("1- NbAgr %d Cnt %d Avant BoundSubstitution NbVarContinues %d NbVarEntieres %d \n",NbAgr,Cnt,NbVarContinues,NbVarEntieres);*/
	
	if ( NbVarContinues + NbVarEntieres == 0 ) goto RAZ;
		
  OK = PNE_BoundSubstitution( Pne, NbVarContinues, NbVarEntieres, NumeroDesVariables, CoeffDesVariables,
                            	SecondMembreContrainteAgregee,
	                            /* En sortie, la contrainte pour y faire une MIR */                             
															&NombreDeVariablesBinaires, NumeroDeLaVariableBinaire,
                              CoeffDeLaVariableBinaire, &b, &CoeffDeLaVariableContinue,
								              &ValeurDeLaVariableContinue,
															&NombreDeVariablesSubstituees, NumeroDesVariablesSubstituees, TypeDeSubsitution,
														  CoefficientDeLaVariableSubstituee, &YaUneVariableS );
															
	if ( OK == OUI_PNE && NombreDeVariablesBinaires >= 1 ) {
	  /* Si la valeur de la variable continue est negative, c'est a cause des imprecisions et on evite de faire un calcul */
		if ( ValeurDeLaVariableContinue < 0.0 ) goto RAZ;
	
    # if CALCULS_SUR_MIXED_0_1_KNAPSACK == OUI_PNE
			if ( YaUneVariableS == OUI_PNE && ValeurDeLaVariableContinue != 0.0 ) {
        PNE_Knapsack_0_1_AvecVariableContinue( Pne,
                                               NombreDeVariablesBinaires, NumeroDeLaVariableBinaire, CoeffDeLaVariableBinaire,
                                               b, ValeurDeLaVariableContinue,
												                       NombreDeVariablesSubstituees, NumeroDesVariablesSubstituees,
												                       TypeDeSubsitution, CoefficientDeLaVariableSubstituee
																						 );					
      }
			goto RAZ;			
		# endif
	
    /* cMIR separation */		
	  /*printf("2- Avant CMIR NombreDeVariablesBinaires %d b = %e ValeurDeLaVariableContinue = %e\n",NombreDeVariablesBinaires,b,ValeurDeLaVariableContinue);*/		
		
    MIRviolee= PNE_C_MIR( Pne, NombreDeVariablesBinaires, NumeroDeLaVariableBinaire,
                          CoeffDeLaVariableBinaire, &b, &CoeffDeLaVariableContinue,
							            ValeurDeLaVariableContinue );

    if ( MIRviolee == OUI_PNE ) {      
		  /*printf("MIR violee, contrainte de depart: %d - nb termes de la contrainte: %d - NbAgr: %d\n",Cnt,NbTerm[Cnt],NbAgr);*/
			
		  NbMIRCreees++;
			if ( NbAgr == 1 ) Pne->FoisCntSuccesMirMarchandWolseyTrouvees[Cnt]++;
			
	    PNE_SyntheseEtStockageMIR( Pne, NombreDeVariablesBinaires, NumeroDeLaVariableBinaire,
                                 CoeffDeLaVariableBinaire, b, CoeffDeLaVariableContinue,
								                 NombreDeVariablesSubstituees,
																 NumeroDesVariablesSubstituees, TypeDeSubsitution, 
																 CoefficientDeLaVariableSubstituee );													
		  goto RAZ;
	  }
	}
		
	/*printf("3- Avant Agregation\n");*/
	if ( NbVarEntieres > TROP_DE_VARIABLES_ENTIERES && 0 ) goto RAZ;
	
	/* Attention. Dans l'article Marchand-Wolsey il est dit qu'on peut aller jusqu'a 5. Mais experimentalement
	   on constate qu'il ne faut aller aussi loin a cause des imprecisions */
	if ( NbAgr < MAX_AGREG ) {
    /* memset( (char *) VariableSelectionable, OUI_PNE, NombreDeVariables * sizeof( char ) ); */	
	  /*printf("3- PNE_Agregation\n");*/		
    Agreg = PNE_Agregation( Pne, VariableDansContrainteMixte, &NbVarContinues, &NbVarEntieres, NumeroDesVariables,
		                        CoeffDesVariables, &SecondMembreContrainteAgregee, ContrainteAgregee, VariableSelectionable,
														&NbV, NumV, &NbC, NumC );														
		if ( Agreg == NON_PNE || NbVarContinues + NbVarEntieres == 0 ) {
		  /*printf("Echec agregation \n");*/		
		  goto RAZ;
		}		
	  NbAgr++;
		
	  goto DEBUT_MIR;
	}
	else {	  
	  /*printf(" Marchand Wolsey arretee car NbAgr > MAX_AGREG \n");*/		
	}

	RAZ:

  for ( il = 0 ; il < NbV ; il++ ) VariableSelectionable[NumV[il]] = OUI_PNE;
  for ( il = 0 ; il < NbC ; il++ ) {
	  if ( NumC[il] < 0 || NumC[il] >= NombreDeContraintes ) {
      printf("Bug NumC[%d] = %d\n",il,NumC[il]);
			exit(0);
		}
	  ContrainteAgregee[NumC[il]] = NON_PNE;
	}
	
}

/*
TestGrapheDeConflits:
*/
/* Ca sert a rien alors j'ai mis un return dedans */
/*PNE_MIRMarchandWolseySurContrainteConcateeGrapheDeConflits( Pne, CoeffDesVariables, NumeroDesVariables,
																													 	ContrainteAgregee, VariableDansContrainteMixte,
																												   	CoefficientDeLaVariableSubstituee, NumeroDesVariablesSubstituees,
																													 	TypeDeSubsitution, CoeffDeLaVariableBinaire,
																													 	NumeroDeLaVariableBinaire, VariableSelectionable, ContrainteMixte );
*/
/* Fin test avec graphes de conflits */

free( TasPourKnapsack );

if ( NbMIRCreees == 0 ) {	
  Pne->NbEchecsConsecutifsDeCalculsMIRmarchandWolsey++;
	if ( Pne->NbEchecsConsecutifsDeCalculsMIRmarchandWolsey >= NB_ECHECS_INHIB_MIR ) {
    Pne->CalculDeMIRmarchandWolsey = NON_PNE;
	}
}
else {
  /* On a cree une ou plusieurs MIR => on remet le compteur a 0 */
	Pne->NbEchecsConsecutifsDeCalculsMIRmarchandWolsey = 0;
	if ( Bb->NoeudEnExamen->ProfondeurDuNoeud + 1 > Pne->ProfondeurMirMarchandWolseTrouvees ) {
    Pne->ProfondeurMirMarchandWolseTrouvees = Bb->NoeudEnExamen->ProfondeurDuNoeud + 1;
  }
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
	  /*
		printf("Mir cuts found %d at depth %d\n",NbMIRCreees,Bb->NoeudEnExamen->ProfondeurDuNoeud);
	  fflush( stdout );
		*/
  }				
}

return;
}

/*----------------------------------------------------------------------------*/

