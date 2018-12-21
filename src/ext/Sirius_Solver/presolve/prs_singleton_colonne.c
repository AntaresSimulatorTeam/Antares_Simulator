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

   FONCTION: Colonnes singleton.
	           - Si la variable est non bornee et que la contrainte est
						 non bornante et que c'est une egalite, on peut enlever la variable des donnees
						 et la remplacer par les autres variables de la contrainte.
						 Dans les autres cas on conserve la contrainte mais on peut
						 soit calculer la variable duale de la contrainte, soit calculer
						 une borne sur cette variable duale.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"
   
# define TRACES 0
																													
void PRS_SingletonsSurColonnesSubstituerLaVariable( PRESOLVE * , int , int , int * );

char PRS_FabriquerUneVariableNonBorneeAPartirDUneContrainteDoubleton( PROBLEME_PNE * , PRESOLVE * , int , char , int );

/*----------------------------------------------------------------------------*/

void PRS_SingletonsSurColonnesSubstituerLaVariable( PRESOLVE * Presolve, int Var, int Cnt, int * NbModifications )
{
double CoutVar; int il; int ilMax; int V; double * CoutLineaire; double * A; char Flag;
int * Nuvar; int * Mdeb; int * NbTerm; double CoeffPivot; PROBLEME_PNE * Pne;
int * Cdeb; int * Csui; int * NumContrainte; int iSubstitution; double B;
double * ValeurDeLaConstanteDeSubstitution; int NbVariablesSubstituees; 
int * IndiceDebutVecteurDeSubstitution; int * NbTermesVecteurDeSubstitution;
int Nb; double * CoeffDeSubstitution; int * NumeroDeVariableDeSubstitution;
int * NumeroDesVariablesSubstituees; double * CoutDesVariablesSubstituees;
int * TypeDeBornePourPresolve; double * ValeurDeXPourPresolve;
int * ContrainteDeLaSubstitution; int * CorrespondanceCntPneCntEntree;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales quand on fait des substitutions de variables. */
	/* Par contre on peut le faire s'il y a des variables entieres car de toutes façons les variables
	   duales de la solution optimale ne peuvent pas etre interpretees comme des couts marginaux. */
  return;
}

if ( Pne->NumeroDesVariablesSubstituees == NULL ) return; /* Pas de substitution possible */
/* Si la table de substitution est plein on arrete */
if ( Pne->NbVariablesSubstituees >= Pne->NombreDeVariablesTrav - 1 ) return;
if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;
NbVariablesSubstituees = Pne->NbVariablesSubstituees;
NumeroDesVariablesSubstituees = Pne->NumeroDesVariablesSubstituees;
ValeurDeLaConstanteDeSubstitution = Pne->ValeurDeLaConstanteDeSubstitution;
IndiceDebutVecteurDeSubstitution = Pne->IndiceDebutVecteurDeSubstitution;
NbTermesVecteurDeSubstitution = Pne->NbTermesVecteurDeSubstitution;
CoeffDeSubstitution = Pne->CoeffDeSubstitution;   
NumeroDeVariableDeSubstitution = Pne->NumeroDeVariableDeSubstitution;
CoutDesVariablesSubstituees = Pne->CoutDesVariablesSubstituees;
ContrainteDeLaSubstitution = Pne->ContrainteDeLaSubstitution;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;

*NbModifications = *NbModifications + 1;

CoutLineaire = Pne->LTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

/* Reperage du type de reduction */
Pne->TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_VARIABLE_NON_BORNEE;
Pne->IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbVariablesSubstituees;
Pne->NombreDOperationsDePresolve++;

/* Description de la reduction */
CoeffPivot = 1;
Flag = 0;
il = Cdeb[Var];
while ( il >= 0 ) {
  if ( NumContrainte[il] == Cnt ) {
    CoeffPivot = A[il];
		Flag = 1;
		break;
	}
  il = Csui[il];
}
if ( Flag == 0 ) {
  printf("BUG dans SingletonsSurColonnesSubstituerLaVariable pivot pas trouve \n");
	return;
}

CoutVar = CoutLineaire[Var];
B = Pne->BTrav[Cnt];

il = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
Nb = 0;
iSubstitution = Pne->IndexLibreVecteurDeSubstitution;
IndiceDebutVecteurDeSubstitution[NbVariablesSubstituees] = iSubstitution;
while ( il < ilMax ) {	
  V = Nuvar[il];
  if ( TypeDeBornePourPresolve[V] == VARIABLE_FIXE ) B -= A[il] * ValeurDeXPourPresolve[V];	
	else {
    if ( V != Var ) {
      CoutLineaire[V] -= CoutVar *  A[il] / CoeffPivot;
      /*CoutLineaire[V] -= CoutVar *  A[il] / CoeffPivot;*/ /* Gros bug de copier coller !!! */			
      CoeffDeSubstitution[iSubstitution] = -A[il] / CoeffPivot;						
      NumeroDeVariableDeSubstitution[iSubstitution] = V;
			iSubstitution++;
      Nb++;
	  }
	  else CoutLineaire[V] = 0;
	}
  il++;		
}
NbTermesVecteurDeSubstitution[NbVariablesSubstituees] = Nb;

NumeroDesVariablesSubstituees[NbVariablesSubstituees] = Var;
CoutDesVariablesSubstituees[NbVariablesSubstituees] = CoutVar;
ContrainteDeLaSubstitution[NbVariablesSubstituees] = CorrespondanceCntPneCntEntree[Cnt];
ValeurDeLaConstanteDeSubstitution[NbVariablesSubstituees] = B / CoeffPivot;
NbVariablesSubstituees++;

Pne->Z0 += CoutVar * B / CoeffPivot;

Pne->IndexLibreVecteurDeSubstitution = iSubstitution;
Pne->NbVariablesSubstituees = NbVariablesSubstituees;

/* Quand on supprime une variable, la valeur de remplacement n'a pas d'importance car elle sera calculee.
   Il n'est pas necessaire de mettre les termes de la colonne (ou de la contrainte) a 0 car elle
	 devient inactive */	 
PRS_FixerUneVariableAUneValeur( Presolve, Var, 0.0 );

/* Desactivation de la contrainte */
PRS_DesactiverContrainte( Presolve, Cnt );

# if TRACES == 1
  printf("Singleton sur colonne %d de type VARIABLE_NON_BORNEE: la variable est substituee et la contrainte %d supprimee Pivot %e\n",
	        Var,Cnt,CoeffPivot);
# endif

return;
}

/*----------------------------------------------------------------------------*/

char PRS_FabriquerUneVariableNonBorneeAPartirDUneContrainteDoubleton( PROBLEME_PNE * Pne,
                                                                      PRESOLVE * Presolve,
																																			int VariableSingleton,
																																			char TypeBrnVariableSingleton,
                                                                      int CntDeLaVariableSingleton )
{
int il; int ilMax; int Nb; int V; int * Mdeb; int * NbTerm; int * Nuvar; double * A;
char BorneInfCalculee; char BorneSupCalculee; double BorneInf; double BorneSup;
double * BorneInfPourPresolve; double * BorneSupPourPresolve; int * TypeDeBornePourPresolve;
int * ContrainteBornanteInferieurement; int * ContrainteBornanteSuperieurement;
char OnReboucle; int NbTours; int VariableRestante; int * TypeDeVariable;
double * ValeurDeXPourPresolve; char * TypeDeValeurDeBorneInf; char * TypeDeValeurDeBorneSup;
char * ConserverLaBorneInfDuPresolve; char * ConserverLaBorneSupDuPresolve;
double MargeInfaisabilite;

MargeInfaisabilite = 1.e-6;

/* On verifie que c'est une contrainte a 2 termes */
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
TypeDeVariable = Pne->TypeDeVariableTrav;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
TypeDeValeurDeBorneSup = Presolve->TypeDeValeurDeBorneSup;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;

Nb = 0;
VariableRestante = -1;
il = Mdeb[CntDeLaVariableSingleton];
ilMax = il + NbTerm[CntDeLaVariableSingleton];
while ( il < ilMax ) {
  if ( A[il] != 0.0 ) {
    V = Nuvar[il];		
    if ( TypeDeBornePourPresolve[V] != VARIABLE_FIXE ) {
      Nb++;
			if ( Nb > 2 ) break;
      if ( V != VariableSingleton ) VariableRestante = V;
	  }
  }
  il++;
}
if ( Nb != 2 || VariableRestante < 0 ) return( TypeBrnVariableSingleton );

/* On peut en faire un singleton non borne */
NbTours = 0;
OnReboucle = OUI_PNE;
while ( OnReboucle == OUI_PNE ) {
  OnReboucle = NON_PNE;
  /* On cherche a borner au mieux les variables */
	/* VariableSingleton */
  PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( Presolve, CntDeLaVariableSingleton, VariableSingleton,  
					                                             &BorneInfCalculee, &BorneInf, &BorneSupCalculee, &BorneSup );
  if ( BorneInfCalculee == OUI_PNE ) {
    if ( BorneInf > BorneSupPourPresolve[VariableSingleton] + MargeInfaisabilite ) {
		  /* Infaisabilite */
      Pne->YaUneSolution = PROBLEME_INFAISABLE;						
	    return( TypeBrnVariableSingleton );
	  }			
    if ( BorneInf > BorneInfPourPresolve[VariableSingleton] ) {	 
	    BorneInfPourPresolve[VariableSingleton] = BorneInf;
			TypeDeValeurDeBorneInf[VariableSingleton] = VALEUR_IMPLICITE;
	    ContrainteBornanteInferieurement[VariableSingleton] = CntDeLaVariableSingleton;
			OnReboucle = OUI_PNE;
	    if ( TypeDeBornePourPresolve[VariableSingleton] == VARIABLE_NON_BORNEE ) {
        TypeDeBornePourPresolve[VariableSingleton] = VARIABLE_BORNEE_INFERIEUREMENT;
		  }
		  else if ( TypeDeBornePourPresolve[VariableSingleton] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        TypeDeBornePourPresolve[VariableSingleton] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }						
	  }		
  }	
  if ( BorneSupCalculee == OUI_PNE ) {	
    if ( BorneSup < BorneInfPourPresolve[VariableSingleton] - MargeInfaisabilite ) {
		  /* Infaisabilite */
      Pne->YaUneSolution = PROBLEME_INFAISABLE;						
	    return( TypeBrnVariableSingleton );
		}	
    if ( BorneSup < BorneSupPourPresolve[VariableSingleton] ) {						
	    BorneSupPourPresolve[VariableSingleton] = BorneSup;
			TypeDeValeurDeBorneSup[VariableSingleton] = VALEUR_IMPLICITE;
	    ContrainteBornanteSuperieurement[VariableSingleton] = CntDeLaVariableSingleton;
			OnReboucle = OUI_PNE;
	    if ( TypeDeBornePourPresolve[VariableSingleton] == VARIABLE_NON_BORNEE ) {
        TypeDeBornePourPresolve[VariableSingleton] = VARIABLE_BORNEE_SUPERIEUREMENT;
		  }
		  else if ( TypeDeBornePourPresolve[VariableSingleton] == VARIABLE_BORNEE_INFERIEUREMENT ) {
       TypeDeBornePourPresolve[VariableSingleton] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }
	  }		
  }
	/* VariableRestante */
  PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( Presolve, CntDeLaVariableSingleton, VariableRestante,  
					                                             &BorneInfCalculee, &BorneInf, &BorneSupCalculee, &BorneSup );
  if ( BorneInfCalculee == OUI_PNE ) {
    if ( BorneInf > BorneSupPourPresolve[VariableRestante] + MargeInfaisabilite ) {
		  /* Infaisabilite */
      Pne->YaUneSolution = PROBLEME_INFAISABLE;									
	    return( TypeBrnVariableSingleton );		
    }	
    if ( BorneInf > BorneInfPourPresolve[VariableRestante] ) {		
	    BorneInfPourPresolve[VariableRestante] = BorneInf;
			TypeDeValeurDeBorneInf[VariableRestante] = VALEUR_IMPLICITE;
	    ContrainteBornanteInferieurement[VariableRestante] = CntDeLaVariableSingleton;
			OnReboucle = OUI_PNE;
	    if ( TypeDeBornePourPresolve[VariableRestante] == VARIABLE_NON_BORNEE ) {
        TypeDeBornePourPresolve[VariableRestante] = VARIABLE_BORNEE_INFERIEUREMENT;
		  }
		  else if ( TypeDeBornePourPresolve[VariableRestante] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        TypeDeBornePourPresolve[VariableRestante] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }						
	  }
  }
  if ( BorneSupCalculee == OUI_PNE ) {
    if ( BorneSup < BorneInfPourPresolve[VariableRestante] - MargeInfaisabilite ) {
		  /* Infaisabilite */
      Pne->YaUneSolution = PROBLEME_INFAISABLE;									
	    return( TypeBrnVariableSingleton );			
    }	
    if ( BorneSup < BorneSupPourPresolve[VariableRestante] ) {		
	    BorneSupPourPresolve[VariableRestante] = BorneSup;
			TypeDeValeurDeBorneSup[VariableRestante] = VALEUR_IMPLICITE;			
	    ContrainteBornanteSuperieurement[VariableRestante] = CntDeLaVariableSingleton;
			OnReboucle = OUI_PNE;
	    if ( TypeDeBornePourPresolve[VariableRestante] == VARIABLE_NON_BORNEE ) {
        TypeDeBornePourPresolve[VariableRestante] = VARIABLE_BORNEE_SUPERIEUREMENT;
		  }
		  else if ( TypeDeBornePourPresolve[VariableRestante] == VARIABLE_BORNEE_INFERIEUREMENT ) {
       TypeDeBornePourPresolve[VariableRestante] = VARIABLE_BORNEE_DES_DEUX_COTES;
      }						
	  }
  }
	NbTours++;
	if ( NbTours > 10 ) break;
}

if ( TypeDeVariable[VariableRestante] == ENTIER ) {
  if ( BorneInfPourPresolve[VariableRestante] > SEUIL_INF_POUR_FIXATION_BINAIRE ) {
    if ( BorneSupPourPresolve[VariableRestante] == 1.0 ) {		
      PRS_FixerUneVariableAUneValeur( Presolve, VariableRestante, 1.0 );  					
	  }
 	  else Pne->YaUneSolution = PROBLEME_INFAISABLE; 
		return( TypeBrnVariableSingleton );		
	}
  if ( BorneSupPourPresolve[VariableRestante] < SEUIL_SUP_POUR_FIXATION_BINAIRE ) {	
    if ( BorneInfPourPresolve[VariableRestante] == 0.0 ) {
      PRS_FixerUneVariableAUneValeur( Presolve, VariableRestante, 0.0 );  		 						
	  }
	  else Pne->YaUneSolution = PROBLEME_INFAISABLE; 
		return( TypeBrnVariableSingleton );		
	}
}


/* Ci-dessous c'est une connerie */

/* On n'a pas pu finir ? */
/*
if ( OnReboucle == OUI_PNE ) return( TypeBrnVariableSingleton );	
return( VARIABLE_NON_BORNEE );
*/

TypeBrnVariableSingleton = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales(
                             Pne->TypeDeBorneTrav[VariableSingleton], TypeDeBornePourPresolve[VariableSingleton],
									           BorneInfPourPresolve[VariableSingleton], Pne->UminTrav[VariableSingleton],
										         BorneSupPourPresolve[VariableSingleton], Pne->UmaxTrav[VariableSingleton],
										         ConserverLaBorneInfDuPresolve[VariableSingleton], ConserverLaBorneSupDuPresolve[VariableSingleton],
										         TypeDeValeurDeBorneInf[VariableSingleton], TypeDeValeurDeBorneSup[VariableSingleton] );

return( TypeBrnVariableSingleton );
}

/*----------------------------------------------------------------------------*/

void PRS_SingletonsSurColonnes( PRESOLVE * Presolve, int * NbModifications )
{
int il; int Cnt; int Var; double BorneInf; double BorneSup; int NbTermes; char SensCnt ; 
int CntDeLaVariable; int TypeBrnPresolve; PROBLEME_PNE * Pne; char BorneInfCalculee;
char BorneSupCalculee;  int * Cdeb; int * Csui; char * ContrainteInactive; double * A;
int * NumContrainte; char * SensContrainte; char TypeBrnNative; double * BorneInfPourPresolve;
double * BorneSupPourPresolve; int * TypeDeBornePourPresolve; int * TypeDeVariable;
int * ContrainteBornanteInferieurement; int * ContrainteBornanteSuperieurement;
int NombreDeVariables; int * TypeDeBorneNative; char BorneInfPresolveDisponible;
char BorneSupPresolveDisponible; char * ConserverLaBorneSupDuPresolve; char * ConserverLaBorneInfDuPresolve;
double BorneInfPresolve; double	BorneSupPresolve; char TypeBrn; double * BorneInfNative;
double * BorneSupNative; int NbVariablesFixees; int NbLambdaModifies; double Ai;
double * CoutLineaire; char * TypeDeValeurDeBorneInf; char * TypeDeValeurDeBorneSup;
char TypeValBorneInf; char TypeValBorneSup; char BorneAmelioree;

# if TRACES == 1
  int CntS; int Cnti; char CsvSup; char CsvInf;
# endif

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
  
*NbModifications = 0;

NombreDeVariables = Pne->NombreDeVariablesTrav;
CoutLineaire = Pne->LTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
SensContrainte = Pne->SensContrainteTrav;
A = Pne->ATrav;

TypeDeBorneNative = Pne->TypeDeBorneTrav;
BorneInfNative = Pne->UminTrav;
BorneSupNative = Pne->UmaxTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;

ContrainteInactive = Presolve->ContrainteInactive;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;

BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
TypeDeValeurDeBorneSup = Presolve->TypeDeValeurDeBorneSup;

NbVariablesFixees = 0;
NbLambdaModifies = 0;

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {

  if ( TypeDeVariable[Var] == ENTIER ) continue;
	
  TypeBrnPresolve = TypeDeBornePourPresolve[Var];
	TypeBrnNative = TypeDeBorneNative[Var];
	
  if ( TypeBrnPresolve == VARIABLE_FIXE ) continue;
  NbTermes = 0;
	SensCnt = '=';
	CntDeLaVariable = -1;
	Ai = 0.0;
  il  = Cdeb[Var];
  while ( il >= 0 ) {
    Cnt = NumContrainte[il]; 
    if ( A[il] != 0.0 ) { 
      if ( ContrainteInactive[Cnt] == NON_PNE ) {
			  Ai = A[il];
        NbTermes++; 
        SensCnt = SensContrainte[Cnt];
	      CntDeLaVariable = Cnt;
        if ( NbTermes > 1 ) break;
      }
    }
    il = Csui[il]; 
  }
	
  if ( NbTermes != 1 ) continue; /* Variable suivante */

  if ( fabs( Ai ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) continue;
	
  TypeBrn = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( TypeBrnNative, TypeBrnPresolve,
									                                                       BorneInfPourPresolve[Var], BorneInfNative[Var],
										                                                     BorneSupPourPresolve[Var], BorneSupNative[Var],
										                                                     ConserverLaBorneInfDuPresolve[Var], ConserverLaBorneSupDuPresolve[Var],
										                                                     TypeDeValeurDeBorneInf[Var], TypeDeValeurDeBorneSup[Var] );
																																					
  if ( TypeBrn != VARIABLE_NON_BORNEE && SensCnt == '=' ) {
	  /* Doubleton ? Si oui on essai d'en faire un singleton non borne */		
    TypeBrn = PRS_FabriquerUneVariableNonBorneeAPartirDUneContrainteDoubleton( Pne, Presolve, Var, TypeBrn, CntDeLaVariable );		
	}
																			
 	if ( TypeBrn == VARIABLE_NON_BORNEE ) {		
	  /* On peut fixer la variable duale de la contrainte ou enlever la variable et la contrainte */
    if ( SensCnt == '=' ) {
		  # if TRACES == 1
		    printf("1- VARIABLE_NON_BORNEE substitution  BorneInfPresolve %e BorneSupPresolve %e | BorneInfNative %e BorneSupNative %e\n",
			          BorneInfPourPresolve[Var],BorneSupPourPresolve[Var],BorneInfNative[Var],BorneSupNative[Var]);
			  CntS = Presolve->ContrainteBornanteSuperieurement[Var];
			  Cnti = Presolve->ContrainteBornanteInferieurement[Var];
			  CsvSup = ConserverLaBorneSupDuPresolve[Var];
			  CsvInf = ConserverLaBorneInfDuPresolve[Var];
			  printf("Contrainte %d CntBornanteSup %d CntBornanteInf %d CsvSup %d CsvInf %d\n",CntDeLaVariable,CntS,Cnti,CsvSup,CsvInf);
			  if ( CntS >= 0 ) printf("ContrainteInactive[%d] = %d\n",CntS,Presolve->ContrainteInactive[CntS]);
			  if ( Cnti >= 0 ) printf("ContrainteInactive[%d] = %d\n",Cnti,Presolve->ContrainteInactive[Cnti]);
      # endif			
      PRS_SingletonsSurColonnesSubstituerLaVariable( Presolve, Var, CntDeLaVariable, &NbVariablesFixees );
		}
		else {
		  # if TRACES == 1
		    printf("1- VARIABLE_NON_BORNEE calcul de la variable duale\n");
      # endif
      PRS_MajVariableDuale( Presolve, CntDeLaVariable, CoutLineaire[Var], Ai, '=', &NbLambdaModifies );		
		}
    continue;		  
	}
  else {
		  
	  /* On calcule les meilleures bornes primales sur la variable a l'aide de la contrainte puis on examine les
		   consequences sur la variable duale */
    PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( Presolve, CntDeLaVariable, Var, &BorneInfCalculee, &BorneInf, &BorneSupCalculee, &BorneSup );

		/* On regarde si on a pu ameliorer la borne */    
		PRS_MettreAJourLesBornesDUneVariable( Presolve, Var, BorneInfCalculee, BorneInf, CntDeLaVariable,
																		      BorneSupCalculee, BorneSup, CntDeLaVariable, &BorneAmelioree );

	  TypeBrnPresolve = TypeDeBornePourPresolve[Var]; /* Car le type de borne a pu changer lors de la mise a jour des bornes */		
    TypeBrn = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( TypeBrnNative, TypeBrnPresolve,
									                                                         BorneInfPourPresolve[Var], BorneInfNative[Var],
										                                                       BorneSupPourPresolve[Var], BorneSupNative[Var],
										                                                       ConserverLaBorneInfDuPresolve[Var], ConserverLaBorneSupDuPresolve[Var],
										                                                       TypeDeValeurDeBorneInf[Var], TypeDeValeurDeBorneSup[Var] );		
																																								
		/* On examine a nouveau les consequence sur les couts reduits */
 	  if ( TypeBrn == VARIABLE_NON_BORNEE ) {		
	    /* On peut fixer la variable duale de la contrainte ou enlever la variable et la contrainte */
      if ( SensCnt == '=' ) {
		    # if TRACES == 1
		      printf("2- VARIABLE_NON_BORNEE substitution\n");
        # endif
				PRS_SingletonsSurColonnesSubstituerLaVariable( Presolve, Var, CntDeLaVariable, &NbVariablesFixees );
		  }
		  else {
		    # if TRACES == 1
		      printf("2- VARIABLE_NON_BORNEE calcul de la variable duale\n");
        # endif				
        PRS_MajVariableDuale( Presolve, CntDeLaVariable, CoutLineaire[Var], Ai, '=', &NbLambdaModifies );		
		  }			
    }
 	  else if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
      /* Le singleton sur colonne est borne inferieurement: son cout reduit est >= 0 */			
		  # if TRACES == 1
			  printf("2- VARIABLE_BORNEE_INFERIEUREMENT calcul d'une borne sur la variable duale\n");
      # endif
      PRS_MajVariableDuale( Presolve, CntDeLaVariable, CoutLineaire[Var], Ai, '>', &NbLambdaModifies );		
    }
 	  else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      /* Le singleton sur colonne est borne superieurement: son cout reduit est <= 0 */
		  # if TRACES == 1
			  printf("2- VARIABLE_BORNEE_SUPERIEUREMENT calcul d'une borne sur la variable duale\n");
      # endif			
      PRS_MajVariableDuale( Presolve, CntDeLaVariable, CoutLineaire[Var], Ai, '<', &NbLambdaModifies );			
    }						
  }
}

# if VERBOSE_PRS == 1 
  printf("-> Colonne singleton: variables supprimees %d / contraintes supprimees %d / variables duales modifiees %d / total %d\n",
	           NbVariablesFixees,NbVariablesFixees,NbLambdaModifies,( 2 * NbVariablesFixees ) + NbLambdaModifies);	
# endif

*NbModifications = ( 2 * NbVariablesFixees ) + NbLambdaModifies;

return;
}
