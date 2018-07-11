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

   FONCTION: Resolution d'un probleme par le simplexe primal.
             Apres essais, le hot-start avec l'algorithme primal est 
             moins performant et plus compliqué a faire qua'avec
             l'algorithme primal.
             On s'interdit donc de faire du hot-start avec l'algorithme
             primal.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define COMPILER_PNE_SolvePbRlxSpxPrimal NON_PNE

# if COMPILER_PNE_SolvePbRlxSpxPrimal == OUI_PNE

/*----------------------------------------------------------------------------*/

void PNE_SolvePbRlxSpxPrimal( PROBLEME_PNE * Pne,
	               double   CoutMax,			      /* Information en Entree */
	               int     UtiliserCoutMax,                      /* Information en Entree: Oui ou non */
                 int     BaseFournie,                          /* Information en Entree: Oui ou non */
                 int     IndiceDeLaNouvelleVariableInstanciee, /* Information en Entree */
	               int	NombreDeVariablesInstanciees,	      /* Information en Entree */
	               int   * IndicesDesVariablesInstanciees,	      /* Information en Entree */
                 int   * PositionDeLaVariable,                 /* Information en Entree et Sortie */
                 int   * NbVarDeBaseComplementaires,           /* Information en Entree et Sortie */
                 int   * ComplementDeLaBase,                   /* Information en Entree et Sortie */
                 double * Critere, 
                 int   * Faisabilite 
                            )
{

int i; int BaseDeDepartFournie; int LibererMemoireALaFin; int Contexte            ;
int ExistenceDUneSolution      ; int ChoixDeLAlgorithme  ; int NombreMaxDIterations;

double * VariablesDualesDesContraintes; 

if ( BaseFournie == OUI_SPX ) {
  printf(" PNE_SolvePbRlxSpxPrimal: tentative de hot-start avec l'algortihme primal or on se l'est interdit \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

*Faisabilite = OUI_PNE;	

UtiliserCoutMax = NON_SPX; /* Dans tous les cas, le simplexe primal n'utilise pas le CoutMax */

LibererMemoireALaFin = NON_SPX;
Contexte             = BRANCH_AND_BOUND_OU_CUT;
				 
/* Au plus on cree une contraintes supplementaires qui represente l'instanciation de la variable choisie */

VariablesDualesDesContraintes = (double *) malloc( ( Pne->NombreDeContraintesTrav + 1 ) * sizeof( double ) ); 
if ( VariablesDualesDesContraintes == NULL ) {
  printf(" PNE, memoire insuffisante dans le sous programme PNE_SolvePbRlxSpxPrimal  \n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

BaseDeDepartFournie = BaseFournie;

printf("----------------------------------------------------------\n");
printf("Appel du simplexe primal. Nombre de variables %d contraintes %d\n",Pne->NombreDeVariablesTrav,Pne->NombreDeContraintesTrav);

ChoixDeLAlgorithme   = SPX_PRIMAL;
NombreMaxDIterations = -1;

/* Mettre ici l'appel a la routine du simplexe: SPX_Simplexe ... */

free( VariablesDualesDesContraintes ); /* On ne les utilise pas ici */ 	  

if ( ExistenceDUneSolution == OUI_PNE ) { 
  *Faisabilite = OUI_PNE;					   
/*
  On recupere la base optimale pour le coup d'apres. 
  Pour la PositionDeLaVariable, le simplexe renvoie: 
  * -1 pour les variables fixees en entree 
  * EN_BASE ou HORS_BASE_SUR_BORNE_INF ou HORS_BASE_SUR_BORNE_SUP
  Le simplexe renvoie aussi le complement de base.
  On se contente de recalculer le critere.
*/
  printf("Resultat de la resolution du probleme relaxe: \n");
  for ( *Critere = 0. , i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
    *Critere+= Pne->LTrav[i] * Pne->UTrav[i];    
  }
  printf("Valeur du critere recalculee en sortie de l'optimisation %lf \n",*Critere); 
}
else if ( ExistenceDUneSolution == NON_PNE ) { 
  *Faisabilite = NON_PNE;
}
else {
  printf("Bug dans PNE_SolvePbRlxSpxPrimal, ExistenceDUneSolution mal renseigne\n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}
  
return;
}

# endif











