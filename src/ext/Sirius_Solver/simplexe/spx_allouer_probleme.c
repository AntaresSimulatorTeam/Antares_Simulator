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

   FONCTION: Allouer / desallouer la structure du probleme
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"
  
# include "spx_define.h"
# include "spx_fonctions.h"

# include "lu_fonctions.h"

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif
  
/*----------------------------------------------------------------------------*/

void SPX_AllouerProbleme( PROBLEME_SPX * Spx,
                          int   NbVar_E,
                          int   NbContr_E,
                          int * Mdeb_E,
                          int * NbTerm_E )
{ 
int i; int NbTrm; int NbVarMx; 

Spx->MatriceFactorisee = NULL;
Spx->ProblemePneDeSpx  = NULL;

NbVarMx = NbVar_E + ( 2 * NbContr_E );

Spx->NombreDeVariablesAllouees = NbVarMx;

Spx->NumeroDesVariablesACoutNonNul = (int *) malloc( NbVarMx * sizeof( int ) );      
Spx->C                   = (double *) malloc( NbVarMx * sizeof( double ) );      
Spx->Csv                 = (double *) malloc( NbVarMx * sizeof( double ) ); 
Spx->X                   = (double *) malloc( NbVarMx * sizeof( double ) );       
Spx->Xmin                = (double *) malloc( NbVarMx * sizeof( double ) );   
Spx->Xmax                = (double *) malloc( NbVarMx * sizeof( double ) );   
Spx->TypeDeVariable      = (char *)   malloc( NbVarMx * sizeof( char   ) );   
Spx->OrigineDeLaVariable = (char *)   malloc( NbVarMx * sizeof( char   ) );
Spx->StatutBorneSupCourante   = (char *)   malloc( NbVarMx * sizeof( char   ) );
Spx->BorneSupAuxiliaire       = (double *) malloc( NbVarMx * sizeof( double ) );    
Spx->StatutBorneSupAuxiliaire = (char *)   malloc( NbVarMx * sizeof( char   ) );

Spx->CntVarEcartOuArtif  = (int *)   malloc( NbVarMx * sizeof( int   ) );
Spx->XEntree    = (double *) malloc( NbVarMx * sizeof( double ) );                  
Spx->XminEntree = (double *) malloc( NbVarMx * sizeof( double ) );                         
Spx->XmaxEntree = (double *) malloc( NbVarMx * sizeof( double ) );       
Spx->SeuilDeViolationDeBorne = (double *) malloc( NbVarMx * sizeof( double ) );
Spx->SeuilDAmissibiliteDuale1 = (double *) malloc( NbVarMx * sizeof( double ) );
Spx->SeuilDAmissibiliteDuale2 = (double *) malloc( NbVarMx * sizeof( double ) );
Spx->ScaleX                  = (double *) malloc( NbVarMx * sizeof( double ) ); 
Spx->CorrespondanceVarEntreeVarSimplexe = (int *) malloc( NbVarMx * sizeof( int ) );  
Spx->CorrespondanceVarSimplexeVarEntree = (int *) malloc( NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->NombreDeContraintesAllouees = NbContr_E;

Spx->B                               = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->BAvantTranslationEtApresScaling = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->ScaleB                          = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->Mdeb                            = (int *)   malloc( NbContr_E * sizeof( int   ) );
Spx->NbTerm                          = (int *)   malloc( NbContr_E * sizeof( int   ) ); 

/* Calcul du nombre de termes */
for ( NbTrm = -1 , i = 0 ; i < NbContr_E ; i++ ) {
  if ( ( Mdeb_E[i] + NbTerm_E[i] ) > NbTrm ) NbTrm = Mdeb_E[i] + NbTerm_E[i]; /* Un terme de plus */
}

NbTrm+= NbContr_E; /* Par exces pour les variables d'ecart sur les inegalites */
NbTrm+= NbContr_E; /* Par exces pour la base triviale de depart */

if ( NbTrm <= 0 ) NbTrm = 1;

Spx->NbTermesAlloues = NbTrm;

Spx->A      = (double *) malloc( NbTrm * sizeof( double ) );            
Spx->Indcol = (int *)    malloc( NbTrm * sizeof( int    ) );
/*------------------------------------------------------------------------*/
Spx->CorrespondanceCntSimplexeCntEntree = (int *) malloc( NbContr_E * sizeof( int ) ); 
Spx->CorrespondanceCntEntreeCntSimplexe = (int *) malloc( NbContr_E * sizeof( int ) );     
/*------------------------------------------------------------------------*/
Spx->Cdeb                        = (int *)   malloc( NbVarMx   * sizeof( int   ) );
Spx->CNbTerm                     = (int *)   malloc( NbVarMx   * sizeof( int   ) );
Spx->CNbTermSansCoupes           = (int *)   malloc( NbVarMx   * sizeof( int   ) );
Spx->CNbTermesDeCoupes           = (int *)   malloc( NbVarMx   * sizeof( int   ) );
Spx->Csui                        = (int *)   malloc( NbTrm     * sizeof( int   ) ); 
Spx->ACol                        = (double *) malloc( NbTrm     * sizeof( double ) );            
Spx->NumeroDeContrainte          = (int *)   malloc( NbTrm     * sizeof( int   ) );
Spx->CdebBase                    = (int *)   malloc( NbContr_E * sizeof( int   ) );
Spx->NbTermesDesColonnesDeLaBase = (int *)   malloc( NbContr_E * sizeof( int   ) );
/*------------------------------------------------------------------------*/
NbTrm = CYCLE_DE_REFACTORISATION + 1 /* Marge */;

Spx->EtaDeb     = (int *) malloc( NbTrm * sizeof( int ) );
Spx->EtaNbTerm  = (int *) malloc( NbTrm * sizeof( int ) );
Spx->EtaColonne = (int *) malloc( NbTrm * sizeof( int ) );

NbTrm*= NbContr_E;
Spx->EtaIndiceLigne  = (int *)   malloc( NbTrm * sizeof( int   ) );
Spx->EtaMoins1Valeur = (double *) malloc( NbTrm * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->T = (int *) malloc( NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->CntDeABarreSNonNuls = (int *)   malloc( NbContr_E * sizeof( int   ) );
Spx->ABarreS             = (double *) malloc( NbContr_E * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->Bs              = (double *) malloc( NbContr_E      * sizeof( double ) );                      
Spx->BBarre          = (double *) malloc( NbContr_E      * sizeof( double ) );   
/*------------------------------------------------------------------------*/
Spx->BoundFlip = (int *) malloc( NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->Pi                           = (double *) malloc( NbContr_E * sizeof( double ) );        
Spx->CBarre                       = (double *) malloc( NbVarMx   * sizeof( double ) );        
Spx->PositionDeLaVariable         = (char *)   malloc( NbVarMx   * sizeof( char   ) );  
Spx->ContrainteDeLaVariableEnBase = (int *)   malloc( NbVarMx   * sizeof( int   ) );
Spx->VariableEnBaseDeLaContrainte = (int *)   malloc( NbContr_E * sizeof( int   ) );
Spx->NombreDeVariablesHorsBaseDeLaContrainte = (int *)   malloc( NbContr_E * sizeof( int   ) );
/*------------------------------------------------------------------------*/
Spx->IndexDansContrainteASurveiller   = (int *)    malloc( NbContr_E * sizeof( int   ) );
Spx->NumerosDesContraintesASurveiller = (int *)    malloc( NbContr_E * sizeof( int   ) );
Spx->ValeurDeViolationDeBorne         = (double  *) malloc( NbContr_E * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->NumerosDesVariablesHorsBase = (int *)   malloc( ( NbVarMx - NbContr_E ) * sizeof( int   ) );
Spx->NBarreR                     = (double *) malloc( NbVarMx * sizeof( double ) );
Spx->IndexDeLaVariableDansLesVariablesHorsBase = (int *) malloc( NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->NumVarNBarreRNonNul      = (int *) malloc( ( NbVarMx - NbContr_E ) * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->NumeroDesVariableATester      = (int *)   malloc( NbVarMx   * sizeof( int   ) ); 
Spx->CBarreSurNBarreR              = (double *) malloc( NbVarMx   * sizeof( double ) ); 
Spx->CBarreSurNBarreRAvecTolerance = (double *) malloc( NbVarMx   * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->IndexTermesNonNulsDeErBMoinsUn = (int *)   malloc( NbContr_E * sizeof( int   ) );  
Spx->ErBMoinsUn                     = (double *) malloc( NbContr_E * sizeof( double ) );  
/*------------------------------------------------------------------------*/
Spx->InDualFramework = (char *)   malloc( NbVarMx   * sizeof( char   ) ); 
Spx->DualPoids       = (double *) malloc( NbContr_E * sizeof( double ) ); 
Spx->Tau             = (double *) malloc( NbContr_E * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->CorrectionDuale = (char *) malloc( NbVarMx * sizeof( char ) ); 
/*------------------------------------------------------------------------*/
Spx->V                       = (double *) malloc( NbContr_E * sizeof( double ) );        
Spx->FaisabiliteDeLaVariable = (char *)   malloc( NbVarMx   * sizeof( char   ) );        
/*------------------------------------------------------------------------*/
Spx->PositionHorsBaseReduiteAutorisee = (char *)   malloc( NbVarMx   * sizeof( char   ) ); 
Spx->OrdreColonneDeLaBaseFactorisee = (int *) malloc( NbContr_E * sizeof( int ) ); 
Spx->ColonneDeLaBaseFactorisee = (int *) malloc( NbContr_E * sizeof( int ) ); 
Spx->OrdreLigneDeLaBaseFactorisee = (int *) malloc( NbContr_E * sizeof( int ) ); 
Spx->LigneDeLaBaseFactorisee = (int *) malloc( NbContr_E * sizeof( int ) );

Spx->AReduit = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->IndexAReduit = (int *) malloc( NbContr_E * sizeof( int ) );
Spx->Marqueur = (int *) malloc( NbContr_E * sizeof( int ) );

/*------------------------------------------------------------------------*/

if ( 
  Spx->NumeroDesVariablesACoutNonNul      == NULL ||
  Spx->C                                  == NULL ||      
  Spx->Csv                                == NULL ||    
  Spx->X                                  == NULL ||       
  Spx->Xmin                               == NULL ||     
  Spx->Xmax                               == NULL ||
  Spx->TypeDeVariable                     == NULL ||
  Spx->OrigineDeLaVariable                == NULL ||
  Spx->StatutBorneSupCourante             == NULL ||
  Spx->BorneSupAuxiliaire                 == NULL ||
  Spx->StatutBorneSupAuxiliaire           == NULL ||	
/*------------------------------------------------------------------------*/ 	
  Spx->CntVarEcartOuArtif                 == NULL ||  
  Spx->XEntree                            == NULL ||                  
  Spx->XminEntree                         == NULL ||                     
  Spx->XmaxEntree                         == NULL ||      
  Spx->SeuilDeViolationDeBorne            == NULL ||   
  Spx->SeuilDAmissibiliteDuale1           == NULL ||   
  Spx->SeuilDAmissibiliteDuale2           == NULL ||   
  Spx->ScaleX                             == NULL ||   
  Spx->CorrespondanceVarEntreeVarSimplexe == NULL ||                           
  Spx->CorrespondanceVarSimplexeVarEntree == NULL ||
/*------------------------------------------------------------------------*/
  Spx->B                                  == NULL ||
  Spx->BAvantTranslationEtApresScaling    == NULL ||
  Spx->ScaleB                             == NULL ||
  Spx->Mdeb                               == NULL ||
  Spx->NbTerm                             == NULL ||
  Spx->A                                  == NULL ||                
  Spx->Indcol                             == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CorrespondanceCntSimplexeCntEntree == NULL || 
  Spx->CorrespondanceCntEntreeCntSimplexe == NULL || 
/*------------------------------------------------------------------------*/
  Spx->Cdeb                        == NULL ||
  Spx->CNbTerm                     == NULL ||
  Spx->CNbTermSansCoupes           == NULL ||
  Spx->CNbTermesDeCoupes           == NULL ||  
  Spx->ACol                        == NULL ||
  Spx->Csui                        == NULL ||   
  Spx->NumeroDeContrainte          == NULL ||
  Spx->CdebBase                    == NULL ||
  Spx->NbTermesDesColonnesDeLaBase == NULL ||
/*------------------------------------------------------------------------*/
  Spx->EtaDeb          == NULL ||
  Spx->EtaNbTerm       == NULL ||
  Spx->EtaColonne      == NULL ||
  Spx->EtaIndiceLigne  == NULL ||
  Spx->EtaMoins1Valeur == NULL ||
/*------------------------------------------------------------------------*/
  Spx->T == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CntDeABarreSNonNuls == NULL ||
  Spx->ABarreS             == NULL ||
/*------------------------------------------------------------------------*/	
  Spx->Bs              == NULL ||               
  Spx->BBarre          == NULL ||
/*------------------------------------------------------------------------*/     
  Spx->BoundFlip == NULL || 
/*------------------------------------------------------------------------*/     
  Spx->Pi                           == NULL ||        
  Spx->CBarre                       == NULL ||        
  Spx->PositionDeLaVariable         == NULL ||   
  Spx->ContrainteDeLaVariableEnBase == NULL ||
  Spx->VariableEnBaseDeLaContrainte == NULL ||
  Spx->NombreDeVariablesHorsBaseDeLaContrainte == NULL ||
/*------------------------------------------------------------------------*/     
  Spx->IndexDansContrainteASurveiller   == NULL ||
  Spx->NumerosDesContraintesASurveiller == NULL ||
  Spx->ValeurDeViolationDeBorne         == NULL ||
/*------------------------------------------------------------------------*/     
  Spx->NumerosDesVariablesHorsBase == NULL ||
  Spx->NBarreR                     == NULL ||	
	Spx->IndexDeLaVariableDansLesVariablesHorsBase == NULL ||
/*------------------------------------------------------------------------*/     
	Spx->NumVarNBarreRNonNul       == NULL ||
/*------------------------------------------------------------------------*/     
  Spx->NumeroDesVariableATester      == NULL ||
  Spx->CBarreSurNBarreR              == NULL ||
  Spx->CBarreSurNBarreRAvecTolerance == NULL ||
/*------------------------------------------------------------------------*/
  Spx->IndexTermesNonNulsDeErBMoinsUn == NULL ||
  Spx->ErBMoinsUn                     == NULL ||
/*------------------------------------------------------------------------*/
  Spx->InDualFramework == NULL ||
  Spx->DualPoids       == NULL ||
  Spx->Tau             == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CorrectionDuale == NULL ||
/*------------------------------------------------------------------------*/
  Spx->V                       == NULL ||
  Spx->FaisabiliteDeLaVariable == NULL ||
/*------------------------------------------------------------------------*/
  Spx->PositionHorsBaseReduiteAutorisee == NULL ||
  Spx->OrdreColonneDeLaBaseFactorisee == NULL ||
  Spx->ColonneDeLaBaseFactorisee      == NULL ||
  Spx->OrdreLigneDeLaBaseFactorisee   == NULL ||
  Spx->LigneDeLaBaseFactorisee        == NULL ||

	Spx->AReduit                             == NULL ||
  Spx->IndexAReduit                        == NULL || 
  Spx->Marqueur                            == NULL 
	
   ) { 

  printf("Simplexe, sous-programme SPX_AllouerProbleme: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

/* Finalement on le fait toujours car on en a besoin pour sauvegarder les bases */
/*if ( Spx.Contexte != BRANCH_AND_BOUND_OU_CUT ) return;*/

Spx->XSV                            = (double *) malloc( NbVarMx * sizeof( double ) );        
Spx->PositionDeLaVariableSV         = (char *)   malloc( NbVarMx * sizeof( char   ) );  
Spx->CBarreSV                       = (double *) malloc( NbVarMx * sizeof( double ) );        
Spx->InDualFrameworkSV              = (char *)   malloc( NbVarMx * sizeof( char   ) );  
Spx->ContrainteDeLaVariableEnBaseSV = (int *)   malloc( NbVarMx * sizeof( int   ) );  

Spx->BBarreSV                       = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->DualPoidsSV                    = (double *) malloc( NbContr_E * sizeof( double ) );
Spx->VariableEnBaseDeLaContrainteSV = (int *)   malloc( NbContr_E * sizeof( int   ) );

Spx->CdebBaseSV                    = (int *)   malloc( NbContr_E * sizeof( int ) );
Spx->NbTermesDesColonnesDeLaBaseSV = (int *)   malloc( NbContr_E * sizeof( int ) );

if ( 
  Spx->XSV                            == NULL ||        
  Spx->PositionDeLaVariableSV         == NULL ||   
  Spx->CBarreSV                       == NULL ||        
  Spx->InDualFrameworkSV              == NULL ||  
  Spx->ContrainteDeLaVariableEnBaseSV == NULL ||  
      
  Spx->BBarreSV                       == NULL ||
  Spx->DualPoidsSV                    == NULL ||
  Spx->VariableEnBaseDeLaContrainteSV == NULL ||

  Spx->CdebBaseSV                    == NULL ||
  Spx->NbTermesDesColonnesDeLaBaseSV == NULL 
	
   ) {

  printf("Simplexe, sous-programme SPX_AllouerProbleme: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

Spx->DonneesPourCoupesDIntersection = NULL;
Spx->CoupesDintersectionAllouees    = NON_SPX;

for ( i = 0 ; i < NbVarMx ; i++ ) Spx->T[i] = -1;

Spx->NbElementsAllouesPourLeProblemeReduit = NbContr_E;
Spx->CdebProblemeReduit = (int *) malloc( NbVarMx * sizeof( int ) );
Spx->CNbTermProblemeReduit = (int *) malloc( NbVarMx * sizeof( int ) );
Spx->ValeurDesTermesDesColonnesDuProblemeReduit = (double *) malloc( Spx->NbElementsAllouesPourLeProblemeReduit * sizeof( double ) );
Spx->IndicesDeLigneDesTermesDuProblemeReduit = (int *) malloc( Spx->NbElementsAllouesPourLeProblemeReduit * sizeof( int ) );
if ( 
  Spx->CdebProblemeReduit == NULL                         || Spx->CNbTermProblemeReduit                   == NULL ||        
  Spx->ValeurDesTermesDesColonnesDuProblemeReduit == NULL || Spx->IndicesDeLigneDesTermesDuProblemeReduit == NULL   	
   ) {

  printf("Simplexe, sous-programme SPX_AllouerProbleme: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

memset( (double *) Spx->AReduit, 0, NbContr_E * sizeof( double ) );

for ( i = 0 ; i < NbContr_E ; i++ ) Spx->Marqueur[i] = -1;

Spx->IndexDansLaMatriceHorsBase = NULL;
Spx->MdebHorsBase = NULL;
Spx->NbTermHorsBase = NULL;
Spx->AHorsBase = NULL;
Spx->IndcolHorsBase = NULL;
Spx->InverseIndexDansLaMatriceHorsBase = NULL;

return;
}

/*----------------------------------------------------------------------------*/

void SPX_AugmenterLeNombreDeVariables( PROBLEME_SPX * Spx )
{ 
int NbVarMx; int i;

/*printf(" SIMPLEXE Augmentation du nombre de variables\n"); fflush(stdout);*/

NbVarMx = Spx->NombreDeVariablesAllouees + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_VARIABLES_SPX;

Spx->NombreDeVariablesAllouees = NbVarMx;

Spx->C                   = (double *) realloc( Spx->C                  , NbVarMx * sizeof( double ) );      
Spx->Csv                 = (double *) realloc( Spx->Csv                , NbVarMx * sizeof( double ) ); 
Spx->X                   = (double *) realloc( Spx->X                  , NbVarMx * sizeof( double ) );       
Spx->Xmin                = (double *) realloc( Spx->Xmin               , NbVarMx * sizeof( double ) );   
Spx->Xmax                = (double *) realloc( Spx->Xmax               , NbVarMx * sizeof( double ) );   
Spx->TypeDeVariable      = (char *)   realloc( Spx->TypeDeVariable     , NbVarMx * sizeof( char   ) );   
Spx->OrigineDeLaVariable = (char *)   realloc( Spx->OrigineDeLaVariable, NbVarMx * sizeof( char   ) );

Spx->StatutBorneSupCourante   = (char *)   realloc( Spx->StatutBorneSupCourante  , NbVarMx * sizeof( char   ) );
Spx->BorneSupAuxiliaire       = (double *) realloc( Spx->BorneSupAuxiliaire      , NbVarMx * sizeof( double ) );  
Spx->StatutBorneSupAuxiliaire = (char *)   realloc( Spx->StatutBorneSupAuxiliaire, NbVarMx * sizeof( char   ) );

Spx->CntVarEcartOuArtif  = (int *)   realloc( Spx->CntVarEcartOuArtif , NbVarMx * sizeof( int   ) );
Spx->XEntree    = (double *) realloc( Spx->XEntree   , NbVarMx * sizeof( double ) );                  
Spx->XminEntree = (double *) realloc( Spx->XminEntree, NbVarMx * sizeof( double ) );                         
Spx->XmaxEntree = (double *) realloc( Spx->XmaxEntree, NbVarMx * sizeof( double ) );       
Spx->SeuilDeViolationDeBorne = (double *) realloc( Spx->SeuilDeViolationDeBorne, NbVarMx * sizeof( double ) );   
Spx->SeuilDAmissibiliteDuale1 = (double *) realloc( Spx->SeuilDAmissibiliteDuale1, NbVarMx * sizeof( double ) );   
Spx->SeuilDAmissibiliteDuale2 = (double *) realloc( Spx->SeuilDAmissibiliteDuale2, NbVarMx * sizeof( double ) );   
Spx->ScaleX                  = (double *) realloc( Spx->ScaleX                 , NbVarMx * sizeof( double ) ); 
Spx->CorrespondanceVarEntreeVarSimplexe = (int *) realloc( Spx->CorrespondanceVarEntreeVarSimplexe, NbVarMx * sizeof( int ) );  
Spx->CorrespondanceVarSimplexeVarEntree = (int *) realloc( Spx->CorrespondanceVarSimplexeVarEntree, NbVarMx * sizeof( int ) );                          
/*------------------------------------------------------------------------*/
Spx->Cdeb    = (int *) realloc( Spx->Cdeb   , NbVarMx * sizeof( int ) );
Spx->CNbTerm = (int *) realloc( Spx->CNbTerm, NbVarMx * sizeof( int ) );
Spx->CNbTermSansCoupes = (int *) realloc( Spx->CNbTermSansCoupes, NbVarMx * sizeof( int ) );
Spx->CNbTermesDeCoupes = (int *) realloc( Spx->CNbTermesDeCoupes, NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->T = (int *) realloc( Spx->T, NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->CBarre                       = (double *) realloc( Spx->CBarre                      , NbVarMx * sizeof( double ) );        
Spx->PositionDeLaVariable         = (char *)   realloc( Spx->PositionDeLaVariable        , NbVarMx * sizeof( char   ) );  
Spx->ContrainteDeLaVariableEnBase = (int *)   realloc( Spx->ContrainteDeLaVariableEnBase, NbVarMx * sizeof( int   ) );
/*------------------------------------------------------------------------*/
Spx->NumerosDesVariablesHorsBase = (int *)   realloc( Spx->NumerosDesVariablesHorsBase , ( NbVarMx - Spx->NombreDeContraintesAllouees ) * sizeof( int   ) );
Spx->NBarreR                     = (double *) realloc( Spx->NBarreR                    , NbVarMx * sizeof( double ) );
Spx->IndexDeLaVariableDansLesVariablesHorsBase = (int *) realloc( Spx->IndexDeLaVariableDansLesVariablesHorsBase , NbVarMx * sizeof( int ) );
/*------------------------------------------------------------------------*/
Spx->NumVarNBarreRNonNul      = (int *) realloc( Spx->NumVarNBarreRNonNul , ( NbVarMx - Spx->NombreDeContraintesAllouees ) * sizeof( int   ) );
/*------------------------------------------------------------------------*/
Spx->NumeroDesVariableATester      = (int *)   realloc( Spx->NumeroDesVariableATester     , NbVarMx * sizeof( int   ) ); 
Spx->CBarreSurNBarreR              = (double *) realloc( Spx->CBarreSurNBarreR             , NbVarMx * sizeof( double ) ); 
Spx->CBarreSurNBarreRAvecTolerance = (double *) realloc( Spx->CBarreSurNBarreRAvecTolerance, NbVarMx * sizeof( double ) ); 
/*------------------------------------------------------------------------*/
Spx->InDualFramework = (char *) realloc( Spx->InDualFramework, NbVarMx * sizeof( char ) ); 
/*------------------------------------------------------------------------*/
Spx->CorrectionDuale = (char *) realloc( Spx->CorrectionDuale, NbVarMx * sizeof( char ) ); 
/*------------------------------------------------------------------------*/
Spx->FaisabiliteDeLaVariable = (char *) realloc( Spx->FaisabiliteDeLaVariable, NbVarMx * sizeof( char ) );        
/*------------------------------------------------------------------------*/
Spx->BoundFlip = (int *) realloc( Spx->BoundFlip, NbVarMx * sizeof( int ) );        
/*------------------------------------------------------------------------*/
Spx->PositionHorsBaseReduiteAutorisee = (char *) realloc( Spx->PositionHorsBaseReduiteAutorisee, NbVarMx * sizeof( char ) );
/*------------------------------------------------------------------------*/
Spx->CdebProblemeReduit = (int *) realloc( Spx->CdebProblemeReduit, NbVarMx * sizeof( int ) );        
/*------------------------------------------------------------------------*/
Spx->CNbTermProblemeReduit = (int *) realloc( Spx->CNbTermProblemeReduit, NbVarMx * sizeof( int ) );        

if ( 
  Spx->C                                  == NULL ||      
  Spx->Csv                                == NULL ||    
  Spx->X                                  == NULL ||       
  Spx->Xmin                               == NULL ||     
  Spx->Xmax                               == NULL ||
  Spx->TypeDeVariable                     == NULL ||
  Spx->OrigineDeLaVariable                == NULL ||
  Spx->StatutBorneSupCourante             == NULL ||
  Spx->BorneSupAuxiliaire                 == NULL ||
  Spx->StatutBorneSupAuxiliaire           == NULL ||	
  Spx->CntVarEcartOuArtif                 == NULL ||        
  Spx->XEntree                            == NULL ||                  
  Spx->XminEntree                         == NULL ||                     
  Spx->XmaxEntree                         == NULL ||      
  Spx->SeuilDeViolationDeBorne            == NULL ||
  Spx->SeuilDAmissibiliteDuale1           == NULL ||
  Spx->SeuilDAmissibiliteDuale2           == NULL ||	
  Spx->ScaleX                             == NULL ||   
  Spx->CorrespondanceVarEntreeVarSimplexe == NULL ||                           
  Spx->CorrespondanceVarSimplexeVarEntree == NULL ||      
/*------------------------------------------------------------------------*/
  Spx->Cdeb    == NULL ||
  Spx->CNbTerm == NULL ||
  Spx->CNbTermSansCoupes == NULL ||
  Spx->CNbTermesDeCoupes == NULL || 
/*------------------------------------------------------------------------*/
  Spx->T == NULL || 
/*------------------------------------------------------------------------*/
  Spx->CBarre                       == NULL ||        
  Spx->PositionDeLaVariable         == NULL ||   
  Spx->ContrainteDeLaVariableEnBase == NULL ||
/*------------------------------------------------------------------------*/
  Spx->NumerosDesVariablesHorsBase == NULL ||
  Spx->NBarreR                     == NULL ||
	Spx->IndexDeLaVariableDansLesVariablesHorsBase == NULL ||
/*------------------------------------------------------------------------*/
	Spx->NumVarNBarreRNonNul == NULL ||
/*------------------------------------------------------------------------*/	
  Spx->NumeroDesVariableATester      == NULL ||
  Spx->CBarreSurNBarreR              == NULL ||
  Spx->CBarreSurNBarreRAvecTolerance == NULL ||
/*------------------------------------------------------------------------*/
  Spx->InDualFramework == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CorrectionDuale == NULL ||
/*------------------------------------------------------------------------*/
  Spx->FaisabiliteDeLaVariable == NULL ||
/*------------------------------------------------------------------------*/
  Spx->BoundFlip == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CdebProblemeReduit == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CNbTermProblemeReduit == NULL 
	
   ) {

  printf("Simplexe, sous-programme SPX_AugmenterLeNombreDeVariables: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

for ( i = 0 ; i < NbVarMx ; i++ )  Spx->T[i] = -1;
for ( i = Spx->NombreDeVariables ; i < NbVarMx ; i++ )  Spx->PositionHorsBaseReduiteAutorisee[i] = OUI_1_FOIS;

Spx->XSV                            = (double *) realloc( Spx->XSV                           , NbVarMx * sizeof( double ) );        
Spx->PositionDeLaVariableSV         = (char *)   realloc( Spx->PositionDeLaVariableSV        , NbVarMx * sizeof( char   ) );  
Spx->CBarreSV                       = (double *) realloc( Spx->CBarreSV                      , NbVarMx * sizeof( double ) );        
Spx->InDualFrameworkSV              = (char *)   realloc( Spx->InDualFrameworkSV             , NbVarMx * sizeof( char   ) );  
Spx->ContrainteDeLaVariableEnBaseSV = (int *)   realloc( Spx->ContrainteDeLaVariableEnBaseSV, NbVarMx * sizeof( int   ) );  

if ( 
  Spx->XSV                            == NULL ||        
  Spx->PositionDeLaVariableSV         == NULL ||   
  Spx->CBarreSV                       == NULL ||        
  Spx->InDualFrameworkSV              == NULL ||  
  Spx->ContrainteDeLaVariableEnBaseSV == NULL         
   ) {

  printf("Simplexe, sous-programme SPX_AugmenterLeNombreDeVariables: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}
/*
printf(" Fin augmentation du nombre de variables\n"); fflush(stdout);
*/
return;
}

/*----------------------------------------------------------------------------*/

void SPX_AugmenterLeNombreDeContraintes( PROBLEME_SPX * Spx )
{
int NbContr_E; int NbTrm; int i;

/*printf(" SIMPLEXE Augmentation du nombre de contraintes\n"); fflush(stdout);*/

NbContr_E = Spx->NombreDeContraintesAllouees + INCREMENT_DALLOCATION_POUR_LE_NOMBRE_DE_CONTRAINTES_SPX;  
Spx->NombreDeContraintesAllouees = NbContr_E;

/*------------------------------------------------------------------------*/
Spx->B                               = (double *) realloc( Spx->B                              , NbContr_E * sizeof( double ) );
Spx->BAvantTranslationEtApresScaling = (double *) realloc( Spx->BAvantTranslationEtApresScaling, NbContr_E * sizeof( double ) );
Spx->ScaleB                          = (double *) realloc( Spx->ScaleB                         , NbContr_E * sizeof( double ) );
Spx->Mdeb                            = (int *)   realloc( Spx->Mdeb                           , NbContr_E * sizeof( int   ) );
Spx->NbTerm                          = (int *)   realloc( Spx->NbTerm                         , NbContr_E * sizeof( int   ) ); 
/*------------------------------------------------------------------------*/
Spx->CorrespondanceCntSimplexeCntEntree = (int *) realloc( Spx->CorrespondanceCntSimplexeCntEntree, NbContr_E * sizeof( int ) ); 
Spx->CorrespondanceCntEntreeCntSimplexe = (int *) realloc( Spx->CorrespondanceCntEntreeCntSimplexe, NbContr_E * sizeof( int ) ); 
/*------------------------------------------------------------------------*/
Spx->CdebBase                    = (int *) realloc( Spx->CdebBase                   , NbContr_E * sizeof( int ) );
Spx->NbTermesDesColonnesDeLaBase = (int *) realloc( Spx->NbTermesDesColonnesDeLaBase, NbContr_E * sizeof( int ) );
/*------------------------------------------------------------------------*/
NbTrm = CYCLE_DE_REFACTORISATION + 1 /* Marge */;
NbTrm*= NbContr_E;
Spx->EtaIndiceLigne  = (int *)   realloc( Spx->EtaIndiceLigne , NbTrm * sizeof( int   ) );
Spx->EtaMoins1Valeur = (double *) realloc( Spx->EtaMoins1Valeur, NbTrm * sizeof( double ) );
/*------------------------------------------------------------------------*/
Spx->CntDeABarreSNonNuls = (int *)    realloc( Spx->CntDeABarreSNonNuls, NbContr_E * sizeof( int    ) );  
Spx->ABarreS             = (double *) realloc( Spx->ABarreS            , NbContr_E * sizeof( double ) );  
/*------------------------------------------------------------------------*/
Spx->Bs              = (double *) realloc( Spx->Bs             , NbContr_E * sizeof( double ) );                      
Spx->BBarre          = (double *) realloc( Spx->BBarre         , NbContr_E * sizeof( double ) );      
/*------------------------------------------------------------------------*/
Spx->Pi                           = (double *) realloc( Spx->Pi, NbContr_E * sizeof( double ) );        
Spx->VariableEnBaseDeLaContrainte = (int *) realloc( Spx->VariableEnBaseDeLaContrainte, NbContr_E * sizeof( int ) );
Spx->NombreDeVariablesHorsBaseDeLaContrainte = (int *) realloc( Spx->NombreDeVariablesHorsBaseDeLaContrainte, NbContr_E * sizeof( int) );
/*------------------------------------------------------------------------*/
Spx->IndexDansContrainteASurveiller   = (int *)   realloc( Spx->IndexDansContrainteASurveiller  , NbContr_E * sizeof( int   ) );
Spx->NumerosDesContraintesASurveiller = (int *)   realloc( Spx->NumerosDesContraintesASurveiller, NbContr_E * sizeof( int   ) );
Spx->ValeurDeViolationDeBorne         = (double *) realloc( Spx->ValeurDeViolationDeBorne        , NbContr_E * sizeof( double ) );
/*------------------------------------------------------------------------*/
/* Il n'y a pas lieu de diminuer la taille allouee aux tableaux ci-dessous */
/*
Spx->NumerosDesVariablesHorsBase = (int *)   realloc( Spx->NumerosDesVariablesHorsBase , ( Spx->NombreDeVariablesAllouees - NbContr_E ) * sizeof( int   ) );
Spx->NBarreRHorsBase             = (double *) realloc( Spx->NBarreRHorsBase             , ( Spx->NombreDeVariablesAllouees - NbContr_E ) * sizeof( double ) );
*/
/*------------------------------------------------------------------------*/
Spx->IndexTermesNonNulsDeErBMoinsUn = (int *)   realloc( Spx->IndexTermesNonNulsDeErBMoinsUn , NbContr_E * sizeof( int ) ); 
Spx->ErBMoinsUn                     = (double *) realloc( Spx->ErBMoinsUn , NbContr_E * sizeof( double ) ); 
/*------------------------------------------------------------------------*/
Spx->DualPoids = (double *) realloc( Spx->DualPoids, NbContr_E * sizeof( double ) ); 
Spx->Tau       = (double *) realloc( Spx->Tau      , NbContr_E * sizeof( double ) ); 
/*------------------------------------------------------------------------*/
Spx->V = (double *) realloc( Spx->V, NbContr_E * sizeof( double ) );        
/*------------------------------------------------------------------------*/

Spx->OrdreColonneDeLaBaseFactorisee = (int *) realloc( Spx->OrdreColonneDeLaBaseFactorisee, NbContr_E * sizeof( int ) );        
Spx->ColonneDeLaBaseFactorisee = (int *) realloc( Spx->ColonneDeLaBaseFactorisee, NbContr_E * sizeof( int ) );        
Spx->OrdreLigneDeLaBaseFactorisee = (int *) realloc( Spx->OrdreLigneDeLaBaseFactorisee, NbContr_E * sizeof( int ) );        
Spx->LigneDeLaBaseFactorisee = (int *) realloc( Spx->LigneDeLaBaseFactorisee, NbContr_E * sizeof( int ) );        

Spx->AReduit = (double *) realloc( Spx->AReduit, NbContr_E * sizeof( double ) );        
Spx->IndexAReduit = (int *) realloc( Spx->IndexAReduit, NbContr_E * sizeof( int ) );
Spx->Marqueur = (int *) realloc( Spx->Marqueur, NbContr_E * sizeof( int ) );

if ( 
  Spx->B                               == NULL ||
  Spx->BAvantTranslationEtApresScaling == NULL ||
  Spx->ScaleB                          == NULL ||
  Spx->Mdeb                            == NULL ||
  Spx->NbTerm                          == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CorrespondanceCntSimplexeCntEntree == NULL || 
  Spx->CorrespondanceCntEntreeCntSimplexe == NULL || 
/*------------------------------------------------------------------------*/
  Spx->CdebBase                    == NULL ||
  Spx->NbTermesDesColonnesDeLaBase == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CntDeABarreSNonNuls == NULL ||
  Spx->ABarreS             == NULL ||
/*------------------------------------------------------------------------*/
  Spx->Bs              == NULL ||               
  Spx->BBarre          == NULL ||
/*------------------------------------------------------------------------*/     
  Spx->Pi                           == NULL ||        
  Spx->VariableEnBaseDeLaContrainte == NULL ||
  Spx->NombreDeVariablesHorsBaseDeLaContrainte == NULL ||
/*------------------------------------------------------------------------*/
  Spx->IndexDansContrainteASurveiller   == NULL ||
  Spx->NumerosDesContraintesASurveiller == NULL ||
  Spx->ValeurDeViolationDeBorne         == NULL ||
/*------------------------------------------------------------------------*/
	Spx->NumerosDesVariablesHorsBase == NULL ||
	Spx->NBarreR                     == NULL ||	
/*------------------------------------------------------------------------*/     
  Spx->IndexTermesNonNulsDeErBMoinsUn == NULL ||
  Spx->ErBMoinsUn                     == NULL ||
/*------------------------------------------------------------------------*/
  Spx->DualPoids == NULL ||
  Spx->Tau       == NULL ||
/*------------------------------------------------------------------------*/
  Spx->CorrectionDuale == NULL ||
/*------------------------------------------------------------------------*/
  Spx->V == NULL ||      
/*------------------------------------------------------------------------*/
  Spx->OrdreColonneDeLaBaseFactorisee == NULL ||      
  Spx->ColonneDeLaBaseFactorisee      == NULL ||      
  Spx->OrdreLigneDeLaBaseFactorisee   == NULL ||    
  Spx->LigneDeLaBaseFactorisee        == NULL ||

  Spx->AReduit                             == NULL ||
  Spx->IndexAReduit                        == NULL ||
  Spx->Marqueur                            == NULL 
	
   ) {
	 
  printf("Simplexe, sous-programme SPX_AugmenterLeNombreDeContraintes: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

memset( (double *) Spx->AReduit, 0, NbContr_E * sizeof( double ) );

for ( i = 0 ; i < NbContr_E ; i++ ) Spx->Marqueur[i] = -1;

Spx->BBarreSV                       = (double *) realloc( Spx->BBarreSV                      , NbContr_E * sizeof( double ) );
Spx->DualPoidsSV                    = (double *) realloc( Spx->DualPoidsSV                   , NbContr_E * sizeof( double ) );
Spx->VariableEnBaseDeLaContrainteSV = (int *)   realloc( Spx->VariableEnBaseDeLaContrainteSV, NbContr_E * sizeof( int   ) );

Spx->CdebBaseSV                    = (int *)   realloc( Spx->CdebBaseSV                   , NbContr_E * sizeof( int ) );
Spx->NbTermesDesColonnesDeLaBaseSV = (int *)   realloc( Spx->NbTermesDesColonnesDeLaBaseSV, NbContr_E * sizeof( int ) );

if (       
  Spx->BBarreSV                       == NULL ||
  Spx->DualPoidsSV                    == NULL ||
  Spx->VariableEnBaseDeLaContrainteSV == NULL ||

  Spx->CdebBaseSV                     == NULL ||
  Spx->NbTermesDesColonnesDeLaBaseSV  == NULL   
   ) {

  printf("Simplexe, sous-programme SPX_AugmenterLeNombreDeContraintes: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  fflush(stdout);
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_AugmenterLaTailleDeLaMatriceDesContraintes( PROBLEME_SPX * Spx )
{

int NbTrm;
/*
printf(" SIMPLEXE Augmentation de la taille de la matrice des contraintes NbTermesAlloues %d\n",Spx.NbTermesAlloues); fflush(stdout);
*/
NbTrm = Spx->NbTermesAlloues + INCREMENT_DALLOCATION_POUR_LA_MATRICE_DES_CONTRAINTES_SPX; 
Spx->NbTermesAlloues = NbTrm;

/*------------------------------------------------------------------------*/
Spx->A      = (double *) realloc( Spx->A, NbTrm * sizeof( double ) );            
Spx->Indcol = (int *)   realloc( Spx->Indcol, NbTrm * sizeof( int   ) );           
/*------------------------------------------------------------------------*/
Spx->Csui               = (int *) realloc( Spx->Csui, NbTrm * sizeof( int   ) ); 
Spx->ACol               = (double *) realloc( Spx->ACol, NbTrm * sizeof( double ) );            
Spx->NumeroDeContrainte = (int *) realloc( Spx->NumeroDeContrainte, NbTrm * sizeof( int   ) );
/*------------------------------------------------------------------------*/

if ( 
  Spx->A      == NULL ||                
  Spx->Indcol == NULL ||                  
/*------------------------------------------------------------------------*/
  Spx->Csui               == NULL ||   
  Spx->ACol               == NULL ||
  Spx->NumeroDeContrainte == NULL    
   ) {

  printf("Simplexe, sous-programme SPX_AugmenterLaTailleDeLaMatriceDesContraintes: \n");
  printf("  -> memoire insuffisante pour l allocation de l espace de travail \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); /* rq: le 2eme argument ne sera pas utilise */
}

return;
}

/*----------------------------------------------------------------------------*/

void SPX_LibererProbleme( PROBLEME_SPX * Spx )
{

if ( Spx->MatriceFactorisee != NULL ) {
  LU_LibererMemoireLU( (MATRICE *) Spx->MatriceFactorisee );
}

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  MEM_Quit( Spx->Tas );
	return;
# endif

free( Spx->NumeroDesVariablesACoutNonNul );
free( Spx->C );
free( Spx->Csv );
free( Spx->X );
free( Spx->Xmin );
free( Spx->Xmax );
free( Spx->TypeDeVariable );
free( Spx->OrigineDeLaVariable );

free( Spx->StatutBorneSupCourante );
free( Spx->BorneSupAuxiliaire );
free( Spx->StatutBorneSupAuxiliaire );

free( Spx->CntVarEcartOuArtif );
free( Spx->XEntree );
free( Spx->XminEntree );
free( Spx->XmaxEntree );
free( Spx->SeuilDeViolationDeBorne );
free( Spx->SeuilDAmissibiliteDuale1 );
free( Spx->SeuilDAmissibiliteDuale2 );
free( Spx->ScaleX );		        
free( Spx->CorrespondanceVarEntreeVarSimplexe );
free( Spx->CorrespondanceVarSimplexeVarEntree );
  
free( Spx->B );
free( Spx->BAvantTranslationEtApresScaling );
free( Spx->ScaleB );
free( Spx->Mdeb );
free( Spx->NbTerm );
free( Spx->A );
free( Spx->Indcol );
free( Spx->CorrespondanceCntSimplexeCntEntree );
free( Spx->CorrespondanceCntEntreeCntSimplexe );

free( Spx->Cdeb ); 
free( Spx->CNbTerm );
free( Spx->CNbTermSansCoupes );
free( Spx->CNbTermesDeCoupes );
free( Spx->ACol ); 
free( Spx->Csui );
free( Spx->NumeroDeContrainte );
free( Spx->CdebBase );
free( Spx->NbTermesDesColonnesDeLaBase );

free( Spx->EtaDeb );
free( Spx->EtaNbTerm );
free( Spx->EtaColonne );
free( Spx->EtaIndiceLigne );
free( Spx->EtaMoins1Valeur );

free( Spx->T );

free( Spx->CntDeABarreSNonNuls );
free( Spx->ABarreS );
free( Spx->Bs );
free( Spx->BBarre );

free( Spx->BoundFlip );

free( Spx->Pi );
free( Spx->CBarre );
free( Spx->PositionDeLaVariable );
free( Spx->ContrainteDeLaVariableEnBase );
free( Spx->VariableEnBaseDeLaContrainte );
free( Spx->NombreDeVariablesHorsBaseDeLaContrainte );
free( Spx->IndexDansContrainteASurveiller );
free( Spx->NumerosDesContraintesASurveiller );
free( Spx->ValeurDeViolationDeBorne );

free( Spx->NumerosDesVariablesHorsBase );
free( Spx->NBarreR );
free( Spx->IndexDeLaVariableDansLesVariablesHorsBase );
free( Spx->NumVarNBarreRNonNul );

free( Spx->NumeroDesVariableATester );
free( Spx->CBarreSurNBarreR );
free( Spx->CBarreSurNBarreRAvecTolerance );

free( Spx->IndexTermesNonNulsDeErBMoinsUn );
free( Spx->ErBMoinsUn );

free( Spx->InDualFramework );
free( Spx->DualPoids );
free( Spx->Tau );

free( Spx->CorrectionDuale );

free( Spx->V );
free( Spx->FaisabiliteDeLaVariable );

free( Spx->XSV );
free( Spx->PositionDeLaVariableSV );
free( Spx->CBarreSV );
free( Spx->InDualFrameworkSV );
free( Spx->ContrainteDeLaVariableEnBaseSV );

free( Spx->BBarreSV );
free( Spx->DualPoidsSV );
free( Spx->VariableEnBaseDeLaContrainteSV );
free( Spx->CdebBaseSV );
free( Spx->NbTermesDesColonnesDeLaBaseSV );

free( Spx->PositionHorsBaseReduiteAutorisee );
free( Spx->OrdreColonneDeLaBaseFactorisee );
free( Spx->ColonneDeLaBaseFactorisee );
free( Spx->OrdreLigneDeLaBaseFactorisee );
free( Spx->LigneDeLaBaseFactorisee );

free( Spx->CdebProblemeReduit );
free( Spx->CNbTermProblemeReduit );
free( Spx->ValeurDesTermesDesColonnesDuProblemeReduit );
free( Spx->IndicesDeLigneDesTermesDuProblemeReduit );
free( Spx->AReduit );
free( Spx->IndexAReduit );
free( Spx->Marqueur );

free( Spx->IndexDansLaMatriceHorsBase );
free( Spx->MdebHorsBase );
free( Spx->NbTermHorsBase );
free( Spx->AHorsBase );
free( Spx->IndcolHorsBase );
free( Spx->InverseIndexDansLaMatriceHorsBase );

free( Spx );

return;
}










