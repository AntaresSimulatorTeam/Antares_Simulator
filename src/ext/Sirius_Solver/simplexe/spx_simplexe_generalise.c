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

   FONCTION: Programmation lineaire generalisee (pour Metrix)
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"  
# include "spx_define.h"

# define TRACES
# undef TRACES

# define ZERO_CR  SEUIL_ADMISSIBILITE_DUALE_1

# define COEFF_NB_VAR_DEPART  10 /* Sert a multiplier le nombre de contraintes pour
                                   determiner le nombre de variables a prendre en
			           compte dans le 1er simplexe d'un simplexe generalise */

# define COEFF_INCREMENTATION 2 /* Il n'est pas impossible que dans le premier simplexe
                                   du simplexe generalise, il n'y ait pas de solution admissible.
				   En effet, comme on optimise sur un ensemble reduit de variable,
				   le nombre de degres de liberte s'en trouve reduit. Il est donc
				   necessaire de prevoir un mecanisme d'augmentation du nombre
				   de variables optimisees.
				   La valeur de ce parametre permet de determiner un nouveau
				   nombre de variables sur lequel on relancera un simplexe si le
				   simplexe precedent n'a pas trouve de solution admissible */

# define COEFF_POUR_VIOLATION_MAX  1.0 /*1.0*/

# define INCONNU 1
# define POSITIF 2
# define NEGATIF 3
# define POSITIF_ET_NEGATIF 4

void SPX_ControlerOptimaliteSpxGeneralise( int , double * , int * , int * , int * , int * , int * , int * 
																					 # ifdef TRACES																					 
                                           ,double * 
																					 ,double * ,
                                           ,int * 
                                           ,int * 
																					 # endif
																				 );
																					 
int cptSpxClassique = 0;	
int cptSpxGen = 0;																					 
double sumRatioVarContraintes = 0.;

/*---------------------------------------------------------------------------------------------------------*/
			
void SPX_ControlerOptimaliteSpxGeneralise( int NombreDeVariables, double * CoutsReduits_PbSpx,
                                           int * ReferencePositionDesVariables, int * TypeDeVariable_PbSpx,
																					 int * PositionDeLaVariable_PbSpx, int * NbVarNonOptimisees,
			                                     int * NumeroDeVariableNonOptimisee, int * NbErreurs
																					 # ifdef TRACES																					 
                                           ,double * Xmin_PbSpx
																					 ,double * Xmax_PbSpx,
                                           ,int * NbVariablesOptimisees
                                           ,int * NbVariablesDansLOptimisation
																					 # endif
																				 )
{ int Var; int NombreDErreurs; 
# ifdef TRACES
int NombreDeVariablesOptimisees;																			 
int NombreDeVariablesDansLOptimisation;
# endif
int NbVarNonOpt; int i;

NombreDeVariables = 0; /* Pour ne pas avoir de warning a la compilation */
	
/* Controle */
NombreDErreurs = 0;

# ifdef TRACES			
  NombreDeVariablesOptimisees        = *NbVariablesOptimisees;
  NombreDeVariablesDansLOptimisation = *NbVariablesDansLOptimisation;
# endif

NbVarNonOpt = *NbVarNonOptimisees;

for ( i = 0 ; i < NbVarNonOpt ; i++ ) {
  Var = NumeroDeVariableNonOptimisee[i];
  if ( CoutsReduits_PbSpx[Var] > ZERO_CR ) {
    /* La variable aurait du etre sur borne inf */
		if ( ReferencePositionDesVariables[Var] != HORS_BASE_SUR_BORNE_INF ) {
      NombreDErreurs++;
			# ifdef TRACES
        printf("       La variable %d n'est pas HORS_BASE_SUR_BORNE_INF hors son cout reduit est %e Xmin = %e Xmax = %e\n", 
	                     Var,CoutsReduits_PbSpx[Var],Xmin_PbSpx[Var],Xmax_PbSpx[Var]);
			# endif
	    /* Il faut remettre le type de la variable */
      TypeDeVariable_PbSpx      [Var] = VARIABLE_BORNEE_DES_DEUX_COTES;    	
      PositionDeLaVariable_PbSpx[Var] = HORS_BASE_SUR_BORNE_INF;
      /* On inverse avec la derniere variable non optimisee */
			if ( NbVarNonOpt > 1 ) {
			  NumeroDeVariableNonOptimisee[i] = NumeroDeVariableNonOptimisee[NbVarNonOpt-1];
				NbVarNonOpt--;
				i--;
			}
			# ifdef TRACES
		    NombreDeVariablesOptimisees++;	
        NombreDeVariablesDansLOptimisation++;
			# endif    			
    }
  }		
  else if ( CoutsReduits_PbSpx[Var] < -ZERO_CR ) {
    /* La variable aurait du etre sur borne sup */
    if ( ReferencePositionDesVariables[Var] != HORS_BASE_SUR_BORNE_SUP ) {
      NombreDErreurs++;
			# ifdef TRACES
        printf("       La variable %d n'est pas HORS_BASE_SUR_BORNE_SUP hors son cout reduit est %e Xmin = %e Xmax = %e\n",
	                     Var,CoutsReduits_PbSpx[Var],Xmin_PbSpx[Var],Xmax_PbSpx[Var]);
			# endif
	    /* Il faut remettre le type de la variable */
      TypeDeVariable_PbSpx      [Var] = VARIABLE_BORNEE_DES_DEUX_COTES;    		
      PositionDeLaVariable_PbSpx[Var] = HORS_BASE_SUR_BORNE_SUP;    		
      /* On inverse avec la derniere variable non optimisee */
			if ( NbVarNonOpt > 1 ) {
			  NumeroDeVariableNonOptimisee[i] = NumeroDeVariableNonOptimisee[NbVarNonOpt-1];
				NbVarNonOpt--;
				i--;
			}			
			# ifdef TRACES
		    NombreDeVariablesOptimisees++;
        NombreDeVariablesDansLOptimisation++;
			# endif
    }
  }
}

*NbErreurs = NombreDErreurs;
*NbVarNonOptimisees = NbVarNonOpt;

# ifdef TRACES
  *NbVariablesOptimisees        = NombreDeVariablesOptimisees;
  *NbVariablesDansLOptimisation = NombreDeVariablesDansLOptimisation;
# endif

return;
}

/*---------------------------------------------------------------------------------------------------------*/

PROBLEME_SPX * SPX_SimplexeGeneralise( PROBLEME_SIMPLEXE * PbSpx,
                                       PROBLEME_SPX * Spx,
				                               char FaireProgrammationLineaireGeneralisee, /* OUI_SPX ou NON_SPX */
				                               double ViolationMaximale,
                                       int * VecteurDeTravail_1,   /* dimensionnement PbSpx->NombreDeVariables */
                                       char * VecteurDeTravail_2,   /* dimensionnement PbSpx->NombreDeVariables */
                                       int * VecteurDeTravail_3,   /* dimensionnement PbSpx->NombreDeVariables */
                                       int * VecteurDeTravail_4,   /* dimensionnement PbSpx->NombreDeVariables */
				                               double * VecteurDeTravail_5, /* dimensionnement PbSpx->NombreDeVariables */
				                               char ClasserLesCoutsReduits,    /* OUI_SPX ou NON_SPX. Attention: obsolete */
				                               char ControlerOptimaliteDeLaSolution /* OUI_SPX ou NON_SPX Attention: obsolete */
				                             )
{
int Cnt; int il ; int ilMax; int i; /*int k; */
int Var; /*double u;*/
# ifdef TRACES
  int NombreDeVariablesOptimisees;
# endif
int NombreDErreurs;
int * ReferencePositionDesVariables; /*int NbVarMxDeDepart;*/
int * TypeDeVariableReference; PROBLEME_SPX * SpxRetour;

int * PositionDeLaVariable_PbSpx; int * TypeDeVariable_PbSpx; double * X_PbSpx; double * Xmin_PbSpx;
double * Xmax_PbSpx              ; double * CoutsReduits_PbSpx; double * CoutLineaire_PbSpx          ;
double * CoutsMarginauxDesContraintes_PbSpx                   ; int * IndicesDebutDeLigne_PbSpx     ;
int * NombreDeTermesDesLignes_PbSpx                          ; int * IndicesColonnes_PbSpx         ;
double * CoefficientsDeLaMatriceDesContraintes_PbSpx          ; 
/*double MargeDisponibleALaHausse; double MargeRequiseALaHausse;*/
/*double MargeDisponibleALaBaisse; double MargeRequiseALaBaisse;*/
/*int IndiceDuDernier; int VarDernier; double X; int NombreDeValeursAClasser;*/
/*int NombreTotalDeValeursClassees; int NombreDeValeursClassees;*/
/*int DebutPourLaProchaineRecherche; int DebutDeLaRecherche;*/
/*char ClasserToutesLesValeurs*/ char * SigneCoeff; double * A_PbSpx;
double A; char * Sens_PbSpx; int NombreDeVariables_PbSpx; int NombreDeContraintes_PbSpx;
/*int varFixe; double valCourant; int NombreDeVariablesDansLOptimisation;*/
/*double valPrec;*/ char SensCnt; char Sgn; int NbVarNonOptimisees; int * NumeroDeVariableNonOptimisee;
/*int nbVarDansPb;*/
int premiereCtre; /*int nbContr;*/
/*int	nbVarBaseComp;int * ComplementDeLaBase;*/
/*double cout;*/

ViolationMaximale = 0.0; /* Pour ne pas avoir de warning a la compilation */
PositionDeLaVariable_PbSpx = (int*) VecteurDeTravail_2; /* Pour ne pas avoir de warning a la compilation */
ClasserLesCoutsReduits = 0; /* Pour ne pas avoir de warning a la compilation */
ControlerOptimaliteDeLaSolution = 0; /* Pour ne pas avoir de warning a la compilation */

PositionDeLaVariable_PbSpx = PbSpx->PositionDeLaVariable;
TypeDeVariable_PbSpx       = PbSpx->TypeDeVariable;
X_PbSpx                    = PbSpx->X;
Xmin_PbSpx                 = PbSpx->Xmin;
Xmax_PbSpx                 = PbSpx->Xmax;
CoutsReduits_PbSpx         = PbSpx->CoutsReduits;
CoutLineaire_PbSpx         = PbSpx->CoutLineaire;
NombreDeVariables_PbSpx    = PbSpx->NombreDeVariables;
NombreDeContraintes_PbSpx  = PbSpx->NombreDeContraintes;

# ifdef TRACES
  varFixe = 0 ;
  for ( Var = 0 ; Var < PbSpx->NombreDeVariables ; Var++ ) {
    if ( TypeDeVariable_PbSpx[Var] == VARIABLE_FIXE ) varFixe++;
  }
  printf(" Spx Gen  %d  Spx Classique %d nb de Var non fixes %d  var Fixe %d\n",
 	         cptSpxGen, cptSpxClassique,PbSpx->NombreDeVariables - varFixe, varFixe );

  if (PbSpx->NombreDeContraintes > 1 && (cptSpxClassique + cptSpxGen) > 0){
    sumRatioVarContraintes += ((PbSpx->NombreDeVariables - varFixe)/PbSpx->NombreDeContraintes);
    printf(" ratio (moyen sur les precedents simplexe) variables(non fixes)/nb contraintes %f  ratio courant %f \n",
	           sumRatioVarContraintes/(cptSpxClassique + cptSpxGen) , (PbSpx->NombreDeVariables - varFixe)*1.0/PbSpx->NombreDeContraintes);
  }
# endif

if ( FaireProgrammationLineaireGeneralisee == NON_SPX || PbSpx->BaseDeDepartFournie == NON_SPX ) {
		cptSpxClassique ++ ;
    # ifdef TRACES
		  printf(" simplexe classique ... \n");
    # endif		
		SpxRetour = SPX_Simplexe( PbSpx , Spx );
		VecteurDeTravail_1[0] = PbSpx->NombreDeContraintes ;

		return( SpxRetour );
}
cptSpxGen++;

/* Utilisation du simplexe generalise */
/* Si on passe là, la base de depart est toujours fournie */
if ( PbSpx->BaseDeDepartFournie == NON_SPX ) {
  printf("Buuuug: la base de depart n'est pas fournie et on veut faire du simplexe generalise\n");
	exit(0);
}

premiereCtre = VecteurDeTravail_1[0]; 
# ifdef TRACES
  printf(" premiere contrainte %d \n",premiereCtre);
# endif

CoutsMarginauxDesContraintes_PbSpx          = PbSpx->CoutsMarginauxDesContraintes;
IndicesDebutDeLigne_PbSpx                   = PbSpx->IndicesDebutDeLigne;
NombreDeTermesDesLignes_PbSpx               = PbSpx->NombreDeTermesDesLignes;
IndicesColonnes_PbSpx                       = PbSpx->IndicesColonnes;
CoefficientsDeLaMatriceDesContraintes_PbSpx = PbSpx->CoefficientsDeLaMatriceDesContraintes;
A_PbSpx = PbSpx->CoefficientsDeLaMatriceDesContraintes;
Sens_PbSpx = PbSpx->Sens;


/* On utilise le contexte SIMPLEXE_SEUL car il est inutile de faire des sauvegardes internes necessaires
   au mecanisme des coupes. D'autre part en mode BRANCH and BOUND il n'est pas prevu que le type de
   borne des variables puisse changer */
PbSpx->Contexte = SIMPLEXE_SEUL;
/* Du coup on libere la memoire a la fin */
PbSpx->LibererMemoireALaFin = OUI_SPX;

ReferencePositionDesVariables = VecteurDeTravail_3;
TypeDeVariableReference       = VecteurDeTravail_4;

memcpy( (char *) ReferencePositionDesVariables, (char *) PositionDeLaVariable_PbSpx, NombreDeVariables_PbSpx * sizeof( int ) );

memcpy( (char *) TypeDeVariableReference, (char *) TypeDeVariable_PbSpx, NombreDeVariables_PbSpx * sizeof( int ) );

# ifdef TRACES
  NombreDeVariablesOptimisees = 0;
	NombreDeVariablesDansLOptimisation = 0;
# endif

/*
On garde toutes les variables qui etaient en base. Pour les variables qui etaient hors base, on enleve:
- Les variables hors base sur borne sup dont tous les coeff sont negatifs. Explication: si une variable a un
coeff positif et qu'elle est sur borne sup, on peut avoir interet a lui faire quitter sa borne pour diminuer
une contrainte.
- Les variables hors base sur borne inf dont tous les coefficients sont positifs. Explication: si une variable a un
coeff negatitf et qu'elle est sur borne inf, on peut avoir interet a lui faire quitter sa borne pour diminuer
une contrainte.
*/
/* La base est fournie, on met les variables en base dans la liste des variables a optimiser */

SigneCoeff = (char *) VecteurDeTravail_5;
memset( (char * ) SigneCoeff, INCONNU, NombreDeVariables_PbSpx * sizeof( char ) );

NumeroDeVariableNonOptimisee = (int *) (&SigneCoeff[NombreDeVariables_PbSpx]);

for ( Cnt = premiereCtre ; Cnt < NombreDeContraintes_PbSpx ; Cnt++ ) {
  SensCnt = Sens_PbSpx[Cnt];
  il = IndicesDebutDeLigne_PbSpx[Cnt];
  ilMax = il + NombreDeTermesDesLignes_PbSpx[Cnt];
	while (il <ilMax ){
		Var = IndicesColonnes_PbSpx[il];
		Sgn = SigneCoeff[Var];
		if ( SensCnt == '<' ) A = A_PbSpx[il];
		else if ( SensCnt == '>' ) A = -A_PbSpx[il];
		else break;
		if ( A >= 1.e-4 ) {
			if ( Sgn == INCONNU ) SigneCoeff[Var] = POSITIF;
			else if ( Sgn == NEGATIF ) SigneCoeff[Var] = POSITIF_ET_NEGATIF;
		}
		else if (A <= -1.e-4){
			if ( Sgn == INCONNU ) SigneCoeff[Var] = NEGATIF;
			else if ( Sgn == POSITIF ) SigneCoeff[Var] = POSITIF_ET_NEGATIF;
		}				  
	  il++;
	}
}

NbVarNonOptimisees = 0;
	
for ( Var = 0 ; Var < NombreDeVariables_PbSpx ; Var++ ) {

  if ( TypeDeVariable_PbSpx[Var] == VARIABLE_FIXE ) {
    # ifdef TRACES
      NombreDeVariablesDansLOptimisation++;
    # endif
		continue;
  }
	
	if ( PositionDeLaVariable_PbSpx[Var] == EN_BASE ) {
    # ifdef TRACES			
			NombreDeVariablesOptimisees++;
			NombreDeVariablesDansLOptimisation++;
		# endif
	}
	else if ( PositionDeLaVariable_PbSpx[Var] == HORS_BASE_SUR_BORNE_SUP ) {
		if ( SigneCoeff[Var] != NEGATIF ) {
      # ifdef TRACES			
				NombreDeVariablesOptimisees++;
				NombreDeVariablesDansLOptimisation++;
			# endif
		}		
		else { /* La variable n'est pas optimisee */
		  TypeDeVariable_PbSpx[Var] = VARIABLE_FIXE;    
      X_PbSpx[Var] = Xmax_PbSpx[Var];
			NumeroDeVariableNonOptimisee[NbVarNonOptimisees] = Var;
			NbVarNonOptimisees++;
		}
	}
	else if ( PositionDeLaVariable_PbSpx[Var] == HORS_BASE_SUR_BORNE_INF ) {
		if ( SigneCoeff[Var] != POSITIF ) {
      # ifdef TRACES			
			  NombreDeVariablesOptimisees++;
				NombreDeVariablesDansLOptimisation++;
			# endif
		}
		else { /* La variable n'est pas optimisee */
      TypeDeVariable_PbSpx[Var] = VARIABLE_FIXE;    		
      X_PbSpx[Var] = Xmin_PbSpx[Var];
			NumeroDeVariableNonOptimisee[NbVarNonOptimisees] = Var;
			NbVarNonOptimisees++;			
		}		
	}
}	


# ifdef TRACES
  printf(" nombre de variables fixes %d\n", varFixe);
  printf(" variables en base: NombreDeVariablesOptimisees = %d,PbSpx->NombreDeVariables=%d \n",
           NombreDeVariablesOptimisees,PbSpx->NombreDeVariables);
# endif

/* Resolution du probleme reduit */

SIMPLEXE:

SpxRetour = SPX_Simplexe( PbSpx , Spx );
  # ifdef TRACES
if ( PbSpx->ExistenceDUneSolution != OUI_SPX ) {
  printf("Simplexe reduit: pas de solution \n");
	printf("poursuivre le developpement de l'algorithme en ajoutant des variables\n");
	exit(0);	
}  
	# endif
/* Controle */

SPX_ControlerOptimaliteSpxGeneralise( NombreDeVariables_PbSpx, CoutsReduits_PbSpx,
                                      ReferencePositionDesVariables, TypeDeVariable_PbSpx,
																		  PositionDeLaVariable_PbSpx, &NbVarNonOptimisees,
			                                NumeroDeVariableNonOptimisee,  &NombreDErreurs 
																			# ifdef TRACES																					 
                                        , Xmin_PbSpx
																				, Xmax_PbSpx,
                                        , &NbVariablesOptimisees
                                        , &NbVariablesDansLOptimisation
																				# endif
																		);
if ( NombreDErreurs > 0 ) {
  # ifdef TRACES
    printf("NombreDeVariablesOptimisees: %d au lieu de %d\n",NombreDeVariablesOptimisees,PbSpx->NombreDeVariables);
    printf("NombreDErreurs %d sur %d\n",NombreDErreurs,PbSpx->NombreDeVariables-NombreDeVariablesOptimisees);
    printf("           => relance du simplexe apres avoir ajoute %d variables\n",NombreDErreurs);  
	# endif
  /* On repart avec des variables supplementaires, les variables ont ete positionnees donc la base de
     depart est fournie */
  PbSpx->BaseDeDepartFournie = OUI_SPX;
  Spx = SpxRetour;
  goto SIMPLEXE;
}

/* Remettre la position de la variable comme elle etait en entree si la variable n'a pas ete optimisee car le simplexe a mis -1
   ainsi que le type initial de variable */
# ifdef TRACES
  printf("NombreDeVariablesOptimisees: %d au lieu de %d\n",NombreDeVariablesOptimisees,PbSpx->NombreDeVariables - varFixe);
# endif

for ( i = 0 ; i < NbVarNonOptimisees ; i++ ) {
  Var = NumeroDeVariableNonOptimisee[i];
  TypeDeVariable_PbSpx      [Var] = TypeDeVariableReference[Var];
  PositionDeLaVariable_PbSpx[Var] = ReferencePositionDesVariables[Var];        
}

# ifdef TRACES
  printf("         Le Simplexe a trouve une solution \n");
# endif

VecteurDeTravail_1[0] = PbSpx->NombreDeContraintes;
return( SpxRetour );
} 
