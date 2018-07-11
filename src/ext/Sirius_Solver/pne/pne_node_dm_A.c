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

   FONCTION: Mise sous forme coarse (Dumalge Mendelson) de la matrice
	           des contraintes
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif
		
# include "lu_define.h"
# include "lu_fonctions.h"

# if DUMALGE_MENDELSON_PNE == OUI_PNE

# include "btf.h"
# include "btf_internal.h"

# include "cs.h"


# define UMIN_EGAL_UMAX  10
# define PROFONDEUR_MIN_POUR_NODE_PRESOLVE 10
# define CYCLE_NODE_PRESOLVE 1
# define MX_CYCLES 5
# define MARGE 1.e-6
# define MARGE_POUR_ARRONDI 1.e-4
# define MARGE_POUR_VARIABLE_CONTINUE 1.e-2
# define ZERO  1.e-7


# define LIGNE 1   
# define COLONNE 2

/*----------------------------------------------------------------------------*/

void PNE_NodeDumalgeMendelsonMatriceDesContraintes( PROBLEME_PNE * Pne,
                                                   double * ValeurDeBorneInf,   
																									 double * ValeurDeBorneSup,
																									 char * BorneInfConnue,
																									 char * BorneSupConnue,   
																									 char * UneVariableAEteFixee,
																									 int * Faisabilite )
{
int Var; int il; int ilMax; int ilAi; int ic; int i; int j; char Flag; char Resolu;
int * VarOldVarNew; int * VarNewVarOld;
int NbIneg;
int Cb; int C1; int C2; int C3; int IndexRR; int IndexCC;
int R1; int R2; int R3; int Rb;
int * Ligne; int * Colonne; int NbCntDuProblemeReduit; int NbVarDuProblemeReduit;
int ContrainteDepart; int ContrainteMx;

int NbCntSupprimees; int NombreDeContraintesTrav; int NombreDeVariablesTrav;
char * CntSupprimee;
int * LigneIndeterminee; int NombreDeLignesIndeterminees;

MATRICE * MatriceFactorisee; int NbVar0; int ccDeb; int ccFin; int rrDeb; int rrFin;
void * MatriceAFactoriser;
int NbCnt; int NbVar; int Cnt; double * B; int Cnt1;
int il1; int il1Max; char * LaContrainteEstBornante;
cs * A; double * Scatter; double Norme; double Norme1; double Pscal; double Cos;

int * MdebTrav; int * NbTermTrav; int * NuvarTrav; double * ATrav; char * SensContrainteTrav; int * CdebTrav; int * CsuiTrav; int * NumContrainteTrav;
int * CntOldCntNew; char * ContrainteInactive; int * TypeDeBorneTrav;
int * CntNewCntOld; csd * Csd; csi seed; double S; char CodeRet; char FactorisationOK; char MatriceSinguliere; char Choix;

printf("PNE_NodeDumalgeMendelsonMatriceDesContraintes \n");

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
NombreDeContraintesTrav = Pne->NombreDeContraintesTrav;

TypeDeBorneTrav = Pne->TypeDeBorneTrav;

MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
ATrav = Pne->ATrav;
CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
SensContrainteTrav = Pne->SensContrainteTrav;

CntSupprimee = (char *) malloc( NombreDeContraintesTrav * sizeof( char ) );
memset( (char *) CntSupprimee, 0, NombreDeContraintesTrav * sizeof( char ) );

i = Pne->NombreDeContraintesTrav;

B = (double *) malloc( i * sizeof( double ) );
A = (cs *) malloc( sizeof( cs ) );

/* Decompte du nombre de variables: on enleve les colonnes sans terme */

CntOldCntNew = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );
CntNewCntOld = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );

Debut:

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) CntOldCntNew[Cnt] = -1;

/* Decompte du nombre max de termes de la matrice */
A->nzmax = 0;
NbCnt = 0;
NbIneg = 0;

ContrainteDepart = 0;
if ( ContrainteDepart >= Pne->NombreDeContraintesTrav ) ContrainteDepart = 0;
ContrainteMx = Pne->NombreDeContraintesTrav;

for ( Cnt = 0 ; Cnt < ContrainteMx ; Cnt++ ) {

	S = Pne->BTrav[Cnt];
  B[Cnt] = S;
	Flag = 0;
	if ( CntSupprimee[Cnt] == 1 ) goto DECOMPTE_CNT;
	
  /* Toutes les contraintes d'inegalite ont ete mises sous forme < */
	if ( SensContrainteTrav[Cnt] == '<' ) NbIneg++;
	il = MdebTrav[Cnt];
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
    if ( ATrav[il] != 0.0 ) {
		  Var = NuvarTrav[il];
		  if ( BorneInfConnue[Var] != UMIN_EGAL_UMAX ) {
		    A->nzmax++;
				Flag = 1;
			}
			else {
			  S -= ATrav[il] * Pne->UTrav[Var];
			}
		}
    il++;
  }
	DECOMPTE_CNT:
	if ( Flag == 1 ) {
	  CntOldCntNew[Cnt] = NbCnt;
	  CntNewCntOld[NbCnt] = Cnt;
		B[Cnt] = S;
	  NbCnt++;
	}	
}

printf("NbCnt %d   sur %d\n",NbCnt,Pne->NombreDeContraintesTrav);

A->nzmax += NbIneg;

NbVar = NbIneg;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( BorneInfConnue[Var] != UMIN_EGAL_UMAX ) NbVar++;
}

VarOldVarNew = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
VarNewVarOld = (int *) malloc( NbVar * sizeof( int ) );
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) VarOldVarNew[Var] = -1;

printf("A->nzmax %d\n",(int) A->nzmax);

A->p = (csi *) malloc( ( NbVar + 1 ) * sizeof( csi ) );
A->i = (csi *) malloc( A->nzmax * sizeof( csi ) );
A->x = (double *) malloc( A->nzmax * sizeof( double ) );
A->nz = -1; /* compressed-col */

/* Il faut recompter les variables et les contraintes */
NbVar = 0;
ilAi = 0;  
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if (  BorneInfConnue[Var] == UMIN_EGAL_UMAX ) continue;
	Flag = 0;
  A->p[NbVar] = ilAi;
	ic = CdebTrav[Var];
	while ( ic >= 0 ) {
		i = CntOldCntNew[NumContrainteTrav[ic]];
	  if ( ATrav[ic] != 0.0 && i >= 0 ) {
      A->i[ilAi] = i;
			A->x[ilAi] = ATrav[ic];
      ilAi++;
			Flag = 1;
		}
    ic = CsuiTrav[ic];
  }
	if ( Flag == 1 ) {
	  VarOldVarNew[Var] = NbVar;
    VarNewVarOld[NbVar] = Var;
	  NbVar++;
	}
}

for ( Cnt = ContrainteDepart ; Cnt < ContrainteMx ; Cnt++ ) {
  if ( SensContrainteTrav[Cnt] != '<' ) continue;
	i = CntOldCntNew[Cnt];
  if ( i >= 0 ) {
    A->p[NbVar] = ilAi;		
    A->i[ilAi] = i;
		A->x[ilAi] = 1.0;
		VarNewVarOld[NbVar] = Pne->NombreDeVariablesTrav + Cnt;
    ilAi++;
		NbVar++;
	}
}

printf("NbVar %d\n",NbVar);


NbVar0 = NbVar;
A->p[NbVar] = ilAi;
A->n = NbVar;
A->m = NbCnt;
A->p[NbVar] = ilAi;

printf("NbCnt = %d\n",NbCnt);


A->nz = -1;
A->nzmax = ilAi;


seed = 0;
Csd = cs_dmperm( A, seed );

/* A faire: verifier que toutes les colonnes de Rb sont dans C3 */

printf("\nPrimal\n");
Cb = Csd->cc[1];
printf("Nombre d'elements de CBarre : %d\n",Cb);
C1 = Csd->cc[2]-Csd->cc[1];
printf("Nombre d'elements de C1 : %d\n",C1);
C2 = Csd->cc[3]-Csd->cc[2];
printf("Nombre d'elements de C2 : %d\n",C2);
C3 = Csd->cc[4]-Csd->cc[3];
printf("Nombre d'elements de C3 : %d\n",C3);

printf("\n");
R1 = Csd->rr[1];
printf("Nombre d'elements de R1 : %d\n",R1);
R2 = Csd->rr[2]-Csd->rr[1];
printf("Nombre d'elements de R2 : %d\n",R2);
R3 = Csd->rr[3]-Csd->rr[2];
printf("Nombre d'elements de R3 : %d\n",R3);
Rb = Csd->rr[4]-Csd->rr[3];
printf("Nombre d'elements de RBarre : %d\n",Rb);

Ligne = (int *) malloc( NbCnt * sizeof( int ) );
Colonne = (int *) malloc( NbVar * sizeof( int ) );


FactorisationOK = 0;
MatriceSinguliere = 1;


if ( Rb != 0 ) {
  /* Il faut verifier que toutes les colonnes de R3 font partie de C3 */
	
}

Resolu = OUI_PNE;
if ( C3 != 0 ) {

  printf("C3 %d R3 %d\n",C3,R3);
  NbVarDuProblemeReduit = C3;
	NbCntDuProblemeReduit = C3;
	IndexCC = 3;
	IndexRR = 2;

  for ( i = 0 ; i < NbCnt ; i++ ) Ligne[i] = -1;
  for ( i = 0 ; i < NbVar ; i++ ) Colonne[i] = -1;

	ccDeb = (int) Csd->cc[IndexCC];
	ccFin = (int) Csd->cc[IndexCC+1];
	rrDeb = (int) Csd->rr[IndexRR];
	rrFin = (int) Csd->rr[IndexRR+1];
	
  Resolu = NON_PNE;
  MatriceFactorisee = (MATRICE * ) PNE_DumalgeFactoriserMatrice( Pne, &MatriceAFactoriser,
                                                                 NbVarDuProblemeReduit,
																			                           NbCntDuProblemeReduit,																			 
																			                           ccDeb,
																			                           ccFin,
																			                           rrDeb, 
																			                           rrFin,																			 
                                                                 (void *) Csd,
																			                           (void *) A,  
																			                           Ligne,
																			                           Colonne,
																			                           &CodeRet, FactorisationOK, MatriceSinguliere );
																			 
  if ( CodeRet != MatriceSinguliere ) {
    /* On essaie de resoudre C3 */
		
    PNE_DumalgeResoudrePrimal( Pne, (void *) MatriceFactorisee, (void *) MatriceAFactoriser,                                  
                               NbVarDuProblemeReduit, NbCntDuProblemeReduit,
				 
																			 ccDeb,
																			 ccFin,
																			 rrDeb, 
																			 rrFin,
																			 
														           (void *) Csd,
																			 (void *) A,  	 
 	                   																																								
																			  VarNewVarOld,
																			  CntNewCntOld,  
																			  B,
																				&CodeRet,
																			  FactorisationOK,


		                            ValeurDeBorneInf,
                                ValeurDeBorneSup,
                                BorneInfConnue,
                                BorneSupConnue,
																UneVariableAEteFixee
																				
		                                 );
    Resolu = OUI_PNE;
		if ( CodeRet != FactorisationOK ) {
		  *Faisabilite = NON_PNE;
      goto FIN;
		}
		
	}
  else {
 
	}

	
}



if ( C2 != 0 && Resolu ) {
  printf("C2 %d R2 %d\n",C2,R2);
  NbVarDuProblemeReduit = C2;
	NbCntDuProblemeReduit = C2;
	IndexCC = 2;
	IndexRR = 1;

  for ( i = 0 ; i < NbCnt ; i++ ) Ligne[i] = -1;
  for ( i = 0 ; i < NbVar ; i++ ) Colonne[i] = -1;

	ccDeb = (int) Csd->cc[IndexCC];
	ccFin = (int) Csd->cc[IndexCC+1];
	rrDeb = (int) Csd->rr[IndexRR];
	rrFin = (int) Csd->rr[IndexRR+1];

	Resolu = NON_PNE;
  MatriceFactorisee = (MATRICE * ) PNE_DumalgeFactoriserMatrice( Pne, &MatriceAFactoriser,
                                                                 NbVarDuProblemeReduit,
																			                           NbCntDuProblemeReduit,																			 
																			                           ccDeb,
																			                           ccFin,
																			                           rrDeb, 
																			                           rrFin,																			 
                                                                 (void *) Csd,
																			                           (void *) A,  
																			                           Ligne,
																			                           Colonne,
																			                           &CodeRet, FactorisationOK, MatriceSinguliere );
																			 																			 	
	if ( CodeRet != MatriceSinguliere ) {

    PNE_DumalgeResoudrePrimal( Pne, (void *) MatriceFactorisee, (void *) MatriceAFactoriser,                                  
                               NbVarDuProblemeReduit, NbCntDuProblemeReduit,
				 
																			 ccDeb,
																			 ccFin,
																			 rrDeb, 
																			 rrFin,
																			 
														           (void *) Csd,
																			 (void *) A,  	 
 	                   																																								
																			  VarNewVarOld,
																			  CntNewCntOld,  
																			  B,
																				&CodeRet,
																			  FactorisationOK,



		                            ValeurDeBorneInf,
                                ValeurDeBorneSup,
                                BorneInfConnue,
                                BorneSupConnue,
                                UneVariableAEteFixee
																				
		                                 );
	

    Resolu = OUI_PNE;
		if ( CodeRet != FactorisationOK ) {
		  *Faisabilite = NON_PNE;
      goto FIN;
		}
		


		
  }
  else {
	  /*
    if ( C3 == 0 ) {
      Choix = LIGNE;
	    LigneIndeterminee = LU_Indeterminees( MatriceFactorisee, &NombreDeLignesIndeterminees, Choix, LIGNE, COLONNE );
      printf("NombreDeLignesIndeterminees %d\n",NombreDeLignesIndeterminees);
		  int i; int r;
		  NbCntSupprimees = 0;
      for ( i = 0; i < NombreDeLignesIndeterminees ; i++ , j++ ) {
		    j = LigneIndeterminee[i];
	      r = Csd->p[rrDeb+j];
			  Cnt = CntNewCntOld[r];
		    if ( Presolve->LaContrainteEstBornante[Cnt] != OUI_PNE ) {
			    CntSupprimee[Cnt] = 1;			
			    ContrainteInactive[Cnt] = OUI_PNE;
				  NbCntSupprimees++;
			  }
		  }
      printf("NbCntSupprimees %d\n",NbCntSupprimees);
	    if ( NbCntSupprimees > 0 ) goto Debut;

		}
    */
	}   
}
if ( C1 != 0 && C2 == 0 && 0 ) {
  NbVarDuProblemeReduit = C1;
	NbCntDuProblemeReduit = C1;
	IndexCC = 1;
	IndexRR = 0;
  for ( i = 0 ; i < NbCnt ; i++ ) Ligne[i] = -1;
  for ( i = 0 ; i < NbVar ; i++ ) Colonne[i] = -1;

	if ( IndexCC == 0 ) ccDeb = 0;
	else ccDeb = (int) Csd->cc[IndexCC];
	ccFin = (int) Csd->cc[IndexCC+1];
	if ( IndexRR == 0 ) rrDeb = 0;
	else rrDeb = (int) Csd->rr[IndexRR];
	rrFin = (int) Csd->rr[IndexRR+1];

	printf("resolution de la partie C1/R1     C1 %d R1 %d\n",C1,R1);
	printf("ccDeb %d  ccFin %d  rrDeb %d rrFin %d \n",ccDeb,ccFin,rrDeb,rrFin);	 
  MatriceFactorisee = (MATRICE * ) PNE_DumalgeFactoriserMatrice( Pne, &MatriceAFactoriser,
                                                                 NbVarDuProblemeReduit,
																			                           NbCntDuProblemeReduit,																			 
																			                           ccDeb,
																			                           ccFin,
																			                           rrDeb, 
																			                           rrFin,																			 
                                                                 (void *) Csd,
																			                           (void *) A,  
																			                           Ligne,
																			                           Colonne,
																			                           &CodeRet, FactorisationOK, MatriceSinguliere );
																			 																														 																			   
	if ( CodeRet == MatriceSinguliere ) {

	}

	exit(0);
}

FIN:

free( VarOldVarNew );
free( VarNewVarOld );

free( CntOldCntNew );
free( CntNewCntOld );
free( Ligne );
free( Colonne );
free ( A->p );
free ( A->i );
free( A->x );
free( A );
free( B );

cs_dfree( Csd );

return;
}

# endif

  
