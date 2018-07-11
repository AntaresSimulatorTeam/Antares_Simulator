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

/*----------------------------------------------------------------------------*/

void * PRS_DumalgeMendelsonResoudreSystemeTranspose( PRESOLVE * Presolve,
                                          int NbVarDuProblemeReduit,
																					int NbCntDuProblemeReduit,
																				 	int IndexCCdeb,
																				 	int IndexCCfin,					
																					int IndexRRdeb,
																					int IndexRRfin,								  													
                                          void * Csd_in,
																					void * A_in,
																					int * Ligne,
																					int * Colonne,
																					int * NuVarDeNewCnt,
																					int * NuCntDeNewVar,
																					double * C,
																				  char * CodeRet,
																					char FactorisationOK,
																					char MatriceSinguliere																					
		)
{
int Var; int il; int ilMax; int ilAi; int YaErreur; 
int ic; int icMax; int icNew; double X;
int i; int j; int l; csd * Csd; cs * A; csi ccDeb; csi ccFin; csi rrDeb; csi rrFin;

PROBLEME_PNE * Pne; long NombreDeVariables; 

MATRICE_A_FACTORISER * Matrice; MATRICE * MatriceFactorisee;

int Cnt;

int * MdebTrav; int * NbTermTrav; int * NuvarTrav; double * ATrav; 
int c;

*CodeRet = FactorisationOK;

Csd = (csd *) Csd_in;
A = (cs *) A_in;   

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

ccDeb = (csi) IndexCCdeb;
ccFin = (csi) IndexCCfin;
rrDeb = (csi) IndexRRdeb;  
rrFin = (csi) IndexRRfin;

for ( j = 0 , i = ccDeb ; i < ccFin ; i++ ) {
	/* Csd->q[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->q[i];
  Colonne[l] = j;
	j++;
}

for ( j = 0 , i = rrDeb ; i < rrFin ; i++ ) {
	/* Csd->p[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->p[i];
  Ligne[l] = j;
	j++;  
}

Matrice = (MATRICE_A_FACTORISER *) malloc( sizeof( MATRICE_A_FACTORISER ) );
Matrice->NombreDeColonnes = NbVarDuProblemeReduit;
Matrice->IndexDebutDesColonnes = (int *) malloc( NbVarDuProblemeReduit * sizeof( int ) );
Matrice->NbTermesDesColonnes   = (int *) malloc( NbVarDuProblemeReduit * sizeof( int ) );
Matrice->ValeurDesTermesDeLaMatrice = (double *) malloc( A->nzmax * sizeof( double ) );
Matrice->IndicesDeLigne             = (int *)    malloc( A->nzmax * sizeof( int    ) );

icNew = 0;
for ( j = 0 , i = ccDeb ; i < ccFin ; i++ ) {
  Matrice->IndexDebutDesColonnes[j] = icNew;
  Matrice->NbTermesDesColonnes  [j] = 0;
	c = Csd->q[i];
	ic = A->p[c];
	icMax = A->p[c+1];		
	while ( ic < icMax ) {
		if ( Ligne[A->i[ic]] >= 0 ) {					
      Matrice->ValeurDesTermesDeLaMatrice[icNew] = A->x[ic];   
      Matrice->IndicesDeLigne[icNew] = Ligne[A->i[ic]];
      Matrice->NbTermesDesColonnes[j]++;
      icNew++;
		}
		ic++;
	}
	j++;
}
		 
printf("DEBUT DE FACTORISATION\n");
Matrice->ContexteDeLaFactorisation  = LU_SIMPLEXE; 
Matrice->UtiliserLesSuperLignes     = NON_LU;
Matrice->FaireScalingDeLaMatrice    = NON_LU; 

Matrice->UtiliserLesValeursDePivotNulParDefaut = NON_LU;
Matrice->ValeurDuPivotMin        = 1.e-8;     
Matrice->ValeurDuPivotMinExtreme = 1.e-8;

Matrice->SeuilPivotMarkowitzParDefaut = OUI_LU;
Matrice->FaireDuPivotageDiagonal      = NON_LU;
Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU;
Matrice->LaMatriceEstSymetrique       = NON_LU;

MatriceFactorisee = LU_Factorisation( Matrice );
if ( MatriceFactorisee == NULL ) {
  printf("PB factorisation  \n");
  exit(0);
}

if ( Matrice->ProblemeDeFactorisation == MATRICE_SINGULIERE ) {
  printf("Matrice singuliere \n");	
	 *CodeRet = MatriceSinguliere;
  return( (void *) MatriceFactorisee );
}
  
printf("FIN DE FACTORISATION\n");
return;

double  * SecondMembreEtSolution; int CodeRetour; int NombreMaxIterationsDeRaffinement; double ValeurMaxDesResidus;
double * Verif;

SecondMembreEtSolution = (double *) malloc( NbCntDuProblemeReduit * sizeof( double ) );
Verif                  = (double *) malloc( NbCntDuProblemeReduit * sizeof( double ) );

for ( j = 0 , i = rrDeb ; i < rrFin ; i++ , j++) {
	/* Csd->p[i]: dans la numerotation du probleme soumis a dmperm */
	l = Csd->p[i];
	Var = NuVarDeNewCnt[l];
	if ( Var < 0 || Var >= Pne->NombreDeVariablesTrav ) {
	  printf("Bug Resolution DM Var = %d\n",Var);
		exit(0);  
	}
	SecondMembreEtSolution[j] = C[Var];

	if ( C[Var] != 0.0 ) printf("C[%d] = %e\n",Var,C[Var]);
	
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

int Nb0;
Nb0 = 0;
for ( j = 0 , i = ccDeb ; i < ccFin ; i++ , j++) {
  X = SecondMembreEtSolution[j];
	l = Csd->q[i];
	Cnt =	NuCntDeNewVar[l];	
	if ( Cnt < Pne->NombreDeContraintesTrav ) {
		/*
		if ( X < Pne->UminTrav[Var] || X > Pne->UmaxTrav[Var] ) {
	    printf("Erreur Variable %d = %e Xmin %e Xmax %e\n",Var,X,Pne->UminTrav[Var],Pne->UmaxTrav[Var]);
			YaErreur = 1;
		}
		*/
	 /*	printf("u %e \n",X); */
	  
	  Presolve->Lambda[Cnt] = X;
		Presolve->LambdaEstConnu[Cnt] = OUI_PNE;
		if ( Pne->SensContrainteTrav[Cnt] == '<' ) {
      if ( Presolve->Lambda[Cnt] == 0.0 ) {
			  Nb0++;
			  printf("Contrainte d'inegalite %d : variable duale = %e\n",Cnt,Presolve->Lambda[Cnt]);
        if ( Presolve->LaContrainteEstBornante[Cnt] != OUI_PNE || 1 ) {
          Presolve->ContrainteInactive[Cnt] = OUI_PNE;
			    printf("Desactivation de la contrainte %d\n",Cnt);
				}			  
			}
		}
		else {
      if ( Presolve->Lambda[Cnt] == 0.0 ) {
			  Nb0++;
				/*
			  printf("Contrainte d'egalite %d : variable duale = %e\n",Cnt,Presolve->Lambda[Cnt]);
        if ( Presolve->LaContrainteEstBornante[Cnt] != OUI_PNE || 1 ) {
          Presolve->ContrainteInactive[Cnt] = OUI_PNE;
			    printf("Desactivation de la contrainte %d\n",Cnt);
				}
				*/
      }
		}
		

		/* Passer la colonne l dans le second membre */		
	  ic = A->p[l];
	  icMax = A->p[l+1];		
	  while ( ic < icMax ) {				
			C[NuVarDeNewCnt[A->i[ic]]] -= A->x[ic] * X;		  
		  ic++;
	  }		
		
	}

}

printf("Nombre de variables duales nulles %d\n",Nb0);

if ( YaErreur == 1 ) exit(0);

LU_LibererMemoireLU( (MATRICE *) MatriceFactorisee );

free( Matrice->IndexDebutDesColonnes );
free( Matrice->NbTermesDesColonnes );
free( Matrice->ValeurDesTermesDeLaMatrice );
free( Matrice->IndicesDeLigne );
free( Matrice );

free( SecondMembreEtSolution );
free( Verif );

return;
}

# endif


