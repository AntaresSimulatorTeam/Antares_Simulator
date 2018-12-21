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

   FONCTION: Recherche de contraintes colineaires.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif
 
# include "lu_define.h"
# include "lu_fonctions.h"
 
# define ZERO_COLINEAIRE  1.e-10 /*1.e-10*/
# define ZERO_QUASI_COLINEAIRE  1.e-7 /*1.e-7*/
# define MARGE_POUR_INFAISABILITE  1.e-6 /*1.e-6*/
# define MAX_NBTMX  1000

void PRS_ComparerContraintes( PRESOLVE * , int , int , double , double * , char * , int , char * , int * );
void PRS_InhiberUneContrainte( PROBLEME_PNE * , PRESOLVE * , char * , int , int , int * );
															 
/*----------------------------------------------------------------------------*/

void PRS_ContraintesColineaires( PRESOLVE * Presolve , int * NbContraintesSupprimees ) 
{
int il; int ic; int ilDebCnt; int ilMaxCnt; int ic1;  int NombreDeContraintes;
int NombreDeVariables; int Cnt; int Cnt1; double * V; char * T; int LallocTas;
double BCnt; double * B; int * Mdeb; int * NbTerm; int * Nuvar; double * A;
int * NumContrainte; int NbTermesUtilesDeCnt; double S; char * Flag; char * pt;
char * ContrainteInactive; int * Cdeb; int * Csui; int * TypeDeBornePourPresolve;
int Var; int NbT; int * ParColonnePremiereVariable; int * ParColonneVariableSuivante;
char * Buffer; char * SensContrainte; int HashCnt; double * ValeurDeXPourPresolve;
int * HashCode; char InitV; int NbCntDeVar; int * NumCntDeVar; int VarCnt;
PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*NbContraintesSupprimees = 0;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
  return;
}

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
SensContrainte = Pne->SensContrainteTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

ContrainteInactive = Presolve->ContrainteInactive;
ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ParColonnePremiereVariable = Presolve->ParColonnePremiereVariable;	
ParColonneVariableSuivante = Presolve->ParColonneVariableSuivante;

LallocTas = 0;
LallocTas += NombreDeVariables * sizeof( double ); /* V */
LallocTas += NombreDeVariables * sizeof( char ); /* T */
LallocTas += NombreDeContraintes * sizeof( char ); /* Flag */
LallocTas += NombreDeContraintes * sizeof( int ); /* HashCode */
LallocTas += NombreDeContraintes * sizeof( int ); /* NumCntDeVar */

Buffer = (char *) malloc( LallocTas );
if ( Buffer == NULL ) {
  printf(" Solveur PNE , memoire insuffisante dans le presolve. Sous-programme: PRS_ContraintesColineaires \n");
	return;
}

pt = Buffer;
V = (double *) pt;
pt += NombreDeVariables * sizeof( double );
T = (char *) pt;
pt +=  NombreDeVariables * sizeof( char );
Flag = (char *) pt;
pt += NombreDeContraintes * sizeof( char );
HashCode = (int *) pt;
pt += NombreDeContraintes * sizeof( int );
NumCntDeVar = (int *) pt;
pt += NombreDeContraintes * sizeof( int ); 

memset( (char *) T, 0, NombreDeVariables * sizeof( char ) );

/* Flag = 0 s'il faut prendre en compte la contrainte */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) Flag[Cnt] = 1;
	else Flag[Cnt] = 0;
	/* Calcul d'un hashcode */
	NbT = 0;
	ic = 0;
  il = Mdeb[Cnt];
  ilMaxCnt = il + NbTerm[Cnt];
  while ( il < ilMaxCnt ) {
    if ( A[il] != 0.0 ) {
      Var = Nuvar[il];
		  if ( TypeDeBornePourPresolve[Var] != VARIABLE_FIXE ) {
			  NbT++;
				ic += Var;
			}
		}
	  il++;
  }
	HashCode[Cnt] = ( ic + NbT ) % NombreDeVariables;
}

/* On balaye les colonnes dans l'ordre croissant du nombre de termes et on ne compare que les
   les lignes qui ont un terme dans cette colonne */

for ( NbT = 2 ; NbT <= Presolve->NbMaxTermesDesColonnes ; NbT++ ) {
  Var = ParColonnePremiereVariable[NbT];
	while ( Var >= 0 ) {
	  /*********************************************/
	  if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) goto NextVar;
	  /* On prepare la table des contraintes a explorer */
 	  NbCntDeVar = 0;
	  ic = Cdeb[Var];
    while ( ic >= 0 ) {
		  if ( A[ic] != 0 ) {
		    Cnt = NumContrainte[ic];
        if ( Flag[Cnt] == 0 ) {
			    NumCntDeVar[NbCntDeVar] = Cnt;
				  NbCntDeVar++;
			  }
      }
	    ic = Csui[ic];
	  }

    if ( NbCntDeVar > MAX_NBTMX ) NbCntDeVar = MAX_NBTMX;

	  for ( ic = 0 ; ic < NbCntDeVar ; ic++ ) {
	    Cnt = NumCntDeVar[ic];
		  if ( Flag[Cnt] == 1 ) continue;
      ilDebCnt = 0;
		  HashCnt = HashCode[Cnt];
		  InitV = NON_PNE;
			ilMaxCnt = -1;
			BCnt = LINFINI_PNE;
			NbTermesUtilesDeCnt = 0;
      /* On compare a Cnt toutes les contraintes suivantes de la colonne */
	    for ( ic1 = ic + 1 ; ic1 < NbCntDeVar ; ic1++ ) {
	      Cnt1 = NumCntDeVar[ic1];
			  if ( HashCode[Cnt1] != HashCnt ) continue;
		    if ( Flag[Cnt1] != 0 ) continue;	
			  /* Comparaison de Cnt a Cnt1 */
			  if ( InitV == NON_PNE ) {
		      /* Preparation des tables pour la contrainte Cnt */
			    S = 0.0;
          ilDebCnt = Mdeb[Cnt];
	        ilMaxCnt = ilDebCnt + NbTerm[Cnt];
	        il = ilDebCnt;
	        while ( il < ilMaxCnt ) {
			      if ( A[il] != 0.0 ) {
					    VarCnt = Nuvar[il];
				      if ( TypeDeBornePourPresolve[VarCnt] != VARIABLE_FIXE ) {
		            V[VarCnt] = A[il];
		            T[VarCnt] = 1;
						    NbTermesUtilesDeCnt++;
					    }
					    else S += A[il] * ValeurDeXPourPresolve[VarCnt];
				    }  
		        il++;
	        }
		      BCnt = B[Cnt] - S;
          InitV = OUI_PNE;
					if ( NbTermesUtilesDeCnt <= 0 ) break;					
			  }
        PRS_ComparerContraintes( Presolve, Cnt, NbTermesUtilesDeCnt, BCnt, V, T,
				                         Cnt1, Flag, NbContraintesSupprimees );

				/* Si le Flag de Cnt a change on passe a la contrainte suivante */
				if ( Flag[Cnt] == 1 ) break;
																 
	    }
      /* RAZ de V et T avant de passer a la contrainte suivante */
		  Flag[Cnt] = 1;
		  if ( InitV == OUI_PNE ) {
        il = ilDebCnt;
 	      while ( il < ilMaxCnt ) {
		      V[Nuvar[il]] = 0.0;
		      T[Nuvar[il]] = 0;
		      il++;
	      }
		  }
  	}
	  /*********************************************/
		NextVar:
	  Var = ParColonneVariableSuivante[Var];
	}
}

free( Buffer );

# if VERBOSE_PRS == 1 
  printf("-> Nombre de contraintes supprimees par colinearite %d\n",*NbContraintesSupprimees);
	fflush(stdout);
# endif

return;
}

/*----------------------------------------------------------------------------*/

void PRS_ComparerContraintes( PRESOLVE * Presolve, int Cnt, int NbTermesUtilesDeCnt, double BCnt,
                              double * V, char * T, int Cnt1, char * Flag, int * NbContraintesSupprimees )
{
int Nb; double S1; int il1; int il1Max; int * Mdeb; int * NbTerm; double * A; int * Nuvar;
int Var1; double * B; long double BCnt1; long double Rapport; double * ValeurDeXPourPresolve; 
char * SensContrainte; int * TypeDeBornePourPresolve; char SensCnt1; char SensCnt; PROBLEME_PNE * Pne;
long double X;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
B = Pne->BTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

Nb = NbTermesUtilesDeCnt;
S1 = 0.0;		
il1 = Mdeb[Cnt1];
il1Max = il1 + NbTerm[Cnt1];
/* Determination du rapport */
Rapport = 1.0;
while ( il1 < il1Max ) {
  if ( A[il1] != 0.0 ) {
    Var1 = Nuvar[il1];
	  if ( TypeDeBornePourPresolve[Var1] == VARIABLE_FIXE ) S1 += A[il1] * ValeurDeXPourPresolve[Var1];  
	  else {
      if ( T[Var1] == 0 ) return; /* Pas de terme correspondant dans la colonne */		
		  Rapport = (long double) V[Var1] / (long double) A[il1];
      Nb--;
			il1++;
			break;
	  }
	}
  il1++;
}

/* On poursuit l'analyse de la contrainte Cnt1 */
while ( il1 < il1Max ) {
  if ( A[il1] != 0.0 ) {
    Var1 = Nuvar[il1];
	  if ( TypeDeBornePourPresolve[Var1] == VARIABLE_FIXE ) S1 += A[il1] * ValeurDeXPourPresolve[Var1];  
    else {
		  if ( T[Var1] == 0 ) return;
			/* Il est utile de supprimer les contraintes quasi colineaires. ces contraintes sont
			   souvent produites par les modeleurs. Elles n'apportent rien mais les conserver
				 conduisent a des difficultes numeriques. Pour cette raison, le ZERO_RAPPORT_COLINEAIRE
				 reste assez grand. Dans une etape suivante on controle que la combinaison lineaire
				 permet bien d'avoir un vecteur quasi nul */				 
			X = fabs( (long double) V[Var1] - ( Rapport * (long double) A[il1]) );
      if ( X > ZERO_QUASI_COLINEAIRE ) return;			
			/* Le terme resultant est-il suffisament plus petit que le terme d'origine ? */
			if ( X / ( fabs( A[il1]  ) + ZERO_COLINEAIRE ) > ZERO_COLINEAIRE ) return;
	    Nb--;
		}
	}
	il1++;
}

if ( Nb != 0 ) return;

SensCnt = SensContrainte[Cnt];

BCnt1 = (long double) B[Cnt1] - (long double) S1;
SensCnt1 = SensContrainte[Cnt1];

if ( SensCnt == '=' ) {
  if ( SensCnt1 == '=' ) {
	  X = fabs( (long double) BCnt - (Rapport * BCnt1) );
    if ( X > ZERO_QUASI_COLINEAIRE ) return; /* Il est preferable de ne pas se prononcer sur la faisabilite */
	  /* Cnt et Cnt1 sont colineaires on inhibe Cnt1 */
    PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt, Cnt1, NbContraintesSupprimees );	 	
	}
  else if ( SensCnt1 == '<' ) {
	  if ( Rapport > 0.0 ) {
      if ( Rapport * BCnt1 < (long double) BCnt - MARGE_POUR_INFAISABILITE ) {
				Pne->YaUneSolution = PROBLEME_INFAISABLE;
        return;
			}
			else if ( Rapport * BCnt1 >= (long double) BCnt ) {
	      /* Cnt et Cnt1 sont colineaires on inhibe Cnt1 */
        PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt, Cnt1, NbContraintesSupprimees );	 				
			}
		}
		else {
      /* Le rapport est negatif */
      if ( Rapport * BCnt1 > (long double) BCnt + MARGE_POUR_INFAISABILITE ) {
				Pne->YaUneSolution = PROBLEME_INFAISABLE;
        return;				
			}
			else if ( Rapport * BCnt1 <= (long double) BCnt ) {
	      /* Cnt et Cnt1 sont colineaires on inhibe Cnt1 */
        PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt, Cnt1, NbContraintesSupprimees );	 								
			}
		}
	}
}
else {
  /* La contrainte Cnt est de type < */
  if ( SensCnt1 == '=' ) {
    if ( (long double) BCnt < (Rapport * BCnt1) - MARGE_POUR_INFAISABILITE ) {
			Pne->YaUneSolution = PROBLEME_INFAISABLE;
      return;				
		}
		else if ( (long double) BCnt >= Rapport * BCnt1 ) {
	    /* Cnt et Cnt1 sont colineaires on inhibe Cnt */
      PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt1, Cnt, NbContraintesSupprimees );	 											
		}
	}
	else {	
	  /* La contrainte Cnt1 est aussi de type < */
	  if ( Rapport > 0.0 ) {		
		  if ( (long double) BCnt < Rapport * BCnt1 ) {			
			  /* On inhibe Cnt1 */
        PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt, Cnt1, NbContraintesSupprimees );	 														
			}
			else {			
			  /* On inhibe Cnt */
        PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt1, Cnt, NbContraintesSupprimees );	 															
			}
		}
		else {		
		  /* Rapport < 0 */
			if ( Rapport * BCnt1 > (long double) BCnt + MARGE_POUR_INFAISABILITE ) {
			  Pne->YaUneSolution = PROBLEME_INFAISABLE;
        return;						
			}
			else 	if ( fabs( (long double) BCnt - (Rapport * BCnt1) ) < ZERO_COLINEAIRE ) {
        /* On inhibe Cnt1 */
        PRS_InhiberUneContrainte( Pne, Presolve, Flag, Cnt, Cnt1, NbContraintesSupprimees );	 														
				/* On transforme Cnt en = */
        SensContrainte[Cnt] = '=';
				/* Comme on transforme le sens de la contrainte, la variable duale que l'on avait n'est plus exploitable
				   puisque cette nouvelle contrainte en regroupe 2 */			  
				Presolve->ConnaissanceDeLambda[Cnt] = LAMBDA_NON_INITIALISE;		 
        Presolve->LambdaMin[Cnt] = -LINFINI_PNE;
	      Presolve->LambdaMax[Cnt] = LINFINI_PNE;							
			}
		}
	}
}

return;
}

/*----------------------------------------------------------------------------*/

void PRS_InhiberUneContrainte( PROBLEME_PNE * Pne, PRESOLVE * Presolve, char * Flag,
                               int ContrainteConservee, int ContrainteInhibee,
															 int * NbContraintesSupprimees ) 
{
int il1; int il1Max; int Var1; int * Mdeb; int * NbTerm; int * Nuvar; 
int * ContrainteBornanteInferieurement; int * ContrainteBornanteSuperieurement;

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;

ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;
ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;

/* On reporte ContrainteBornanteSuperieurement et ContrainteBornanteInferieurement
	 sur la contrainte conservee */
il1 = Mdeb[ContrainteInhibee];
il1Max = il1 + NbTerm[ContrainteInhibee];				 
while ( il1 < il1Max ) {	
  Var1 = Nuvar[il1];
  if ( ContrainteBornanteSuperieurement[Var1] == ContrainteInhibee ) ContrainteBornanteSuperieurement[Var1] = ContrainteConservee;
  if ( ContrainteBornanteInferieurement[Var1] == ContrainteInhibee ) ContrainteBornanteInferieurement[Var1] = ContrainteConservee;
  il1++;		
}			
Presolve->ContrainteInactive[ContrainteInhibee] = OUI_PNE;
Pne->NumeroDesContraintesInactives[Pne->NombreDeContraintesInactives] = ContrainteInhibee;
Pne->NombreDeContraintesInactives++;

/* Pour le postsolve */
Pne->TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_CONTRAINTE_COLINEAIRE;
Pne->IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbSuppressionsDeContraintesColineaires;
Pne->NombreDOperationsDePresolve++;	

Pne->ContrainteConservee[Pne->NbSuppressionsDeContraintesColineaires] = Pne->CorrespondanceCntPneCntEntree[ContrainteConservee]; 	
Pne->ContrainteSupprimee[Pne->NbSuppressionsDeContraintesColineaires] = Pne->CorrespondanceCntPneCntEntree[ContrainteInhibee]; 	
Pne->NbSuppressionsDeContraintesColineaires++;

Flag[ContrainteInhibee] = 1;
*NbContraintesSupprimees = *NbContraintesSupprimees + 1;

return;
}
  
