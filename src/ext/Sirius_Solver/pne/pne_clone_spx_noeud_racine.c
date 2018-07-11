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

   FONCTION: Clone du simplexe du noeud racine pour utilisation ulterieure.
                
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

/*----------------------------------------------------------------------------*/
void PNE_LibererMatriceDeContraintesDuSimplexeAuNoeudRacine( MATRICE_DE_CONTRAINTES * Contraintes ) 
{
if ( Contraintes == NULL ) return;
free( Contraintes->IndexDebut );
free( Contraintes->NombreDeTermes );
free( Contraintes->SecondMembre );
free( Contraintes->Sens );
free( Contraintes->Colonne );
free( Contraintes->Coefficient );
free( Contraintes->X );
free( Contraintes->CoutsReduits );
free( Contraintes->PositionDeLaVariable );
free( Contraintes->PositionDeLaVariableSV );
free( Contraintes->ComplementDeLaBase );
free( Contraintes->ComplementDeLaBaseSV );
free( Contraintes );
return;
}
/*----------------------------------------------------------------------------*/

MATRICE_DE_CONTRAINTES * PNE_ConstruireMatriceDeContraintesDuSimplexeAuNoeudRacine( PROBLEME_PNE * Pne,
                                           int * PositionDeLaVariable,                
                                           int   NbVarDeBaseComplementaires,
                                           int * ComplementDeLaBase ) 
{
int NombreDeContraintesNatives; int NbTermesNecessaires; int * IndexDebut; int * NombreDeTermes;
int NombreDeVariablesNatives; int il; int NbTermesNatif; int Nb; int NombreDeContraintes;
int NbContraintesNecessaires; double * SecondMembre; char * Sens;  int * Colonne; double * Coefficient;
int * PositionVariable; int * ComplementBase;  int * PositionVariableSV; int * ComplementBaseSV;
double * X; double * CoutsReduits; int Cnt; int ilC; int ilCmax; 
MATRICE_DE_CONTRAINTES * Contraintes; 

Contraintes = NULL;
/* Allocation */
Contraintes = (MATRICE_DE_CONTRAINTES *) malloc( sizeof( MATRICE_DE_CONTRAINTES ) );
if ( Contraintes == NULL ) return( NULL );

NombreDeContraintesNatives = Pne->NombreDeContraintesTrav;
NombreDeVariablesNatives = Pne->NombreDeVariablesTrav;
NbContraintesNecessaires = NombreDeContraintesNatives;
NbContraintesNecessaires += Pne->Coupes.NombreDeContraintes;

/* Attention on suppose que les contraintes sont rangees dans l'ordre */
NbTermesNatif = Pne->MdebTrav[NombreDeContraintesNatives-1] + Pne->NbTermTrav[NombreDeContraintesNatives-1];
NbTermesNecessaires = NbTermesNatif;
if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {
  /* Pour la borne sur le cout */
	NbTermesNecessaires += NombreDeVariablesNatives;
}

/* Les coupes */
for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) NbTermesNecessaires += Pne->Coupes.Mdeb[Cnt] + Pne->Coupes.NbTerm[Cnt];

IndexDebut = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
NombreDeTermes = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
SecondMembre = (double *) malloc( NbContraintesNecessaires * sizeof( double ) );
Sens = (char *) malloc( NbContraintesNecessaires * sizeof( char ) );
Colonne = (int *) malloc( NbTermesNecessaires * sizeof( int ) );
Coefficient = (double *) malloc( NbTermesNecessaires * sizeof( double ) );
/* Donnees complementaires */
X = (double *) malloc( NombreDeVariablesNatives * sizeof( double ) );
CoutsReduits = (double *) malloc( NombreDeVariablesNatives * sizeof( double ) );
PositionVariable = (int *) malloc( NombreDeVariablesNatives * sizeof( int ) );
PositionVariableSV = (int *) malloc( NombreDeVariablesNatives * sizeof( int ) );
ComplementBase = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
ComplementBaseSV = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );

if ( IndexDebut == NULL || NombreDeTermes == NULL || SecondMembre == NULL || Sens == NULL || Colonne == NULL || Coefficient == NULL ||
     X == NULL || CoutsReduits == NULL || PositionVariable == NULL || ComplementBase == NULL || PositionVariableSV == NULL || ComplementBaseSV == NULL
) {
  free( IndexDebut ); free( NombreDeTermes ); free( SecondMembre ); free( Sens ); free( Colonne ); free( Coefficient );
	free( X );	free( CoutsReduits ); free( PositionVariable );	free( ComplementBase );
	free( PositionVariableSV );	free( ComplementBaseSV );
	free( Contraintes );
	return( NULL );
}

Contraintes->IndexDebut = IndexDebut;
Contraintes->NombreDeTermes = NombreDeTermes;
Contraintes->SecondMembre = SecondMembre;
Contraintes->Sens = Sens;
Contraintes->Colonne = Colonne;
Contraintes->Coefficient = Coefficient;
Contraintes->X = X;
Contraintes->CoutsReduits = CoutsReduits;
Contraintes->PositionDeLaVariable = PositionVariable;
Contraintes->PositionDeLaVariableSV = PositionVariableSV;
Contraintes->ComplementDeLaBase = ComplementBase;
Contraintes->ComplementDeLaBaseSV = ComplementBaseSV;

/* Recopie de la matrice des contraintes natives */
/* Attention on suppose que les contraintes sont rangees dans l'ordre */
memcpy( (char *) IndexDebut, (char *) Pne->MdebTrav, NombreDeContraintesNatives * sizeof( int ) );
memcpy( (char *) NombreDeTermes, (char *) Pne->NbTermTrav, NombreDeContraintesNatives * sizeof( int ) );
memcpy( (char *) SecondMembre, (char *) Pne->BTrav, NombreDeContraintesNatives * sizeof( double ) );
memcpy( (char *) Sens, (char *) Pne->SensContrainteTrav, NombreDeContraintesNatives * sizeof( char ) );
memcpy( (char *) Colonne, (char *) Pne->NuvarTrav, NbTermesNatif * sizeof( int ) );
memcpy( (char *) Coefficient, (char *) Pne->ATrav, NbTermesNatif * sizeof( double ) );

memcpy( (char *) X, (char *) Pne->UTrav, NombreDeVariablesNatives * sizeof( double ) );
memcpy( (char *) PositionVariable, (char *) PositionDeLaVariable, NombreDeVariablesNatives * sizeof( int ) );
memcpy( (char *) ComplementBase, (char *) ComplementDeLaBase, NbVarDeBaseComplementaires * sizeof( int ) );
Contraintes->NbVarDeBaseComplementaires = NbVarDeBaseComplementaires;

NombreDeContraintes = NombreDeContraintesNatives;
il = NbTermesNatif;

/* Les coupes */
for ( Cnt = 0 ; Cnt < Pne->Coupes.NombreDeContraintes ; Cnt++ ) {
  /* Si la variable d'ecart est EN_BASE, on ne recopie pas la contrainte ce qui evite de mettre a jour
	   ComplementBase */
	if ( Pne->Coupes.PositionDeLaVariableDEcart[Cnt] == EN_BASE ) continue;
  ilC = Pne->Coupes.Mdeb[Cnt] ;
	ilCmax = ilC + Pne->Coupes.NbTerm[Cnt];
	Nb = 0;
	IndexDebut[NombreDeContraintes] = il;		
  while ( ilC < ilCmax ) {
    Colonne[il] = Pne->Coupes.Nuvar[ilC];
    Coefficient[il] = Pne->Coupes.A[ilC];      
    il++; Nb++;		
		ilC++;
	}
  NombreDeTermes[NombreDeContraintes] = Nb;
  SecondMembre[NombreDeContraintes] = Pne->Coupes.B[Cnt];
	Sens[NombreDeContraintes] = '<';
  NombreDeContraintes++;			
}

Contraintes->NombreDeContraintes = NombreDeContraintes;

return( Contraintes );

}

/*----------------------------------------------------------------------------*/

void PNE_CloneProblemeSpxDuNoeudRacine( PROBLEME_PNE * Pne,
               int    * PositionDeLaVariable,                 /* Information en Entree et Sortie */
               int    * NbVarDeBaseComplementaires,           /* Information en Entree et Sortie */
               int    * ComplementDeLaBase                    /* Information en Entree et Sortie */
                          )
{
int BaseDeDepartFournie; int Contexte; int ExistenceDUneSolution; int ChoixDeLAlgorithme;
int NombreMaxDIterations; char PremiereResolutionAuNoeudRacine;
PROBLEME_SIMPLEXE Probleme; PROBLEME_SPX * Spx;

Pne->MatriceDesContraintesAuNoeudRacine = PNE_ConstruireMatriceDeContraintesDuSimplexeAuNoeudRacine( Pne,
                                          PositionDeLaVariable, *NbVarDeBaseComplementaires, ComplementDeLaBase );
if ( Pne->MatriceDesContraintesAuNoeudRacine == NULL ) return;

PremiereResolutionAuNoeudRacine = OUI_PNE;
BaseDeDepartFournie = OUI_SPX;
ChoixDeLAlgorithme = SPX_DUAL;
Pne->ProblemeSpxDuNoeudRacine = NULL; /* Par precaution mais c'est deja fait */

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

/* Normalement ca converge en 1 iteration */
NombreMaxDIterations = 100;
	 						
Probleme.Contexte = Contexte;
Probleme.NombreMaxDIterations = NombreMaxDIterations;
Probleme.DureeMaxDuCalcul     = -1.;
  
Probleme.CoutLineaire      = Pne->LTrav;
Probleme.X                 = Pne->MatriceDesContraintesAuNoeudRacine->X;
Probleme.Xmin              = Pne->UminTrav;
Probleme.Xmax              = Pne->UmaxTrav;
Probleme.NombreDeVariables = Pne->NombreDeVariablesTrav;
Probleme.TypeDeVariable    = Pne->TypeDeBorneTrav;

Probleme.NombreDeContraintes                   = Pne->MatriceDesContraintesAuNoeudRacine->NombreDeContraintes;
Probleme.IndicesDebutDeLigne                   = Pne->MatriceDesContraintesAuNoeudRacine->IndexDebut;
Probleme.NombreDeTermesDesLignes               = Pne->MatriceDesContraintesAuNoeudRacine->NombreDeTermes;
Probleme.IndicesColonnes                       = Pne->MatriceDesContraintesAuNoeudRacine->Colonne;
Probleme.CoefficientsDeLaMatriceDesContraintes = Pne->MatriceDesContraintesAuNoeudRacine->Coefficient;
Probleme.Sens                                  = Pne->MatriceDesContraintesAuNoeudRacine->Sens;
Probleme.SecondMembre                          = Pne->MatriceDesContraintesAuNoeudRacine->SecondMembre;

Probleme.ChoixDeLAlgorithme = ChoixDeLAlgorithme;    

Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE;
Probleme.FaireDuScaling              = OUI_SPX;   
Probleme.StrategieAntiDegenerescence = AGRESSIF;

Probleme.BaseDeDepartFournie        = BaseDeDepartFournie;
Probleme.PositionDeLaVariable       = Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariable;
Probleme.NbVarDeBaseComplementaires = Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementaires;
Probleme.ComplementDeLaBase         = Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBase;
  
Probleme.LibererMemoireALaFin  = NON_SPX;	

Probleme.CoutMax         = LINFINI_PNE;
Probleme.UtiliserCoutMax = NON_SPX;

Probleme.NombreDeContraintesCoupes        = 0; /* Car integrees dans les contraintes */
Probleme.BCoupes                          = NULL;
Probleme.PositionDeLaVariableDEcartCoupes = NULL;
Probleme.MdebCoupes                       = NULL;
Probleme.NbTermCoupes                     = NULL;
Probleme.NuvarCoupes                      = NULL;
Probleme.ACoupes                          = NULL;

Probleme.CoutsMarginauxDesContraintes = NULL;

Probleme.CoutsReduits = NULL;
  
Probleme.AffichageDesTraces = OUI_SPX /*Pne->AffichageDesTraces*/;

Pne->ProblemeSpxDuNoeudRacine = SPX_Simplexe( &Probleme , Pne->ProblemeSpxDuNoeudRacine );

/* On renseigne le Simplexe pour qu'il sache qui l'appelle */
Spx = NULL;
if ( Pne->ProblemeSpxDuNoeudRacine != NULL ) {
  Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuNoeudRacine;
  Spx->ProblemePneDeSpx = (void *) Pne;
}

ExistenceDUneSolution = Probleme.ExistenceDUneSolution;

if ( ExistenceDUneSolution == OUI_PNE ) {
  /* On recupere la base optimale pour la passer aux problemes suivant comme base de depart */
  Pne->MatriceDesContraintesAuNoeudRacine->BaseDeDepartFournie = OUI_SPX; /* Pret pour les coups suivants */
	memcpy( (char *) Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariableSV, 
	        (char *) Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariable,
					Pne->NombreDeVariablesTrav * sizeof( int ) );
	
  Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementairesSV = Probleme.NbVarDeBaseComplementaires;
	
	memcpy( (char *) Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBaseSV,
	        (char *) Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBase,
					Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementairesSV * sizeof( int ) );
	
	if ( Pne->AffichageDesTraces == OUI_PNE ) {
	  /* Mettre une trace */
	}	
}
else { 
  if ( Pne->ProblemeSpxDuNoeudRacine != NULL ) {  
    SPX_LibererProbleme( (PROBLEME_SPX *) Pne->ProblemeSpxDuNoeudRacine );
    Pne->ProblemeSpxDuNoeudRacine = NULL;
  }	
}
  
return;
}

















