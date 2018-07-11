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

   FONCTION: Decomposition LU de la base. Methode de pivot total.
                
 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# if OUTILS_DE_GESTION_MEMOIRE_DE_PNE_SOLVEUR	
  # include "mem_fonctions.h"
#endif

/*--------------------------------------------------------------------------------------------------*/
void LU_FactorisationCalculs( MATRICE * Matrice , MATRICE_A_FACTORISER * MatriceEntree )	  
{
/* La matrice d'entree */
double * ValeurDesTermesDeLaMatrice; int * IndicesDeLigne; int * Ideb; int * NbTermesDesColonnes; int NbColonnes;
int   * CodeRetour;
/*                     */
int Kp   ; int OrdreKp        ; int LignePivot; int ColonnePivot; int OrdreLignePivot; int OrdreColonnePivot; 
int SeuilDeChangementDePivotMin; int Ki; int Ligne; int Colonne ; char RetourAuDebut  ;
int SeuilPourReintegration     ; char UtiliserLesSuperLignes       ;

int Rang; int NbT; int * OrdreLigne ; int * OrdreColonne;int * InverseOrdreLigne;

int * InverseOrdreColonne             ; double * AbsValeurDuTermeDiagonal              ;
char * LigneRejeteeTemporairementPourPivotage  ; double * AbsDuPlusGrandTermeDeLaLigne  ;
char * ColonneRejeteeTemporairementPourPivotage; SUPER_LIGNE_DE_LA_MATRICE *  SuperLigne;
double X; int k; char ContexteDeLaFactorisation; char WarningMatricePleine;

int * LNbTerm; int * CNbTermMatriceActive; 

/*----------------------------------------------------------------------------------------------------*/  
ValeurDesTermesDeLaMatrice = MatriceEntree->ValeurDesTermesDeLaMatrice;
IndicesDeLigne             = MatriceEntree->IndicesDeLigne;
Ideb                       = MatriceEntree->IndexDebutDesColonnes;
NbTermesDesColonnes        = MatriceEntree->NbTermesDesColonnes;
NbColonnes                 = MatriceEntree->NombreDeColonnes;
Matrice->FaireScaling      = (char)(MatriceEntree->FaireScalingDeLaMatrice);
CodeRetour                 = &MatriceEntree->ProblemeDeFactorisation;	    
/*----------------------------------------------------------------------------------------------------*/  
Matrice->ContexteDeLaFactorisation = MatriceEntree->ContexteDeLaFactorisation;
if ( Matrice->ContexteDeLaFactorisation != LU_SIMPLEXE && Matrice->ContexteDeLaFactorisation != LU_POINT_INTERIEUR &&  
     Matrice->ContexteDeLaFactorisation != LU_GENERAL ) {
  printf("Attention, il faut preciser le contexte de la factorisation parmi les choix LU_SIMPLEXE/LU_POINT_INTERIEUR/LU_GENERAL\n");
  printf("=> on initialise de contexte a LU_GENERAL\n");
	Matrice->ContexteDeLaFactorisation = LU_GENERAL;
}
/*----------------------------------------------------------------------------------------------------*/  
if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) {
  Matrice->NombreMaxDeChangementsDeSeuilDePivotage = NOMBRE_MAX_DE_CHANGEMENTS_DE_SEUIL_DE_PIVOTAGE_SIMPLEXE;
}
else {
  Matrice->NombreMaxDeChangementsDeSeuilDePivotage = NOMBRE_MAX_DE_CHANGEMENTS_DE_SEUIL_DE_PIVOTAGE_AUTRES_CONTEXTES;
  if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) {
    Matrice->ValeurDeRegularisation = MatriceEntree->ValeurDeRegularisation;
    MatriceEntree->OnARegularise    = NON_LU;
    Matrice->TermeDeRegularisation  = MatriceEntree->TermeDeRegularisation;
		Matrice->OnARegularise          = NON_LU;
  }	
}
Matrice->OnPeutRegulariser      = NON_LU;
/*----------------------------------------------------------------------------------------------------*/  
Matrice->UtiliserLesSuperLignes = MatriceEntree->UtiliserLesSuperLignes;
/* Precaution pour les etourdis */
if ( Matrice->UtiliserLesSuperLignes != OUI_LU && Matrice->UtiliserLesSuperLignes != NON_LU ) {
  Matrice->UtiliserLesSuperLignes = NON_LU;
}
Matrice->LaMatriceEstSymetriqueEnStructure= MatriceEntree->LaMatriceEstSymetriqueEnStructure;
/* Precaution pour les etourdis */
if ( Matrice->LaMatriceEstSymetriqueEnStructure != OUI_LU && Matrice->LaMatriceEstSymetriqueEnStructure != NON_LU ) {
  Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU; 
}
/*----------------------------------------------------------------------------------------------------*/  
Matrice->UtiliserLesValeursDePivotNulParDefaut = MatriceEntree->UtiliserLesValeursDePivotNulParDefaut;
if ( Matrice->UtiliserLesValeursDePivotNulParDefaut == OUI_LU ) {
  if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) {
    Matrice->ValeurDuPivotMin        = PIVOT_MIN_SIMPLEXE;
    Matrice->ValeurDuPivotMinExtreme = PIVOT_MIN_EXTREME_SIMPLEXE;
  }
  else {
    Matrice->ValeurDuPivotMin        = PIVOT_MIN_AUTRES_CONTEXTES;
    Matrice->ValeurDuPivotMinExtreme = PIVOT_MIN_EXTREME_AUTRES_CONTEXTES;
  }
}
else {
  Matrice->ValeurDuPivotMin        = MatriceEntree->ValeurDuPivotMin;
  Matrice->ValeurDuPivotMinExtreme = MatriceEntree->ValeurDuPivotMinExtreme;
}
/*----------------------------------------------------------------------------------------------------*/  
Matrice->FaireDuPivotageDiagonal = MatriceEntree->FaireDuPivotageDiagonal;  
Matrice->LaMatriceEstSymetrique  = MatriceEntree->LaMatriceEstSymetrique;
if ( Matrice->FaireDuPivotageDiagonal != OUI_LU ) Matrice->LaMatriceEstSymetrique = NON_LU;
/* La symetrie de structure est analyses dans la construction de la matrice. On ne met cet indicateur
   a OUI_LU que si:
   1- on demande le pivotage diagonal,
   2- et si la matrice n'est pas symetrique
   3- et si le controle montre tout de meme une symetrie de structure */
   
if ( Matrice->FaireDuPivotageDiagonal != OUI_LU ) Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU;
if ( Matrice->LaMatriceEstSymetrique  == OUI_LU ) Matrice->LaMatriceEstSymetriqueEnStructure = NON_LU;

/* Donc une matrice pour laquelle on ne veut pas de pivotage diagonal sera toujours vue comme
   non symetrique ni en valeur ni en structure.
   A l'inverse une matrice peut ne peut pas etre symetrique tout en ayant demande le pivotage diagonal */

/*----------------------------------------------------------------------------------------------------*/

Matrice->NbFillIn = 0;
Matrice->Kp       = -1;
Matrice->ExclureLesEchecsMarkowitz  = OUI_LU;
Matrice->CompteurExclusionMarkowitz = 0;
/*
printf("                              LU_Factorisation ##############\n"); fflush(stdout);
*/
*CodeRetour = (int) NON_LU;

if ( NbColonnes == 0 ) return;

LU_AllouerLU( Matrice, ValeurDesTermesDeLaMatrice, IndicesDeLigne, Ideb, /*Isui*/ NbTermesDesColonnes, &NbColonnes );
/* Tester le code retour */

UtiliserLesSuperLignes = Matrice->UtiliserLesSuperLignes;

SeuilPourReintegration = (int) (SEUIL_DE_REINTRODUCTION * Matrice->NombreDeTermesDeLaMatriceAFactoriser);

/* Classement de toutes les lignes et de toutes les colonnes */
LU_ClasserToutesLesLignesEtToutesLesColonnes( Matrice );

if ( UtiliserLesSuperLignes == OUI_LU ) LU_InitPourSuperLignes( Matrice );	    

/* Debut de la factorisation */
SeuilDeChangementDePivotMin = (int) (0.9 * Matrice->Rang);

Rang = Matrice->Rang;
OrdreLigne   = Matrice->OrdreLigne;
OrdreColonne = Matrice->OrdreColonne;
InverseOrdreLigne   = Matrice->InverseOrdreLigne;
InverseOrdreColonne = Matrice->InverseOrdreColonne;
AbsValeurDuTermeDiagonal     = Matrice->AbsValeurDuTermeDiagonal;
AbsDuPlusGrandTermeDeLaLigne = Matrice->AbsDuPlusGrandTermeDeLaLigne;
LigneRejeteeTemporairementPourPivotage   = Matrice->LigneRejeteeTemporairementPourPivotage;
ColonneRejeteeTemporairementPourPivotage = Matrice->ColonneRejeteeTemporairementPourPivotage;
ContexteDeLaFactorisation                = Matrice->ContexteDeLaFactorisation;
WarningMatricePleine                     = OUI_LU;

LNbTerm = Matrice->LNbTerm;
CNbTermMatriceActive = Matrice->CNbTermMatriceActive;

for ( Kp = 0 ; Kp < Rang ; Kp++ ) {

  Matrice->Kp = Kp;

  if ( UtiliserLesSuperLignes == OUI_LU ) {
    /* Si on utilise la version turbo de l'elimination des lignes, on ne fait rien sur les super lignes */
    if ( Matrice->LaMatriceEstSymetriqueEnStructure != OUI_LU ) { 
      LU_DeclenchementEventuelRechercheSuperLignes( Matrice );
    }
  }
       
  EtapeDebut:
  
  if ( Kp > SeuilDeChangementDePivotMin ) {
	  Matrice->PivotMin = Matrice->ValeurDuPivotMinExtreme;
    if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) Matrice->OnPeutRegulariser = OUI_LU;		
	}

  /* A la derniere etape il ne reste plus qu'un terme a inverser => les question de dimensionnement
     importent moins et on peut se permettre d'avoir un seuil de pivotage tres proche de 0 */
  if ( Kp == Rang - 1 /*&& ContexteDeLaFactorisation != LU_SIMPLEXE on le fait aussi pour le simplexe*/ ) {    
    Matrice->PivotMin = PIVOT_MIN_FINAL;  
    /* Il faut reclasse la derniere ligne au cas ou elle aurait ete declassee */
    Ligne   = OrdreLigne[Kp];
    Colonne = OrdreColonne[Kp];    
    if ( Matrice->FaireDuPivotageDiagonal == OUI_LU ) {
      AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.;    
      if ( UtiliserLesSuperLignes == NON_LU ) {
			  /* Il faut utiliser "Matrice->" a cause des redimensionnements */
        AbsValeurDuTermeDiagonal[Ligne] = fabs( Matrice->Elm[Matrice->Ldeb[Ligne]] );
      }
      else {
	      SuperLigne = Matrice->SuperLigneDeLaLigne[Ligne];
        if ( SuperLigne != NULL ) {
          for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ ) {      
            if ( SuperLigne->NumerosDesLignesDeLaSuperLigne[k] != Ligne ) continue;
            X = fabs( SuperLigne->ElmColonneDeSuperLigne[ ( SuperLigne->IndexDuTermeDiagonal[k] *
																														SuperLigne->CapaciteDesColonnes) + k ] ); 
            AbsValeurDuTermeDiagonal[Ligne] = X;
						break;
          }	    
        }
				else {
			    /* Il faut utiliser "Matrice->" a cause des redimensionnements */
          AbsValeurDuTermeDiagonal[Ligne] = fabs(  Matrice->Elm[Matrice->Ldeb[Ligne]] );
				}	    
      }      
    }
    
    if ( UtiliserLesSuperLignes == NON_LU ) NbT = LNbTerm[Ligne];   
    else {
			SuperLigne = Matrice->SuperLigneDeLaLigne[Ligne];
      if ( SuperLigne != NULL ) NbT = SuperLigne->NombreDeTermes;	
			else NbT = LNbTerm[Ligne]; 
    }  
    LU_DeClasserUneLigne( Matrice, Ligne, NbT ); /* A quoi ça sert de declasser pour reclasser tout de suite apres ? */
    LU_ClasserUneLigne( Matrice, Ligne, NbT ); 
		
    if ( Matrice->FaireDuPivotageDiagonal != OUI_LU ) {
      NbT = CNbTermMatriceActive[Colonne];  
      LU_DeClasserUneColonne( Matrice, Colonne, NbT ); /* A quoi ça sert de declasser pour reclasser tout de suite apres ? */
      LU_ClasserUneColonne( Matrice, Colonne, NbT );
    }
  }	
  
  /* Reintegration des lignes et colonnes exclues */
  if ( Matrice->CompteurExclusionMarkowitz >= SeuilPourReintegration && Matrice->ExclureLesEchecsMarkowitz == OUI_LU ) {  
    #if VERBOSE_LU     
      printf("Reintegration des lignes et colonnes exclues etape %d NbFillIn %d\n",Kp,Matrice->NbFillIn);
    #endif
    Matrice->CompteurExclusionMarkowitz = 0;
    for ( Ki = Kp ; Ki < Matrice->Rang ; Ki++ ) {
      /* Inutile de reintegrer les ligne a un seul terme */
      Ligne = OrdreLigne[Ki];
      if ( LNbTerm[Ligne] > 1 ) {
        LigneRejeteeTemporairementPourPivotage[Ligne] = NON_LU;
      }
      /* Inutile de reintegrer les colonnes a un seul terme */
      if ( ( Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU && Matrice->FaireDuPivotageDiagonal == OUI_LU ) ||
           ( Matrice->LaMatriceEstSymetrique            == OUI_LU && Matrice->FaireDuPivotageDiagonal == OUI_LU ) ) {
        /* Si la matrice est symetrique en valeur ou en structure et qu'on fait du pivotage diagonal
           alors on n'utilise pas les colonnes */	   
      }
      else {
        Colonne = Matrice->OrdreColonne[Ki];
        if ( CNbTermMatriceActive[Colonne] > 1 ) {
          ColonneRejeteeTemporairementPourPivotage[Colonne] = NON_LU;
        }
      }      
    }
  }

  if ( Matrice->FaireDuPivotageDiagonal == OUI_LU && Matrice->FactoriserEnMatricePleine == NON_LU ) {
    LU_SelectionDuPivotDiagonal( Matrice, &LignePivot, &ColonnePivot );    
    goto FinChoixPivot;
  }

  if ( Matrice->FactoriserEnMatricePleine == NON_LU ) {
    LU_SelectionDuPivot( Matrice, &LignePivot, &ColonnePivot );
  }
  else {
    /* On est dans le cas de la factorisation en matrice pleine */
    LU_SelectionDuPivotMatricePleine( Matrice, &LignePivot, &ColonnePivot );    
  }
    /*  */    
  FinChoixPivot:  
  if ( LignePivot >= 0 && ColonnePivot >= 0 ) {
		
    OrdreLignePivot                    = InverseOrdreLigne[LignePivot];
    InverseOrdreLigne[LignePivot]      = Kp;
    OrdreColonnePivot                  = InverseOrdreColonne[ColonnePivot];
    InverseOrdreColonne[ColonnePivot] = Kp;
    
    OrdreKp                            = OrdreLigne[Kp];
    OrdreLigne       [Kp]              = LignePivot;
    OrdreLigne       [OrdreLignePivot] = OrdreKp;
    InverseOrdreLigne[OrdreKp]         = OrdreLignePivot;

    OrdreKp                                = OrdreColonne[Kp];
    OrdreColonne       [Kp]                = ColonnePivot;
    OrdreColonne       [OrdreColonnePivot] = OrdreKp;
    InverseOrdreColonne[OrdreKp]           = OrdreColonnePivot;
       
    if ( Matrice->FactoriserEnMatricePleine == NON_LU ) {
      /* Si la ligne pivot est pleine et la colonne pivot est pleine alors on est certain
         qu'a la prochaine etape on sera en matrice pleine */
      /*
      printf("Ligne pivot= %d Colonne pivot= %d\n",LignePivot,ColonnePivot);
      */			
      if ( Matrice->NombreDeTermes == (Rang - Kp) * (Rang - Kp) ) {			
				if ( UtiliserLesSuperLignes == OUI_LU ) {
				/* La matrice sera pleine le coup d'apres */
          Matrice->LaMatriceEstPleine = OUI_LU;
				}
				if ( Matrice->LaMatriceEstSymetrique == NON_LU ) {	 
          #if VERBOSE_LU
					  if ( Matrice->Rang - Matrice->Kp > 100 && WarningMatricePleine == OUI_LU ) {
              printf(" --> Matrice pleine a l'etape %d rang %d NbFillIn %d taille du carre plein %d\n",
							             Matrice->Kp+1, Matrice->Rang, Matrice->NbFillIn,Matrice->Rang - Matrice->Kp);
							WarningMatricePleine = NON_LU;
						}
					#endif													 
					/* En fait il semblerait que ça ne sert pas à grand chose */
          /* Matrice->FactoriserEnMatricePleine = OUI_LU;*/
          /* printf("Sous-matrice active pleine a Kp = %d  Rang = %d\n",Matrice->Kp,Matrice->Rang); exit(0);*/
        }	
      }
      if ( Matrice->FaireDuPivotageDiagonal == OUI_LU && Matrice->LaMatriceEstSymetrique == OUI_LU ) {
        LU_EliminationDUneLigneCasSymetrique( Matrice, LignePivot , ColonnePivot );
      }
      else {
				if ( Matrice->LaMatriceEstSymetriqueEnStructure == OUI_LU ) {
					/* Symetrie en structure (possible que si on fait aussi du pivotage diagonal
						 et que la matrice n'est pas symetrique */
					LU_EliminationDUneLigneVersionTurbo( Matrice , LignePivot , ColonnePivot );
				}
				else {
					LU_EliminationDUneLigne( Matrice, LignePivot , ColonnePivot );
				}
      }
      if ( Matrice->FactoriserEnMatricePleine == OUI_LU ) {
        /* Comme le prochain coup on va passer en mode matrice pleine, on cherche le plus grand
					 terme de la matrice active */
        LU_RechercherLePlusGrandTermeDeLaMatriceActive( Matrice );	   
      }
    }
    else {   
      Matrice->ValeurDuPlusGrandTerme = -1.;
      LU_EliminationDUneLigneMatricePleine( Matrice, LignePivot , ColonnePivot );
    }
  }
  else {
      
    RetourAuDebut = NON_LU;

    if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) {
      if ( Matrice->OnPeutRegulariser == NON_LU ) {
	      Matrice->PivotMin = Matrice->ValeurDuPivotMinExtreme;
        Matrice->OnPeutRegulariser = OUI_LU;		
        RetourAuDebut = OUI_LU;
			}
    }			
    
    if ( Matrice->ExclureLesEchecsMarkowitz == OUI_LU ) {		
      Matrice->ExclureLesEchecsMarkowitz  = NON_LU;
      Matrice->CompteurExclusionMarkowitz = 0;
      for ( Ki = Kp ; Ki < Matrice->Rang ; Ki++ ) { 
        Ligne   = OrdreLigne[Ki];
        Colonne = OrdreColonne[Ki];
        LigneRejeteeTemporairementPourPivotage[Ligne] = NON_LU; 
        ColonneRejeteeTemporairementPourPivotage[Colonne] = NON_LU;
        if ( ContexteDeLaFactorisation == LU_SIMPLEXE ) continue;
	
				/* Il est necessaire de remettre dans le classement les lignes qui en avaient ete exclues
					 car le pivot a change. C'est indispensable pour le pivotage diagonal. */
        if ( Matrice->FaireDuPivotageDiagonal == OUI_LU ) {
          AbsDuPlusGrandTermeDeLaLigne[Ligne] = -1.;
          if ( UtiliserLesSuperLignes == NON_LU ) {
			      /* Il faut utiliser "Matrice->" a cause des redimensionnements */					
            AbsValeurDuTermeDiagonal[Ligne] = fabs( Matrice->Elm[Matrice->Ldeb[Ligne]] );
					}
					else {
						SuperLigne = Matrice->SuperLigneDeLaLigne[Ligne];
            if ( SuperLigne != NULL ) {
              for ( k = 0 ; k < SuperLigne->NombreDeLignesDeLaSuperLigne; k++ ) {      
                if ( SuperLigne->NumerosDesLignesDeLaSuperLigne[k] != Ligne ) continue;
                X = fabs( SuperLigne->ElmColonneDeSuperLigne[ ( SuperLigne->IndexDuTermeDiagonal[k] *
																																SuperLigne->CapaciteDesColonnes) + k ] ); 
                AbsValeurDuTermeDiagonal[Ligne] = X;
								break;
              }	    
            }
						else {
			        /* Il faut utiliser "Matrice->" a cause des redimensionnements */										
              AbsValeurDuTermeDiagonal[Ligne] = fabs( Matrice->Elm[Matrice->Ldeb[Ligne]] );
						}	    
					}
				}
    
        if ( UtiliserLesSuperLignes == NON_LU ) NbT = LNbTerm[Ligne];   
        else {
					SuperLigne = Matrice->SuperLigneDeLaLigne[Ligne];
          if ( SuperLigne != NULL ) NbT = SuperLigne->NombreDeTermes;	
					else NbT = LNbTerm[Ligne];   
        }
	
        LU_DeClasserUneLigne( Matrice, Ligne, NbT ); 
        LU_ClasserUneLigne( Matrice, Ligne, NbT );
        if ( Matrice->FaireDuPivotageDiagonal != OUI_LU ) {
          NbT = CNbTermMatriceActive[Colonne];		
          LU_DeClasserUneColonne( Matrice, Colonne, NbT ); 
          LU_ClasserUneColonne( Matrice, Colonne, NbT );
        }		   
      }
      RetourAuDebut = OUI_LU;
    }
    if ( RetourAuDebut == NON_LU ) {
      /* Switch eventuel de la methode */
      if ( Matrice->FaireDuPivotageDiagonal == OUI_LU && 0 /* desactive pour l'instant car utilite pas demontree */ ) {
        /* PB: pour l'instant je ne gere pas le fait de ne pas avoir cree de colonnes si la matrice est symetrique
					 ou symetrique en structure */
        if ( Matrice->LaMatriceEstSymetrique != OUI_LU &&  Matrice->LaMatriceEstSymetriqueEnStructure != OUI_LU ) {
          LU_SwitchVersMarkowitz( Matrice );
          RetourAuDebut = OUI_LU;
				}
      }
    }
    
    if ( RetourAuDebut == OUI_LU ) goto EtapeDebut;
    
    #if VERBOSE_LU     
      printf("Factorisation LU: la matrice est singuliere, etape %d etape max %d\n",Kp,Matrice->Rang-1);
    #endif
    Matrice->EtapeSinguliere  = Kp;
    Matrice->AnomalieDetectee = MATRICE_SINGULIERE;
    longjmp( Matrice->Env, Matrice->AnomalieDetectee ); 
  }
}

/* RAZ de SolutionIntermediaire avant toute resolution de systeme */
memset( (char *) Matrice->SolutionIntermediaire, 0, Rang * sizeof( double ) );

/* Attention, dans les cas hors Simplexe on a initialise le PivotMin a PIVOT_MIN_FINAL. Il faut
   donc remettre ValeurDuPivotMinExtreme dans le cas ContexteDeLaFactorisation = LU_POINT_INTERIEUR */
if ( Matrice->ContexteDeLaFactorisation == LU_POINT_INTERIEUR ) {
  Matrice->PivotMin            = Matrice->ValeurDuPivotMinExtreme;
  MatriceEntree->OnARegularise = Matrice->OnARegularise ;	
}

if ( UtiliserLesSuperLignes == OUI_LU ) {
  if ( Matrice->NbScanSuperLignes > 0 ) {
    printf("Nombre moyen de lignes dans une SuperLigne scannee %d\n",Matrice->NbLignesSuperLignes/Matrice->NbScanSuperLignes);
    printf("Nombre superlignes a moins de 2 lignes scannees %d\n",Matrice->NbScanMin);
  }
}

Matrice->MxTermesCreesLuUpdate = 0;
if ( Matrice->ContexteDeLaFactorisation == LU_GENERAL ) {
  /* On conserve le stockage par ligne de U dont on dispose grace a la factorisation */
  LU_ChainageParLigneDeULUGeneral( Matrice );		 
	/* On construit le chainage par colonne de U pour les resolutions */
  LU_ChainageParColonneDeUHorsSimplexe( Matrice );
}
else if ( Matrice->ContexteDeLaFactorisation == LU_SIMPLEXE ) {
  /* On se reconstruit un chainage par ligne de U pour recuperer la place vide qu'on peut
	   avoir construite pendant la factorisation */
  LU_ChainageParLigneDeU( Matrice );		 
	/* On construit un chainage par colonne de U adapte a la LU update */
  LU_ChainageParColonneDeU( Matrice );
  /* Pour la LU_Update, on stocke le nombre max de termes crees a partir duquel il faudra refaire une 
     factorisation: un certain pourcentage du nombre de terme du triangle U */ 
  Matrice->MxTermesCreesLuUpdate = (int) ( POURCENTAGE_MAX_DE_TERMES_CREES_PAR_LU_UPDATE * Matrice->IndexLibreDeU );
	/* Attention, en faisant ca on peut avoir un nombre tres grand et ne jamais demander de refactorisation
	   dans la lu update si Rang est grand donc on teste le resultat par rapport a Matrice->IndexLibreDeU */
  k = Matrice->NombreDeVecteursHAlloues * (int) ( 0.1 * Matrice->Rang );
	if ( k > Matrice->IndexLibreDeU ) k = Matrice->IndexLibreDeU;
  if ( k < 100 ) k = 100;
  if ( Matrice->MxTermesCreesLuUpdate < k ) Matrice->MxTermesCreesLuUpdate = k;
  	
}
else { /* Contexte LU_POINT_INTERIEUR */
  /* On se recontruit un chainage par ligne de U pour recuperer la place vide qu'on peut
	   avoir construite pendant la factorisation */
  LU_ChainageParLigneDeU( Matrice );		 
	/* On classe le stockage par ligne de U dans l'ordre croissant de l'ordre d'elimination */
  LU_StockageDeUDansLOrdreCroissantDesIndices( Matrice );
}

#if VERBOSE_LU     
  printf("Fin de factorisation NbFillIn %d\n", Matrice->NbFillIn); fflush(stdout);	 
#endif

/* Liberation des tableaux de travail */
LU_LibererLesTableauxTemporaires( Matrice );

/* En retour, les triangles de la factorisee */
MatriceEntree->IndexDebutDesColonnesDeL = Matrice->CdebParColonneDeL;
MatriceEntree->NbTermesDesColonnesDeL   = Matrice->NbTermesParColonneDeL;
MatriceEntree->ValeurDesTermesDeL       = Matrice->ElmDeL;
MatriceEntree->IndicesDeLigneDeL        = Matrice->IndiceLigneDeL;

MatriceEntree->IndexDebutDesLignesDeU = Matrice->LdebParLigneDeU;
MatriceEntree->NbTermesDesLignesDeU   = Matrice->NbTermesParLigneDeU;
MatriceEntree->ValeurDesTermesDeU     = Matrice->ElmDeU;
MatriceEntree->IndicesDeColonneDeU    = Matrice->IndiceColonneDeU;

return;
}


































































