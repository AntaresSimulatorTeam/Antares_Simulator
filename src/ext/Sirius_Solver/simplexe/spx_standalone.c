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

   FONCTION: Main de la SPX pour le standalone
                
   AUTEUR: R. GONZALEZ, N. LHUILLIER

************************************************************************/

# define CPLUSPLUS
  # undef CPLUSPLUS

# include "spx_sys.h"
# include "pne_fonctions.h"

# ifdef __cplusplus
  # include "spx_definition_arguments.h"
  # include "spx_constantes_externes.h"	       						 
  # include "spx_define.h"
  extern "C"
  {
  PROBLEME_SPX * SPX_Simplexe( PROBLEME_SIMPLEXE * Probleme , PROBLEME_SPX * Spx );
  void PNE_LireJeuDeDonneesMPS(void);
  }
# else
  # include "spx_fonctions.h"
  # include "spx_constantes_externes.h"	       						 
  # include "spx_define.h"
# endif

# include "mps_define.h"
# include "mps_extern_global.h"


PROBLEME_SIMPLEXE probleme;
PROBLEME_SPX *resultat;


int main( int argc , char ** argv ) {
int i ; int j; int YaUneSolution; double Critere; int Cnt; double S; 
int il; int ilMax; int CntMx; double Smx; double EcX; int Mip; double EcMoy; 
char AfficherLesValeursDesVariables=NON_SPX; char VerifierLesContraintes=NON_SPX;
int Nbn;
FILE * FlotDeSortie; 

for ( i = 1 ; i < argc ; i++ ) {
  if ( strcmp( argv[i] , "-help" ) == 0 || strcmp( argv[i] , "-h" ) == 0 ) {
    printf("\n");
    printf("Options disponibles:\n");
    printf("  -help ou -h  Pour obtenir la liste des options disponibles\n");
    printf("  -printv      Pour afficher la valeur des variables en fin d'optimisation\n");
    printf("  -verif       Pour verifier que toutes les contraintes sont satisfaites par la solution trouvee\n");
    printf("  -all         Pour faire l'equivalent de -printv et -verif\n");
    printf("\n");
    exit(0);    
  }
  if ( strcmp( argv[i] , "-printv" ) == 0 ) {
    AfficherLesValeursDesVariables = OUI_SPX;
    continue;
  }
  if ( strcmp( argv[i] , "-verif" ) == 0 ) {
    VerifierLesContraintes = OUI_SPX;
    continue;
  }
  if ( strcmp( argv[i] , "-all" ) == 0 ) {
    AfficherLesValeursDesVariables = OUI_SPX;
    VerifierLesContraintes = OUI_SPX;
    continue;
  }
  printf("Option %s inconnue, taper l'option -help pour connaitre la liste des options disponibles\n",argv[i]);
  exit(0);
}

FlotDeSortie = fopen( "RESULTAT.csv", "w" ); 
if( FlotDeSortie == NULL ) {
  printf("Erreur ouverture du fichier de resultats \n");
  exit(0);
}

/* Lecture du jeu de donnees */
/*
printf("Debut de la lecture des donnees \n"); fflush(stdout);
*/
PNE_LireJeuDeDonneesMPS();

/*
printf("Fin de la lecture des donnees \n"); fflush(stdout);
*/
/* Mip: indicateur positionne a OUI_MPS si on veut optimiser en variables entieres 
        et positionne a NON_MPS si on veut juste faire un simplexe */

Mip = OUI_MPS;        	  			
if ( Mip == NON_MPS ) {    
  for ( j = 0 ; j < Mps.NbVar ; j++ ) Mps.TypeDeVariable[j] = REEL;
}
  
/*
printf("**********  Attention on fait une maximisation i.e Cout = -Cout ******************\n");
for ( j = 0 ; j < Mps.NbVar ; j++ ) Mps.L[j] = -Mps.L[j];
*/

/*
printf("Verification des bornes des variables\n");
for ( j = 0 ; j < Mps.NbVar ; j++ ) { 
  if ( Mps.Umin[j] > Mps.Umax[j] ) {printf("Erreur Umin %lf Umax %lf Var %d\n",Mps.Umin[j],Mps.Umax[j],j); exit(0);}
}
*/
/*
printf("Appel du solveur de PNE \n"); fflush(stdout);
*/

/* Resolution */

for ( j = 0 ; j < 1 ; j++ ) { /* Pour tester les fuites memoire on enchaine les resolutions du meme probleme */
	probleme.TypeDePricing                         = PRICING_STEEPEST_EDGE;//PRICING_STEEPEST_EDGE PRICING_DANTZIG()
	probleme.FaireDuScaling                        = OUI_SPX; // Vaut OUI_SPX ou NON_SPX
	probleme.StrategieAntiDegenerescence           = AGRESSIF; // Vaut AGRESSIF ou PEU_AGRESSIF
	probleme.NombreMaxDIterations                  = -1; // si i < 0 , alors le simplexe prendre sa valeur par defaut
	probleme.DureeMaxDuCalcul                      = -1; // si i < 0 , alors le simplexe prendre sa valeur par defaut
	probleme.CoutLineaire                          = Mps.L;
	probleme.X                                     = Mps.U;
	probleme.Xmin                                  = Mps.Umin;
	probleme.Xmax                                  = Mps.Umax;
	probleme.NombreDeVariables                     = Mps.NbVar;
	probleme.TypeDeVariable                        = Mps.TypeDeBorneDeLaVariable;
	probleme.NombreDeContraintes                   = Mps.NbCnt;
	probleme.IndicesDebutDeLigne                   = Mps.Mdeb;
	probleme.NombreDeTermesDesLignes               = Mps.NbTerm;
	probleme.IndicesColonnes                       = Mps.Nuvar;
	probleme.CoefficientsDeLaMatriceDesContraintes = Mps.A;
	probleme.Sens                                  = Mps.SensDeLaContrainte;
	probleme.SecondMembre                          = Mps.B; 
	probleme.CoutsMarginauxDesContraintes          = Mps.VariablesDualesDesContraintes;
	probleme.ChoixDeLAlgorithme                    = SPX_DUAL;

	probleme.LibererMemoireALaFin                  = OUI_SPX;
	probleme.AffichageDesTraces                    = OUI_SPX;
	probleme.CoutMax                               = -1;
	probleme.UtiliserCoutMax                       = NON_SPX;

	probleme.Contexte		                       = SIMPLEXE_SEUL;
	probleme.BaseDeDepartFournie	               = NON_SPX ;

	//Instantiation du résultat
	probleme.ComplementDeLaBase                    = (int*) malloc( Mps.NbCnt * sizeof(int)); 
	probleme.PositionDeLaVariable                  = (int*) malloc( Mps.NbVar * sizeof(int));
	probleme.CoutsReduits                          = (double*) malloc( Mps.NbVar * sizeof(double));

	// Appel du simplexe
	SPX_Simplexe(&probleme, NULL);

	YaUneSolution = probleme.ExistenceDUneSolution;

	/*
	for ( j = 0 ; j < Mps.NbVar ; j++ ){
	if ( Mps.TypeDeVariable[j] == ENTIER ) Mps.L[j] = 0.0;
	}
	*/


	Nbn = 0;
	printf("Dual variables count %d\n",Mps.NbCnt);
	for ( Cnt = 0 ; Cnt < Mps.NbCnt ; Cnt++ ) {
		goto ABS;
		printf("Sense[%d] %c B = %e dual variable = %e\n",Cnt,Mps.SensDeLaContrainte[Cnt],Mps.B[Cnt],
			Mps.VariablesDualesDesContraintes[Cnt]);
ABS:
		if ( fabs( Mps.VariablesDualesDesContraintes[Cnt] ) < 1.e-8 ) Nbn++;
	}
	printf("Zero dual variables count %d over %d\n",Nbn,Mps.NbCnt);

	/* Resultats */

	if ( YaUneSolution == NON_SPX ) printf("No solution found\n"); 
	if ( YaUneSolution == SPX_ERREUR_INTERNE ) printf("Internal error\n"); 
	if ( YaUneSolution == SPX_MATRICE_DE_BASE_SINGULIERE ) printf("Problem is infeasible\n"); 
	if ( YaUneSolution == OUI_SPX) { 

		Critere = 0.;
		for ( i = 0 ; i < Mps.NbVar ; i++ ) {
			Critere+= Mps.L[i] * Mps.U[i];
			fprintf( FlotDeSortie , "%s;%e\n" , Mps.LabelDeLaVariable[i], Mps.U[i] ); 
			if ( AfficherLesValeursDesVariables == OUI_SPX ) {
				printf("Variable number %d name %s value %lf ",i, Mps.LabelDeLaVariable[i], Mps.U[i]);
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_FIXE ) printf(" FIXED variable\n");
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
					if ( Mps.TypeDeVariable[i] == ENTIER ) printf(" min %lf max %lf binary variable\n",Mps.Umin[i], Mps.Umax[i]);
					else printf(" min %lf max %lf\n",Mps.Umin[i], Mps.Umax[i]);
				}
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_INFERIEUREMENT ) 
					printf(" min %lf max +INFINI\n",Mps.Umin[i]);
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) 
					printf(" min -INFINI max %lf\n",Mps.Umax[i]);
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_NON_BORNEE ) 
					printf(" min -INFINI max +INFINI\n");
			}
		}

		printf("\n********** Optimal solution found, objective %e **********\n\n",Critere);

		if ( VerifierLesContraintes == OUI_SPX ) {

			for ( i = 0 ; i < Mps.NbVar ; i++ ) {
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
					if ( Mps.U[i] < Mps.Umin[i] - 1.e-6 ) {
						printf("Variable number %d name %s violated lower bound: lo= %e value= %e violation= %e\n",
							i, Mps.LabelDeLaVariable[i],Mps.Umin[i],Mps.U[i],Mps.Umin[i]-Mps.U[i]);
					}
					else if ( Mps.U[i] > Mps.Umax[i] + 1.e-6 ) {
						printf("Variable number %d name %s violated upper bound: up= %e value= %e violation= %e\n",
							i, Mps.LabelDeLaVariable[i],Mps.Umax[i],Mps.U[i],Mps.U[i]-Mps.Umax[i]);
					}
					continue;
				}
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_INFERIEUREMENT ) {
					if ( Mps.U[i] < Mps.Umin[i] - 1.e-6 ) {
						printf("Variable number %d name %s violated lower bound: lo= %e value= %e violation= %e\n",
							i, Mps.LabelDeLaVariable[i],Mps.Umin[i],Mps.U[i],Mps.Umin[i]-Mps.U[i]);
					}
					continue;
				}
				if ( Mps.TypeDeBorneDeLaVariable[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
					if ( Mps.U[i] > Mps.Umax[i] + 1.e-6 ) {
						printf("Variable number %d name %s violated upper bound: up= %e value= %e violation= %e\n",
							i, Mps.LabelDeLaVariable[i],Mps.Umax[i],Mps.U[i],Mps.U[i]-Mps.Umax[i]);
					}
					continue;
				}    
			}	

			Smx   = -1.;
			EcX   = 1.e-6;
			CntMx = -1;
			EcMoy = 0.;
			for ( Cnt = 0 ; Cnt < Mps.NbCnt ; Cnt++ ) {
				S = 0.;
				il    = Mps.Mdeb[Cnt];
				ilMax = il + Mps.NbTerm[Cnt];
				while ( il < ilMax ) {
					i = Mps.Nuvar[il];
					S+= Mps.A[il] * Mps.U[i];
					il++;
				}
				if ( Mps.SensDeLaContrainte[Cnt] == '=' ) {
					EcMoy+= fabs( S - Mps.B[Cnt] );
					if ( fabs( S - Mps.B[Cnt] ) > EcX ) {
						EcX   = fabs( S - Mps.B[Cnt] );
						Smx   = S;
						CntMx = Cnt;
					}
				}
				if ( Mps.SensDeLaContrainte[Cnt] == '>' && S < Mps.B[Cnt] ) {
					EcMoy+= fabs( S - Mps.B[Cnt] );      
					if ( fabs( S - Mps.B[Cnt] ) > EcX ) {
						EcX   = fabs( S - Mps.B[Cnt] );
						Smx   = S;
						CntMx = Cnt;
					}
				}
				if ( Mps.SensDeLaContrainte[Cnt] == '<' && S > Mps.B[Cnt] ) {
					EcMoy+= fabs( S - Mps.B[Cnt] );      
					if ( fabs( S - Mps.B[Cnt] ) > EcX ) {
						EcX   = fabs( S - Mps.B[Cnt] );
						Smx   = S;
						CntMx = Cnt;
					}
				}
				/*printf("cnt %ld S %e B %e\n",Cnt,S,Mps.B[Cnt]);*/		
			}
			if ( CntMx >= 0 ) {
				printf("Higher violation:\n");
				if ( Mps.SensDeLaContrainte[CntMx] == '=' ) printf("Cnt num %d - %s -- type = ",CntMx,Mps.LabelDeLaContrainte[CntMx]);
				if ( Mps.SensDeLaContrainte[CntMx] == '<' ) printf("Cnt num %d - %s -- type < ",CntMx,Mps.LabelDeLaContrainte[CntMx]);
				if ( Mps.SensDeLaContrainte[CntMx] == '>' ) printf("Cnt num %d - %s -- type > ",CntMx,Mps.LabelDeLaContrainte[CntMx]);
				printf(" B %e computed %e violation %e\n",Mps.B[CntMx],Smx,fabs(Smx-Mps.B[CntMx]));
			}
			EcMoy/= Mps.NbCnt;
			printf("Violations average value: %e\n",EcMoy);

		}

	}

	printf("\n");
}
exit(0);
}
