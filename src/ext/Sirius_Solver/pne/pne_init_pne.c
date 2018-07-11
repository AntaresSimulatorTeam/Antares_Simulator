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

   FONCTION: 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_fonctions.h"  

# define TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES  1.e-5

/*----------------------------------------------------------------------------*/

void PNE_InitialiserLaPne( PROBLEME_PNE * Pne, PROBLEME_A_RESOUDRE * Probleme )   
{
int NombreDeVariablesE; int * TypeDeVariableE; int * TypeDeBorneTravE;
double * UE; double * UmaxE; double * UminE; double * LE; int NombreDeContraintesE;
double * BE; char * SensE; int * MdebE; int * NbtermE; double * AE; int * NuvarE;
double * VariablesDualesDesContraintesE;
int i    ; int j       ; int ilTrav   ; int ilTravSv; int NouvelleContrainte;
int ilE  ; int ilEmax  ; int VarE     ; double S     ; double APrime          ;
int iPrec; int Cnt     ; int il       ; int ilMax   ; int NbContraintesIneg ;
int Var  ; char Gub     ; char OnInverse; char YaQueDesVarFixes;
double Amax; int ic; double * SeuilDeFractionnalite; int * TypeDeVariableTrav;
int * TypeDeBorneTrav; int * CdebTrav; int * CsuiTrav; double * ATrav;

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("Attention nouvelle version de INIT pne qui conserve les variables fixes d'entree\n");
}

NombreDeVariablesE = Probleme->NombreDeVariables;
TypeDeVariableE = Probleme->TypeDeVariable;
TypeDeBorneTravE = Probleme->TypeDeBorneDeLaVariable;
UE = Probleme->X;
UmaxE = Probleme->Xmax;
UminE = Probleme->Xmin;
LE = Probleme->CoutLineaire;
NombreDeContraintesE = Probleme->NombreDeContraintes;
BE = Probleme->SecondMembre;
SensE = Probleme->Sens;
MdebE = Probleme->IndicesDebutDeLigne;
NbtermE = Probleme->NombreDeTermesDesLignes;
AE = Probleme->CoefficientsDeLaMatriceDesContraintes;
NuvarE= Probleme->IndicesColonnes;
VariablesDualesDesContraintesE = Probleme->VariablesDualesDesContraintes;

Pne->DernierTauxParCoupe = -1.;

Pne->YaDesVariablesEntieres = NON_PNE;
Pne->NumeroDeLaContrainteDeCoutMax = -1;
Pne->NumeroDeLaVariableDEcartPourCoutMax = -1;

Pne->PrioriteDansSpxAuxVariablesSortantesEntieres = NON_PNE;

Pne->ChainageTransposeeExploitable = NON_PNE;

Pne->NombreDeGub = 0;

Pne->Z0 = 0.0; /* Cout fixe lie aux transformation: translations de borne, presolve etc.. */

NbContraintesIneg = 0;


/* On met une valeur invalide afin de pouvoir verifier a la fin si toutes les valeurs ont ete initialisees */
S = 2 * VALEUR_NON_INITIALISEE;
if ( VariablesDualesDesContraintesE != NULL ) {
  for ( i = 0 ; i < NombreDeContraintesE ; i++ ) VariablesDualesDesContraintesE[i] = S;
}

/* On conserve les variables fixes dans le probleme pne */
for ( j = 0 , i = 0 ; i < NombreDeVariablesE ; i++ ) {
  Pne->CorrespondanceVarEntreeVarNouvelle[i] = -1;
	
  Pne->TypeDeVariableTrav[j] = TypeDeVariableE[i];
  if ( Pne->TypeDeVariableTrav[j] == ENTIER ) {
    if ( Pne->SolveurPourLeProblemeRelaxe == SIMPLEXE || MPCC_DANS_PI == OUI_PNE ) Pne->YaDesVariablesEntieres = OUI_PNE;
    else Pne->TypeDeVariableTrav[j] = REEL;
    if ( TypeDeBorneTravE[i] != VARIABLE_BORNEE_DES_DEUX_COTES && TypeDeBorneTravE[i] != VARIABLE_FIXE ) {
		if ( Pne->AffichageDesTraces == OUI_PNE ) {
		  printf("Erreur dans les donnees, la variable %d est entiere mais elle n'est pas bornee des 2 cotes\n",i);
			printf("Arret du solveur car seules les variables entieres de type binaire sont acceptees\n");
		}
      Pne->AnomalieDetectee = OUI_PNE;
      longjmp( Pne->Env , Pne->AnomalieDetectee );       
    }
    if ( UmaxE[i] - UminE[i] != 1.0 && TypeDeBorneTravE[i] != VARIABLE_FIXE ) {
		if ( Pne->AffichageDesTraces == OUI_PNE ) {
		  printf("Erreur dans les donnees, la variable %d est entiere mais elle n'est pas de type binaire\n",i);
		  printf("Min %e  Max %e Ecart %20.17f\n",UminE[i],UmaxE[i],UmaxE[i]-UminE[i]);
			printf("Arret du solveur car seules les variables entieres de type binaire sont acceptees\n");
		}
      Pne->AnomalieDetectee = OUI_PNE;
      longjmp( Pne->Env , Pne->AnomalieDetectee );       
    }      
  }
  Pne->TypeDeBorneTrav   [j] = TypeDeBorneTravE[i];
  Pne->VariableAInverser [j] = NON_PNE;
  Pne->UTrav   [j] = UE[i]; /* Pour disposer de la valeur des variables qui sont fixes en entree */
  Pne->UmaxTrav[j] = UmaxE[i];
  Pne->UminTrav[j] = UminE[i];
  if ( TypeDeBorneTravE[i] == VARIABLE_BORNEE_INFERIEUREMENT ) { 
    Pne->UmaxTrav[j] = LINFINI_PNE;
  }
  else if ( TypeDeBorneTravE[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    Pne->UminTrav[j] = -LINFINI_PNE;
  }
  else if ( TypeDeBorneTravE[i] == VARIABLE_NON_BORNEE ) {
    Pne->UmaxTrav[j] = LINFINI_PNE;
    Pne->UminTrav[j] = -LINFINI_PNE;
  }
  Pne->LTrav[j] = LE[i];		
  Pne->CorrespondanceVarEntreeVarNouvelle[i] = j;    
  j++;  
  if ( j >= Pne->NombreDeVariablesAllouees ) PNE_AugmenterLeNombreDeVariables( Pne );  
  
}

Pne->NombreDeVariablesElimineesSansValeur = 0;

Pne->NombreDeVariablesTrav = j;

/* Cas particulier ou toutes les variables sont fixes: on verifie que la solution est bonne */
/* Alexandre Ortiz m'a sorti un cas ou toutes les variables etaient fixes */
if ( Pne->NombreDeVariablesTrav == 0 ) {
  for ( i = 0 ; i < NombreDeContraintesE ; i++ ) {	
    S = 0.0;
    ilE    = MdebE[i];
    ilEmax = MdebE[i] + NbtermE[i] - 1;
		/* Correction du 15/3/13: si la contrainte est desactivee en entree, il n'y a pas lieu de faire un test */
		if ( ilE < 0 || NbtermE[i] <= 0 ) {
      if ( VariablesDualesDesContraintesE != NULL ) VariablesDualesDesContraintesE[i] = 0.0;	
		  continue; /* Car en realite la contrainte n'existe pas */
		}
    while ( ilE <= ilEmax ) {
      S += AE[ilE] * UE[NuvarE[ilE]];
			ilE++;
    }
    if ( SensE[i] == '=' ) {
      if ( fabs( S -  BE[i] ) > TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES ) {
			  Pne->YaUneSolution = PROBLEME_INFAISABLE;
        break;
      }
		}
		else if ( SensE[i] == '<' ) {
      if ( S >  BE[i] + TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES ) {
			  Pne->YaUneSolution = PROBLEME_INFAISABLE;
        break;
			}
    }
		else if ( SensE[i] == '>' ) {
      if ( S <  BE[i] - TOLERANCE_SUR_LES_VIOLATIONS_DE_CONTRAINTES ) {
			  Pne->YaUneSolution = PROBLEME_INFAISABLE;
        break;
      }		
		}
  }
}

/* Transfert dans les tableaux de travail: les contraintes de type > 
   sont transformees en contraintes de type < et les variables fixées sont 
   eliminees du probleme */
 
Pne->NombreDeContraintesTrav = 0;
for ( ilTrav = -1 , i = 0 ; i < NombreDeContraintesE ; i++ ) {
	/* Correction du 15/3/13: si la contrainte est desactivee en entree, il n'y a pas lieu de continuer dans la boucle */
	if ( MdebE[i] < 0 || NbtermE[i] <= 0 ) {
	  /* Car en realite la contrainte n'existe pas */	
    if ( VariablesDualesDesContraintesE != NULL ) VariablesDualesDesContraintesE[i] = 0.0;	
    continue;
	}
  NouvelleContrainte = NON_PNE;	
  Pne->NombreDeContraintesTrav++; 
  if ( Pne->NombreDeContraintesTrav >= Pne->NombreDeContraintesAllouees ) PNE_AugmenterLeNombreDeContraintes( Pne );

  ilTravSv = ilTrav;

  ilE    = MdebE[i];
  ilEmax = MdebE[i] + NbtermE[i] - 1;
	
  if ( ( ilTrav + 1 ) >= Pne->TailleAlloueePourLaMatriceDesContraintes ) PNE_AugmenterLaTailleDeLaMatriceDesContraintes( Pne );

  Pne->MdebTrav  [Pne->NombreDeContraintesTrav - 1] = ilTrav + 1;
  Pne->NbTermTrav[Pne->NombreDeContraintesTrav - 1] = 0;
  
  S = 0.; iPrec = -1;

  while ( ilE <= ilEmax ) {
    VarE = NuvarE[ilE];		
    if ( iPrec == VarE ) { 
     if ( Pne->AffichageDesTraces == OUI_PNE ) printf("Erreur, la contrainte %i contient 2 fois la variable %d\n",i,iPrec); 
      Pne->AnomalieDetectee = OUI_PNE;
      longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
    }
		
    if ( TypeDeBorneTravE[VarE] != VARIABLE_FIXE ) NouvelleContrainte = OUI_PNE;
		else {
      APrime = AE[ilE];
      if ( SensE[i] == '>' ) APrime = -AE[ilE];
      /* printf("Variable fixee dans TRANSFERT PROB \n");*/
      S += APrime * UE[VarE];			
		}
    Pne->NbTermTrav[Pne->NombreDeContraintesTrav - 1]++;
    ilTrav++;
    if ( ilTrav >= Pne->TailleAlloueePourLaMatriceDesContraintes ) PNE_AugmenterLaTailleDeLaMatriceDesContraintes( Pne );
    Pne->ATrav[ilTrav] = AE[ilE];				
    if ( SensE[i] == '>' ) Pne->ATrav[ilTrav] = -AE[ilE];
		
    j = Pne->CorrespondanceVarEntreeVarNouvelle[VarE];				
    if ( j >= 0 ) Pne->NuvarTrav[ilTrav] = j;
		else if ( Pne->AffichageDesTraces == OUI_PNE ) printf("BUG dans PNE_InitialiserLaPne, vecteur CorrespondanceVarEntreeVarNouvelle mal initialise\n");
         
    iPrec = NuvarE[ilE];
    ilE++;
  }

  if ( NouvelleContrainte == NON_PNE ) {	
		if ( VariablesDualesDesContraintesE != NULL ) VariablesDualesDesContraintesE[i] = 0.0;	
    Pne->NombreDeContraintesTrav--;  
    ilTrav = ilTravSv;
    /* On controle la satisfaction de la contrainte */
    if ( SensE[i] == '=' ) {
      if ( fabs( S - BE[i] ) > SEUIL_DADMISSIBILITE ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
      }
    }
    else if ( SensE[i] == '<' ) {
      if ( S > BE[i] + SEUIL_DADMISSIBILITE ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
      }
    }
    else {
      /* Contrainte de type > */
			/* Copier coller de la ligne ci-dessus malencontreux !!!!!!!!!!!!!!!!*/
      /* Comme on a change le signe des coefficients pour anticiper le changement de sens de l'inegalite,
			   on doit faire pareil pour le second membre */
      /*if ( S > BE[i] + SEUIL_DADMISSIBILITE ) {*/
      if ( S > -BE[i] + SEUIL_DADMISSIBILITE ) {
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
      }
    }    
  }
  else {		
    Pne->CorrespondanceCntPneCntEntree[Pne->NombreDeContraintesTrav - 1] = i;
    Pne->BTrav             [Pne->NombreDeContraintesTrav - 1] = BE[i];
    Pne->SensContrainteTrav[Pne->NombreDeContraintesTrav - 1] = SensE[i];
    if ( SensE[i] == '>' ) { 
      Pne->BTrav             [Pne->NombreDeContraintesTrav - 1] = -BE[i];
      Pne->SensContrainteTrav[Pne->NombreDeContraintesTrav - 1] = '<';
    }				
    if ( Pne->SensContrainteTrav[Pne->NombreDeContraintesTrav - 1] != '=' ) NbContraintesIneg++;
		/* On ajoute toujours de la marge utilisable pour le presolve */
    ilTrav += MARGE_EN_FIN_DE_CONTRAINTE;
    if ( ilTrav >= Pne->TailleAlloueePourLaMatriceDesContraintes ) PNE_AugmenterLaTailleDeLaMatriceDesContraintes( Pne );    		
  }		       
}

Pne->NombreDeContraintesDInegalite = NbContraintesIneg;
Pne->PremierIndexLibre = ilTrav + 1;

# if PNE_ACTIVATION_SUPPRESSION_PETITS_TERMES == OUI_PNE
  PNE_EnleverLesToutPetitsTermes( Pne->MdebTrav, Pne->NbTermTrav, Pne->NuvarTrav, Pne->ATrav,
	                                Pne->UmaxTrav, Pne->UminTrav, Pne->NombreDeContraintesTrav,
																	Pne->AffichageDesTraces );																			
# endif

/* Presolve */

if ( Pne->FaireDuPresolve == OUI_PNE ) PNE_AllocationsPourLePostSolve( Pne );

PRS_Presolve( Pne );

if ( Pne->Controls != NULL ) {
  if ( Pne->Controls->PresolveUniquement == OUI_PNE ) {
	  PNE_CleanPostSolve( Pne );
	  return;
	}
}

/* Test: on essaie de transformer des variables entieres en variables continues */
/*PNE_ChangerLesTypesDeVariables( Pne );*/
/* Fin test */

PNE_CompacterLaMatriceDesContraintes( Pne );

# if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE
  # if PROBING_JUSTE_APRES_LE_PRESOLVE == OUI_PNE
    if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {		
      PNE_VariableProbing( Pne );			
      if ( Pne->CoupesDeProbing != NULL ) {
			  if ( Pne->AffichageDesTraces == OUI_PNE ) {
			    printf("%d probing cut(s) found\n",Pne->CoupesDeProbing->NombreDeCoupesDeProbing);
				}
			}			
	    if ( Pne->ProbingOuNodePresolve != NULL ) {			  
	      if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) Pne->YaUneSolution = NON_PNE;
			  else PNE_CliquesConflictGraph( Pne );			  
      }			
      /* Le variable probing peut conduire aussi a des possibilites d'amelioration de coeffs
			   de variables binaires */				 
      PNE_AmeliorerLesCoefficientsDesVariablesBinaires( Pne, NULL, MODE_PNE );
			
			/* Pour tenir compte des coupes de probing */
      PNE_ProbingModifierLaMatriceDesContraintes( Pne, Pne->ProbingOuNodePresolve );			
      /* Il se peut que des contraintes soient devenues inactives ou fixing */			
      PNE_PostProbing( Pne );
      /* Nouveau compactage au cas ou des variables entieres auraient ete fixees */
      PNE_CompacterLaMatriceDesContraintes( Pne );			
    }	
  # endif
# endif

/*
PNE_EcrirePresolvedMPS( Pne );
exit(0);
*/

if ( Pne->FaireDuPresolve == OUI_PNE ) PNE_CleanPostSolve( Pne );

PNE_TranslaterLesBornes( Pne );

/* Cas ou le solveur est appele par lui-meme */
if ( Pne->Controls != NULL &&  Pne->YaUneSolution == OUI_PNE && 0 ) {
	{ int NbFix; int NbVarEnt; PROBLEME_PNE * PnePere; int NbNonFix; double Seuil;
	  NbFix = 0;
		NbVarEnt = 0;
    for ( Var = 0 ;  Var < Pne->NombreDeVariablesTrav ; Var++ ) {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) NbFix++;
			else if ( Pne->UmaxTrav[Var] == Pne->UminTrav[Var] ) NbFix++;
			else if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) NbVarEnt++;
		}
		PnePere = (PROBLEME_PNE * ) (Pne->Controls->Pne);
			NbNonFix = Pne->NombreDeVariablesTrav-NbFix;
	   if ( Pne->AffichageDesTraces == OUI_PNE ) 
	   {
			   printf("Init PNE NombreDeContraintesTrav %d NombreDeVariablesTrav %d  No non Fix %d  \n",Pne->NombreDeContraintesTrav,
				  Pne->NombreDeVariablesTrav, NbNonFix);
						
			   printf("Init PNE nombre de variables entieres non fixes %d   et dans le pere  %d  \n",
				  NbVarEnt, PnePere->NombreDeVariablesEntieresNonFixes);
			  printf("Init PNE nombre de variables non fixes %d   et dans le pere  %d  \n",
				  NbNonFix, PnePere->NombreDeVariablesNonFixes);
	   }
		
	   Seuil = 0.1;
		if ( PnePere->YaUneSolutionEntiere == NON_PNE ) Seuil = 0.5;
		if ( NbVarEnt > (int) (Seuil * PnePere->NombreDeVariablesEntieresNonFixes) && NbVarEnt > 50 ) {
			if ( Pne->AffichageDesTraces == OUI_PNE ) {
			  printf("Refus car trop de var ent  NbVarEnt  %d   Seuil * PnePere->NombreDeVariablesEntieresNonFixes %d\n",
						NbVarEnt,(int) (Seuil * PnePere->NombreDeVariablesEntieresNonFixes) );
			  printf("NombreDeVariablesEntieresNonFixes %d\n", PnePere->NombreDeVariablesEntieresNonFixes);
			}
		  Pne->YaUneSolution = NON_PNE;
		}  
		if ( NbNonFix > (int) (Seuil * PnePere->NombreDeVariablesNonFixes) && NbNonFix > 50 ) {  
			if ( Pne->AffichageDesTraces == OUI_PNE ) printf("Refus car trop de var\n");
			Pne->YaUneSolution = NON_PNE;
		}
  }
}

if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {

  /*
  printf("PNE_DetectionContraintesEntieresInfaisable\n");
  PNE_DetectionContraintesEntieresInfaisable( Pne );
  */	

  /* Recherche de GUB */
  PNE_DetecterLesGub( Pne );

  Pne->CalculDeMIRmarchandWolsey = OUI_PNE;
  Pne->NbEvitementsDeCalculsMIRmarchandWolsey = 0;
  Pne->NbEchecsConsecutifsDeCalculsMIRmarchandWolsey = 0;	
  Pne->ProfondeurMirMarchandWolseTrouvees = 0;	

  /* Detection des bornes variables */
	PNE_AnalyseInitialeDesBornesVariables( Pne );	
	
  /* Detection des contraintes sur lesquelles on pourrait faire des Knapsack */
  PNE_AnalyseInitialeDesKnapsack( Pne );	
	
	/* Detection des contraintes mixtes. Imperativement apres detection des bornes variables */
	PNE_DetecterLesContraintesMixtes( Pne );	
	
}

Pne->YaDesBigM = NON_PNE;  
/* Detection de variables binaires avec un Big M */
PNE_DetecterLesVariablesBigM( Pne );

/* Test recherche de epsilon des variables entieres */
SeuilDeFractionnalite = Pne->SeuilDeFractionnalite;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
ATrav = Pne->ATrav;
 for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( TypeDeVariableTrav[Var] != ENTIER ) continue;
  if ( TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;
	Amax = -1;
	ic = CdebTrav[Var];
  while ( ic >= 0 ) {
    if ( fabs( ATrav[ic] ) > Amax ) Amax = fabs( ATrav[ic] );
		ic = CsuiTrav[ic];
  }	
	SeuilDeFractionnalite[Var] = VALEUR_DE_FRACTIONNALITE_NULLE;
	if ( Amax > 0 ) {
	  SeuilDeFractionnalite[Var] = SEUIL_DADMISSIBILITE / Amax;		
		if ( SeuilDeFractionnalite[Var] < 0.1 * VALEUR_DE_FRACTIONNALITE_NULLE ) {  
		  SeuilDeFractionnalite[Var] = 0.1 * VALEUR_DE_FRACTIONNALITE_NULLE;
		}		
	}
}

Pne->Coupes.NombreDeContraintesAllouees              = 0;
Pne->Coupes.TailleAlloueePourLaMatriceDesContraintes = 0;
Pne->Coupes.NombreDeContraintes                      = 0;

Pne->DureeDuPremierSimplexe = -1.;

/* Sauvegarde des bornes sup apres translation et scaling */
memcpy( (char * ) Pne->UmaxTravSv, (char * ) Pne->UmaxTrav, Pne->NombreDeVariablesTrav * sizeof( double ) );  
memcpy( (char * ) Pne->UminTravSv, (char * ) Pne->UminTrav, Pne->NombreDeVariablesTrav * sizeof( double ) );  
memcpy( (char * ) Pne->TypeDeBorneTravSv, (char * ) Pne->TypeDeBorneTrav, Pne->NombreDeVariablesTrav * sizeof( int ) );

Pne->FaireHeuristiqueRINS = OUI_PNE;
Pne->StopHeuristiqueRINS = NON_PNE;
Pne->NombreDeSolutionsHeuristiqueRINS = 0;
Pne->NombreDEchecsSuccessifsHeuristiqueRINS = 0;
Pne->NombreDeRefusSuccessifsHeuristiqueRINS = 0;
Pne->NombreDeReactivationsSansSuccesHeuristiqueRINS = 0;

Pne->FaireHeuristiqueFixation = OUI_PNE;
Pne->StopHeuristiqueFixation = NON_PNE;
Pne->NombreDeSolutionsHeuristiqueFixation = 0;
Pne->NombreDEchecsSuccessifsHeuristiqueFixation = 0;
Pne->NombreDeRefusSuccessifsHeuristiqueFixation = 0;
Pne->NombreDeReactivationsSansSuccesHeuristiqueFixation = 0;

Pne->FaireHeuristiqueFractionalDive = OUI_PNE;
Pne->StopHeuristiqueFractionalDive = NON_PNE;
Pne->NombreDeSolutionsHeuristiqueFractionalDive = 0;
Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive = 0;
Pne->NombreDeRefusSuccessifsHeuristiqueFractionalDive = 0;
Pne->NombreDeReactivationsSansSuccesHeuristiqueFractionalDive = 0;

/* Par precaution */
if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

/* Si presence de variables entieres:
  Contraintes de borne inf
	ContrainteActivable */
if ( Pne->YaDesVariablesEntieres == OUI_PNE ) {
  Pne->ContrainteActivable = (char *) malloc( Pne->NombreDeContraintesTrav * sizeof( char ) );
  if ( Pne->ContrainteActivable == NULL ) {
	  if ( Pne->AffichageDesTraces == OUI_PNE ) printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_InitPne \n");
    Pne->AnomalieDetectee = OUI_PNE;
    longjmp( Pne->Env , Pne->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
	}
  for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) Pne->ContrainteActivable[Cnt] = OUI_PNE;

	Pne->NombreDeK = 0;
  Pne->SommeViolationsK = 0;
  Pne->SeuilDeViolationK = SEUIL_VIOLATION_KNAPSACK;

  Pne->NombreDeMIR_MARCHAND_WOLSEY = 0;
  Pne->SommeViolationsMIR_MARCHAND_WOLSEY = 0;
  Pne-> SeuilDeViolationMIR_MARCHAND_WOLSEY = SEUIL_VIOLATION_MIR_MARCHAND_WOLSEY;

	Pne->NombreDeCliques = 0;
  Pne->SommeViolationsCliques = 0;
  Pne->SeuilDeViolationCliques = SEUIL_VIOLATION_CLIQUES;

	Pne->NombreDImplications = 0;
  Pne->SommeViolationsImplications = 0;
  Pne->SeuilDeViolationImplications = SEUIL_VIOLATION_IMPLICATIONS;

	Pne->NombreDeBornesVariables = 0;
  Pne->SommeViolationsBornesVariables = 0;
  Pne->SeuilDeViolationBornesVariables = SEUIL_VIOLATION_BORNES_VARIABLES;	
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_TranslaterLesBornes( PROBLEME_PNE * Pne )
{
int Var; int Cnt; int il ; int ilMax; double S; int * TypeDeBorne; int NombreDeVariables;
double * CoutLineaire; double * Xmin; double * Xmax; char * VariableAInverser;
int NombreDeContraintes; int * Mdeb; int * NbTerm; int * Nuvar; double * A;
double * XminEntre; double * XmaxEntre; double * B;
 
# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
CONTRAITES_DE_BORNE_VARIABLE * ContraintesDeBorneVariable; int NombreDeContraintesDeBorne;
int * First; int * Colonne; double * SecondMembre; double * Coefficient; 
# endif

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
CoutLineaire = Pne->LTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
XminEntre = Pne->UminEntree;
XmaxEntre = Pne->UmaxEntree;

VariableAInverser = Pne->VariableAInverser;

/* Transformation interne: les variables de type BORNEE_SUPERIEUREMENT sont transformees 
   en BORNEE_INFERIEUREMENT par un simple changement de variable */
/* Attention, on ne prevoit pas encore le cas de variables entieres negatives */
/* 1- Partie cout et bornes */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBorne[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {	
    CoutLineaire[Var] = -CoutLineaire[Var];
    Xmin[Var] = -Xmax[Var];
    Xmax[Var] = LINFINI_PNE;
    VariableAInverser[Var] = OUI_PNE;
	}
}

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
B = Pne->BTrav;

/* 2- Partie matrice des contraintes */
/* 2-a- Contraintes natives */
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    if ( TypeDeBorne[Nuvar[il]] == VARIABLE_BORNEE_SUPERIEUREMENT ) A[il] = -A[il];
    il++;
  }
}
/* 2-b Variables bound constraintes ajoutees dans le variable probing */
# if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE && PROBING_JUSTE_APRES_LE_PRESOLVE == OUI_PNE && CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
  ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;
  if ( ContraintesDeBorneVariable != NULL ) {
    NombreDeContraintesDeBorne = ContraintesDeBorneVariable->NombreDeContraintesDeBorne;
    First = ContraintesDeBorneVariable->First;
    SecondMembre = ContraintesDeBorneVariable->SecondMembre;
    Colonne = ContraintesDeBorneVariable->Colonne;
    Coefficient = ContraintesDeBorneVariable->Coefficient;
    for ( Cnt = 0 ; Cnt < NombreDeContraintesDeBorne ; Cnt++ ) {		
      if ( First[Cnt] < 0 ) continue;	
      il = First[Cnt];
	    ilMax = il + 2;
	    while ( il < ilMax ) {
        if ( TypeDeBorne[Colonne[il]] == VARIABLE_BORNEE_SUPERIEUREMENT ) Coefficient[il] = -Coefficient[il];
	    	il++;
			}
		}		
  }
# endif

/* 3- Changement de variable */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBorne[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeDeBorne[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
}
/* Fin de la transformation interne BORNEE_SUPERIEUREMENT -> BORNEE_INFERIEUREMENT */

memcpy( (char *) XmaxEntre , (char *) Xmax , NombreDeVariables * sizeof( double ) );
memcpy( (char *) XminEntre , (char *) Xmin , NombreDeVariables * sizeof( double ) );

/* A ce stade il n'y a plus de variable de type VARIABLE_BORNEE_SUPERIEUREMENT */

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) { 
  if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) { 	
    Xmax[Var] = XmaxEntre[Var] - XminEntre[Var];
		Pne->Z0 += CoutLineaire[Var] * XminEntre[Var];
    Xmin[Var] = 0.;
  }
}
 
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++) {
  S = 0.;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    Var = Nuvar[il];   
    if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) S += A[il] * XminEntre[Var];  
    il++;
  }
  B[Cnt] -= S;
}

/* Variables bound constraintes ajoutees dans le variable probing */
# if UTILISER_LE_GRAPHE_DE_CONFLITS == OUI_PNE && PROBING_JUSTE_APRES_LE_PRESOLVE == OUI_PNE && CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
  ContraintesDeBorneVariable = Pne->ContraintesDeBorneVariable;
  if ( ContraintesDeBorneVariable != NULL ) {
    NombreDeContraintesDeBorne = ContraintesDeBorneVariable->NombreDeContraintesDeBorne;
    First = ContraintesDeBorneVariable->First;
    SecondMembre = ContraintesDeBorneVariable->SecondMembre;
    Colonne = ContraintesDeBorneVariable->Colonne;
    Coefficient = ContraintesDeBorneVariable->Coefficient;
    for ( Cnt = 0 ; Cnt < NombreDeContraintesDeBorne ; Cnt++ ) {		
      if ( First[Cnt] < 0 ) continue;
      S = 0.;			
      il = First[Cnt];
	    ilMax = il + 2;
	    while ( il < ilMax ) {
			  Var = Colonne[il];
        if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorne[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) S += Coefficient[il] * XminEntre[Var];
	    	il++;
			}
		  SecondMembre[Cnt] -= S;
		}
  }
# endif

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("Fixed cost is %e\n",Pne->Z0);
	fflush( stdout );
}

return;
}





















