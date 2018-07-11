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

   FONCTION: On met NULL dans tous les tableaux. ceci permet d'eviter
             les plantages lors de free mais aussi de reconnaitre
             un tableau non alloue.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "lu_define.h"
# include "lu_fonctions.h"

# ifdef LU_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "lu_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void LU_InitTableauxANull( MATRICE * Matrice )
{  

Matrice->LigneRejeteeTemporairementPourPivotage   = NULL; 
Matrice->ColonneRejeteeTemporairementPourPivotage = NULL;
Matrice->AbsDuPlusGrandTermeDeLaLigne = NULL; 

Matrice->LignePrecedente = NULL;
Matrice->LigneSuivante   = NULL;
Matrice->Ldeb    = NULL;
Matrice->LNbTerm = NULL;
Matrice->LDernierPossible = NULL;
Matrice->LIndiceColonne   = NULL;  
Matrice->Elm = NULL;

Matrice->ColonnePrecedente = NULL;
Matrice->ColonneSuivante   = NULL;
Matrice->Cdeb    = NULL;
Matrice->CNbTerm = NULL;
Matrice->CNbTermMatriceActive = NULL;
Matrice->CDernierPossible = NULL;
Matrice->CIndiceLigne     = NULL;

Matrice->SuperLigneDeLaLigne = NULL;     
Matrice->SuperLigneAScanner  = NULL;
Matrice->SuperLigne          = NULL;
Matrice->PoidsDesColonnes    = NULL;
Matrice->HashCodeLigne       = NULL;
Matrice->HashCodeSuperLigne  = NULL;
Matrice->HashModuloPrem      = NULL;
Matrice->HashNbModuloIdentiques         = NULL;
Matrice->TypeDeClassementHashCodeAFaire = NULL;
Matrice->HashModuloSuiv = NULL;
Matrice->HashModuloPrec = NULL;
 
Matrice->PremLigne = NULL; 						 		     
Matrice->SuivLigne = NULL; 				        
Matrice->PrecLigne = NULL;
 
Matrice->PremColonne = NULL;
Matrice->SuivColonne = NULL;
Matrice->PrecColonne = NULL;

Matrice->W = NULL;
Matrice->Marqueur = NULL;

Matrice->OrdreLigne          = NULL;  
Matrice->InverseOrdreLigne   = NULL;  
Matrice->OrdreColonne        = NULL;
Matrice->InverseOrdreColonne = NULL; 

Matrice->NbTermesParColonneDeL = NULL; 
Matrice->CdebParColonneDeL     = NULL; 
Matrice->ElmDeL                = NULL;		        					  
Matrice->IndiceLigneDeL        = NULL;
Matrice->IndexKpDeUouL         = NULL;

Matrice->NbTermesParLigneDeU = NULL; 
Matrice->LdebParLigneDeU     = NULL; 	   
Matrice->ElmDeU              = NULL;
Matrice->IndiceColonneDeU    = NULL;
Matrice->CapaciteParLigneDeU = NULL;

Matrice->LdebParLigneDeL          = NULL; 
Matrice->NbTermesParLigneDeL      = NULL; 
Matrice->IndiceColonneParLigneDeL = NULL;    
Matrice->ElmDeLParLigne           = NULL;		        					  
   	   	   
Matrice->CdebParColonneDeU        = NULL;			     		   
Matrice->NbTermesParColonneDeU    = NULL;	  
Matrice->IndiceLigneParColonneDeU = NULL;
Matrice->CapaciteParColonneDeU    = NULL;
Matrice->ElmDeUParColonne         = NULL;

Matrice->StockageColonneVersLigneDeU = NULL;
Matrice->StockageLigneVersColonneDeU = NULL;

Matrice->SolutionIntermediaire = NULL;

# ifdef HYPER_CREUX
  Matrice->NoeudDansLaliste = NULL;
  Matrice->ListeDesNoeuds   = NULL;
  Matrice->PseudoPile       = NULL;
# endif

Matrice->NumeroDeTriangleDeLaVariable = NULL;

Matrice->OrdreUcolonne = NULL;
Matrice->InverseOrdreUcolonne = NULL;

Matrice->HDeb           = NULL;
Matrice->HLigne         = NULL;
Matrice->HNbTerm        = NULL;
Matrice->HIndiceColonne = NULL; 
Matrice->HValeur        = NULL; 

Matrice->ValeurElmSpike = NULL;
Matrice->IndicesLignesDuSpike = NULL;

Matrice->ScaleX = NULL; 
Matrice->ScaleB = NULL; 

Matrice->AbsValeurDuTermeDiagonal = NULL;

Matrice->DebutInfosAdressesQueKpModifie = NULL;
Matrice->AdresseDeUModifie              = NULL;
Matrice->AdresseUHaut                   = NULL;
Matrice->DebutInfosLignesQueKpModifie   = NULL;
Matrice->NombreDeLignesQueKpModifie     = NULL;
Matrice->AdresseUGauche                 = NULL;
Matrice->NombreDeTermesParLigneQueKpModifie = NULL;

Matrice->SecondMembreSV = NULL; 
Matrice->SolutionSV     = NULL;

Matrice->TermeDeRegularisation = NULL;

return;
}










