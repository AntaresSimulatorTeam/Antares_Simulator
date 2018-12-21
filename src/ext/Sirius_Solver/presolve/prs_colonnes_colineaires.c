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

   FONCTION: Recherche des colonnes colineaires.					 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

# define ZERO_COLINEAIRE  1.e-12
# define SEUIL_POUR_DELTAC_NON_NUL 1.e-7 /* Pour affirmer que DeltaC est non nul */
# define SEUIL_POUR_DELTAC_NUL 1.e-9 /* Pour affirmer que DeltaC est nul */
# define MARGE_EGALITE_BORNE_MIN_ET_MAX  1.e-8
# define MAX_NBTMX 1000000000 /*10000*/

# define COLONNE_A_ETUDIER 0
# define COLONNE_A_EVITER  1

void PRS_ComparerVariables( PRESOLVE * , int , int , double * , char * , int , char * , int * , int * );

void PRS_ColonneColineairesDeltaCNonNul( PROBLEME_PNE * , PRESOLVE * , int , int , double , double , char * , int * , int * );

void PRS_ColonneColineairesDeltaCNul( PROBLEME_PNE * , PRESOLVE * , int , int , double , char * , int * , int * );
																			
/*----------------------------------------------------------------------------*/

void PRS_ColonnesColineaires( PRESOLVE * Presolve, int * NbColonnesSupprimees )     
{
int NombreDeVariables; int NombreDeContraintes; int * Mdeb; int * NbTerm;
int * Nuvar; double * A; int Cnt; int il1; char InitV; int Var1; int ic;
int * Cdeb; int * Csui; int * NumContrainte; int ilMax; char * SensContrainte;
double * B; int HashVar; int NbVarDeCnt; int il; char * Buffer; int * TypeDeVariable;
int * TypeDeBornePourPresolve; int NbT; int Var; int * NumVarDeCnt; char * Flag;
double * V; int LallocTas; char * ContrainteInactive; char * T; char * pt;
int * HashCode; int CntDeVar; int * ParLignePremiereContrainte; int * ParLigneContrainteSuivante;
int * TypeDeBorneNative; double * BorneInfPourPresolve;
double * BorneSupPourPresolve; double * BorneInfNative; double * BorneSupNative;
char * ConserverLaBorneInfDuPresolve; char * ConserverLaBorneSupDuPresolve;
int * VariableEquivalente; int NbComparaisons; int * NbTermesUtilesDeVar;
int NbTermesUtiles; int icPrec; int NbVarDispo; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*NbColonnesSupprimees = 0;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
  return;
}

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorneNative = Pne->TypeDeBorneTrav;
BorneInfNative = Pne->UminTrav;
BorneSupNative = Pne->UmaxTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

ParLignePremiereContrainte = Presolve->ParLignePremiereContrainte;
ParLigneContrainteSuivante = Presolve->ParLigneContrainteSuivante;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;
ContrainteInactive = Presolve->ContrainteInactive;
VariableEquivalente = Presolve->VariableEquivalente;

LallocTas = 0;
LallocTas += NombreDeContraintes * sizeof( double ); /* V */
LallocTas += NombreDeContraintes * sizeof( char ); /* T */
LallocTas += NombreDeVariables * sizeof( char ); /* Flag */
LallocTas += NombreDeVariables * sizeof( int ); /* HashCode */
LallocTas += NombreDeVariables * sizeof( int ); /* NumVarDeCnt */
LallocTas += NombreDeVariables * sizeof( int ); /* NbTermesUtilesDeVar */

Buffer = (char *) malloc( LallocTas );
if ( Buffer == NULL ) {
  printf(" Solveur PNE , memoire insuffisante dans le presolve. Sous-programme: PRS_ColonnesColineaires \n");
	return;
}

pt = Buffer;
V = (double *) pt;
pt += NombreDeContraintes * sizeof( double );
T = (char *) pt;
pt +=  NombreDeContraintes * sizeof( char );
Flag = (char *) pt;
pt += NombreDeVariables * sizeof( char );
HashCode = (int *) pt;
pt += NombreDeVariables * sizeof( int );
NumVarDeCnt = (int *) pt;
pt += NombreDeVariables * sizeof( int ); 
NbTermesUtilesDeVar = (int *) pt;
pt += NombreDeVariables * sizeof( int );

memset( (char *) T, 0, NombreDeContraintes * sizeof( char ) );

NbVarDispo = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  /* On ne prend pas en compte ni les variables entieres ni les variables fixes */
  if ( TypeDeVariable[Var] == ENTIER || TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) {
	  Flag[Var] = COLONNE_A_EVITER;
		continue;
	}
	/* Il faut eviter d'utiliser les variables equivalentes car leurs bornes sont des resultats
	   d'autres calculs et ca peut entrainer des problemes de precision numerique */
  if ( VariableEquivalente[Var] > 5 ) {
	  Flag[Var] = COLONNE_A_EVITER;
		continue;
	}
	
	Flag[Var] = COLONNE_A_ETUDIER;
	NbVarDispo++;
	
	/* Calcul d'un hashcode */
	/* On en profite pour dechainer les lignes qui correspondent a des contraintes inactives */
  NbT = 0;
	il = 0;
	ic = Cdeb[Var]; 
	icPrec = -1;
  while ( ic >= 0 ) {
	  Cnt = NumContrainte[ic];
    if ( ContrainteInactive[Cnt] != OUI_PNE && A[ic] != 0.0 ) {
		  NbT++;
		  il += Cnt;
		}
		else {
	    /* On en profite pour dechainer les lignes qui correspondent a des contraintes inactives */						
      ic = Csui[ic];			
			if ( icPrec >= 0 ) {
			  Csui[icPrec] = ic;				
			  continue;
			}
			else {
			  Cdeb[Var] = ic;
				if ( ic < 0 ) break; /* Attention s'il ne reste plus rien dans la ligne il faut sortir */
			}
		}
		
		icPrec = ic;
		ic = Csui[ic];
  }
	HashCode[Var] = ( il + NbT ) % NombreDeContraintes;
	NbTermesUtilesDeVar[Var] = NbT;
}

/* On balaye les lignes dans l'ordre croissant du nombre de termes et on ne compare que les
   les colonnes qui ont un terme dans cette ligne */

for ( NbT = 2 ; NbT <= Presolve->NbMaxTermesDesLignes ; NbT++ ) {
  if ( NbVarDispo <= 0 ) goto FinComparaisons;	
  Cnt = ParLignePremiereContrainte[NbT];
	while ( Cnt >= 0 ) {
    if ( NbVarDispo <= 0 ) goto FinComparaisons;
	  /*********************************************/
	  if ( ContrainteInactive[Cnt] != NON_PNE ) goto NextCnt;
	  /* On prepare la table des colonnes a explorer */
		il = Mdeb[Cnt];
		ilMax = il + NbTerm[Cnt];
		NbVarDeCnt = 0;
		while ( il < ilMax ) {
		  if ( A[il] != 0 ) {
        if ( Flag[Nuvar[il]] == COLONNE_A_ETUDIER ) {
				  NumVarDeCnt[NbVarDeCnt] = Nuvar[il];
					NbVarDeCnt++;
				}
		  }
	    il++;
	  }
		NbComparaisons = 0;
	  for ( il = 0 ; il < NbVarDeCnt ; il++ ) {
      if ( NbVarDispo <= 0 ) goto FinComparaisons;
      Var = NumVarDeCnt[il];
		  if ( Flag[Var] == COLONNE_A_EVITER ) continue;
			NbTermesUtiles = NbTermesUtilesDeVar[Var];			
		  HashVar = HashCode[Var];
		  InitV = NON_PNE;
			NbComparaisons++;
		  if ( NbComparaisons > MAX_NBTMX ) break;		
			
      /* On compare a Var toutes les variables suivantes de la contrainte */
	    for ( il1 = il + 1 ; il1 < NbVarDeCnt ; il1++ ) {
        if ( NbVarDispo <= 0 ) goto FinComparaisons;
        Var1 = NumVarDeCnt[il1];								
		    if ( Flag[Var1] == COLONNE_A_EVITER ) continue;
			  if ( HashCode[Var1] != HashVar ) continue;
        if ( NbTermesUtilesDeVar[Var1] != NbTermesUtiles ) continue;				
			  /* Comparaison de Var a Var1 et suppression eventuelle de Var1 */
			  if ( InitV == NON_PNE ) {
		      /* Preparation des tables pour la variable Var */
			    NbTermesUtiles = 0;
	        ic = Cdeb[Var];
	        while ( ic >= 0 ) {
					  CntDeVar = NumContrainte[ic];
		        V[CntDeVar] = A[ic];
		        T[CntDeVar] = 1;
						NbTermesUtiles++;					    				      
		        ic = Csui[ic];
	        }
          InitV = OUI_PNE;			
			  }

        PRS_ComparerVariables( Presolve, Var, NbTermesUtiles, V, T, Var1, Flag, NbColonnesSupprimees, &NbVarDispo );

				/* Si le Flag de Var a change on passe a la variable suivante */
				if ( Flag[Var] == COLONNE_A_EVITER ) goto NextColonne;
	
	    }
      /* RAZ de V et T avant de passer a la variable suivante */
		  Flag[Var] = COLONNE_A_EVITER;
			NbVarDispo--;
			NextColonne:
		  if ( InitV == OUI_PNE ) {
	      ic = Cdeb[Var];
 	      while ( ic >= 0 ) {
		      V[NumContrainte[ic]] = 0.0;
		      T[NumContrainte[ic]] = 0;
		      ic = Csui[ic];
	      }
		  }
  	}
	  /*********************************************/
    NextCnt:
		Cnt = ParLigneContrainteSuivante[Cnt];
	}
}

FinComparaisons:

free( Buffer );

# if VERBOSE_PRS == 1 
  printf("-> Nombre de colonnes supprimees par colinearite %d\n",*NbColonnesSupprimees);
	fflush(stdout);
# endif

return;
}

/*----------------------------------------------------------------------------*/

void PRS_ComparerVariables( PRESOLVE * Presolve, int Var, int NbTermesUtilesDeVar,
                            double * V, char * T, int Var1, char * Flag,
														int * NbColonnesSupprimees, int * NbVarDispo )
{
int Nb; int ic1; int * Cdeb; int * Csui; double Nu; int Cnt1; int * NumContrainte;
double * A; double DeltaC; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;

Nu = 1.; /* Juste pour eviter les warning de compilation */
Nb = NbTermesUtilesDeVar;
/* Determination du rapport */
ic1 = Cdeb[Var1];
Cnt1 = NumContrainte[ic1];
if ( T[Cnt1] == 0 ) return; /* Pas de terme correspondant dans la ligne */		
Nu = A[ic1] / V[Cnt1];
Nb--;
ic1 = Csui[ic1];

/* On poursuit l'analyse de la variable Var1 */		
while ( ic1 >= 0 ) {
  Cnt1 = NumContrainte[ic1];
  if ( T[Cnt1] == 0 ) return; /* Pas de terme correspondant dans la ligne */		
  if ( fabs( A[ic1] - (Nu * V[Cnt1] ) ) > ZERO_COLINEAIRE ) return;
	Nb--;			
  ic1 = Csui[ic1];
}

if ( Nb == 0 ) {
  DeltaC = Pne->LTrav[Var1] - ( Nu * Pne->LTrav[Var] );
	if ( fabs( DeltaC ) > SEUIL_POUR_DELTAC_NON_NUL ) {
    PRS_ColonneColineairesDeltaCNonNul( Pne, Presolve, Var, Var1, Nu, DeltaC, Flag, NbColonnesSupprimees, NbVarDispo );
		if ( Flag[Var1] == COLONNE_A_ETUDIER ) {
      /* On essaie a nouveau mais en inversant le role de Var et de Var1 */
			Nu = 1. / Nu;
      DeltaC = Pne->LTrav[Var] - ( Nu * Pne->LTrav[Var1] );
      PRS_ColonneColineairesDeltaCNonNul( Pne, Presolve, Var1, Var, Nu, DeltaC, Flag, NbColonnesSupprimees, NbVarDispo );						
		}
	}
	else if ( fabs( DeltaC ) < SEUIL_POUR_DELTAC_NUL ) {		
    PRS_ColonneColineairesDeltaCNul( Pne, Presolve, Var, Var1, Nu, Flag, NbColonnesSupprimees, NbVarDispo );	
	}
}

return;
}

/*----------------------------------------------------------------------------*/
/* Cas ou la combinaison lineaire des couts n'est pas nulle */
void PRS_ColonneColineairesDeltaCNonNul( PROBLEME_PNE * Pne, PRESOLVE * Presolve,
                                         int Var, int Var1, double Nu, double DeltaC,
																				 char * Flag, int * NbColonnesSupprimees, int * NbVarDispo )
{
char TypeBnrVar1; char TypeBrnVar;

TypeBrnVar = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( 
                   Pne->TypeDeBorneTrav[Var], Presolve->TypeDeBornePourPresolve[Var],
                   Presolve->BorneInfPourPresolve[Var], Pne->UminTrav[Var],									 
                   Presolve->BorneSupPourPresolve[Var], Pne->UmaxTrav[Var],										 
									 Presolve->ConserverLaBorneInfDuPresolve[Var], Presolve->ConserverLaBorneSupDuPresolve[Var],
									 Presolve->TypeDeValeurDeBorneInf[Var], Presolve->TypeDeValeurDeBorneSup[Var] );		


/*
TypeBrnVar = (char) Presolve->TypeDeBornePourPresolve[Var];
Presolve->ConserverLaBorneSupDuPresolve[Var] = OUI_PNE;
Presolve->ConserverLaBorneInfDuPresolve[Var] = OUI_PNE;
*/

TypeBnrVar1 = (char) Presolve->TypeDeBornePourPresolve[Var1];

if ( TypeBrnVar == VARIABLE_BORNEE_INFERIEUREMENT ) {
  /* Le cout reduit doit etre positif ou nul */
	if ( Nu > 0 ) {
    if ( DeltaC > SEUIL_POUR_DELTAC_NON_NUL ) {
			/* Le cout reduit de Var1 est strictement positif */
			/* Variable Var1 sur borne inf */
			if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
				return;
			}
      *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
			Flag[Var1] = COLONNE_A_EVITER;
			*NbVarDispo = *NbVarDispo - 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneInfPourPresolve[Var1] );			
		}
	}
	else {
    if ( DeltaC < -SEUIL_POUR_DELTAC_NON_NUL ) {
			/* Variable Var1 sur borne sup */
			if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_INFERIEUREMENT ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
			 	return;
			}					
      *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
			Flag[Var1] = COLONNE_A_EVITER;
			*NbVarDispo = *NbVarDispo - 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneSupPourPresolve[Var1] );			
		}
	}
}
else if ( TypeBrnVar == VARIABLE_BORNEE_SUPERIEUREMENT ) {
  /* Le cout reduit doit etre negatif ou nul */
	if ( Nu > 0 ) {
    if ( DeltaC < -SEUIL_POUR_DELTAC_NON_NUL ) {
		  /* Variable Var1 sur borne sup */
			if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_INFERIEUREMENT ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
				return;
			}							
      *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
			Flag[Var1] = COLONNE_A_EVITER;
			*NbVarDispo = *NbVarDispo - 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneSupPourPresolve[Var1] );
		}
	}
	else {
    if ( DeltaC > SEUIL_POUR_DELTAC_NON_NUL ) {
			/* Variable Var1 sur borne inf */
			if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
				return;
			}					
      *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
			Flag[Var1] = COLONNE_A_EVITER;
			*NbVarDispo = *NbVarDispo - 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneInfPourPresolve[Var1] );			
		}
	}
}
else if ( TypeBrnVar == VARIABLE_NON_BORNEE ) {
  if ( DeltaC > SEUIL_POUR_DELTAC_NON_NUL ) {
		/* Variable Var1 sur borne inf */
		if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
			return;
		}					
    *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
		Flag[Var1] = COLONNE_A_EVITER;
		*NbVarDispo = *NbVarDispo - 1;
    PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneInfPourPresolve[Var1] );		
	}
	else if ( DeltaC < -SEUIL_POUR_DELTAC_NON_NUL ) {
		/* Variable Var1 sur borne sup */
		if ( TypeBnrVar1 == VARIABLE_NON_BORNEE || TypeBnrVar1 == VARIABLE_BORNEE_INFERIEUREMENT ) {
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
			return;
		}					
    *NbColonnesSupprimees = *NbColonnesSupprimees + 1;
		Flag[Var1] = COLONNE_A_EVITER;
		*NbVarDispo = *NbVarDispo - 1;
    PRS_FixerUneVariableAUneValeur( Presolve, Var1, Presolve->BorneSupPourPresolve[Var1] );		
	}				
}

return;
}

/*----------------------------------------------------------------------------*/
/* Cas ou la combinaison lineaire des couts est nulle: On remplace les variables
   Var et Var1 par une autre variable dont on ajuste les bornes. Cette nouvelle
	 variable prend le numero Var */
void PRS_ColonneColineairesDeltaCNul( PROBLEME_PNE * Pne, PRESOLVE * Presolve,
                                      int Var, int Var1, double Nu,
                                      char * Flag, int * NbColonnesSupprimees, int * NbVarDispo )
{
long double XminDeVar; long double XmaxDeVar; long double XminDeVar1; long double XmaxDeVar1;
long double Xmin; long double Xmax; int * TypeDeBornePourPresolve; char XminDeVarValide;
char XmaxDeVarValide; char XminDeVar1Valide; char XmaxDeVar1Valide; char XminValide;
char XmaxValide; char * ConserverLaBorneInfDuPresolve; char * ConserverLaBorneSupDuPresolve;
double * BorneInfPourPresolve; double * BorneSupPourPresolve; int * Cdeb; int * Csui;
int ic1; double * A; char * TypeDeValeurDeBorneInf; int * ContrainteBornanteInferieurement;
int * ContrainteBornanteSuperieurement;

if ( Pne->NbCouplesDeVariablesColineaires >= Pne->NombreDeVariablesTrav ) return;
if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;

XminDeVarValide = NON_PNE;
XmaxDeVarValide = NON_PNE;
XminDeVar1Valide = NON_PNE;
XmaxDeVar1Valide = NON_PNE;
XminValide = NON_PNE;
XmaxValide = NON_PNE;
Xmin = -LINFINI_PNE;
Xmax = LINFINI_PNE;
XminDeVar = (long double) BorneInfPourPresolve[Var];
XminDeVar1 = (long double) BorneInfPourPresolve[Var1];
XmaxDeVar = (long double) BorneSupPourPresolve[Var];
XmaxDeVar1 = (long double) BorneSupPourPresolve[Var1];

if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
  XminDeVarValide = OUI_PNE;
  XmaxDeVarValide = OUI_PNE;
}
else if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
  XminDeVarValide = OUI_PNE;
}
else if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
  XmaxDeVarValide = OUI_PNE;
}

if ( TypeDeBornePourPresolve[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
  XminDeVar1Valide = OUI_PNE;
  XmaxDeVar1Valide = OUI_PNE;
}
else if ( TypeDeBornePourPresolve[Var1] == VARIABLE_BORNEE_INFERIEUREMENT ) {
  XminDeVar1Valide = OUI_PNE;
}
else if ( TypeDeBornePourPresolve[Var1] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
  XmaxDeVar1Valide = OUI_PNE;
}

if ( Nu > 0 ) {
  if ( XminDeVarValide == OUI_PNE && XminDeVar1Valide == OUI_PNE ) {
    Xmin = XminDeVar + ( (long double ) Nu * XminDeVar1 );
		XminValide = OUI_PNE;		
	}
	if ( XmaxDeVarValide == OUI_PNE && XmaxDeVar1Valide == OUI_PNE ) {
    Xmax = XmaxDeVar + ( (long double ) Nu * XmaxDeVar1 );
		XmaxValide = OUI_PNE;		
	}	
}
else {
  if ( XminDeVarValide == OUI_PNE && XmaxDeVar1Valide == OUI_PNE ) {
    Xmin = XminDeVar + ( (long double ) Nu * XmaxDeVar1 );
		XminValide = OUI_PNE;		
	}
	if ( XmaxDeVarValide == OUI_PNE && XminDeVar1Valide == OUI_PNE ) {
    Xmax = XmaxDeVar + ( (long double ) Nu * XminDeVar1 );
		XmaxValide = OUI_PNE;		
	}	
}

/* La nouvelle variable prend le numero Var */

/* Var1 devient fixe mais on met sa valeur a 0 */
*NbColonnesSupprimees = *NbColonnesSupprimees + 1;
Flag[Var1] = COLONNE_A_EVITER;
*NbVarDispo = *NbVarDispo - 1;
PRS_FixerUneVariableAUneValeur( Presolve, Var1, 0.0 );

/* On change les bornes de Var */
Flag[Var] = COLONNE_A_EVITER;
*NbVarDispo = *NbVarDispo - 1;

if ( XminValide == OUI_PNE ) {
  TypeDeBornePourPresolve[Var] = VARIABLE_BORNEE_INFERIEUREMENT;     
  if ( XmaxValide == OUI_PNE ) {
    TypeDeBornePourPresolve[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
	}	
}
else { /* Xmin moins l'infini */
  if ( XmaxValide == OUI_PNE ) {	
    TypeDeBornePourPresolve[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;		
	}
	else {
    TypeDeBornePourPresolve[Var] = VARIABLE_NON_BORNEE;		
	}
}

BorneInfPourPresolve[Var] = (double) Xmin;	
BorneSupPourPresolve[Var] = (double) Xmax;		
ContrainteBornanteInferieurement[Var] = -1;
ContrainteBornanteSuperieurement[Var] = -1;
ConserverLaBorneInfDuPresolve[Var] = OUI_PNE;
ConserverLaBorneSupDuPresolve[Var] = OUI_PNE;  
TypeDeValeurDeBorneInf[Var] = VALEUR_NATIVE;

Presolve->VariableEquivalente[Var]++;

/*printf("Xmin %e  Xmax %e  Var %d\n",(double) Xmin,(double) Xmax,Var);*/

/***************************************************************************************************/
/* Attention modif temporaire: on ecrase UminTrav et UmaxTrav car il peut y avoir des incoherences */
Pne->UminTrav[Var] = (double) Xmin;
Pne->UmaxTrav[Var] = (double) Xmax;
Pne->TypeDeBorneTrav[Var] = TypeDeBornePourPresolve[Var];
/***************************************************************************************************/

/* On met des 0 dans la colonne de Var1 */
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
A = Pne->ATrav;
ic1 = Cdeb[Var1];
while ( ic1 >= 0 ) {
  A[ic1] = 0.0;
  ic1 = Csui[ic1];
}

/* Infos pour le postsolve */
Pne->TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_COLONNE_COLINEAIRE;
Pne->IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbCouplesDeVariablesColineaires;
Pne->NombreDOperationsDePresolve++;

Pne->PremiereVariable    [Pne->NbCouplesDeVariablesColineaires] = Var;
Pne->XminPremiereVariable[Pne->NbCouplesDeVariablesColineaires] = (double) XminDeVar;
Pne->XmaxPremiereVariable[Pne->NbCouplesDeVariablesColineaires] = (double) XmaxDeVar;

Pne->DeuxiemeVariable    [Pne->NbCouplesDeVariablesColineaires] = Var1;
Pne->XminDeuxiemeVariable[Pne->NbCouplesDeVariablesColineaires] = (double) XminDeVar1;
Pne->XmaxDeuxiemeVariable[Pne->NbCouplesDeVariablesColineaires] = (double) XmaxDeVar1;

Pne->ValeurDeNu          [Pne->NbCouplesDeVariablesColineaires] = Nu;
Pne->NbCouplesDeVariablesColineaires++;
/* Fin infos pour le postsolve */

return;

}

