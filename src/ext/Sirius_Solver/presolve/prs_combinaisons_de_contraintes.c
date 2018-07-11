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

   FONCTION: La procedure consiste a chercher des contraintes de meme
						 support et a en calculer une combinaison lineaire pour
						 faire disparaitre une des variables d'une des contraintes.
						 On augmente ainsi le creux de la matrice des contraintes.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

# define TRACES 0

# define MAX_NBTMX  100 /*100*/ /* Nb max de contraintes examinees pour chqie colonne */
# define MAX_TERMES_UTILES 20 /* 20 */ /* Nb max de termes de la colonne pivot */
# define EPSILON_POUR_ANNULATION  1.e-16 /*1.e-16*/

void PRS_ComparerContraintesADeuxTermesUtiles( PRESOLVE * , int , char , double , int * , int ,
                                               double * , char * , int , char * , int * );
																							 
/*----------------------------------------------------------------------------*/

void PRS_CombinaisonDeContraintes( PRESOLVE * Presolve, int * NbModifications ) 
{
int il; int ic; int ilDebCnt; int ilMaxCnt; int ic1;  int NombreDeContraintes;
int NombreDeVariables; int Cnt; int Cnt1; double * V; char * T; int LallocTas;
double BCnt; double * B; int * Mdeb; int * NbTerm; int * Nuvar; double * A;
int * NumContrainte; double S; char * Flag; char * pt;
char * ContrainteInactive; int * Cdeb; int * Csui; int * TypeDeBornePourPresolve;
int Var; int NbT; int * ParColonnePremiereVariable; int * ParColonneVariableSuivante;
char * Buffer; char * SensContrainte; double * ValeurDeXPourPresolve;
int NbCntDeVar; int * NumCntDeVar; char SensCnt; int VarDeCnt;
int * IndexIlDeVar; int * NbTermesUtilesDeCnt; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*NbModifications = 0;
  
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
LallocTas += NombreDeContraintes * sizeof( int ); /* NumCntDeVar */
LallocTas += NombreDeContraintes * sizeof( int ); /* IndexIlDeVar */
LallocTas += NombreDeContraintes * sizeof( int ); /* NbTermesUtilesDeCnt */

Buffer = (char *) malloc( LallocTas );
if ( Buffer == NULL ) {
  printf(" Solveur PNE , memoire insuffisante dans le presolve. Sous-programme: PRS_CombinaisonDeContraintes \n");
	return;
}

pt = Buffer;
V = (double *) pt;
pt += NombreDeVariables * sizeof( double );
T = (char *) pt;
pt +=  NombreDeVariables * sizeof( char );
Flag = (char *) pt;
pt += NombreDeContraintes * sizeof( char );
NumCntDeVar = (int *) pt;
pt += NombreDeContraintes * sizeof( int ); 
IndexIlDeVar = (int *) pt;
pt += NombreDeContraintes * sizeof( int );
NbTermesUtilesDeCnt = (int *) pt;
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
		  if ( TypeDeBornePourPresolve[Nuvar[il]] != VARIABLE_FIXE ) NbT++;			
		}
	  il++;
  }
	NbTermesUtilesDeCnt[Cnt] = NbT;
}

/* On balaye les colonnes dans l'ordre decroissant du nombre de termes et on ne compare que les
   les lignes qui ont un terme dans cette colonne */

/*for ( NbT = 2 ; NbT <= Presolve->NbMaxTermesDesColonnes ; NbT++ ) {*/

/*printf("NbMaxTermesDesColonnes %d\n", Presolve->NbMaxTermesDesColonnes);*/

for ( NbT = Presolve->NbMaxTermesDesColonnes ; NbT >=2 ; NbT-- ) {
  Var = ParColonnePremiereVariable[NbT];
	while ( Var >= 0 ) {
	  /*********************************************/
	  if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) goto NextVar;
	  /* On prepare la table des contraintes a explorer */
 	  NbCntDeVar = 0;
	  ic = Cdeb[Var];
    while ( ic >= 0 ) {
		  if ( fabs( A[ic] ) > ZERO_PRESOLVE ) {
		    Cnt = NumContrainte[ic];
        if ( Flag[Cnt] == 0 && NbTermesUtilesDeCnt[Cnt] <= MAX_TERMES_UTILES ) {
			    NumCntDeVar[NbCntDeVar] = Cnt;
					IndexIlDeVar[NbCntDeVar] = ic;
				  NbCntDeVar++;					
			  }
      }
	    ic = Csui[ic];
	  }

		if ( NbCntDeVar <= 1 ) goto NextVar;
		
    if ( NbCntDeVar > MAX_NBTMX ) NbCntDeVar = MAX_NBTMX;

		/* On balaie les contraintes de la variable Var */
			 
	  for ( ic = 0 ; ic < NbCntDeVar ; ic++ ) {
			if ( A[IndexIlDeVar[ic]] == 0.0 ) continue;
	    Cnt = NumCntDeVar[ic];
		  if ( Flag[Cnt] == 1 ) continue;
			if ( SensContrainte[Cnt] != '=' ) continue;
			if ( NbTermesUtilesDeCnt[Cnt] <= 1 ) continue;
			/*if ( NbTermesUtilesDeCnt[Cnt] > MAX_TERMES_UTILES ) continue;*/
		  /* Preparation des tables pour la contrainte Cnt */
			S = 0.0;
      ilDebCnt = Mdeb[Cnt];
	    ilMaxCnt = ilDebCnt + NbTerm[Cnt];
	    il = ilDebCnt;
	    while ( il < ilMaxCnt ) {
			  if ( A[il] != 0.0 ) {
					VarDeCnt = Nuvar[il];
				  if ( TypeDeBornePourPresolve[VarDeCnt] != VARIABLE_FIXE ) {
		        V[VarDeCnt] = A[il];
		        T[VarDeCnt] = 1;
					}
					else S += A[il] * ValeurDeXPourPresolve[VarDeCnt];
				}  
		    il++;
	    }
						
		  BCnt = B[Cnt] - S;
			SensCnt = SensContrainte[Cnt];
		
      /* On compare a Cnt toutes les contraintes suivantes de la colonne */
	    for ( ic1 = 0 /*ic + 1*/ ; ic1 < NbCntDeVar ; ic1++ ) {
        if ( ic1 == ic ) continue;
			  if ( A[IndexIlDeVar[ic1]] == 0.0 ) continue;
			
	      Cnt1 = NumCntDeVar[ic1];
		    if ( Flag[Cnt1] != 0 ) continue;
				if ( NbTermesUtilesDeCnt[Cnt1] < NbTermesUtilesDeCnt[Cnt] ) continue;
			  /* Comparaison de Cnt a Cnt1 */

         PRS_ComparerContraintesADeuxTermesUtiles( Presolve, Cnt, SensCnt,  BCnt, NbTermesUtilesDeCnt,
				                                           Var, V, T, Cnt1, Flag, NbModifications );

																									

				/* Si le Flag de Cnt a change on passe a la contrainte suivante */
				if ( Flag[Cnt] == 1 ) break;
																 
	    }
      /* RAZ de V et T avant de passer a la contrainte suivante */
		  /*Flag[Cnt] = 1;*/
      il = ilDebCnt;
 	    while ( il < ilMaxCnt ) {
		    V[Nuvar[il]] = 0.0;
		    T[Nuvar[il]] = 0;
		    il++;
	    }
  	}

		/*
	  for ( ic = 0 ; ic < NbCntDeVar ; ic++ ) {
		  Flag[NumCntDeVar[ic]] = 1;
    }
		*/
		
	  /*********************************************/
		NextVar:
	  Var = ParColonneVariableSuivante[Var];
	}
}

free( Buffer );

# if VERBOSE_PRS == 1 
  printf("-> Nombre de combinaisons de contraintes %d\n",*NbModifications);
	fflush(stdout);
# endif

return;
}

/*----------------------------------------------------------------------------*/

void PRS_ComparerContraintesADeuxTermesUtiles( PRESOLVE * Presolve, int Cnt,
                                               char SensCnt, double BCnt,
																							 int * NbTermesUtilesDeCnt,																							 
																							 int VariablePivot,
                                               double * V, char * T, int Cnt1,
																							 char * Flag, int * NbModifications )
{
int Nb; int il1; int il1Max; int * Mdeb; int * NbTerm; double * A; int * Nuvar;
int Var1; double * B; long double Rapport; int ilDeRapport; double * ValeurDeXPourPresolve;
char * SensContrainte; int * TypeDeBornePourPresolve; char SensCnt1; PROBLEME_PNE * Pne;
int	il1Deb; int * TypeDeVariable; double X; double PlusGrandTerme; double PlusPetitTerme; 

if ( SensCnt != '=' ) return;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
B = Pne->BTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
PlusGrandTerme = Pne->PlusGrandTerme;
PlusPetitTerme = Pne->PlusPetitTerme;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
Rapport = 1;
ilDeRapport = -1; 
/* Si la contrainte Cnt1 contient le support de la contrainte Cnt, on cherche a annuler 1
   terme de la contrainte Cnt1 */
Nb = NbTermesUtilesDeCnt[Cnt];
il1Deb = Mdeb[Cnt1];
il1Max = il1Deb + NbTerm[Cnt1];
/* Determination du rapport */
il1 = il1Deb;
while ( il1 < il1Max ) {
  if ( A[il1] != 0.0 ) {
    Var1 = Nuvar[il1];
	  if ( TypeDeBornePourPresolve[Var1] != VARIABLE_FIXE ) {
      if ( T[Var1] == 1 ) Nb--;
			if ( Var1 == VariablePivot ) {
        Rapport = -A[il1] / (long double) V[VariablePivot];
	      ilDeRapport = il1;								
			}
	  }
	}
  il1++;
}

if ( Nb != 0 ) return;
if ( ilDeRapport < 0 ) return;

/* Le support de Cnt est inclus dans celui de Cnt1, on va chercher un terme a annuler dans Cnt1 */

/* On verifie qu'on ne deconditionne pas trop le systeme */
il1 = il1Deb;
while ( il1 < il1Max ) {
  if ( il1 != ilDeRapport ) {
    if ( A[il1] != 0.0 ) {
      Var1 = Nuvar[il1];
	    if ( TypeDeBornePourPresolve[Var1] != VARIABLE_FIXE ) {
			  if ( T[Var1] == 1 ) {
				  X = fabs( A[il1] + ((long double) V[Var1] * Rapport) );
					if ( X < EPSILON_POUR_ANNULATION ) X = 0.0;
          if ( X > PlusGrandTerme || (X < PlusPetitTerme && X != 0.0 ) ) {
					  # if TRACES == 1
						  printf("Refus de la combinaison car mauvais conditionnement: PlusPetitTerme %e PlusGrandTerme %e et X = %e \n",
							        PlusPetitTerme,PlusGrandTerme,X);
						  printf("     A %18.15e  V %18.15e Rapport %18.15e\n",A[il1],V[Var1],(double)Rapport);
							/* Impression des 2 contraintes */
              printf("Contrainte 1: %d\n",Cnt);
							il1 = Mdeb[Cnt];
							il1Max = il1 + NbTerm[Cnt];
							while ( il1 < il1Max ) {
                printf(" %15.10e (%d) ",A[il1],Nuvar[il1]);
							  il1++;
							}
              printf(" %c %15.10e\n",Pne->SensContrainteTrav[Cnt],Pne->BTrav[Cnt]);
							
              printf("Contrainte 2: %d\n",Cnt1);
							il1 = Mdeb[Cnt1];
							il1Max = il1 + NbTerm[Cnt1];
							while ( il1 < il1Max ) {
                printf(" %15.10e (%d) ",A[il1],Nuvar[il1]);
							  il1++;
							}
              printf(" %c %15.10e\n",Pne->SensContrainteTrav[Cnt1],Pne->BTrav[Cnt1]);													
						# endif
					  return;
					}				
			  }
		  }
		}
	}
  il1++;
}

if ( fabs( A[ilDeRapport] + ((long double) V[VariablePivot] * Rapport) ) > EPSILON_POUR_ANNULATION ) {
  # if TRACES == 1
    printf("Refus de la combinaison car elle n'annule pas le terme choisi: %20.16e et Rapport %e\n",
		        fabs( A[ilDeRapport] + ((long double) V[VariablePivot] * Rapport) ),(double) Rapport);	
	# endif
  return;
}

SensCnt1 = SensContrainte[Cnt1];

/* On modifie Cnt1 */
il1 = il1Deb;
while ( il1 < il1Max ) {
  if ( A[il1] != 0.0 ) {
    Var1 = Nuvar[il1];
	  if ( TypeDeBornePourPresolve[Var1] != VARIABLE_FIXE ) {
			if ( T[Var1] == 1 ) {
				A[il1] += (long double) V[Var1] * Rapport;				
			 	if ( fabs( A[il1] ) < EPSILON_POUR_ANNULATION ) { 
				 A[il1] = 0.0;
				 NbTermesUtilesDeCnt[Cnt1]--;
				}				
			}
		}
	}
  il1++;
}

/*
A[ilDeRapport] = 0.0;
NbTermesUtilesDeCnt[Cnt1]--;
*/

/* Si la contrainte etait bornante pour cette variable on initialise la conservation
	 de la borne puisque la variable disparait de la contrainte */
if ( Presolve->ContrainteBornanteSuperieurement[VariablePivot] == Cnt1 ) {
	Presolve->ConserverLaBorneSupDuPresolve[VariablePivot] = OUI_PNE;
	Presolve->ContrainteBornanteSuperieurement[VariablePivot] = -1;
}
if ( Presolve->ContrainteBornanteInferieurement[VariablePivot] == Cnt ) {
	Presolve->ConserverLaBorneInfDuPresolve[VariablePivot] = OUI_PNE;
	Presolve->ContrainteBornanteInferieurement[VariablePivot] = -1;
}
	
B[Cnt1] += Rapport * BCnt;
*NbModifications = *NbModifications + 1;
/*Flag[Cnt1] = 1;*/

												
return;
}



