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

   FONCTION: Routine principale du point interieur 
 
   Optimisation d'un critere quadratique ou lineaire sous contraintes
   lineaires.
             
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/
/*                   Routine d'entree du point interieur                      */
void PI_QuaminCalculs( PROBLEME_POINT_INTERIEUR * Probleme , PROBLEME_PI * Pi )
{
/* Le probleme d'entree */
int NombreDeVariables_E  ; double * Q_E   ; double * L_E           ;
double * Umin_E           ; double * Umax_E; int * TypeDeVariable_E; char * VariableBinaire_E; double * U_E;
int NombreDeContraintes_E; double * A_E   ; int * Mdeb_E          ; int * Nbter_E;
int * Nuvar_E            ; double * B_E   ; char * Sens_E          ;	
double  ToleranceAdmissibilite_E  ; int  ChoixToleranceAdmissibiliteParDefaut  ; 
double  ToleranceStationnarite_E  ; int  ChoixToleranceStationnariteParDefaut  ; 
double  ToleranceComplementarite_E; int  ChoixToleranceComplementariteParDefaut;

int * Convergence; /* Indicateur de convergence en sortie pas utilise */
double * VariablesDualesDeContraintes; double * S1_E; double * S2_E; 
int Preconditionnement; 
/*                     */

int TracesALaFin      ; int ImprimerLeProbleme; int ImprimerLesIterationsSiDivergence;

/*----------------------------------------------------------------------------------------------------*/  

NombreDeVariables_E = Probleme->NombreDeVariables;
Q_E                 = Probleme->CoutQuadratique;
L_E                 = Probleme->CoutLineaire;

Umin_E = Probleme->Xmin;
Umax_E = Probleme->Xmax;
TypeDeVariable_E  = Probleme->TypeDeVariable;
VariableBinaire_E = Probleme->VariableBinaire;
U_E    = Probleme->X;

NombreDeContraintes_E = Probleme->NombreDeContraintes;
A_E                   = Probleme->CoefficientsDeLaMatriceDesContraintes;
Mdeb_E                = Probleme->IndicesDebutDeLigne;
Nbter_E               = Probleme->NombreDeTermesDesLignes;
Nuvar_E               = Probleme->IndicesColonnes;
B_E                   = Probleme->SecondMembre;
Sens_E                = Probleme->Sens;

ToleranceAdmissibilite_E               = Probleme->ToleranceDAdmissibilite;
ChoixToleranceAdmissibiliteParDefaut   = Probleme->UtiliserLaToleranceDAdmissibiliteParDefaut; 
ToleranceStationnarite_E               = Probleme->ToleranceDeStationnarite;
ChoixToleranceStationnariteParDefaut   = Probleme->UtiliserLaToleranceDeStationnariteParDefaut; 
ToleranceComplementarite_E             = Probleme->ToleranceDeComplementarite;
ChoixToleranceComplementariteParDefaut = Probleme->UtiliserLaToleranceDeComplementariteParDefaut;
VariablesDualesDeContraintes           = Probleme->CoutsMarginauxDesContraintes;

/*----------------------------------------------------------------------------------------------------*/  

Pi->YaUneSolution = OUI_PI;

ImprimerLesIterationsSiDivergence = OUI_PI;  /* a faire */
ImprimerLeProbleme                = NON_PI;  /* a faire */
TracesALaFin                      = NON_PI;  /* a faire */

Pi->YaUneSolution = OUI_PI;

PI_AllocProbleme( Pi, NombreDeVariables_E, NombreDeContraintes_E, Nbter_E, Sens_E, VariableBinaire_E, TypeDeVariable_E );

PI_MdEqua( Pi, NombreDeVariables_E, Q_E, L_E, Umin_E , Umax_E , TypeDeVariable_E, VariableBinaire_E, U_E,
           NombreDeContraintes_E, Mdeb_E, Nbter_E, Nuvar_E, A_E, B_E, Sens_E );

if ( Pi->NombreDeVariables <= 0 ) {
  Probleme->ExistenceDUneSolution = Pi->YaUneSolution;
	goto Fin;
}

/* PI_SplitContraintes( Pi ); */

/* Test */

/* PI_SplitColonnes( Pi ); */ 

/* Fin test */

PI_Qinit( Pi,
          ToleranceAdmissibilite_E   , ChoixToleranceAdmissibiliteParDefaut, 
					ToleranceStationnarite_E   , ChoixToleranceStationnariteParDefaut, 
          ToleranceComplementarite_E , ChoixToleranceComplementariteParDefaut );

PI_InitATransposee( Pi , COMPACT );

/*PI_Scaling( Pi );*/ /*fait ailleurs */

PI_InitXS( Pi );

/*PI_AllocMatrice( Pi );*/ /* fait dans PI_Cremat */ 

Pi->YaUneSolution = NON_PI; /* L'indicateur est remis a jour dans PI_Joptimise */

PI_Joptimise( Pi );

Probleme->ExistenceDUneSolution = Pi->YaUneSolution;

PI_UnScaling( Pi ); 

/* a revoir if ( FaireMiseALEchelle == OUI_PI ) PI_InverseDeMiseALEchelle( Pi );*/

/* Restitution de la solution */
S1_E = NULL; /* Car sinon visual studio croit qu'on utilise ces variables sans les avoir initialisees */
S2_E = NULL; /* c'est idiot de sa part mais bon, c'est toujours le moins con qui cède en premier */
PI_RestitutionDesResultats( Pi,
                            NombreDeVariables_E   , U_E , S1_E , S2_E , 
                            NombreDeContraintes_E , VariablesDualesDeContraintes );

 if ( MPCC != OUI_PI ) goto Fin;
{
int Var; double C; double pi; double Cp;int i; double SLambda;
C = 0.0;
Cp = -1.0;
pi = 10.;
i = -1;
SLambda = 0.0;
for ( Var = 0 ; Var < NombreDeVariables_E ; Var++ ) {
  C+= Q_E[Var] * U_E[Var] * U_E[Var];
  C+= L_E[Var] * U_E[Var];
  if ( VariableBinaire_E[Var] == OUI_PI ) {
    /*Cp+= pi * U_E[Var] * ( 1. - U_E[Var] );*/
    SLambda+= Pi->Lambda[Pi->NumeroDeLaContrainteDeComplementarite[Pi->CorrespondanceVarEntreeVarPi[Var]]];
    if ( U_E[Var] < 1. - U_E[Var] ) {
      if ( U_E[Var] > Cp ) {
        i = Var;
        Cp = U_E[Var];
      }
    }
    else {
      if ( 1. - U_E[Var] > Cp ) {
        i = Var;
        Cp = 1. - U_E[Var];
      }
    }
    printf("Variable binaire %d valeur %e Lambda %e\n",Var,U_E[Var],
    Pi->Lambda[Pi->NumeroDeLaContrainteDeComplementarite[Pi->CorrespondanceVarEntreeVarPi[Var]]]);  
  }
}
printf("Cout de la solution %e\n",C);  
printf("Plus grande non integrite %e variable %d \n",Cp,i);
printf("Umin %e U %e Umax %e\n",Umin_E[i],U_E[i],Umax_E[i]);
printf("Valeur de Xi: %e\n",Pi->U[Pi->NumeroDeLaVariableXi]);
printf("SLambda = %e Rho = %e\n",SLambda,Pi->L[Pi->NumeroDeLaVariableXi]);
exit(0);
}

/* liberer le probleme et la matrice aussi */
Fin:

PI_LibereProbleme( Pi );
PI_LibereMatrice( Pi );

# ifdef PI_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  MEM_Quit( Pi->Tas );
# else
  free( Pi );
# endif

return;
}

/*----------------------------------------------------------------------------*/
/*                          Routine d'optimisation                            */

void PI_Joptimise( PROBLEME_PI * Pi ) 
{
int NombreDeTestsDArretNonSatisfaits;
int IterationComplementariteOK; 
int IterationAdmissibiliteOK; 
int IterationStationnariteOK; 
	 
Pi->ComplementariteAtteinte = NON_PI;
Pi->RealisabiliteAtteinte   = NON_PI;
Pi->StationnariteAtteinte   = NON_PI;	 
Pi->NumeroDIteration = 1;
Pi->TypeDIteration   = AFFINE;

while ( Pi->NumeroDIteration <= NOMBRE_MAX_DITERATION ) {

  Pi->Gamma = 0.9995; /* 0.9995 */

  /* Pour ne pas s'approcher trop rapidement des bornes afin de ne pas detruite trop rapidement
	   le conditionnement de la matrice */
  if ( Pi->NumeroDIteration <= 2 ) Pi->Gamma = 0.9;
	
  if( Pi->TypeDIteration == CENTRALISATION ) {
    PI_Calmuk( Pi );   /* Calcul de muk */
    Pi->MukIteration[Pi->NumeroDIteration-1] = Pi->Muk; /* Sauvegarde pour les les traces */
    Pi->MukPrecedent = Pi->Muk;
    #if VERBOSE_PI
      printf("\n*** Iteration numero %d de type CENTRALISATION, valeur utilisee pour Muk %e\n",Pi->NumeroDIteration,Pi->Muk);
    #endif
    PI_Calcent( Pi ); /* Correction pour la centralisation */ 
  }
  else { /* Cas d'une iteration affine */
    #if VERBOSE_PI
      printf("\n*** Iteration numero %d de type AFFINE\n",Pi->NumeroDIteration);
    #endif
      
    memset( (char *) Pi->SecondMembre , 0 , ( Pi->NombreDeVariables + Pi->NombreDeContraintes ) * sizeof( double ) );  /* Nettoyage */	

    NombreDeTestsDArretNonSatisfaits = 0;

    Pi->ArretOpt = OUI_PI; 
    Pi->Traces   = NON_PI;
    PI_Caldel( Pi ); /* Calcul de l'ecart de complementarite et test par rapport au seuil d'optimalite */
    if ( Pi->ArretOpt == NON_PI ) NombreDeTestsDArretNonSatisfaits++;
    else { Pi->ComplementariteAtteinte = OUI_PI; IterationComplementariteOK = Pi->NumeroDIteration; }   
    
    Pi->Muk = 0.; /* muk = 0 pour une iteration affine */		
		
    Pi->ArretCnt = OUI_PI; 
    Pi->ArretSta = OUI_PI; 
    Pi->Traces   = NON_PI;
    PI_Calstaf( Pi ); /* Test d'admissibilite et de stationnarite + calcul du second membre */
    if ( Pi->ArretCnt == NON_PI ) NombreDeTestsDArretNonSatisfaits++;
    else { Pi->RealisabiliteAtteinte = OUI_PI; IterationAdmissibiliteOK = Pi->NumeroDIteration; }       
    if ( Pi->ArretSta == NON_PI ) NombreDeTestsDArretNonSatisfaits++;
    else { Pi->StationnariteAtteinte = OUI_PI; IterationStationnariteOK = Pi->NumeroDIteration; }
    
    if( NombreDeTestsDArretNonSatisfaits == 0 ) {
      if ( MPCC == OUI_PI && Pi->NombreDeVariablesBinaires > 0 && 0) {
        printf("Valeur de Xi:%e et de Rho: %e\n",Pi->U[Pi->NumeroDeLaVariableXi], Pi->Rho);
        if ( Pi->U[Pi->NumeroDeLaVariableXi] > 1.e-6 ) {
          {
            int Var; double SLambda; double X; double Ec; double RhoSv; int Cnt; int il; int ilMax;
            SLambda = 0.0;
            for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
              if ( Pi->VariableBinaire[Var] == OUI_PI ) {
                SLambda+= Pi->Lambda[Pi->NumeroDeLaContrainteDeComplementarite[Var]];
								Cnt = Pi->NumeroDeLaContrainteDeComplementarite[Var];
								printf("Cnt de la variable binaire %d\n",Cnt);
								printf(" U(Umax-U)= %e ",Pi->U[Var] * ( Pi->Umax[Var] - Pi->U[Var] ) );
                il = Pi->Mdeb[Cnt];
                ilMax = il + Pi->NbTerm[Cnt];
								il++;
                while ( il < ilMax ) {
                  printf("U = %e ",Pi->U[Pi->Indcol[il]]);
                  il++;
                }
								printf("\n");
              }
						}
            printf("SLambda = %e Rho = %e\n",SLambda,Pi->L[Pi->NumeroDeLaVariableXi]);
						RhoSv = Pi->Rho;
            Pi->Rho*= 10.;
						Pi->L[Pi->NumeroDeLaVariableXi] = Pi->Rho;
            NombreDeTestsDArretNonSatisfaits = 3;
						printf("Nouvelle valeur de Rho: %e\n",Pi->Rho);
            for ( Var = 0 ; Var < Pi->NombreDeVariables ; Var++ ) {
              if ( Pi->VariableBinaire[Var] == OUI_PI ) {
								Pi->Lambda[Pi->NumeroDeLaContrainteDeComplementarite[Var]] = Pi->Rho / Pi->NombreDeVariablesBinaires;		
              }
							Ec = 1.e-3;
							if ( Pi->TypeDeVariable[Var] == BORNEE ) {
								X = Ec * ( Pi->Umax[Var] - Pi->Umin[Var] );
								if ( Pi->U[Var] < Pi->Umin[Var] + X ) Pi->U[Var] = Pi->Umin[Var] + X;
								else if ( Pi->U[Var] > Pi->Umax[Var] - X ) Pi->U[Var] = Pi->Umax[Var] - X;
								if ( Pi->S1[Var] < Ec ) Pi->S1[Var] = Ec;
								if ( Pi->S2[Var] < Ec ) Pi->S2[Var] = Ec;
							}
							if ( Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
								X = Ec;
								if ( Pi->U[Var] < Pi->Umin[Var] + X ) Pi->U[Var] = Pi->Umin[Var] + X;
								if ( Pi->S1[Var] < Ec ) Pi->S1[Var] = Ec;
              }
							if ( Pi->TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) {
								X = Ec;
								if ( Pi->U[Var] > Pi->Umax[Var] - X ) Pi->U[Var] = Pi->Umax[Var] - X;	      
								if ( Pi->S2[Var] < Ec ) Pi->S2[Var] = Ec;
              }	      
						}
						/*Pi->U[Pi->NumeroDeLaVariableXi]+= 1000.;*/
					}
				}      
      }
    }

    if( NombreDeTestsDArretNonSatisfaits == 0 && Pi->NumeroDIteration > 1 ) {
      /* On a converge mais on impose un test complementaire: 
         il faut en plus que muk soit suffisamment petit*/
      if ( Pi->MukPrecedent < 1.e-3 ) {
        Pi->YaUneSolution = OUI_PI;
        /*printf("Point interieur, convergence atteinte, iteration %d \n", Pi->NumeroDIteration);*/
        break; 
      }
    }
  }
  /*   */
  if ( Pi->NumeroDIteration == 1 && Pi->TypeDIteration == AFFINE ) {
    PI_Cremat( Pi ); /* A la premiere iteration, elimination ordonnee et factorisation de la matrice du point interieur */
  }
  else {
    if ( Pi->TypeDIteration == AFFINE ) {
      PI_Crebis( Pi ); 
    }  
  }
  /*   */
  PI_Resolution( Pi );  /* Resolution de systeme */
  if ( Pi->TypeDIteration == AFFINE ) Pi->TypeDIteration = CENTRALISATION; 	 
  else {
    /* En fin d'iteration de centralisation, mise a jour des variables */
    PI_Incrementation( Pi );
    Pi->NumeroDIteration++; 
    Pi->TypeDIteration = AFFINE;
  }
}

#if VERBOSE_PI                                 
  {int i; double copt;
  for(copt=0,i=0;i<Pi->NombreDeVariables;i++) {
    copt+=Pi->L[i]*Pi->U[i];
  }
  printf("Fin du point interieur a l iteration %d Cout optimal (valeur interne) %e \n",Pi->NumeroDIteration,copt);
}
#endif

return;
}




