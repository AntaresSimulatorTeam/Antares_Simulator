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

   FONCTION: Recherche d'une matrice carree permettant de calculer
	           des variables duales.
						 On cherche a resoudre u A1 = c1 ou A1 est la restiction
						 de A aux variables basique, u le vecteur des variables
						 duales, c1 la restriction du vecteur des couts aux variables
						 basiques.
						 On veut determiner le plus grand nombre possible de composantes
						 de u.
						 On forme la matrice B = 0 A1^t et on en fait une BTF.
						                         A1 0
						 En parcourant les blocs on resout [v u] B = [0 c1] en
						 prenant chaque bloc obtenu dans la decompsiton BTF.																 															 
                
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
  
# if DUMALGE_MENDELSON == OUI_PNE

# include "btf.h"
# include "btf_internal.h"

# include "cs.h"

# define MARGE 1.e-4
# define INUTILE 255

# define LIGNE 1
# define COLONNE 2

/*----------------------------------------------------------------------------*/
/* Attention: dans le dual on n'a pas le droit de supprimer des contraintes en
regardant les colonnes non calculables car une colonne ne correspond pas toujours
a une contrainte entiere: il n'y a pas les termes des variables bornees des 2 cotes */

void PRS_DumalgeMendelson( PRESOLVE * Presolve )   
{
int Var; int il; int ilMax; int ilAi;
int ic; int i; int j; int ccDeb; int ccFin; int rrDeb; int rrFin;
int NbIneg;  char Flag; char CalculOK;
PROBLEME_PNE * Pne; 
int * MdebTrav; int * NbTermTrav; int * NuvarTrav; double * ATrav;
int * NuCntDeNewVar; int * NuVarDeOldCnt; 
int * NuVarDeNewCnt; int * NuCntDeOldVar; int NombreDeContraintesTrav;
double * C; int NombreDeVariablesAUtiliser;
char * ContrainteInactive; int * TypeDeBorneTrav;
int Cb; int C1; int C2; int C3; int IndexRR; int IndexCC;
int R1; int R2; int R3; int Rb; int NombreDeVariablesTrav;
int * Ligne; int * Colonne; int NbCntDuProblemeReduit; int NbVarDuProblemeReduit;
int NbIt;
MATRICE * MatriceFactorisee; void * MatriceAFactoriser;
double * Lambda; char * ConnaissanceDeLambda;
int NbCnt; int NbVar; int Cnt;
char CodeRet; char FactorisationOK; char MatriceSinguliere;
cs * A; csd * Csd; csi seed; char * TypeBorne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
NombreDeContraintesTrav = Pne->NombreDeContraintesTrav;
MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
ATrav = Pne->ATrav;
ContrainteInactive = Presolve->ContrainteInactive;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;

C = (double *) malloc( NombreDeVariablesTrav * sizeof( double ) );
NuVarDeNewCnt = (int *) malloc( NombreDeVariablesTrav * sizeof( int ) ); 
NuCntDeOldVar = (int *) malloc( NombreDeVariablesTrav * sizeof(int ) );
TypeBorne = (char *) malloc( NombreDeVariablesTrav * sizeof( char ) );
for ( Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) {
  NuVarDeNewCnt[Var] = -1;
  NuCntDeOldVar[Var] = -1;
	C[Var] = Pne->LTrav[Var];
}

NbIt = 0;

for ( Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) {
  TypeBorne[Var] = INUTILE;
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE /* || Presolve->TypeDeBorneSv[Var] == VARIABLE_NON_BORNEE */ ) {
	  TypeBorne[Var] = VARIABLE_NON_BORNEE;
		continue;
  }
	/*
  if ( Pne->UmaxTrav[Var] < Presolve->UmaxSv[Var]-MARGE && Pne->UminTrav[Var] > Presolve->UminSv[Var]+MARGE ) {
	  TypeBorne[Var] = VARIABLE_NON_BORNEE;
		continue;
	}
	*/
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT /*|| Presolve->TypeDeBorneSv[Var] == VARIABLE_BORNEE_INFERIEUREMENT*/ ) {
	  TypeBorne[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		continue;
	}	
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT /*|| Presolve->TypeDeBorneSv[Var] == VARIABLE_BORNEE_SUPERIEUREMENT*/ ) {
	  TypeBorne[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
		continue;
	}
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_DES_DEUX_COTES /*|| Presolve->TypeDeBorneSv[Var] == VARIABLE_BORNEE_DES_DEUX_COTES*/ ) {
    /*
		if ( Pne->UmaxTrav[Var] < Presolve->UmaxSv[Var]-MARGE ) {    
		  continue;
	    TypeBorne[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		  continue;
		}
    if ( Pne->UminTrav[Var] > Presolve->UminSv[Var]+MARGE ) {
		  continue;
	    TypeBorne[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;
		  continue;
		}
		*/
	}			
}

/* Decompte du nombre de variables a prendre en compte : ce sera le nombre de contraintes */
NombreDeVariablesAUtiliser = 0;
NbIneg = 0;
for ( Var = 0 ; Var < NombreDeVariablesTrav ; Var++ ) {
  /* La variable est initialement bornee d'un seul ou des 2 cotes et on peut en faire une variable non bornee */
	/* Attention au cas des variables non bornees */
  if ( TypeBorne[Var] == VARIABLE_NON_BORNEE ) {
    NuVarDeNewCnt[NombreDeVariablesAUtiliser] = Var;
		NuCntDeOldVar[Var] = NombreDeVariablesAUtiliser;
		NombreDeVariablesAUtiliser++;
		continue;
  }
  if ( TypeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT || TypeBorne[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
	  NuVarDeNewCnt[NombreDeVariablesAUtiliser] = Var;
		NuCntDeOldVar[Var] = NombreDeVariablesAUtiliser;
	 	NombreDeVariablesAUtiliser++;
		NbIneg++;		
	}	
}

printf("\n\nNombre de variables a utiliser %d  NbIneg %d\n",NombreDeVariablesAUtiliser,NbIneg);

NbCnt = NombreDeVariablesAUtiliser;
printf("\n\nNombre de contraintes decoulant du nombre de variables retenues %d\n",NbCnt);

NuVarDeOldCnt = (int *) malloc( NombreDeContraintesTrav * sizeof( int ) );
NuCntDeNewVar = (int *) malloc( NombreDeContraintesTrav * sizeof( int ) );

/* Nombre de variables du probleme: c'est le nombre de contraintes dans lesquelles interviennent
   des variables a prendre en compte. Decompte du nombre de termes a allouer. */

A = (cs *) malloc( sizeof( cs ) );

Debut:


for ( Cnt = 0 ; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
  NuCntDeNewVar[Cnt] = -1;
  NuVarDeOldCnt[Cnt] = -1;
}


NbVar = 0;
A->nzmax = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
	if ( Presolve->ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU ) {
	  /* Il reste a modifier le second membre */
		continue;
	}
	Flag = 0;
	il = MdebTrav[Cnt];
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
    if ( ATrav[il] != 0.0 ) {
		  Var = NuvarTrav[il];
      if ( NuCntDeOldVar[Var] >= 0 ) {
			  A->nzmax++;
				Flag = 1;
			}
		}
    il++;
  }
	if ( Flag == 1 ) {
	  NuCntDeNewVar[NbVar] = Cnt;
		NuVarDeOldCnt[Cnt] = NbVar;
		NbVar++;	  
	}
}
printf("\n\nNombre de variables decoulant du nombre de variables retenues %d\n",NbVar);

A->m = NbCnt;
A->n = NbVar;
A->n += NbIneg;
A->nzmax += NbIneg;

A->p = (csi *) malloc( ( A->n + 1 )* sizeof( csi ) );
A->i = (csi *) malloc( A->nzmax * sizeof( csi ) );
A->x = (double *) malloc( A->nzmax * sizeof( double ) );
A->nz = -1; /* compressed-col */

/* Remplissage de la matrice */
ilAi = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
	j = NuVarDeOldCnt[Cnt];
  if ( j < 0 ) continue;
  A->p[j] = ilAi;
	il = MdebTrav[Cnt];
	ilMax = il + NbTermTrav[Cnt];
	while ( il < ilMax ) {
	  if ( ATrav[il] != 0.0 ) {
		  Var = NuvarTrav[il];
	    if ( NuCntDeOldVar[Var] >= 0 ) {
        A->i[ilAi] = NuCntDeOldVar[Var];
		    if ( A->i[ilAi] < 0 || A->i[ilAi] >= A->m ) {
				  printf("A->i %d A->m %d\n",(int) A->i[ilAi],(int) A->m);
				  printf("NuvarTrav[il] %d \n",Var);
				  printf("NuCntDeOldVar  %d \n",NuCntDeOldVar[Var]);
					
				  exit(0);
				}
			  A->x[ilAi] = ATrav[il];
        ilAi++;
			}
		}
    il++;
  }
}

/* Ajout des logicals */
j = NbVar;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( TypeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
		if ( ilAi >= A->nzmax ) { printf("ilAi %d A->nzmax %d VARIABLE_BORNEE_INFERIEUREMENT j - NbVar %d\n",ilAi,(int) A->nzmax,j-NbVar);exit(0);} 
		A->p[j] = ilAi;
    A->i[ilAi] = NuCntDeOldVar[Var];
		if ( A->i[ilAi] < 0 || A->i[ilAi] >= A->m ) exit(0); 
    A->x[ilAi] = 1;
    ilAi++;
		j++;	
	  continue;
	}
	if ( TypeBorne[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {	
		if ( ilAi >= A->nzmax ) { printf("ilAi %d A->nzmax %d VARIABLE_BORNEE_SUPERIEUREMENT j - NbVar %d\n",ilAi,(int) A->nzmax,j-NbVar);exit(0);} 
		A->p[j] = ilAi;
    A->i[ilAi] = NuCntDeOldVar[Var];
		if ( A->i[ilAi] < 0 || A->i[ilAi] >= A->m ) exit(0); 
    A->x[ilAi] = -1;
    ilAi++;
		j++;
		continue;
	}
}

if ( A->n != j ) exit(0);

A->p[A->n] = ilAi;
A->nz = -1;

printf("Probleme soumis a dmperm: %d Variables et %d contraintes\n",j,(int) A->m);

seed = 0;
Csd = cs_dmperm( A, seed );

/* A faire: verifier que toutes les colonnes de Rb sont dans C3 */

printf("\nDual\n");
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

Ligne = (int *) malloc( (int) A->m * sizeof( int ) );
Colonne = (int *) malloc( (int) A->n * sizeof( int ) );

FactorisationOK = 0;
MatriceSinguliere = 1;
	
	
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
		
  MatriceFactorisee = (MATRICE * ) PRS_DumalgeFactoriserMatrice( Presolve, &MatriceAFactoriser,
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
    /* On essaie de calculer des variables duales */
    PRS_DumalgeResoudreDual( Presolve, (void *) MatriceFactorisee, (void *) MatriceAFactoriser, 
                             NbVarDuProblemeReduit, NbCntDuProblemeReduit,
														 ccDeb, ccFin, rrDeb, rrFin,								  													
                             (void *) Csd, (void *) A,  	 
														 NuVarDeNewCnt, NuCntDeNewVar, C, &CalculOK );		

    C3 = 0;     
  }
}
if ( C2 != 0 && C3 == 0 ) {
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

  MatriceFactorisee = (MATRICE * ) PRS_DumalgeFactoriserMatrice( Presolve, &MatriceAFactoriser,
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
    /* On essaie de calculer des variables duales */
    PRS_DumalgeResoudreDual( Presolve, (void *) MatriceFactorisee, (void *) MatriceAFactoriser, 
                             NbVarDuProblemeReduit, NbCntDuProblemeReduit,
														 ccDeb, ccFin, rrDeb, rrFin,								  													
                             (void *) Csd, (void *) A,  	 
														 NuVarDeNewCnt, NuCntDeNewVar, C, &CalculOK );		
    C2 = 0;     
  }
}

if ( C1 != 0 && C2 == 0 && 0 ) {
  /* Finalement la seule chose qu'on peut tirer de la resolution de C1 serait
	   une relation entre les variables duales donc on le fait pas */
	/* Sauf si Cbarre = 0 */
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

	/* On suppose qu'on fixe a 0 les variables dans CBarre */
	printf("resolution de la partie C1/R1     C1 %d R1 %d\n",C1,R1);
	printf("ccDeb %d  ccFin %d  rrDeb %d rrFin %d \n",ccDeb,ccFin,rrDeb,rrFin);

  MatriceFactorisee = (MATRICE * ) PRS_DumalgeFactoriserMatrice( Presolve, &MatriceAFactoriser,
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



  /* Essais */
  if ( CodeRet != MatriceSinguliere ) {
    /* On essaie de calculer des variables duales */
    PRS_DumalgeResoudreDual( Presolve, (void *) MatriceFactorisee, (void *) MatriceAFactoriser, 
                             NbVarDuProblemeReduit, NbCntDuProblemeReduit,
														 ccDeb, ccFin, rrDeb, rrFin,								  													
                             (void *) Csd, (void *) A,  	 
														 NuVarDeNewCnt, NuCntDeNewVar, C, &CalculOK );		
  }


	/* Fin essais */
																																 
																			 
  if ( CodeRet != MatriceSinguliere || NbIt > 10 ) {
	  goto FIN;
	  C3 = 0;
	}
	else {
    printf("Matrice singuliere\n");
	  
	}
																			 
}

free( TypeBorne );

TestProduitsScalaires:
printf("On teste les produits scalaires \n");

double psc; int NbcTot; int NbcValide; int NbFix; double CBarre; double a;

Lambda = Presolve->Lambda;
ConnaissanceDeLambda = Presolve->ConnaissanceDeLambda;

NbFix = 0;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  /* Pas de calcul pour les variables non bornees */
  if ( TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;
	/*
  if ( TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) continue;
  if ( Pne->UmaxTrav[Var] < Presolve->UmaxSv[Var]-1.e-6 && Pne->UminTrav[Var] > Presolve->UminSv[Var]+1.e-6 ) continue;
	*/
	psc = 0.0;
	NbcTot = 0;
	NbcValide = 0;
	ic = Pne->CdebTrav[Var];
	while ( ic >= 0 ) {
	  NbcTot++;
    Cnt = Pne->NumContrainteTrav[ic];
		if ( ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU  ) {
		  NbcValide++;			
		  psc += Pne->ATrav[ic] * Lambda[Cnt];
		}
		else {
		  a = Pne->ATrav[ic];
		}
    ic = Pne->CsuiTrav[ic];
  }	
	if ( NbcTot - NbcValide == 0 ) {
	  CBarre = Pne->LTrav[Var] - psc;
	  if ( CBarre > 0.0 ) {
      
			printf("cbarre[%d] = %e variable fixee a %e  UmaxTrav = %e Cout %e\n",Var,CBarre,Pne->UminTrav[Var],Pne->UmaxTrav[Var],Pne->LTrav[Var]);
	    
			NbFix++;
		
      Pne->UTrav[Var] = Pne->UminTrav[Var];
      Pne->UmaxTrav[Var] = Pne->UminTrav[Var];  
      Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;    
      Pne->TypeDeVariableTrav[Var] = REEL;
			
		}
		else if ( CBarre < 0.0 ) {
      
			printf("cbarre[%d] = %e variable fixee a %e  UminTrav = %e Cout %e\n",Var,CBarre,Pne->UmaxTrav[Var],Pne->UminTrav[Var],Pne->LTrav[Var]);
	    
			NbFix++;
		
      Pne->UTrav[Var] = Pne->UmaxTrav[Var];
      Pne->UminTrav[Var] = Pne->UmaxTrav[Var];  
      Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;    
      Pne->TypeDeVariableTrav[Var] = REEL;
			
		}
	}
	else {
	  if ( NbcTot - NbcValide == 1 ) {
		  if ( Pne->TypeDeBorneTrav[Var] != VARIABLE_BORNEE_DES_DEUX_COTES ) {
			  printf("1- Possibilite d'affecter une borne sur une variable duale C[%d] = %e a = %e\n",Var,C[Var],a);
				if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) printf("La variable est non bornee\n");
			}
			/*
		  if ( Presolve->TypeDeBorneSv[Var] != VARIABLE_BORNEE_DES_DEUX_COTES ) {
			  printf("2- Possibilite d'affecter une borne sur une variable duale C[%d] = %e a = %e\n",Var,C[Var],a);
			}
			*/
		}
	  /*printf("Nb de valeurs manquantes %d  NbcTot %d\n",NbcTot-NbcValide,NbcTot);*/
	}
}
printf("Nombre de variables que l'on a pu fixer %d\n",NbFix);

FIN:
 
return;
}

# endif

