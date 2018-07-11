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

   FONCTION: Resolution d'un sous systeme issu de la decomposition
	           Dumalge-Mendelson
                
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

/*----------------------------------------------------------------------------*/

void PNE_DumalgeResoudrePrimal( PROBLEME_PNE * Pne,
                                void * MatriceFact,    
																void * MatriceAFactoriser,                                  
                                int NbVarDuProblemeReduit,
															 	int NbCntDuProblemeReduit,																					
 	                              int IndexCCdeb,
															 	int IndexCCfin,					   
															 	int IndexRRdeb,
															 	int IndexRRfin,																																											
                                void * Csd_in,
																void * A_in,
																int * VarNewVarOld,
															 	int * CntNewCntOld,  
																double * B,
															  char * CodeRet,
																char FactorisationOK,

		                            double * ValeurDeBorneInf,
                                double * ValeurDeBorneSup,
                                char * BorneInfConnue,
                                char * BorneSupConnue,

	                              char * UneVariableAEteFixee
																
																
		                          )
{
int Var; int YaErreur; int NbVarFix;
int ic; int icMax; double X; csi ccDeb; csi ccFin; csi rrDeb; csi rrFin;
int i; int j; int l; csd * Csd; cs * A;  

MATRICE_A_FACTORISER * Matrice; MATRICE * MatriceFactorisee;

int Cnt;

double  * SecondMembreEtSolution; int CodeRetour; int NombreMaxIterationsDeRaffinement; double ValeurMaxDesResidus;   
double * Verif;


*CodeRet = FactorisationOK;

Matrice = (MATRICE_A_FACTORISER *) MatriceAFactoriser;
MatriceFactorisee = (MATRICE *) MatriceFact;

Csd = (csd *) Csd_in;
A = (cs *) A_in;

ccDeb = (csi) IndexCCdeb;
ccFin = (csi) IndexCCfin;
rrDeb = (csi) IndexRRdeb;
rrFin = (csi) IndexRRfin;

SecondMembreEtSolution = (double *) malloc( NbCntDuProblemeReduit * sizeof( double ) );
Verif                  = (double *) malloc( NbCntDuProblemeReduit * sizeof( double ) );

for ( j = 0 , i = rrDeb ; i < rrFin ; i++ ) {
	/* Csd->p[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->p[i];
	Cnt = CntNewCntOld[l];
	if ( Cnt < 0 || Cnt >= Pne->NombreDeContraintesTrav ) {
	  printf("Bug Resolution DM Cnt = %d\n",Cnt);
		exit(0);  
	}
	SecondMembreEtSolution[j] = B[Cnt];

	/*if ( B[Cnt] != 0.0 ) printf("B[%d] = %e\n",Cnt,B[Cnt]);*/
	
	j++;
}

for ( l = 0 ; l < NbCntDuProblemeReduit ; l++ ) {
  Verif[l] = SecondMembreEtSolution[l];
}

NombreMaxIterationsDeRaffinement = 0;
ValeurMaxDesResidus = 1.e-9;

MatriceFactorisee->SauvegardeDuResultatIntermediaire = 0;
MatriceFactorisee->SecondMembreCreux = 0;

LU_LuSolv( MatriceFactorisee,
           SecondMembreEtSolution, /* Le vecteur du second membre et la solution */
           &CodeRetour,             /* Le code retour ( 0 si tout s'est bien passe ) */
	         Matrice, /* Peut etre NULL, dans ce cas on ne fait pas de raffinement */
	         NombreMaxIterationsDeRaffinement,
		       ValeurMaxDesResidus        /* En norme L infini i.e. le plus grand */
         );
				 
printf("Fin de resolution CodeRetour %d\n",CodeRetour);

for ( j = 0 ; j < NbVarDuProblemeReduit ; j++ ) {
  ic = Matrice->IndexDebutDesColonnes[j];  
  icMax = ic + Matrice->NbTermesDesColonnes[j];
	while ( ic < icMax ) {							
	  i = Matrice->IndicesDeLigne[ic];
	  Verif[i] -= Matrice->ValeurDesTermesDeLaMatrice[ic] * SecondMembreEtSolution[j];
    ic++;
	}
}

YaErreur = 0;
for ( i = 0 ; i < NbCntDuProblemeReduit ; i++ ) {
  if ( fabs(Verif[i]) > 1.e-8 ) {
	  YaErreur = 1;
	  printf("Erreur[%d] = %e\n",i,Verif[i]);
	}
}
if ( YaErreur == 0 ) printf("Pas d'erreur de resolution\n");

YaErreur = 0;
NbVarFix = 0;
for ( j = 0 , i = ccDeb ; i < ccFin ; i++ , j++) {
  X = SecondMembreEtSolution[j];
	l = Csd->q[i];
	Var =	VarNewVarOld[l];	  
	if ( Var < Pne->NombreDeVariablesTrav ) {
		if ( X < Pne->UminTrav[Var] || X > Pne->UmaxTrav[Var] ) {
	    printf("Erreur Variable %d = %e Xmin %e Xmax %e\n",Var,X,Pne->UminTrav[Var],Pne->UmaxTrav[Var]);
			YaErreur = 1;
			*CodeRet = 10;
			continue;
		}
		
    Pne->UTrav[Var] = X;
    Pne->UminTrav[Var] = X;   
    Pne->UmaxTrav[Var] = X;

		ValeurDeBorneInf[Var] = X;
		ValeurDeBorneSup[Var] = X;
    BorneInfConnue[Var] = UMIN_EGAL_UMAX; 
    BorneSupConnue[Var] = UMIN_EGAL_UMAX;

		*UneVariableAEteFixee = OUI_PNE;
		
    /*
		Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;    
    Pne->TypeDeVariableTrav[Var] = REEL;
		*/
		NbVarFix++;

		/* Passer la colonne l dans le second membre */		
	  ic = A->p[l];
	  icMax = A->p[l+1];		
	  while ( ic < icMax ) {				
			B[CntNewCntOld[A->i[ic]]] -= A->x[ic] * X;		  
		  ic++;
	  }		
		
	}
	else {
	  Cnt = Var - Pne->NombreDeVariablesTrav;
		if ( X < 0.0 ) {
	    printf("Erreur Variable d'ecart contrainte %d %e \n",Cnt,SecondMembreEtSolution[j]);
			YaErreur = 1;
		}
		/*Pne->SensContrainteTrav[Cnt] = '=';*/
	}		
}
/*if ( YaErreur == 1 ) exit(0);*/

LU_LibererMemoireLU( (MATRICE *) MatriceFactorisee );

free( Matrice->IndexDebutDesColonnes );
free( Matrice->NbTermesDesColonnes );
free( Matrice->ValeurDesTermesDeLaMatrice );
free( Matrice->IndicesDeLigne );
free( Matrice );

free( SecondMembreEtSolution );
free( Verif );

printf("Nombre de variables fixees par le primal %d\n",NbVarFix);

return;
}

# endif


