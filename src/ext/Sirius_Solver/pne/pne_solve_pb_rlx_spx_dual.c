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

   FONCTION: Resolution d'un probleme relaxe par le simplexe dual 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_constantes_externes.h"
# include "spx_definition_arguments.h"
# include "spx_define.h"
# include "spx_fonctions.h"   

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define SEUIL_POUR_PRIORITE_DANS_SPX_AUX_VARIABLES_SORTANTES_ENTIERES  0.75 /* 75% soit 3/4 */

/*----------------------------------------------------------------------------*/

void PNE_SolvePbRlxSpxDual( PROBLEME_PNE * Pne,
	             char     PremiereResolutionAuNoeudRacine,      /* Information en Entree */
	             double   CoutMax,			                        /* Information en Entree */
	             int      UtiliserCoutMax,                      /* Information en Entree: Oui ou non */
               int      BaseFournie,                          /* Information en Entree: Oui ou non */
               int    * PositionDeLaVariable,                 /* Information en Entree et Sortie */
               int    * NbVarDeBaseComplementaires,           /* Information en Entree et Sortie */
               int    * ComplementDeLaBase,                   /* Information en Entree et Sortie */
               int    * Faisabilite 
                          )
{
int BaseDeDepartFournie; int LibererMemoireALaFin; int Contexte; int i; double S;
int ExistenceDUneSolution; int ChoixDeLAlgorithme; int NombreMaxDIterations;	 
time_t HeureDeCalendrierDebut; time_t HeureDeCalendrierCourant; double TempsEcoule; int Nb;
double * VariablesDualesDesContraintesTravEtDesCoupes; int * ContrainteSaturee;
PROBLEME_SIMPLEXE Probleme; BB * Bb; PROBLEME_SPX * Spx;

#if VERBOSE_PNE
  printf("----------------------------------------------------------\n");
  printf("Appel du simplexe dual. Nombre de variables %d contraintes %d\n",Pne->NombreDeVariablesTrav,Pne->NombreDeContraintesTrav);
  fflush(stdout);
#endif

*Faisabilite = OUI_PNE;	

if ( Pne->TailleAlloueeVariablesDualesDesContraintesTravEtDesCoupes < Pne->NombreDeContraintesTrav + Pne->Coupes.NombreDeContraintes ) {
  i = Pne->NombreDeContraintesTrav + Pne->Coupes.NombreDeContraintes;
	if ( Pne->VariablesDualesDesContraintesTravEtDesCoupes == NULL ) {
    Pne->VariablesDualesDesContraintesTravEtDesCoupes = (double *) malloc( i * sizeof( double ) ); 
	}
	else {
    Pne->VariablesDualesDesContraintesTravEtDesCoupes = (double *) realloc( Pne->VariablesDualesDesContraintesTravEtDesCoupes, i * sizeof( double ) ); 
	}
  if ( Pne->VariablesDualesDesContraintesTravEtDesCoupes == NULL ) {
    printf("PNE, memoire insuffisante dans le sous programme PNE_SolvePbRlxSpxDual  \n"); 
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
  }
	Pne->TailleAlloueeVariablesDualesDesContraintesTravEtDesCoupes = i;
}

LibererMemoireALaFin = NON_SPX;

if ( PremiereResolutionAuNoeudRacine == OUI_PNE ) {
  if ( Pne->NombreDeVariablesEntieresTrav <= 0 ) {
    /* Si pas de variables entieres, alors simplexe seul */
    Contexte = SIMPLEXE_SEUL;
  }
  else {
    /* Premiere resolution du noeud racine dans le cas du branch and bound */
    Contexte = BRANCH_AND_BOUND_OU_CUT;	  
  }
}
else {
  /* Resolutions suivantes dans le cas du branch and bound */
  Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
}

BaseDeDepartFournie = BaseFournie;

ChoixDeLAlgorithme = SPX_DUAL;

if ( Contexte != BRANCH_AND_BOUND_OU_CUT_NOEUD ) {
  if ( Pne->ProblemeSpxDuSolveur != NULL ) {
    SPX_LibererProbleme( (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur );
    Pne->ProblemeSpxDuSolveur = NULL;
  }
}

/* Si on vient d'ajouter des coupes calculees c'est qu'il s'agit de la reoptimisation d'un noeud deja 
   resolu. Dans ce cas, on impose une limitation du nombre d'iterations. */
if ( Pne->NombreDeCoupesCalculees > 0 ) {
  NombreMaxDIterations = (int) (5. * Pne->NombreDeCoupesCalculees); 
  if ( NombreMaxDIterations < 1000 ) NombreMaxDIterations = 1000;
}
else {
  /* On met un nombre negatif pour que la donnee ne soit pas prise en compte */
  NombreMaxDIterations = -10;
}
	 						
Probleme.Contexte = Contexte;
Probleme.NombreMaxDIterations = NombreMaxDIterations;
Probleme.DureeMaxDuCalcul     = -1.;
  
Probleme.CoutLineaire      = Pne->LTrav;
Probleme.X                 = Pne->UTrav;
Probleme.Xmin              = Pne->UminTrav;
Probleme.Xmax              = Pne->UmaxTrav;
Probleme.NombreDeVariables = Pne->NombreDeVariablesTrav;
Probleme.TypeDeVariable    = Pne->TypeDeBorneTrav;

Probleme.NombreDeContraintes                   = Pne->NombreDeContraintesTrav;
Probleme.IndicesDebutDeLigne                   = Pne->MdebTrav;
Probleme.NombreDeTermesDesLignes               = Pne->NbTermTrav;
Probleme.IndicesColonnes                       = Pne->NuvarTrav;
Probleme.CoefficientsDeLaMatriceDesContraintes = Pne->ATrav;
Probleme.Sens                                  = Pne->SensContrainteTrav;
Probleme.SecondMembre                          = Pne->BTrav;     

Probleme.ChoixDeLAlgorithme = ChoixDeLAlgorithme;    

Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE /*PRICING_DANTZIG*/;
Probleme.FaireDuScaling              = OUI_SPX /*OUI_SPX*/;   
Probleme.StrategieAntiDegenerescence = AGRESSIF;
  
Probleme.BaseDeDepartFournie        = BaseDeDepartFournie;
Probleme.PositionDeLaVariable       = PositionDeLaVariable;
Probleme.NbVarDeBaseComplementaires = *NbVarDeBaseComplementaires;
Probleme.ComplementDeLaBase         = ComplementDeLaBase;
  
Probleme.LibererMemoireALaFin  = LibererMemoireALaFin;	


Probleme.CoutMax = CoutMax;
Probleme.UtiliserCoutMax = UtiliserCoutMax;

Probleme.NombreDeContraintesCoupes        = Pne->Coupes.NombreDeContraintes;
Probleme.BCoupes                          = Pne->Coupes.B;
Probleme.PositionDeLaVariableDEcartCoupes = Pne->Coupes.PositionDeLaVariableDEcart;
Probleme.MdebCoupes                       = Pne->Coupes.Mdeb;
Probleme.NbTermCoupes                     = Pne->Coupes.NbTerm;
Probleme.NuvarCoupes                      = Pne->Coupes.Nuvar;
Probleme.ACoupes                          = Pne->Coupes.A;

Probleme.CoutsMarginauxDesContraintes = Pne->VariablesDualesDesContraintesTravEtDesCoupes;

Probleme.CoutsReduits = Pne->CoutsReduits;
  
Probleme.AffichageDesTraces = Pne->AffichageDesTraces;

if ( Pne->DureeDuPremierSimplexe <= 0. ) time( &HeureDeCalendrierDebut );

Pne->ProblemeSpxDuSolveur = SPX_Simplexe( &Probleme , Pne->ProblemeSpxDuSolveur );

/* On renseigne le Simplexe pour qu'il sache qui l'appelle */
Spx = NULL;
if ( Pne->ProblemeSpxDuSolveur != NULL ) {
  Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
  Spx->ProblemePneDeSpx = (void *) Pne;
}

if ( Pne->DureeDuPremierSimplexe <= 0. ) {
  time( &HeureDeCalendrierCourant );
  TempsEcoule = difftime( HeureDeCalendrierCourant , HeureDeCalendrierDebut );
  Pne->DureeDuPremierSimplexe = TempsEcoule;
}

ExistenceDUneSolution = Probleme.ExistenceDUneSolution;

if ( ExistenceDUneSolution == OUI_PNE ) {

  *Faisabilite = OUI_PNE;
  *NbVarDeBaseComplementaires = Probleme.NbVarDeBaseComplementaires;

  Bb = (BB *) Pne->ProblemeBbDuSolveur;
	if ( Bb != NULL ) {
    Bb->NombreDeSimplexes++;
    if ( Spx != NULL ) Bb->SommeDuNombreDIterations += Spx->Iteration;    
	}

	/* Init de l'indicateur ContrainteSaturee */
  /* Si la variable d'ecart est basique on considere que la contrainte n'est pas saturee */
	ContrainteSaturee = Pne->ContrainteSaturee;	
	/* Decompte du nombre de variables duales nulles */
	Nb = 0;
  VariablesDualesDesContraintesTravEtDesCoupes = Pne->VariablesDualesDesContraintesTravEtDesCoupes;
  for ( i = 0 ; i < Pne->NombreDeContraintesTrav ; i++ ) {
	  ContrainteSaturee[i] = OUI_PNE;
    if ( VariablesDualesDesContraintesTravEtDesCoupes[i] == 0.0 ) Nb++;
	}
	for ( i = 0 ; i < *NbVarDeBaseComplementaires ; i++ ) ContrainteSaturee[ComplementDeLaBase[i]] = NON_PNE;
	
	S = (float) Nb / (float) Pne->NombreDeContraintesTrav;
	if ( S > SEUIL_POUR_PRIORITE_DANS_SPX_AUX_VARIABLES_SORTANTES_ENTIERES ) {
	  Pne->PrioriteDansSpxAuxVariablesSortantesEntieres = OUI_PNE;
	}
	else Pne->PrioriteDansSpxAuxVariablesSortantesEntieres = NON_PNE;
}
else if ( ExistenceDUneSolution == NON_PNE ) { 
  *Faisabilite = NON_PNE;  
}
else if ( ExistenceDUneSolution == SPX_MATRICE_DE_BASE_SINGULIERE ) {	
  *Faisabilite = NON_PNE;
  /* Si la matrice de base est singuliere et qu'on avait mis des coupes, c'est que les 
     coupes sont merdiques. On pourrait virer seulement les coupes ajoutee a ce 
     probleme particulier. Par precaution, on prefere virer toutes les coupes du pool car 
     experimentalement on constate que bien d'autres coupes peuvent etre merdiques */ 
  if ( Pne->Coupes.NombreDeContraintes > 0 ) {
    /*printf("-> Nettoyage des coupes pour cause d'instabilites numeriques \n");*/
    Bb = (BB *) Pne->ProblemeBbDuSolveur;
    BB_LeverLeFlagPourEnleverToutesLesCoupes( Bb );     
  }
}
else if ( ExistenceDUneSolution == SPX_ERREUR_INTERNE ) {
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}
else {
  printf("Bug dans PNE_SolvePbRlxSpxDual, ExistenceDUneSolution mal renseigne\n"); 
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}
  
return;
}

















