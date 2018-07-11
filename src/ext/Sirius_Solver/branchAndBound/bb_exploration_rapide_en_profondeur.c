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

   FONCTION: Exploration rapide en profondeur on attaquent directement
             le simplexe et sans calculer de coupes.
             Pour pour cela le noeud duquel on part doit etre evalue
             de facon classique avant la plongee.
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"
   
#include "spx_define.h"
#include "spx_fonctions.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

void BB_RemettreLesDonneesAvantInstanciation( PROBLEME_SPX * , int * , NOEUD * );
void BB_InstancierLaVariableDansLeSimplexe( PROBLEME_SPX * , int * , NOEUD * );
NOEUD * BB_EvaluationRapideDUnNoeud( BB * , PROBLEME_SPX * , PROBLEME_PNE * , int * );
																						
/*---------------------------------------------------------------------------------------------------------*/
void BB_LibererLaBaseSimplexeDuNoeud( NOEUD * Noeud )
{
if ( Noeud->BaseSimplexeDuNoeud != NULL ) {
  free( Noeud->BaseSimplexeDuNoeud->PositionDeLaVariable );
  free( Noeud->BaseSimplexeDuNoeud->InDualFramework );
  free( Noeud->BaseSimplexeDuNoeud->ContrainteDeLaVariableEnBase );
  free( Noeud->BaseSimplexeDuNoeud->DualPoids );
  free( Noeud->BaseSimplexeDuNoeud->VariableEnBaseDeLaContrainte );
  free( Noeud->BaseSimplexeDuNoeud );
  Noeud->BaseSimplexeDuNoeud = NULL;
}
return;
}
/*---------------------------------------------------------------------------------------------------------*/
void BB_RemettreLesDonneesAvantInstanciation( PROBLEME_SPX * Spx, int * TypeInstanciation,
                                              NOEUD * Noeud )
{
int VariablePneARemettre; 

VariablePneARemettre = Noeud->IndiceDeLaNouvelleVariableInstanciee;
Spx_RemettreLesDonneesAvantInstanciation( Spx , VariablePneARemettre , TypeInstanciation[VariablePneARemettre]);
TypeInstanciation[VariablePneARemettre] = SORT_PAS;

return;
}
/*---------------------------------------------------------------------------------------------------------*/
void BB_InstancierLaVariableDansLeSimplexe( PROBLEME_SPX * Spx, int * TypeInstanciation,
                                            NOEUD * Noeud )
{
int VariablePneAInstancier; int i;

VariablePneAInstancier = Noeud->IndiceDeLaNouvelleVariableInstanciee;
/* Determination du type de sortie */
i = Noeud->NombreDeVariablesEntieresInstanciees - 1;
if ( Noeud->ValeursDesVariablesEntieresInstanciees[i] == '0' ) TypeInstanciation[VariablePneAInstancier] = SORT_SUR_XMIN;
else TypeInstanciation[VariablePneAInstancier] = SORT_SUR_XMAX;

Spx_ModifierLesDonneesSurInstanciation( Spx , VariablePneAInstancier , TypeInstanciation[VariablePneAInstancier] ); 

return;
}
/*---------------------------------------------------------------------------------------------------------*/

NOEUD * BB_EvaluationRapideDUnNoeud( BB * Bb , PROBLEME_SPX * Spx , PROBLEME_PNE * Pne , int * TypeInstanciation )
{
NOEUD * NoeudEnExamen; int VariablePneAInstancier; int YaUneSolution;
NOEUD * NoeudSuivant; int SortSurXmaxOuSurXmin; 

NoeudSuivant = NULL;

Bb->SolutionAmelioranteTrouvee = NON;
NoeudEnExamen                  = Bb->NoeudEnExamen;

if ( NoeudEnExamen->StatutDuNoeud != A_EVALUER ) return( NoeudSuivant );

/* On instancie la variable */
BB_InstancierLaVariableDansLeSimplexe( Spx, TypeInstanciation, NoeudEnExamen );

Spx->ExplorationRapideEnCours = OUI_SPX;
SPX_DualSimplexe( Spx );
Spx->ExplorationRapideEnCours = NON_SPX;

Bb->NombreDeProblemesResolus++;
Bb->NombreDeProblemesRelaxesResolus++;

YaUneSolution = Spx->YaUneSolution;

/* N'instancie pas de variable si pas de solution */
SPX_ChoisirLaVariableAInstancier( Spx, (void *) Bb, &VariablePneAInstancier, &SortSurXmaxOuSurXmin );
				       
if ( Spx->YaUneSolution == OUI_SPX ) {
  if ( VariablePneAInstancier >= 0 ) {
    /* Appelle aussi BB_NettoyerLArbre et fait les sauvegardes de la base de depart */
    Spx_CreationNoeudsFils( Spx, (void *) Pne, (void *) Bb, VariablePneAInstancier );
			     
    if ( SortSurXmaxOuSurXmin == SORT_SUR_XMAX ) NoeudSuivant = NoeudEnExamen->NoeudSuivantDroit;
    else NoeudSuivant = NoeudEnExamen->NoeudSuivantGauche;
  }
  else {  
    /* La solution est entiere */
    printf("Solution entiere trouvee cout %e\n",Spx->Cout);

    SPX_RecupererLaSolutionSiExplorationRapide( Spx , Pne , Bb , (void *) NoeudEnExamen , TypeInstanciation ); 
    BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudEnExamen ); /* Fait aussi la mise a jour du statut */


    /* Pour tests */
    PNE_CalculerLaValeurDuCritere( Pne );
    printf("Calcul du critere PNE %e  SolutionAmelioranteTrouvee %d\n",Pne->Critere,Bb->SolutionAmelioranteTrouvee);
    
  }
}
else {
  BB_NettoyerLArbre( Bb, &YaUneSolution , NoeudEnExamen ); /* Fait aussi la mise a jour du statut */
}

return( NoeudSuivant );
}

/*---------------------------------------------------------------------------------------------------------*/
/* On cherche un noeud fils non encore evalue et on l'evalue */

NOEUD * BB_PreparerExplorationRapideEnProfondeur( BB * Bb , NOEUD * NoeudDuMeilleurMinorant )
{
NOEUD * Noeud; int YaUneSolution; int SolutionEntiereTrouvee;

PreparationExploration:
Bb->SolutionAmelioranteTrouvee = NON;
Noeud = NoeudDuMeilleurMinorant->NoeudSuivantGauche;
if ( Noeud != NULL ) {
  if ( Noeud->StatutDuNoeud != A_EVALUER ) {
    Noeud = NoeudDuMeilleurMinorant->NoeudSuivantDroit;
    if ( Noeud != NULL ) {
      if ( Noeud->StatutDuNoeud != A_EVALUER ) Noeud = NULL;     
    }
  }
}
if ( Noeud == NULL ) return( NULL ); /* Rien a faire */

Bb->SolutionAmelioranteTrouvee = NON;
Bb->NoeudEnExamen              = Noeud;

/* Resolution du noeud */
/* Ne pas oublier d'exploiter " SolutionEntiereTrouvee " */
YaUneSolution = BB_ResoudreLeProblemeRelaxe( Bb , Noeud , &SolutionEntiereTrouvee );
      
BB_NettoyerLArbre( Bb, &YaUneSolution , Noeud ); /* Fait aussi la mise a jour du statut */

BB_CreerLesNoeudsFils( Bb , Noeud );    
      
if ( YaUneSolution != OUI ) goto PreparationExploration;
if ( Noeud->NoeudSuivantDroit == NULL || Noeud->NoeudSuivantGauche == NULL ) {
  /* Cas le Noeud a une solution entiere */
  goto PreparationExploration;
}

return( Noeud );
}

/*---------------------------------------------------------------------------------------------------------*/

void BB_ExplorationRapideEnProfondeur( BB * Bb )
{
PROBLEME_PNE * Pne; PROBLEME_SPX * Spx; 
int * TypeInstanciation; NOEUD * NoeudEnExamen; int i; NOEUD * NoeudPere; NOEUD * NoeudEvalue;
NOEUD * NoeudSuivantDroit; NOEUD * NoeudSuivantGauche;
NOEUD * NoeudDuMeilleurMinorant; NOEUD * NoeudDeDepart; int NbProb;
char TerminerLaRecherche; int Fois; int MxProb; char TypeDeRecherche;

clock_t debut; clock_t Fin; double Duree;

printf(" BB_ExplorationRapideEnProfondeur\n");

Duree = 0.0;
debut = clock();

Fois = 1;

Bb->SolutionAmelioranteTrouvee = NON;

/* Precaution */

/* Attention aux longjmp faire un setjmp */

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;
Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;

TypeInstanciation = (int *) malloc( Bb->NombreDeVariablesDuProbleme * sizeof( int ) );
if ( TypeInstanciation == NULL ) return;
for ( i = 0 ; i < Bb->NombreDeVariablesDuProbleme ; i++ ) TypeInstanciation[i] = SORT_PAS;

/*AAA:*/
/* Choix d'un noeud prometteur */
if ( Bb->NombreDeSolutionsEntieresTrouvees != 0 ) TypeDeRecherche = RECHERCHER_LE_PLUS_PETIT_GAP_PROPORTIONNEL;
else TypeDeRecherche = RECHERCHER_LE_PLUS_PETIT;

BB_RechercherLeMeilleurMinorant( Bb, TypeDeRecherche );
if ( Bb->NoeudDuMeilleurMinorant == NULL ) return;

NoeudDuMeilleurMinorant = Bb->NoeudDuMeilleurMinorant;

ChoixArborescence:
TerminerLaRecherche = NON;
NbProb = 1;

/* On evalue un noeud fils du NoeudDuMeilleurMinorant. La fonction renvoie NoeudEvalue qui est
   le noeud fils de NoeudDuMeilleurMinorant qu'elle a evalue */
   
Bb->CalculerDesCoupes = NON_PNE;
NoeudEvalue = BB_PreparerExplorationRapideEnProfondeur( Bb , NoeudDuMeilleurMinorant );
Bb->CalculerDesCoupes = NON_PNE;

if ( NoeudEvalue == NULL ) {
  TerminerLaRecherche = OUI; 
  goto FinExploration; /* Attention c'est pas clair */
}

NoeudEnExamen = NoeudEvalue->NoeudSuivantDroit;
if ( NoeudEnExamen != NULL ) {
  if ( NoeudEnExamen->StatutDuNoeud == A_EVALUER ) SPX_SauvegarderLaBaseDeDepart( Spx , (void *) NoeudEnExamen );
}
NoeudEnExamen = NoeudEvalue->NoeudSuivantGauche;
if ( NoeudEnExamen != NULL ) {
  if ( NoeudEnExamen->StatutDuNoeud == A_EVALUER ) SPX_SauvegarderLaBaseDeDepart( Spx , (void *) NoeudEnExamen );
}

/* Evaluation en profondeur */
ExplorerLes2BranchesDUnFilsDeMeilleurMinorant:
Bb->NoeudEnExamen = NULL;
NoeudEnExamen = NoeudEvalue->NoeudSuivantDroit;
if ( NoeudEnExamen != NULL ) {
  if ( NoeudEnExamen->StatutDuNoeud != A_EVALUER ) {
    NoeudEnExamen = NoeudEvalue->NoeudSuivantGauche;
    if ( NoeudEnExamen != NULL ) {
      if ( NoeudEnExamen->StatutDuNoeud != A_EVALUER ) NoeudEnExamen = NULL;    
    }
  }
}
if ( NoeudEnExamen == NULL ) goto ChoixArborescence;

/* On recupere la base du NoeudEvalue comme base de depart */
SPX_InitialiserLaBaseDeDepart( Spx , (void *) NoeudEnExamen );
/* Le strong branching ne remet pas a jour le tableau des variables hors base une fois qu'il a tourne */
SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );

Spx->FaireScalingLU = 0;
Spx->StrongBranchingEnCours = NON_SPX;
Spx->UtiliserLaLuUpdate = OUI_SPX;

/* Car on repart d'un noeud ou la base n'est pas forcement factorisee */
SPX_FactoriserLaBase( Spx );

NoeudDeDepart = NoeudEnExamen;

/* Evaluation en profondeur */
EvaluationEnProfondeur:
while( NoeudEnExamen != NULL ) {
  Bb->NoeudEnExamen = NoeudEnExamen;
  /* BB_EvaluationRapideDUnNoeud renvoit NULL si on ne peut plus descendre */
  /*
  printf("evaluation noeud %X profondeur %d variable instanciee %d nb prob resolus %d\n",
          NoeudEnExamen,NoeudEnExamen->ProfondeurDuNoeud,NoeudEnExamen->IndiceDeLaNouvelleVariableInstanciee,NbProb);
  */
  NoeudEnExamen = BB_EvaluationRapideDUnNoeud( Bb , Spx , Pne , TypeInstanciation );
  NbProb++;
  MxProb = (int) ( 0.5 * Bb->NombreDeVariablesEntieresDuProbleme );
  if ( MxProb < 100 ) MxProb = 100;
  if ( NbProb > MxProb /*&& Bb->NombreDeSolutionsEntieresTrouvees != 0*/ ) {     
    TerminerLaRecherche = OUI;
    break;
  }
  
  Fin = clock();
  Duree = ( (double) ( Fin - debut) ) / CLOCKS_PER_SEC;
  printf("NbProb %d sur MxProb %d Duree %e\n",NbProb,MxProb,Duree);
  
  if ( Duree > 5.0 ) {
    TerminerLaRecherche = OUI;
    break;
  }
  
}
/*
printf("Fin de descente\n");
*/
/* Si on arrive la c'est qu'il faut remonter car on est tombe sur un noeud terminal */
NoeudEnExamen = Bb->NoeudEnExamen; /* C'est le dernier noeud examine */
NoeudPere     = (NOEUD *) NoeudEnExamen->NoeudAntecedent;

/* Si on a trouve une solution entiere on va a la fin */
if ( NoeudEnExamen->LaSolutionRelaxeeEstEntiere == OUI && Bb->SolutionAmelioranteTrouvee == OUI ) {
  /* Si on a trouve une solution entiere on remonte jusqu'au noeud de depart pour reliberer
     les variables et desallouer les bases simplexe */
  TerminerLaRecherche = OUI;
}

while ( 1 ) {
  if ( NoeudEnExamen == NoeudDeDepart ) {
    /* Cas d'echec au premier noeud */
    BB_RemettreLesDonneesAvantInstanciation( Spx, TypeInstanciation, NoeudDeDepart );
    NoeudEnExamen = NULL;
    break;
  }
  NoeudSuivantDroit  = (NOEUD *) NoeudPere->NoeudSuivantDroit;
  NoeudSuivantGauche = (NOEUD *) NoeudPere->NoeudSuivantGauche;
  /* On remet la variable */
  if ( NoeudSuivantDroit != NULL ) {
    BB_RemettreLesDonneesAvantInstanciation( Spx, TypeInstanciation, NoeudSuivantDroit );
  }
  else if ( NoeudSuivantGauche != NULL ) {
    BB_RemettreLesDonneesAvantInstanciation( Spx, TypeInstanciation, NoeudSuivantGauche );
  }
  /* NoeudPere a forcement ete evalue si un des fils n'a pas ete evalue, on repart de ce noeud la */
  if ( NoeudSuivantDroit != NULL ) {
    if ( TerminerLaRecherche == NON ) {
      if ( NoeudSuivantDroit->StatutDuNoeud == A_EVALUER ) {    
        NoeudEnExamen = NoeudSuivantDroit;
        break;
      }
    }
    else BB_LibererLaBaseSimplexeDuNoeud( NoeudSuivantDroit );          
  }
  if ( NoeudSuivantGauche  != NULL ) {
    if ( TerminerLaRecherche == NON ) {
      if ( NoeudSuivantGauche->StatutDuNoeud == A_EVALUER ) {    
        NoeudEnExamen = NoeudSuivantGauche;
        break;
      }
    }
    else BB_LibererLaBaseSimplexeDuNoeud( NoeudSuivantGauche );          
  }
  if ( NoeudPere == NoeudDeDepart ) {
    /* Les 2 branches qui partent du NoeudDeDepart on ete explorees */
    BB_RemettreLesDonneesAvantInstanciation( Spx, TypeInstanciation, NoeudDeDepart );    
    NoeudEnExamen = NULL;
    break;      
  }  
  NoeudPere = NoeudPere->NoeudAntecedent;       
}
if ( NoeudEnExamen != NULL ) {
  SPX_InitialiserLaBaseDeDepart( Spx , (void *) NoeudEnExamen ); 
  SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );

  Spx->FaireScalingLU = 0;
  Spx->StrongBranchingEnCours = NON_SPX;
  Spx->UtiliserLaLuUpdate = OUI_SPX;

  /* Car on repart d'un noeud ou la base n'est pas forcement factorisee */
  SPX_FactoriserLaBase( Spx );
  
  goto EvaluationEnProfondeur;
}
/* Plus de noeuds a explorer ni a gauche ni a droite de NoeudDeDepart => on va chercher
   a explorer un autre fils du noeud fils du meilleur minorant */
if ( TerminerLaRecherche == NON ) goto ExplorerLes2BranchesDUnFilsDeMeilleurMinorant;

FinExploration:

for ( i = 0 ; i < Bb->NombreDeVariablesDuProbleme ; i++ ) {
  if ( TypeInstanciation[i] != SORT_PAS ) {
    printf("TypeInstanciation pas bien remis\n");
    exit(0);
  }
}

Fois++;

if ( Bb->NombreDeSolutionsEntieresTrouvees == 0 && 0 ) {
  NoeudEnExamen = BB_RechercherLeNoeudLeMoinsFractionnaire( Bb , MOINS_DE_VALEURS_FRACTIONNAIRES );
  if ( NoeudEnExamen != NULL ) {
    Bb->NoeudEnExamen = NoeudEnExamen;
   /* goto ChoixArborescence; *//* Non car noeud sauvegarde */
  }
}
else {
  if ( Bb->SolutionAmelioranteTrouvee == NON ) {
    if ( Fois < 2 && NbProb <= ceil( 0.1 * (float) Bb->NombreDeVariablesEntieresDuProbleme ) ) {
      /* goto AAA; */ /* Non car noeud sauvegarde */
    }
  }
}

free( TypeInstanciation );
TypeInstanciation = NULL;

return;
}
