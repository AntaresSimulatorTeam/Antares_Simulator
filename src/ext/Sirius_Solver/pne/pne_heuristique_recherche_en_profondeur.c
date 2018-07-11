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

   FONCTION: Recherche en prodondeur a partir du noeud courant.

	 ATTENTION c'est a faire
	 
						 Attention, lorsque ce module est appele les variables
						 UminTrav UmaxTrav et TypeDeBorneTrav on ete remises a leur
						 valeur du noeud racine. Donc leurs valeurs n'inclut pas
						 les instanciations faites en amont du noeud a partir duquel
						 on demarre.
						 Par contre les structures du simplexe se trouvent dans l'etat
						 du dernier noeud resolu (bornes et types de bornes).						 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define NB_MAX_CYCLES 10
# define NB_MAX_ITERATIONS 100
# define TAILLE_MOYENNE_MOBILE 3
# define FIXER_VARIABLES_AU_DEPART  OUI_PNE

# define VARIABLE_LA_MOINS_FRACTIONNAIRE 1
# define VARIABLE_LA_PLUS_FRACTIONNAIRE  2
# define TYPE_DE_FIXATION VARIABLE_LA_MOINS_FRACTIONNAIRE

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PNE_HeuristiqueRechercheEnProfondeur( PROBLEME_PNE * Pne, char * PasDeSolutionEntiereDansSousArbre ) 
{
int i ; int j ; int k; int YaUneSolution; int PremFrac; int * SuivFrac; double X;
double * UTrav; char OK; double * UminTrav; double * UmaxTrav; int NombreDeVariablesFixees;
int * NumerosDesVariablesArrondies; double * NouvelleBorneMin; double * NouvelleBorneMax;
int * NumeroDesVariablesPeuFractionnaire; int * PositionDeLaVariable_E; int NbVarDeBaseComplementaires_E;
int * ComplementDeLaBase_E; int NombreMaxDIterations; char * Flag; int NbCycles;
double * USv; int VariableLaPlusFractionnaireSv; double Critere;
int NbMaxCycles; int NombreDeVariablesAValeurFractionnaireSv; double NormeDeFractionnaliteSv;
int NbFix; int NbVarEntFix; int NbVarEntNonFix; int Seuil;
PROBLEME_SPX * Spx; BB * Bb; int NombreDeVariablesEntieresFixees;
int NombreDeVariablesEntieresNonFixesAuDepart; int NombreDeVariablesAValeurFractionnaire[TAILLE_MOYENNE_MOBILE];
int IndexMoyenneMobile; int SommePourMoyenneMobilePrecedente; int SommePourMoyenneMobile;

printf("HeuristiqueRechercheEnProfondeur \n");

*PasDeSolutionEntiereDansSousArbre = NON_PNE;

Bb = (BB *) Pne->ProblemeBbDuSolveur;

NumerosDesVariablesArrondies = (int *) malloc(  Pne->NombreDeVariablesTrav * sizeof( int ) );
NouvelleBorneMin = (double *) malloc(  Pne->NombreDeVariablesTrav * sizeof( double ) );
NouvelleBorneMax = (double *) malloc(  Pne->NombreDeVariablesTrav * sizeof( double ) );
if ( NumerosDesVariablesArrondies == NULL || NouvelleBorneMin == NULL || NouvelleBorneMax  == NULL ) {
  free( NumerosDesVariablesArrondies ); free( NouvelleBorneMin ); free( NouvelleBorneMax );
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_HeuristiqueArrondis \n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */	
}

NbMaxCycles = NB_MAX_CYCLES;
  
VariableLaPlusFractionnaireSv = Pne->VariableLaPlusFractionnaire;
NombreDeVariablesAValeurFractionnaireSv = Pne->NombreDeVariablesAValeurFractionnaire;
NormeDeFractionnaliteSv = Pne->NormeDeFractionnalite;

NumeroDesVariablesPeuFractionnaire = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
Flag = (char *) malloc( Pne->NombreDeVariablesTrav * sizeof( char ) );
PositionDeLaVariable_E = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
ComplementDeLaBase_E = (int *) malloc( Pne->NombreDeContraintesTrav * sizeof( int ) );

USv = Pne->ValeurLocale;
memcpy( (char *) USv, (char *) Pne->UTrav, Pne->NombreDeVariablesTrav * sizeof( double ) );

memset( (char *) Flag, 0, Pne->NombreDeVariablesTrav * sizeof( char ) );

UTrav = Pne->UTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;

NbCycles = 0;
NbFix = 0;
Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;

IndexMoyenneMobile = 0;
SommePourMoyenneMobilePrecedente = Pne->NombreDeVariablesAValeurFractionnaire * TAILLE_MOYENNE_MOBILE;

NbVarEntFix = 0; 
NbVarEntNonFix = 0;
NombreDeVariablesFixees = 0;

NombreDeVariablesEntieresFixees = 0;
NombreDeVariablesEntieresNonFixesAuDepart = 0;
# if FIXER_VARIABLES_AU_DEPART == OUI_PNE
  /* 1ere etape: on fixe les variables qui ont deja une valeur entiere */
  for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {	
    if ( Pne->TypeDeVariableTrav[i] != ENTIER ) continue;
	  if ( Pne->TypeDeBorneTrav[i] == VARIABLE_FIXE ) continue;
    if ( Pne->UminTrav[i] == Pne->UmaxTrav[i] ) continue;
		NombreDeVariablesEntieresNonFixesAuDepart++;
    if ( Pne->LaVariableAUneValeurFractionnaire[i] == OUI_PNE ) continue;
    NombreDeVariablesEntieresFixees++;		
		/* La variable fractionnaire a une variable entiere */
    Flag[i] = 1;
    NumerosDesVariablesArrondies[NombreDeVariablesFixees] = i;	
    if ( fabs( UTrav[i] - UminTrav[i] ) < fabs( UmaxTrav[i] - UTrav[i] ) ) {
      NouvelleBorneMin[NombreDeVariablesFixees] = UminTrav[i];
      NouvelleBorneMax[NombreDeVariablesFixees] =	UminTrav[i];
	  }
    else {
      NouvelleBorneMin[NombreDeVariablesFixees] = UmaxTrav[i];
		  NouvelleBorneMax[NombreDeVariablesFixees] = UmaxTrav[i];
    }
    NombreDeVariablesFixees++;
	  NbFix++;
  }
# endif

NbVarEntNonFix = NombreDeVariablesEntieresNonFixesAuDepart - NombreDeVariablesFixees;

printf("NbVarEntNonFix %d  NombreDeVariablesEntieresNonFixesAuDepart %d\n",NbVarEntNonFix,NombreDeVariablesEntieresNonFixesAuDepart);

if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
  Seuil = 0.1 * NombreDeVariablesEntieresNonFixesAuDepart;
	if ( Seuil < 50 ) Seuil = 50;
  if ( NbVarEntNonFix > Seuil ) {
	  printf("Pas d'heuristique DIVE\n");
	  goto Fin;
	}
}

if ( NbMaxCycles < NbVarEntNonFix + 10 ) NbMaxCycles = NbVarEntNonFix + 10;

if ( NombreDeVariablesFixees > 0 ) {
  /* Pour fixer les variables a valeur entiere dans le simplexe */
	
  NombreMaxDIterations = 50 * NombreDeVariablesFixees;
  if ( NombreMaxDIterations > NB_MAX_ITERATIONS ) NombreMaxDIterations = NB_MAX_ITERATIONS;	
	
  SPX_HeuristiqueArrondis( Spx,
                         &YaUneSolution,
												 Pne->NombreDeVariablesTrav,
												 UTrav,
												 Pne->TypeDeBorneTrav, 
									       Pne->NombreDeContraintesTrav,
									       PositionDeLaVariable_E , 
									       &NbVarDeBaseComplementaires_E ,
									       ComplementDeLaBase_E,									 									 
                         NombreMaxDIterations,
									       NombreDeVariablesFixees,
									       NumerosDesVariablesArrondies,
									       NouvelleBorneMin,
									       NouvelleBorneMax
                            );

																												
  if ( YaUneSolution != OUI_SPX ) {
	  printf("Fin heuristique DIVE par echec du simplexe    apres NbCycles = %d\n",NbCycles);
    Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive++;
	  if ( Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive >= NB_MAX_ECHECS_SUCCESSIFS_HEURISTIQUE ) {
      Pne->FaireHeuristiqueFractionalDive = NON_PNE;
      if ( Pne->NombreDeSolutionsHeuristiqueFractionalDive <= 0 ) Pne->NombreDeReactivationsSansSuccesHeuristiqueFractionalDive++;
		  if ( Pne->NombreDeReactivationsSansSuccesHeuristiqueFractionalDive > NB_MAX_REACTIVATION_SANS_SUCCES ) {
        Pne->StopHeuristiqueFractionalDive = OUI_PNE;
			  printf("****************** arret definitif DIVE \n");			
		  }
		  else printf(" !!!!!!!!!!!! On positionne DIVE a NON_PNE \n");
	  }
    goto Fin;		
	}
}
		
Debut:

PremFrac = Pne->PremFrac;  
SuivFrac = Pne->SuivFrac;

/* A ce stade on dispose deja des variables fractionnaires */

j = 0;
k = -1;
# if TYPE_DE_FIXATION == VARIABLE_LA_PLUS_FRACTIONNAIRE 
  i = PremFrac;
  while ( i >= 0 ) {
		if ( Flag[i] == 0 ) {
		  k = i; /* La variable n'a pas ete deja fixee */
			break;
		}
    i = SuivFrac[i];
  }	
	
# elif TYPE_DE_FIXATION == VARIABLE_LA_MOINS_FRACTIONNAIRE
  i = PremFrac;
	if ( Flag[i] == 0 ) k = i; /* La variable n'a pas ete deja fixee */
  while ( i >= 0 ) {
		if ( Flag[i] == 0 ) k = i; /* La variable n'a pas ete deja fixee */
    i = SuivFrac[i];
  }
# endif

if ( k == -1 ) {
  /* Rien a instancier */
  goto Fin;
}

X = UTrav[k];	
if ( X - floor( X ) < ceil( X ) - X ) {	  
	Flag[k] = 1;
  NumerosDesVariablesArrondies[j] = k;		
  NouvelleBorneMin[j] = UminTrav[k];
  NouvelleBorneMax[j] =	UminTrav[k];		
  # if TRACES == 1
		printf("Fixation %d a %e valeur %e\n",k,UminTrav[k],X);
	# endif		
	j++;
		
}
else {	  
	Flag[k] = 1;
  NumerosDesVariablesArrondies[j] = k;		
  NouvelleBorneMin[j] = UmaxTrav[k];
	NouvelleBorneMax[j] =	UmaxTrav[k];		
  # if TRACES == 1
	  printf("Fixation %d a %e valeur %e\n",k,UmaxTrav[k],X);
	# endif
	j++;		
}

NombreDeVariablesFixees = j;

NbFix += NombreDeVariablesFixees;

if ( NombreDeVariablesFixees == 0 ) {
  printf("Fin par NombreDeVariablesFixees = %d\n",NombreDeVariablesFixees);
  goto Fin;
}

NombreMaxDIterations = 50 * NombreDeVariablesFixees;
if ( NombreMaxDIterations > NB_MAX_ITERATIONS ) NombreMaxDIterations = NB_MAX_ITERATIONS;

NombreMaxDIterations = -1;

SPX_HeuristiqueArrondis( Spx,
                         &YaUneSolution,
												 Pne->NombreDeVariablesTrav,
												 UTrav,
												 Pne->TypeDeBorneTrav, 
									       Pne->NombreDeContraintesTrav,
									       PositionDeLaVariable_E , 
									       &NbVarDeBaseComplementaires_E ,
									       ComplementDeLaBase_E,									 									 
                         NombreMaxDIterations,
									       NombreDeVariablesFixees,
									       NumerosDesVariablesArrondies,
									       NouvelleBorneMin,
									       NouvelleBorneMax
                            );

OK = OUI_PNE;
if ( YaUneSolution != OUI_SPX ) {

	  printf("Fin heuristique DIVE par echec du simplexe    apres NbCycles = %d\n",NbCycles);

  # if TRACES == 1
		printf("Pas de solution dans l'heuristique  NbFix %d\n",NbFix);
		if ( Spx->Iteration < NombreMaxDIterations - 10 ) {
		  printf("        et on n'est pas sorti par Max Iteration \n");
		}
		else printf("   Nombre max d'iterations atteint Spx->Iteration %d NombreMaxDIterations %d\n",Spx->Iteration,NombreMaxDIterations);
	# endif
	OK = NON_PNE;
}
else {
  Critere = Pne->Z0;
	for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) Critere += Pne->LTrav[i] * UTrav[i];
  if ( Critere >= Pne->CoutOpt ) {
		printf("Critere >= Pne->CoutOpt   CoutOpt %e\n",Pne->CoutOpt);		
	  OK = NON_PNE;
	}
}

if ( OK == OUI_PNE ) {
  PNE_DeterminerLesVariablesFractionnaires( Pne, PositionDeLaVariable_E );
  if ( Pne->NombreDeVariablesAValeurFractionnaire > 0 ) {
    /* Controle de la decroissance du nombre de variables a valeur fractionnaire */
		if ( IndexMoyenneMobile < TAILLE_MOYENNE_MOBILE ) {
	    NombreDeVariablesAValeurFractionnaire[IndexMoyenneMobile] = Pne->NombreDeVariablesAValeurFractionnaire;
	    IndexMoyenneMobile++;
    }
		else {
		  /* On fait de la place a la fin */
		  for ( i = 0 ; i < TAILLE_MOYENNE_MOBILE - 1 ; i++ ) {
			  NombreDeVariablesAValeurFractionnaire[i] = NombreDeVariablesAValeurFractionnaire[i+1];
			}
			/* On range la valeur a la fin */
			NombreDeVariablesAValeurFractionnaire[TAILLE_MOYENNE_MOBILE - 1] = Pne->NombreDeVariablesAValeurFractionnaire;
		}

		SommePourMoyenneMobile = 0;
		if ( IndexMoyenneMobile == TAILLE_MOYENNE_MOBILE ) {
		  /* Calcul de la moyenne mobile */
		  for ( i = 0 ; i < TAILLE_MOYENNE_MOBILE ; i++ ) SommePourMoyenneMobile += NombreDeVariablesAValeurFractionnaire[i];	 
		}
	  if ( SommePourMoyenneMobile <= SommePourMoyenneMobilePrecedente ) {
		  if ( IndexMoyenneMobile == TAILLE_MOYENNE_MOBILE ) SommePourMoyenneMobilePrecedente = SommePourMoyenneMobile;
			NbCycles++;
	    if ( NbCycles < NbMaxCycles ) goto Debut;		
		}

    # if TRACES == 1
	    printf("Fin heuristique par nombre de cycles atteint\n");
	  # endif

		if ( SommePourMoyenneMobile <= SommePourMoyenneMobilePrecedente ) {
		  printf("Fin heuristique DIVE par nombre max de cycles \n");
		}
		else {
		  printf("Fin heuristique DIVE SommePourMoyenneMobile %d SommePourMoyenneMobilePrecedente %d\n",
			        SommePourMoyenneMobile,SommePourMoyenneMobilePrecedente);
		}
		
	  OK = NON_PNE;
		
	}
}

if ( OK == NON_PNE ) {
  Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive++;
	if ( Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive >= NB_MAX_ECHECS_SUCCESSIFS_HEURISTIQUE ) {
    Pne->FaireHeuristiqueFractionalDive = NON_PNE;
    if ( Pne->NombreDeSolutionsHeuristiqueFractionalDive <= 0 ) Pne->NombreDeReactivationsSansSuccesHeuristiqueFractionalDive++;
		if ( Pne->NombreDeReactivationsSansSuccesHeuristiqueFractionalDive > NB_MAX_REACTIVATION_SANS_SUCCES ) {
      Pne->StopHeuristiqueFractionalDive = OUI_PNE;
			printf("****************** arret definitif DIVE \n");			
		}
		else printf(" !!!!!!!!!!!! On positionne DIVE a NON_PNE \n");
	}
  goto Fin;
}

# if TRACES == 1
  printf("Heuristic found Critere %e \n",Critere);
# endif
printf("Heuristic found \n");

/* Solution trouvee */

/* On archive la solution: attention il faudra aussi y mettre VariablesDualesDesContraintesTravEtDesCoupes */

PNE_HeuristiqueArchivageSolutionEtMajBranchAndBound( Pne, Critere ); 

Fin:

free( NumeroDesVariablesPeuFractionnaire );
free( PositionDeLaVariable_E );
free( ComplementDeLaBase_E );
free( Flag );

Pne->CestTermine = NON_PNE;

memcpy( (char *) Pne->UTrav, (char *) USv, Pne->NombreDeVariablesTrav * sizeof( double ) );

Pne->VariableLaPlusFractionnaire = VariableLaPlusFractionnaireSv;
Pne->NombreDeVariablesAValeurFractionnaire = NombreDeVariablesAValeurFractionnaireSv;
Pne->NormeDeFractionnalite = NormeDeFractionnaliteSv;

SPX_RAZHeuristiqueArrondis( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );

free( NumerosDesVariablesArrondies );
free( NouvelleBorneMin );
free( NouvelleBorneMax );

return;
}

