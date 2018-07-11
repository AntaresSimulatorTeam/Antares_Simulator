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
# ifndef DEFINITIONS_PRESOLVE_FAITES  
/*******************************************************************************************/

# define VERBOSE_PRS  0

# define PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE 
  # undef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE

# define DUMALGE_MENDELSON NON_PNE /*NON_PNE*/

# define SEUIL_POUR_AMELIORATION_DE_BORNE 1.e-6 /* 1.e-6 */ /* Doit etre superieur a MARGE_SUR_MODIF_DE_BORNE */
# define MARGE_SUR_MODIF_DE_BORNE         0.e-7   /* 5 */ 
# define MARGE_EGALITE_BORNE_MIN_ET_MAX   1.e-8 /* N'est fonctionnel que si on met plus grand que 2 * MARGE_SUR_MODIF_DE_BORNE */
# define SEUIL_INF_POUR_FIXATION_BINAIRE  0.0001  
# define SEUIL_SUP_POUR_FIXATION_BINAIRE  0.9999
# define MARGE_DINFAISABILITE            1.e-7
# define ECART_NUL_LAMBDAMIN_LAMBDAMAX 1.e-8

# define VALEUR_DE_BORNE_DEBILE 1.e+12 /*12*/
# define PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE 1.e-6 /*1.e-5*/

# define ECART_NUL_LAMBDAMIN_LAMBDAMAX 1.e-8 /* si l'ecart en la variable duale min et max est inferieur on considere que c'est egal */

# define CBARRE_POSITIF 1
# define CBARRE_NEGATIF 2
# define CBARRE_NUL     3
# define CBARRE_INDIFFERENT  4

# define LAMBDA_MIN_CONNU  1
# define LAMBDA_MAX_CONNU  2
# define LAMBDA_MIN_ET_MAX_CONNU  3
# define LAMBDA_CONNU  4
# define LAMBDA_NON_INITIALISE 5

# define VALEUR_NATIVE     1
# define VALEUR_IMPLICITE  2

/* Les types de reduction (pour le postsolve) */
/* Remarque: il manque la recuperation des variables duales dans le cas ou on fait des combinaisons lineaires
   de contraintes pour creer du creux. On peut le faire en reconstruisant la matrice avant combinaisons
	 lineaires et en faisant une resolution du systeme ub = c */
# define SUPPRESSION_VARIABLE_NON_BORNEE  1  /* Pour trouver la valeur de la variable et la valeur de la variable duale de la contrainte */

# define SUBSITUTION_DE_VARIABLE           2  /* Pour trouver la valeur de la variable et la valeur de la variable duale de la contrainte */

# define SUPPRESSION_COLONNE_COLINEAIRE    3  /* Pour trouver la valeur des 2 variables */

# define SUPPRESSION_LIGNE_SINGLETON       4   /* Pour trouver la variable duale de la contrainte */

# define SUPPRESSION_FORCING_CONSTRAINT    5  /* Pour trouver la valeur la variable duale de la contrainte */

# define SUPPRESSION_CONTRAINTE_COLINEAIRE 6  /* Pour trouver la variable duale de la contrainte */

# define VARIABLE_FIXEE                    7  /* Inutilise pour l'instant */
	
/*******************************************************************************************************/

typedef struct {
/* Pour les outils de gestion memoire */ 
void * Tas;

double * MinContrainte;
double * MaxContrainte;
char   * MinContrainteCalcule;
char   * MaxContrainteCalcule; 

double * Lambda;
double * LambdaMin;
double * LambdaMax; 
char   * ConnaissanceDeLambda; /* LAMBDA_MIN_CONNU LAMBDA_MAX_CONNU LAMBDA_MIN_ET_MAX_CONNU LAMBDA_CONNU LAMBDA_NON_INITIALISE */
  
char   * ContrainteInactive;

int   * ContrainteBornanteSuperieurement;
int   * ContrainteBornanteInferieurement;
char  * ConserverLaBorneSupDuPresolve;
char  * ConserverLaBorneInfDuPresolve;

/* Pour detecter les contraintes colineaires */
int * ParColonnePremiereVariable;
int * ParColonneVariableSuivante;
int NbMaxTermesDesColonnes;

/* Pour detecter les colonnes colineaires */
int * ParLignePremiereContrainte;
int * ParLigneContrainteSuivante;
int NbMaxTermesDesLignes;
int * VariableEquivalente; /* Une variable est equivalente si elle en remplce 2 autres
                               dont les colonnes sont colineaires */

double * ValeurDeXPourPresolve;
double * BorneInfPourPresolve;
double * BorneSupPourPresolve;
int    * TypeDeBornePourPresolve;
char   * TypeDeValeurDeBorneInf;
char   * TypeDeValeurDeBorneSup;

/*char   * VariableSubstituee;*/

void   * ProblemePneDuPresolve;

} PRESOLVE;

/*******************************************************************************************/
# define DEFINITIONS_PRESOLVE_FAITES	 
# endif  
 

