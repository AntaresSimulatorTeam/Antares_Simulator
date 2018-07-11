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

   FONCTION: Postsolve (apres le presolve) 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_define.h"

/* Pour le chainage de la transposee de la matrice d'entree */
typedef struct{
int * Cdeb;    
int * Csui;
int * NumContrainte;
} TRANSPOSEE;

void PNE_PostSolveChainageTransposee( PROBLEME_PNE * , PROBLEME_A_RESOUDRE * , TRANSPOSEE * );
double PNE_PostSolveProduitScalaireVarDualeParColonneOptimisee( PROBLEME_PNE * , int );
double PNE_PostSolveProduitScalaireVarDualeParColonne( PROBLEME_A_RESOUDRE * , TRANSPOSEE * , int , int );
																			
/*----------------------------------------------------------------------------*/

void PNE_PostSolve( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme ) 
{
int i; int j; TRANSPOSEE * TransposeeEntree; double * VariablesDualesDesContraintesE;
char * TypeDOperationDePresolve;int * IndexDansLeTypeDOperationDePresolve;

VariablesDualesDesContraintesE = Probleme->VariablesDualesDesContraintes;

TransposeeEntree = (TRANSPOSEE *) malloc( sizeof( TRANSPOSEE ) );
if ( TransposeeEntree == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_PostSolve\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee );
}
PNE_PostSolveChainageTransposee( Pne, Probleme, TransposeeEntree );
																			
TypeDOperationDePresolve = Pne->TypeDOperationDePresolve;
IndexDansLeTypeDOperationDePresolve = Pne->IndexDansLeTypeDOperationDePresolve;

for ( j = Pne->NombreDOperationsDePresolve - 1 ; j >= 0 ; j-- ) {
  i = IndexDansLeTypeDOperationDePresolve[j];	
  if ( TypeDOperationDePresolve[j] == SUPPRESSION_VARIABLE_NON_BORNEE ) {		
    PNE_PostSolveVariablesSubstituees( Pne, Probleme, i, TypeDOperationDePresolve[j] );				
	}
  else if ( TypeDOperationDePresolve[j] == SUBSITUTION_DE_VARIABLE ) {
    PNE_PostSolveVariablesSubstituees( Pne, Probleme, i, TypeDOperationDePresolve[j] );		
	}	
	else if ( TypeDOperationDePresolve[j] == SUPPRESSION_COLONNE_COLINEAIRE ) {
    PNE_PostSolveVariablesColineaires( Pne, i );	
	}
	else if ( TypeDOperationDePresolve[j] == SUPPRESSION_LIGNE_SINGLETON ) {
    PNE_PostSolveContraintesSingleton( Pne, i, VariablesDualesDesContraintesE );	
	}
	else if ( TypeDOperationDePresolve[j] == SUPPRESSION_FORCING_CONSTRAINT ) {
    PNE_PostSolveForcingConstraints( Pne, Probleme, (void *) TransposeeEntree, i );		
	}
	else if ( TypeDOperationDePresolve[j] == SUPPRESSION_CONTRAINTE_COLINEAIRE ) {
    PNE_PostSolveContraintesColineaires( Pne, Probleme, i );		
	}	
}

free( TransposeeEntree->Cdeb );
free( TransposeeEntree->Csui );
free( TransposeeEntree->NumContrainte );
free( TransposeeEntree );

return;    
}

/*----------------------------------------------------------------------------*/
/* Recuperation de la variable duale des contraintes singleton supprimees     */

void PNE_PostSolveContraintesSingleton( PROBLEME_PNE * Pne, int i, double * VariablesDualesDesContraintesE )
{
int Var; int * Cdeb; int * Csui; int * NumContrainte; double * A; double * u;
double CBarre; int ic; int CntE; double X0; double X;

/*printf("PNE_PostSolveContraintesSingleton\n");*/

if ( VariablesDualesDesContraintesE == NULL ) return;

CntE = Pne->NumeroDeLaContrainteSingleton[i];  
Var = Pne->VariableDeLaContrainteSingleton[i];
X0 = Pne->SecondMembreDeLaContrainteSingleton[i];
X = Pne->UTrav[Var];
/* Calcul du cout reduit de la variable */
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
u = Pne->VariablesDualesDesContraintesTrav;
CBarre = Pne->LTrav[Var];
ic = Cdeb[Var];
while ( ic >= 0 ) {
  CBarre -= u[NumContrainte[ic]] * A[ic];
  ic = Csui[ic];
}

/* Si la variable est sur la valeur du second membre au moment ou on a supprime la contrainte,
   ou si elle est fixe, alors la variable duale de la ligne singleton est egale au cout reduit de la variable */
/* Dans les autres cas, la variable duale de la contrainte est nulle */
if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
  /*VariablesDualesDesContraintesE[CntE] = CBarre;*/
  /* Dans ce cas on ne dispose pas du cout reduit car la variable a quitte le probleme resolu par le simplxe => on change le signe du cout */
  VariablesDualesDesContraintesE[CntE] = -Pne->LTrav[Var];
}
else if ( fabs( X - X0 ) < 1.e-7 ) {
  VariablesDualesDesContraintesE[CntE] = CBarre;
}
else {
  VariablesDualesDesContraintesE[CntE] = 0.0;
}

return;
}

/*----------------------------------------------------------------------------*/
/* Recuperation de la variable duale des contraintes colineaires              */

void PNE_PostSolveContraintesColineaires( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme, int i )
{
int ContrainteConservee; int ContrainteInhibee; double * u; int CntPneConservee;
int * CorrespondanceCntPneCntEntree; double uPne; int Cnt;

ContrainteConservee = Pne->ContrainteConservee[i]; 	
ContrainteInhibee = Pne->ContrainteSupprimee[i]; 	
/* A ce stade la, normalement u[ContrainteConservee] a ete initialise */
u = Probleme->VariablesDualesDesContraintes;
if ( u == NULL ) return;
u[ContrainteInhibee] = 0.0;

/* Recherche du numero de la contrainte conservee dans le probleme reduit */
CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;
CntPneConservee = -1;
for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  if ( CorrespondanceCntPneCntEntree[Cnt] == ContrainteConservee ) {
	  CntPneConservee = Cnt;
		break;
	}
}
if ( CntPneConservee == -1 ) return;

/* Le seul cas ou on peut etre amene a modifier u[ContrainteConservee] est celui ou
   son sens en ete transforme en = alors qu'avant c'etait < ou > */
if ( Pne->SensContrainteTrav[CntPneConservee] != '=' ) return;
if ( Probleme->Sens[ContrainteConservee] == '=' ) return;
uPne = Pne->VariablesDualesDesContraintesTrav[CntPneConservee];

if ( uPne < 0.0 ) {
  if ( Probleme->Sens[ContrainteConservee] == '>' ) {
    u[ContrainteInhibee] = u[ContrainteConservee];
    u[ContrainteConservee] = 0.0;
	}
}
else if ( uPne > 0.0 ) {
  if ( Probleme->Sens[ContrainteConservee] == '<' ) {
    u[ContrainteInhibee] = u[ContrainteConservee];
    u[ContrainteConservee] = 0.0;
  }
}

return;
}

/*----------------------------------------------------------------------------*/
/* Recuperation de la variable duale des forcing constraints                  */

void PNE_PostSolveForcingConstraints( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme,
                                      void * Transposee, int i )
{
int Cnt; int * Mdeb; int * NbTerm; int * Nuvar; double * A; double * u;
double * CoutLineaire; double CBarre; int il; double ai; double CB;
double uCntMin; double uCntMax; double ZeroCBarre; int ilMax;
TRANSPOSEE * TransposeeEntree; int Var; int NombreDeVariables;

/*printf("PNE_PostSolveForcingConstraints\n");*/

TransposeeEntree = (TRANSPOSEE *) Transposee;
/* Cnt est dans la numerotation d'entree */
Cnt = Pne->NumeroDeLaForcingConstraint[i];

uCntMin = -LINFINI_PNE;
uCntMax = LINFINI_PNE;      
ZeroCBarre = 1.e-8;
NombreDeVariables = Pne->NombreDeVariablesTrav;

CoutLineaire = Pne->LTrav;
Mdeb = Probleme->IndicesDebutDeLigne;	                 
NbTerm = Probleme->NombreDeTermesDesLignes;	        
Nuvar = Probleme->IndicesColonnes;
A = Probleme->CoefficientsDeLaMatriceDesContraintes;
u = Probleme->VariablesDualesDesContraintes;

if ( u == NULL ) return;

il = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
while ( il < ilMax ) {
  Var = Nuvar[il];
	if ( Var < 0 || Var >= NombreDeVariables ) goto NextIl;
	ai = A[il];
	/* Calcul du cout reduit dans le systeme des contraintes utilise pendant
		 l'optimisation */
  CBarre = CoutLineaire[Var];																															 
  CBarre -= PNE_PostSolveProduitScalaireVarDualeParColonneOptimisee( Pne, Var );
	/* Calcul du cout reduit dans les contraintes d'origine sauf Cnt */
  CB = CoutLineaire[Var];																															 
  CB -= PNE_PostSolveProduitScalaireVarDualeParColonne( Probleme, TransposeeEntree, Var, Cnt );	
	/* Il faut conserver le signe */
	if ( CBarre > ZeroCBarre ) {
    if ( ai > 0 ) {
		  if ( CB / ai < uCntMax ) uCntMax = CB / ai;
			if ( uCntMax < uCntMin ) uCntMax = uCntMin; /* Mais probleme quand-meme */
		}
		else if ( ai < 0 ) {
		  if ( CB / ai > uCntMin ) uCntMin = CB / ai;
			if ( uCntMin > uCntMax ) uCntMin = uCntMax; /* Mais probleme quand-meme */
		}
	}
	else if ( CBarre < -ZeroCBarre ) {
    if ( ai > 0 ) {
      if ( CB / ai > uCntMin ) uCntMin = CB / ai;
			if ( uCntMin > uCntMax ) uCntMin = uCntMax; /* Mais probleme quand-meme */			
		}
		else if ( ai < 0 ) {
      if ( CB / ai < uCntMax ) uCntMax = CB / ai;
			if ( uCntMax < uCntMin ) uCntMax = uCntMin; /* Mais probleme quand-meme */
		}
	}
	else {
	  /* CBarre = 0 */
    uCntMin = CB / ai;
		uCntMax = uCntMin;
	}
	NextIl:
	il++;
}
/* Une valeur comprise entre uCntMin et uCntMax convient */
if ( uCntMin > -LINFINI_PNE && uCntMax < LINFINI_PNE ) u[Cnt] = 0.5 * ( uCntMin + uCntMax );
else if ( uCntMin > -LINFINI_PNE ) u[Cnt] = uCntMin;
else if ( uCntMax < LINFINI_PNE ) u[Cnt] = uCntMax;
else u[Cnt] = 0;

return;
}

/*----------------------------------------------------------------------------*/
/* Recuperation des substitutions de variables        */
/* et de la variable duale des contraintes supprimees */

void PNE_PostSolveVariablesSubstituees( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme,
                                        int i, char TypeDOperation )
{
int il; int ilMax; int Var; double S; double * ValeurDeX; double * CoeffDeSubstitution; 
int * NumeroDeVariableDeSubstitution; int Cnt; double * u; double CBarre;
double uN; double ai; int * Nuvar; 

/*printf("PNE_PostSolveVariablesSubstituees \n");*/

ValeurDeX = Pne->UTrav; 
CoeffDeSubstitution = Pne->CoeffDeSubstitution;
NumeroDeVariableDeSubstitution = Pne->NumeroDeVariableDeSubstitution;

S = Pne->ValeurDeLaConstanteDeSubstitution[i];
il = Pne->IndiceDebutVecteurDeSubstitution[i];
ilMax = il + Pne->NbTermesVecteurDeSubstitution[i];
while ( il < ilMax ) {
  Var = NumeroDeVariableDeSubstitution[il];
  S += CoeffDeSubstitution[il] * ValeurDeX[Var];
  /*printf("X de valeur qui est reste %e \n",ValeurDeX[Var]);*/
	il++;
}
Var = Pne->NumeroDesVariablesSubstituees[i];
ValeurDeX[Var] = S;

/*printf("X valeur subsituee %e   \n",ValeurDeX[Var]);*/
Pne->LTrav[Var] = Pne->CoutDesVariablesSubstituees[i];

/* Pour les singletons non bornes sur colonne, leur cout reduit doit etre nul donc u = c / a */
u =  Probleme->VariablesDualesDesContraintes;
if ( u == NULL ) return;
Cnt = Pne->ContrainteDeLaSubstitution[i];
if ( Cnt >= 0 ) u[Cnt] = 0.0;
else return;

CBarre = Pne->LTrav[Var];
if ( TypeDOperation == SUBSITUTION_DE_VARIABLE ) {
  /* Calcul du cout reduit de la variable dans le constexte du probleme reduit */
  uN = PNE_PostSolveProduitScalaireVarDualeParColonneOptimisee( Pne, Var );
  CBarre -= uN;
}
/* Recherche du coefficient de la variable */
ai = 1.;
Nuvar = Probleme->IndicesColonnes;
il = Probleme->IndicesDebutDeLigne[Cnt];
ilMax = il + Probleme->NombreDeTermesDesLignes[Cnt];
while ( il < ilMax ) {
  if ( Nuvar[il] == Var ) {
	  ai = Probleme->CoefficientsDeLaMatriceDesContraintes[il];
		break;
	}
  il++;
}
if ( ai == 0.0 ) return;

/* Remarque: en ajoutant la contrainte de substitution
- si la variable est entre ses 2 bornes le cout reduit doit etre nul
- si la variable est sur borne inf le cout reduit doit etre positif ou nul
- si la variable est sur borne sup le cout reduit doit etre negatif ou nul
donc pour satisfaire ces 3 contrainte une valeur de coute reduit nul convient.
Ce calcul reste approximatif avant d'avoir trouve une methode plus scientifique */
u[Cnt] = CBarre / ai;

return;
}

/*----------------------------------------------------------------------------*/
/*        Recuperation des variables colineaires        */

void PNE_PostSolveVariablesColineaires( PROBLEME_PNE * Pne, int i )
{
int Var1; int Var2; int Var; double Nu; double XVar; double ZeroCBarre; double CBarre1;
double XminVar; double XmaxVar; double XminVar1; double XmaxVar1; double XminVar2;
double XmaxVar2; double XVar1; double XVar2; double CBarre2; int It; double Rho;
double uN;

/*printf("PNE_PostSolveVariablesColineaires \n");*/

ZeroCBarre = 1.e-7;

/* Par convention, la premiere variable est toujours celle qui remplace les 2
   variable */
Var1 = Pne->PremiereVariable[i];
Var = Var1; /* Var est la variable representante */
Var2 = Pne->DeuxiemeVariable[i];
Nu = Pne->ValeurDeNu[i];

XminVar1 = Pne->XminPremiereVariable[i];
XmaxVar1 = Pne->XmaxPremiereVariable[i];

XminVar2 = Pne->XminDeuxiemeVariable[i];
XmaxVar2 = Pne->XmaxDeuxiemeVariable[i];

if ( Nu > 0 ) {
  XminVar = XminVar1 + ( Nu * XminVar2 );	
  XmaxVar = XmaxVar1 + ( Nu * XmaxVar2 ); 
}
else {
  XminVar = XminVar1 + ( Nu * XmaxVar2 );	
  XmaxVar = XmaxVar1 + ( Nu * XminVar2 );		
}

XVar = Pne->UTrav[Var];

/* Calcul du cout reduit de la variable representante */

if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) goto CoutReduitNul;

goto CoutReduitNul;

uN = PNE_PostSolveProduitScalaireVarDualeParColonneOptimisee( Pne, Var );

CBarre1 = Pne->LTrav[Var1] - uN;
CBarre2 = Pne->LTrav[Var2] - (uN * Nu);

if ( CBarre1 > ZeroCBarre ) {
  /* La variable Var1 doit se trouver sur borne inf */
	if ( XminVar1 > - ( 0.99 * LINFINI_PNE ) ) {
	  XVar1 = XminVar1;
		XVar2 = ( XVar - XVar1 ) / Nu;
		
		if ( XVar2 > XmaxVar2 ) {
		  printf("1- XVar2 %e XmaxVar2 %e CBarre2 %e\n",XVar2,XmaxVar2,CBarre2);
		}
		if ( XVar2 < XminVar2 ) {
		  printf("1- XVar2 %e XminVar2 %e CBarre2 %e\n",XVar2,XminVar2,CBarre2);
		}
		
	}
	else goto CoutReduitNul;
}
else if ( CBarre1 < -ZeroCBarre ) {
  /* La variable Var1 doit se trouver sur borne sup */
	if ( XminVar1 < 0.99 * LINFINI_PNE ) {
	  XVar1 = XmaxVar1;
		XVar2 = ( XVar - XVar1 ) / Nu;

		
		if ( XVar2 > XmaxVar2 ) {
		  printf("2- XVar2 %e XmaxVar2 %e CBarre2 %e\n",XVar2,XmaxVar2,CBarre2);
		}
		if ( XVar2 < XminVar2 ) {
		  printf("2- XVar2 %e XminVar2 %e CBarre2 %e\n",XVar2,XminVar2,CBarre2);
		}
		
	}
	else goto CoutReduitNul;
}
else {
  /* Cout reduit nul, il suffit que les variables satisfassent la combinaire lineaire */
	CoutReduitNul:
	XVar1 = XVar;
  It = 0;
	while ( It < 100 ) {
	  It++;
	  if ( XVar1 < XminVar1 - 1.e-9 ) XVar1 = XminVar1;
	  else if ( XVar1 > XmaxVar1 + 1.e-9 ) XVar1 = XmaxVar1;		
		XVar2 = ( XVar - XVar1 ) / Nu;		
	  if ( XVar2 > XmaxVar2 + 1.e-9 ) {
	    Rho = ( XVar2 - XmaxVar2 ) / It;
      if ( Nu < 0 ) Rho *= -1.;
			XVar1 += Rho;
	  }
	  else if ( XVar2 < XminVar2 - 1.e-9 ) {
	    Rho = ( XminVar2 - XVar2 ) / It;
      if ( Nu > 0 ) Rho *= -1.;
			XVar1 += Rho;
	  }
		else goto Fin;
	}
	
  printf("Impossible de trouver une valeur pour un couple de variables:\n");
  printf("  XVar %e  Nu %e\n",XVar,Nu);
  printf("  XminVar1 %e  XmaxVar1 %e\n",XminVar1,XmaxVar1);
  printf("  XminVar2 %e  XmaxVar2 %e\n",XminVar2,XmaxVar2);
  printf("  XVar1 %e XVar2 %e\n",XVar1,XVar2);
  printf("  XVar1 + Nu * XVar2 %e\n",XVar1+(Nu*XVar2));
	
}

Fin:

Pne->UTrav[Var1] = XVar1;
Pne->UTrav[Var2] = XVar2;

/*printf("XVar %e XVar1 %e XVar2 %e    XVar1 + Nu XVar2 %e\n",XVar,XVar1,XVar2,XVar1 + (Nu * XVar2) );*/

return;
}

/*----------------------------------------------------------------------------*/

double PNE_PostSolveProduitScalaireVarDualeParColonneOptimisee( PROBLEME_PNE * Pne, 
																											          int Var )
{
double * A; int * Cdeb; int * Csui; int * NumContrainte; double uN; int ic;
double * u; int Cnt;

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
u = Pne->VariablesDualesDesContraintesTrav;
uN = 0.0;

ic = Cdeb[Var];
while ( ic >= 0 ) {
  Cnt = NumContrainte[ic];
	uN += u[Cnt] * A[ic];
  ic = Csui[ic];
}

return( uN );
}


/*----------------------------------------------------------------------------*/

double PNE_PostSolveProduitScalaireVarDualeParColonne( PROBLEME_A_RESOUDRE * Probleme,
                                                       TRANSPOSEE * TransposeeEntree,
																											 int Var, int CntExclue )
{
double * A; int * Cdeb; int * Csui; int * NumContrainte; double uN; int ic;
double * u; int Cnt;

A = Probleme->CoefficientsDeLaMatriceDesContraintes;
u =  Probleme->VariablesDualesDesContraintes;
Cdeb = TransposeeEntree->Cdeb;
Csui = TransposeeEntree->Csui;
NumContrainte = TransposeeEntree->NumContrainte;

uN = 0.0;
ic = Cdeb[Var];
while ( ic >= 0 ) {
  Cnt = NumContrainte[ic];
	if ( Cnt != CntExclue ) {
	  if ( u[Cnt] > VALEUR_NON_INITIALISEE ) {
	    /*   
	    printf("PNE_PostSolveProduitScalaireVarDualeParColonne: attention utilisation d'une variable duale de la contrainte %d non initialisee\n",Cnt);
	    printf("pour le calcul du produit scalaire du vecteur des variables duales car la colonne de la variable %d\n",Var);
	    printf("-> on force la variable duale a 0 pour faire le calcul\n");
		  */
	  }
		else uN += u[Cnt] * A[ic];
	}
  ic = Csui[ic];
}

return( uN );
}
																			
/*----------------------------------------------------------------------------*/
/*       Construction du chainage de la transposee de la matrice d'entree
         afin de pouvoir calculer des produits scalaires u N                  */

void PNE_PostSolveChainageTransposee( PROBLEME_PNE * Pne,
                                      PROBLEME_A_RESOUDRE * Probleme,
                                      TRANSPOSEE * TransposeeEntree )
{  
int Var; int Cnt; int il; int ilMax; int ilk; int * Cder; int * Cdeb; int * Mdeb;
int * NbTerm; int * Nuvar; int * NumContrainte; int * Csui; int Nz;
int NombreDeVariables; int NombreDeContraintes;

NombreDeVariables = Probleme->NombreDeVariables;    
NombreDeContraintes = Probleme->NombreDeContraintes;
Mdeb = Probleme->IndicesDebutDeLigne;	                 
NbTerm = Probleme->NombreDeTermesDesLignes;	        
Nuvar = Probleme->IndicesColonnes;

/* Calcul du nombre de termes non nuls dans la matrice */
Nz = -1;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( Mdeb[Cnt] + NbTerm[Cnt] > Nz ) Nz = Mdeb[Cnt] + NbTerm[Cnt];
}

Cdeb = (int *) malloc( NombreDeVariables * sizeof( int ) );
Cder = (int *) malloc( NombreDeVariables * sizeof( int ) );
Csui = (int *) malloc( Nz * sizeof( int ) );
NumContrainte = (int *) malloc( Nz  * sizeof( int ) );
if ( Cdeb == NULL || Cder == NULL || Csui == NULL || NumContrainte == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_PostSolveChainageTransposee \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee );
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) Cdeb[Var] = -1;

/* Chainage de la transposee */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {	
    Var = Nuvar[il];		
    if ( Cdeb[Var] < 0 ) {
      Cdeb[Var] = il;
      NumContrainte[il] = Cnt;
      Csui[il] = -1;
      Cder[Var] = il;
    }
    else {
      ilk = Cder[Var];
      Csui[ilk]= il;
      NumContrainte[il] = Cnt;
      Csui[il] = -1;
      Cder[Var] = il;
    }
    il++;
  }
}

TransposeeEntree->Cdeb = Cdeb;
TransposeeEntree->Csui = Csui;
TransposeeEntree->NumContrainte = NumContrainte;

free( Cder );

return;
}
