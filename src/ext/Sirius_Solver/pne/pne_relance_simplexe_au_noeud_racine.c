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

   FONCTION: Relance du simplex au noeud racine en cours de branch and bound.
                
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

typedef struct{
int NombreDeCoupes;
double * SecondMembre;
char   * PositionDeLaVariableDEcart;
/* Chainage de la matrice des contraintes */
int   * Mdeb;
int   * NbTerm;
int   * Colonne; 
double * A;
} COUPES_SIMPLEXE_NOEUD_RACINE;

void PNE_LibererCoupesVioleesPourSimplexeAuNoeudRacine( COUPES_SIMPLEXE_NOEUD_RACINE * );
COUPES_SIMPLEXE_NOEUD_RACINE * PNE_AjouterCoupesVioleesPourSimplexeAuNoeudRacine(  PROBLEME_PNE * );

/*----------------------------------------------------------------------------*/
void PNE_LibererCoupesVioleesPourSimplexeAuNoeudRacine( COUPES_SIMPLEXE_NOEUD_RACINE * C )
{
free( C->SecondMembre ); free( C->PositionDeLaVariableDEcart ); free( C->Mdeb ); free( C->NbTerm );
free( C->Colonne );free( C->A );free( C );
return;
}
/*----------------------------------------------------------------------------*/
/* On insere les contraintes du pool qui sont violees par la solution courante */
COUPES_SIMPLEXE_NOEUD_RACINE * PNE_AjouterCoupesVioleesPourSimplexeAuNoeudRacine(  PROBLEME_PNE * Pne )
{
BB * Bb; NOEUD * NoeudRacine; int NumeroDeCoupe; COUPE ** Coupe; COUPE * Cpe;
double * X; int * IndiceDeLaVariable; double * Coefficient; double S;
char * T; int ilCoupes; int NombreDeCoupes; int il; int NbCoupes; int i; 
int * Mdeb; int * NbTerm; double * SecondMembre; char * PositionDeLaVariableDEcart;
int * Colonne; double * A; COUPES_SIMPLEXE_NOEUD_RACINE * CoupesPourLeSimplexe;

Bb = Pne->ProblemeBbDuSolveur;
if ( Bb == NULL ) return( NULL );
NoeudRacine = Bb->NoeudRacine;
Coupe = NoeudRacine->CoupesGenereesAuNoeud;
X = Pne->MatriceDesContraintesAuNoeudRacine->X;
NombreDeCoupes = NoeudRacine->NombreDeCoupesGenereesAuNoeud;

T = (char *) malloc( NombreDeCoupes * sizeof( char ) );
if ( T == NULL ) return( NULL );
memset( (char *) T, 0, NombreDeCoupes * sizeof( char ) );

ilCoupes = 0;
NbCoupes = 0;
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < NombreDeCoupes ; NumeroDeCoupe++ ) {
  S = 0.;
  Cpe = Coupe[NumeroDeCoupe];
  Coefficient = Cpe->Coefficient;
  IndiceDeLaVariable = Cpe->IndiceDeLaVariable;
  for ( il = 0 ; il < Cpe->NombreDeTermes ; il++ ) S += Coefficient[il] * X[IndiceDeLaVariable[il]];
  if ( S > Cpe->SecondMembre ) {
		T[NumeroDeCoupe] = 1;
		ilCoupes += Cpe->NombreDeTermes;
		NbCoupes++;		
  }
}

if ( NbCoupes <= 0 ) {
	free( T );
  return( NULL ); 
}

CoupesPourLeSimplexe = malloc( sizeof( COUPES_SIMPLEXE_NOEUD_RACINE ) );
Mdeb = (int *) malloc( NbCoupes * sizeof( int ) );
NbTerm = (int *) malloc( NbCoupes * sizeof( int ) );
SecondMembre = (double *) malloc( NbCoupes * sizeof( double ) );
PositionDeLaVariableDEcart = (char *) malloc( NbCoupes * sizeof( char ) );
ilCoupes += 10;
Colonne = (int *) malloc( ilCoupes * sizeof( int ) );
A  = (double *) malloc( ilCoupes * sizeof( double ) );
if ( A == NULL || Colonne == NULL || PositionDeLaVariableDEcart == NULL || SecondMembre == NULL ||
     NbTerm == NULL || Mdeb == NULL || CoupesPourLeSimplexe == NULL ) {
  free( A ); free( Colonne ); free( PositionDeLaVariableDEcart ); free( SecondMembre );
	free( NbTerm ); free( Mdeb );  free( CoupesPourLeSimplexe );
	free( T );
  return( NULL );
}

CoupesPourLeSimplexe->Mdeb = Mdeb;
CoupesPourLeSimplexe->NbTerm = NbTerm;
CoupesPourLeSimplexe->SecondMembre = SecondMembre;
CoupesPourLeSimplexe->PositionDeLaVariableDEcart = PositionDeLaVariableDEcart;
CoupesPourLeSimplexe->Colonne = Colonne;
CoupesPourLeSimplexe->A = A;
		 
il = 0;
NbCoupes = 0;
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < NombreDeCoupes ; NumeroDeCoupe++ ) {
  if ( T[NumeroDeCoupe] == 0 ) continue;
  Cpe = Coupe[NumeroDeCoupe];
  Coefficient = Cpe->Coefficient;
  IndiceDeLaVariable = Cpe->IndiceDeLaVariable;
	/* On ajoute la coupe */
  Mdeb[NbCoupes] = il;
  NbTerm[NbCoupes] = Cpe->NombreDeTermes;
  for ( i = 0 ; i < Cpe->NombreDeTermes ; i++ ) {
    A      [il] = Coefficient[i];
    Colonne[il] = IndiceDeLaVariable[i];  
    il++;
  }
  /* Second membre */
  SecondMembre[NbCoupes] = Cpe->SecondMembre;
	PositionDeLaVariableDEcart[NbCoupes] = EN_BASE;
  NbCoupes++;		
  
}
CoupesPourLeSimplexe->NombreDeCoupes = NbCoupes;

free( T );

return( CoupesPourLeSimplexe );

}

/*----------------------------------------------------------------------------*/

void PNE_RelanceDuSimplexeAuNoeudRacine(  PROBLEME_PNE * Pne, int * ExistenceDUneSolution )
{
int NombreMaxDIterations; double Critere; double * L; double * X; int Var;
PROBLEME_SIMPLEXE Probleme; COUPES_SIMPLEXE_NOEUD_RACINE * Coupes; 

*ExistenceDUneSolution = NON_PNE;
if ( Pne->MatriceDesContraintesAuNoeudRacine == NULL ) return;
if ( Pne->ProblemeSpxDuNoeudRacine == NULL ) return;

Coupes = PNE_AjouterCoupesVioleesPourSimplexeAuNoeudRacine( Pne );
if ( Coupes == NULL ) return;

NombreMaxDIterations = -1;		
Probleme.Contexte = BRANCH_AND_BOUND_OU_CUT_NOEUD;
Probleme.NombreMaxDIterations = NombreMaxDIterations;
Probleme.DureeMaxDuCalcul     = -1.;

Probleme.CoutLineaire      = Pne->LTrav;
Probleme.X                 = Pne->MatriceDesContraintesAuNoeudRacine->X;
Probleme.Xmin              = Pne->UminTravSv;
Probleme.Xmax              = Pne->UmaxTravSv;
Probleme.NombreDeVariables = Pne->NombreDeVariablesTrav;
Probleme.TypeDeVariable    = Pne->TypeDeBorneTravSv;

Probleme.NombreDeContraintes                   = Pne->MatriceDesContraintesAuNoeudRacine->NombreDeContraintes;
Probleme.IndicesDebutDeLigne                   = Pne->MatriceDesContraintesAuNoeudRacine->IndexDebut;
Probleme.NombreDeTermesDesLignes               = Pne->MatriceDesContraintesAuNoeudRacine->NombreDeTermes;
Probleme.IndicesColonnes                       = Pne->MatriceDesContraintesAuNoeudRacine->Colonne;
Probleme.CoefficientsDeLaMatriceDesContraintes = Pne->MatriceDesContraintesAuNoeudRacine->Coefficient;
Probleme.Sens                                  = Pne->MatriceDesContraintesAuNoeudRacine->Sens;
Probleme.SecondMembre                          = Pne->MatriceDesContraintesAuNoeudRacine->SecondMembre;

Probleme.ChoixDeLAlgorithme = SPX_DUAL;    

Probleme.TypeDePricing               = PRICING_STEEPEST_EDGE;
Probleme.FaireDuScaling              = OUI_SPX;   
Probleme.StrategieAntiDegenerescence = AGRESSIF;

/* On reinit de la base de depart */
Pne->MatriceDesContraintesAuNoeudRacine->BaseDeDepartFournie = OUI_SPX; 
memcpy( (char *) Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariable, 
	      (char *) Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariableSV,
				Pne->NombreDeVariablesTrav * sizeof( int ) );

Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementaires = Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementairesSV; 

memcpy( (char *) Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBase,
	      (char *) Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBaseSV,
				Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementairesSV * sizeof( int ) );
	
/* */
Probleme.BaseDeDepartFournie        = Pne->MatriceDesContraintesAuNoeudRacine->BaseDeDepartFournie;
Probleme.PositionDeLaVariable       = Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariable;
Probleme.NbVarDeBaseComplementaires = Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementaires;
Probleme.ComplementDeLaBase         = Pne->MatriceDesContraintesAuNoeudRacine->ComplementDeLaBase;
  
Probleme.LibererMemoireALaFin = NON_SPX;	

Probleme.CoutMax         = LINFINI_PNE;
Probleme.UtiliserCoutMax = NON_SPX;

printf("Relance du SIMPLEXE au noeud racine Nomnre De Coupes = %d\n",Coupes->NombreDeCoupes);

Probleme.NombreDeContraintesCoupes        = Coupes->NombreDeCoupes; 
Probleme.BCoupes                          = Coupes->SecondMembre;
Probleme.PositionDeLaVariableDEcartCoupes = Coupes->PositionDeLaVariableDEcart;
Probleme.MdebCoupes                       = Coupes->Mdeb;
Probleme.NbTermCoupes                     = Coupes->NbTerm;
Probleme.NuvarCoupes                      = Coupes->Colonne;
Probleme.ACoupes                          = Coupes->A;

Probleme.CoutsMarginauxDesContraintes = NULL;

Probleme.CoutsReduits = Pne->MatriceDesContraintesAuNoeudRacine->CoutsReduits;
  
Probleme.AffichageDesTraces = NON_SPX /*Pne->AffichageDesTraces*/;

Pne->ProblemeSpxDuNoeudRacine = SPX_Simplexe( &Probleme , Pne->ProblemeSpxDuNoeudRacine );
*ExistenceDUneSolution = Probleme.ExistenceDUneSolution;

PNE_LibererCoupesVioleesPourSimplexeAuNoeudRacine( Coupes );

if ( *ExistenceDUneSolution == OUI_PNE ) {
  Pne->MatriceDesContraintesAuNoeudRacine->NbVarDeBaseComplementaires = Probleme.NbVarDeBaseComplementaires;
  Pne->MatriceDesContraintesAuNoeudRacine->BaseDeDepartFournie = OUI_SPX;
	Critere = Pne->Z0;
	L = Pne->LTrav;
	X = Pne->MatriceDesContraintesAuNoeudRacine->X;
	for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
	  Critere += L[Var] * X[Var];
		if ( X[Var] < Pne->UminTravSv[Var] || X[Var] > Pne->UmaxTravSv[Var] ) {
      printf("X %e min %e max %e Type borne %d \n",X[Var],Pne->UminTravSv[Var],Pne->UmaxTravSv[Var],Pne->TypeDeBorneTravSv[Var]);
		}
	}   

	printf("RELANCE SIMPLEXE AU NOEUD RACINE Iteration %d\n",((PROBLEME_SPX *) Pne->ProblemeSpxDuNoeudRacine)->Iteration);

	printf("RELANCE SIMPLEXE AU NOEUD RACINE Nouveau critere %e ancien cirter %e\n",Critere,Pne->CritereAuNoeudRacine);

	
  PNE_ArchivagesPourReducedCostFixingAuNoeudRacine( Pne,
																									  Pne->MatriceDesContraintesAuNoeudRacine->PositionDeLaVariable,
																										Pne->MatriceDesContraintesAuNoeudRacine->CoutsReduits,
																										Critere );	
}
else {
  printf("RELANCE SIMPLEXE AU NOEUD RACINE Pas de solution\n");
  Pne->MatriceDesContraintesAuNoeudRacine->BaseDeDepartFournie = NON_SPX;
}

return;
}
