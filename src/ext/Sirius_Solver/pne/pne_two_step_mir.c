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

   FONCTION: Two step MIR tire de l'article paru dans INFORMS journal of
	           computing: Two Step MIR inegualities for mixed integer
						 programs de Dash Goycoolea Gunluk .
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_fonctions.h"
# include "spx_define.h"
  
# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define PAS_DE_SUBSTITUTION 0
# define SUBSTITUTION_BORNE_INF 1
# define SUBSTITUTION_BORNE_SUP 2

# define VIOLATION_MIN 1.e-4
# define F_NUL 1.e-7
# define ALPHA_MIN 5.e-4
# define RHO_NUL 1.e-5
# define ZERO_COEFF 1.e-12


# ifdef CMP2STEP

char PNE_TwoMirCalulerAlphaEtTau( double , double , double * , double * );  

void PNE_RechercheTwoStepMir( PROBLEME_PNE * , double , int , double * , double , double * , int * , char * ,
															double * , int , int * , char * , double * , double * , double * , int * , char * , char );														
															
/*----------------------------------------------------------------------------*/
/* On cherche Alpha = Fi / k ou Fi = partie fractionnaire du coefficent ai
	 On note FO la partie fractionnaire du second membre:
	 on choisi k parmi les entiers qui verifient F0 / (Fi/k) <= 20
	 De plus le choix de t doit conduite a un Alpha admissible c'est a dire
	 etre tel que t = ceil( F0/Alpha ) et 1/Alpha >= t
*/
char PNE_TwoMirCalulerAlphaEtTau( double F0, double Fi, double * TauChoisi, double * AlphaChoisi )   
{
int kmax; int CodeRet; double Alpha; int k; double Rho; double Tau;
/* Doit satisfaire la contion F0 / (Fi/k) <= 20 i.e. k <= 20 Fi/F0 */
kmax = ceil( 20 * (Fi/F0) ); /* Pour tomber su un entier */
CodeRet = 0;
for ( k = 1 ; k <= kmax ; k++ ) {
  Alpha = Fi / (double) k;
	if ( Alpha < ALPHA_MIN ) break;
	if ( F0 > Alpha + F_NUL ) {
	  Tau = (int) ceil( F0/Alpha );
		if ( Tau >= 2 ) {
      if ( 1/Alpha >= Tau ) {
	      if ( Tau > (F0/Alpha) + F_NUL ) {
			    Rho = F0 - ( Alpha * floor( F0/Alpha ) );				
				  if ( Rho > RHO_NUL ) {					
            /* t donne un Alpha admissible => on arrete les calculs */
		        *AlphaChoisi = Alpha;
		        *TauChoisi = Tau;
		        CodeRet = 1;
		        /*
			      printf("AlphaChoisi %e TauChoisi %e \n",*AlphaChoisi,*TauChoisi);
		        */
			      break;
				  }
			  }
			}
		}
	}
}
return( CodeRet );
}
/*----------------------------------------------------------------------------*/
/* On teste la violation de la MIR */

void PNE_RechercheTwoStepMir( PROBLEME_PNE * Pne, double ValDeVarContinue, int NbVarEntieres,
                              double * CoeffDeLaVariableEntiere, double SecondMembre,
															double * ValeurDeLaVariableEntiere,
                              int * NumeroDeLaVariableEntiere,
															char * TypeDeSubstitutionEntiere,
															double * BorneDeSubstitutionEntiere,
															int NbVarContinues,
															int * NumeroDeLaVariableContinue,
	                            char * TypeDeSubstitutionContinue,
															double * BorneDeSubstitutionContinue,															
                              double * CoefficientDeLaVariableContinue,
															double * Coeff,
															int * Indice,
															char * T,
															char NormaliserLaCoupe )															
{
double F0; int j; double Fj; double Fi; double Tau; double Alpha; double RhoFoisTau; int NbT;
int CodeRet; double Rho; double MembreDeGauche; double Sec; double Terme_2; double X;
double Terme_3; double gAlpha; double Coeffi; int i; int Var; double PlusGrandCoeff;
int NbMaxTests; int Step; int NombreDeVariables; double * B; int VarSpx; int il; int ilMax;
int Var2Spx; int Cnt;
PROBLEME_SPX * Spx;

F0 = SecondMembre - floor( SecondMembre );
if ( F0 < F_NUL ) {
  return;
}

Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
if ( Spx == NULL ) return;
B = Spx->DonneesPourCoupesDeGomory->B; /* Attention a la translation de Xminentree */
/*
printf("Attention on est en train de mettre au point l'utilisation d'un tableau T pour les variables d'ecart du simplexe \n");
*/
NombreDeVariables = Pne->NombreDeVariablesTrav;
/* On teste ne teste pas plus de NbMaxTests valeurs car sinon ca consomme trop de temps */
NbMaxTests = 10;
Step = floor( (NbVarEntieres+1) / NbMaxTests );
if ( Step < 1 ) Step = 1;
if ( NbVarEntieres <= 5 ) Step = 1;

for ( j = 0 ; j < NbVarEntieres ; j = j + Step ) {
  Fj = CoeffDeLaVariableEntiere[j] - floor( CoeffDeLaVariableEntiere[j] );
  if ( Fj < F_NUL ) continue;
	if ( Fj >= F0 ) continue;
	/*if ( ValeurDeLaVariableEntiere[j] == 0.0 ) continue;*/
  CodeRet = PNE_TwoMirCalulerAlphaEtTau( F0, Fj, &Tau, &Alpha );  
  if ( CodeRet == 0 ) continue; 
		
	Rho = F0 - ( Alpha * floor( F0/Alpha ) );	
	if ( Rho < RHO_NUL ) continue;
	
	RhoFoisTau = Rho * Tau;
  Sec =  RhoFoisTau * ceil( SecondMembre );
  /* Relatif a toutes les variables continues ayant un coefficient positif */
	MembreDeGauche = ValDeVarContinue;
  for ( i = 0 ; i < NbVarEntieres ; i++ ) {
    Fi = CoeffDeLaVariableEntiere[i] - floor( CoeffDeLaVariableEntiere[i] );	  
    /* Calcul de g^Alpha(Fi) */
    Terme_2 = Rho * floor( Fi/Alpha );
    Terme_2 += Fi;
    Terme_2 -= Alpha * floor( Fi/Alpha );
    Terme_3 = Rho * ceil( Fi/Alpha );
    gAlpha = RhoFoisTau;
    if ( Terme_2 < gAlpha ) gAlpha = Terme_2;
    if ( Terme_3 < gAlpha ) gAlpha = Terme_3;
    Coeffi = ( RhoFoisTau * floor( CoeffDeLaVariableEntiere[i] ) ) + gAlpha;
    MembreDeGauche += Coeffi * ValeurDeLaVariableEntiere[i];
  }	
  if ( MembreDeGauche < Sec - VIOLATION_MIN ) {

	  printf(" violation 2step %e\n",Sec-MembreDeGauche);
		
		/* Variables continues */
    for ( i = 0 ; i < NbVarContinues ; i++ ) {
		  X = CoefficientDeLaVariableContinue[i];
      Var = NumeroDeLaVariableContinue[i];
			if ( Var < 0  ) {
			  /*
        printf("Variable d'ecart du simplexe a trazouiller\n");
				*/
        VarSpx = -Var - 1;
        /* Var est une variable non native */
        /* Contrainte a laquelle appartient la variable d'ecart */
        Cnt = Spx->NumeroDeContrainte[Spx->Cdeb[VarSpx]];	
        /* Remplacement de la variable d'ecart */
        il    = Spx->Mdeb[Cnt]; 
        ilMax = il + Spx->NbTerm[Cnt];
        while ( il < ilMax ) {
          Var2Spx = Spx->Indcol[il];
					if ( Spx->OrigineDeLaVariable[Var2Spx] == NATIVE ) { 
					  Var = Spx->CorrespondanceVarSimplexeVarEntree[Var2Spx]; 
            /* Ici il n'y a pas lieu de tenir compte du fait que la variable serait HORS_BASE_SUR_BORNE_SUP */ 
            if ( T[Var] == 0 ) {
						  Coeff[Var] = -X * Spx->A[il] * Spx->ScaleX[VarSpx] / Spx->ScaleX[Var2Spx];
              T[Var] = 1;
						}
						else {
						  Coeff[Var] -= X * Spx->A[il] * Spx->ScaleX[VarSpx] / Spx->ScaleX[Var2Spx];
						}
		      }
          il++;
        }   
        /* Attention a la translation de Xminentree */
	      Sec -= X * B[Cnt] * Spx->ScaleX[VarSpx] * Spx->SupXmax;
				continue;
			}
			/* Si Var est negatif on a fait la substitution U~ =  Umax - U <=> U = Umax - U~
			   il faut donc revenir aux variables intiales */
			if ( TypeDeSubstitutionContinue[i] == SUBSTITUTION_BORNE_SUP ) {
       /* On a fait U = Umax - U~ donc U~ = Umax - U */
        Sec -= X * BorneDeSubstitutionContinue[i];
				X *= -1.;
			}
			else if ( TypeDeSubstitutionContinue[i] == SUBSTITUTION_BORNE_INF ) {
        /* On a fait U = Umin + U~ donc U~ = U - Umin */
        Sec += X * BorneDeSubstitutionContinue[i];
			}
      if ( T[Var] == 0 ) {
			  Coeff[Var] = X;				 						
			  T[Var] = 1;
			}
			else {
			  Coeff[Var] += X;				 						
			}
    }		
    for ( i = 0 ; i < NbVarEntieres ; i++ ) {
      Fi = CoeffDeLaVariableEntiere[i] - floor( CoeffDeLaVariableEntiere[i] );	  
      /* Calcul de g^Alpha(Fi) */
      Terme_2 = Rho * floor( Fi/Alpha );
      Terme_2 += Fi;
      Terme_2 -= Alpha * floor( Fi/Alpha );
      Terme_3 = Rho * ceil( Fi/Alpha );
      gAlpha = RhoFoisTau;
      if ( Terme_2 < gAlpha ) gAlpha = Terme_2;
      if ( Terme_3 < gAlpha ) gAlpha = Terme_3;			
			X = ( RhoFoisTau * floor( CoeffDeLaVariableEntiere[i] ) ) + gAlpha;
			Var = NumeroDeLaVariableEntiere[i];

			if ( Var < 0 || Var >= Pne->NombreDeVariablesTrav ) {
			  printf("Erreur Var = %d\n",Var);  
				exit(0);
			}
			
			if ( TypeDeSubstitutionEntiere[i] == SUBSTITUTION_BORNE_SUP ) {
        /* On a fait U = Umax - U~ donc U~ = Umax - U */
        Sec -= X * BorneDeSubstitutionEntiere[i];
			  X *= -1.;				
			}
			else if ( TypeDeSubstitutionEntiere[i] == SUBSTITUTION_BORNE_INF ) {
        /* On a fait U = Umin + U~ donc U~ = U - Umin */
        Sec += X * BorneDeSubstitutionEntiere[i];
			}
      if ( T[Var] == 0 ) {
			  Coeff[Var] = X;				 						
			  T[Var] = 1;
			}
			else {
			  Coeff[Var] += X;				 						
			}			
    }
					
		/* On transforme en une contrainte de type <= en multipliant par -1 et on supprime les termes nuls */
		NbT = 0;
		for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
      if ( T[Var] == 1 ) {
			  Coeff[NbT] = -Coeff[Var];
				Indice[NbT] = Var;
				NbT++;
				T[Var] = 0;
			}  
		}
		Sec *= -1.0;			
    if ( NbT > 0 ) {					
			if ( NormaliserLaCoupe == OUI_PNE ) {
			  PlusGrandCoeff = -LINFINI_PNE;			
        for ( i = 0 ; i < NbT ; i++ ) {
			    if ( fabs( Coeff[i] ) > PlusGrandCoeff ) PlusGrandCoeff = fabs( Coeff[i] );
        }
        PNE_NormaliserUnCoupe( Coeff, &Sec, NbT, PlusGrandCoeff );
			}
		  X = 1.e+3; /* La violation ne sert pas a grand chose */
			printf("Archivage d'une 2mir \n");
      PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'G', NbT, Sec, X, Coeff , Indice );			
		}
  }
}

return;
}

/*----------------------------------------------------------------------------*/
/* On recoit une contraite sur laquelle on va faire une 2 step MIR */
void PNE_TwoStepMirSurContrainte( PROBLEME_PNE * Pne )
{
double Signe; double ai; int Cnt; char * ContrainteMixte; double * B; int * Mdeb;
int * NbTerm; int * Nuvar; int * TypeDeVariable; int * TypeDeBorne; int LallocTas;
double * A; double * U; double * Umin; double * Umax; double * VariableDuale;
char * SensContrainte; double ValDeVarContinue; double SecondMembre; char * T;
double * CoeffDeLaVariableEntiere; double * ValeurDeLaVariableEntiere; char * pt;
int * NumeroDeLaVariableEntiere; char * Buff; int NombreDeVariables; int NombreDeContraintes;
int NbVarEntieres; int il; int ilMax; int Var;  double CoeffDiviseur; int i; double Xu;
int IndexVariableContinue; int NbVarContinues; int * NumeroDeLaVariableContinue;
double DeltaSecondMembre; double * CoeffCoupe; int * IndiceCoupe; char NormaliserLaCoupe;
double * CoefficientDeLaVariableContinue; int Nn; double X; double u;
char * TypeDeSubstitutionEntiere; char * TypeDeSubstitutionContinue; char TypeDeSubstitution;
double * BorneDeSubstitutionEntiere; double * BorneDeSubstitutionContinue; double BorneDeSubstitution;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
ContrainteMixte = Pne->ContrainteMixte;
/*if ( ContrainteMixte == NULL ) return;*/
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
U = Pne->UTrav;
Umin = Pne->UminTravSv;
Umax = Pne->UmaxTravSv;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
NormaliserLaCoupe = NON_PNE;

LallocTas = 0; 
LallocTas += NombreDeVariables * sizeof( double ); /* CoeffDeLaVariableEntiere */
LallocTas += NombreDeVariables * sizeof( double ); /* ValeurDeLaVariableEntiere */
LallocTas += NombreDeVariables * sizeof( int ); /* NumeroDeLaVariableEntiere */
LallocTas += NombreDeVariables * sizeof( char ); /* TypeDeSubstitutionEntiere */
LallocTas += NombreDeVariables * sizeof( double ); /* BorneDeSubstitutionEntiere */
LallocTas += NombreDeVariables * sizeof( double ); /* CoeffCoupe */
LallocTas += NombreDeVariables * sizeof( int ); /* IndiceCoupe */
LallocTas += NombreDeVariables * sizeof( char ); /* T */

Buff = (char *) malloc( LallocTas );
if ( Buff == NULL )	return;

pt = Buff;
CoeffDeLaVariableEntiere = (double *) pt;
pt += NombreDeVariables * sizeof( double );
ValeurDeLaVariableEntiere = (double *) pt;
pt += NombreDeVariables * sizeof( double );
NumeroDeLaVariableEntiere = (int *) pt;
pt += NombreDeVariables * sizeof( int );
TypeDeSubstitutionEntiere = (char *) pt;
pt += NombreDeVariables * sizeof( char );
BorneDeSubstitutionEntiere = (double *) pt;
pt += NombreDeVariables * sizeof( double );
CoeffCoupe = (double *) pt;
pt += NombreDeVariables * sizeof( double );
IndiceCoupe = (int *) pt;
pt += NombreDeVariables * sizeof( int );
T = (char *) pt;
pt += NombreDeVariables * sizeof( char );

memset( (char *) T, 0, NombreDeVariables * sizeof( char ) );

VariableDuale = Pne->VariablesDualesDesContraintesTravEtDesCoupes;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {  
  /*if ( ContrainteMixte[Cnt] == NON_PNE ) continue;*/
	/*if ( fabs( VariableDuale[Cnt] ) < SEUIL_VARIABLE_DUALE_POUR_CALCUL_DE_COUPE ) continue;*/
	/* Important: dans la papier de Dash, la contrainte de depart est du type
	   >= . Comme dans la PNE on n'a que des egalites et des <= , si on a une
		 contrainte de type <= on change le signe des coefficients et du second membre */
	if ( SensContrainte[Cnt] == '<' ) Signe = -1.0;
	else Signe = 1.0;	
	SecondMembre = Signe * B[Cnt]; 
	
	ValDeVarContinue = 0.0;
	NbVarEntieres = 0;
	Nn = 0;

	memset( (char *) TypeDeSubstitutionEntiere, PAS_DE_SUBSTITUTION, NombreDeVariables * sizeof( char ) );
	
  NumeroDeLaVariableContinue = &NumeroDeLaVariableEntiere[NombreDeVariables - 1];
	CoefficientDeLaVariableContinue = &CoeffDeLaVariableEntiere[NombreDeVariables - 1];	
  TypeDeSubstitutionContinue = &TypeDeSubstitutionEntiere[NombreDeVariables - 1];
	BorneDeSubstitutionContinue = &BorneDeSubstitutionEntiere[NombreDeVariables - 1];
	
  IndexVariableContinue = 0;
	NbVarContinues = 0;
	CoeffDiviseur = 0;
	il = Mdeb[Cnt];		
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  if ( A[il] == 0.0 ) goto NextIl;
	  Var = Nuvar[il];
    if ( TypeDeBorne[Var] == VARIABLE_FIXE ) {
      SecondMembre -= Signe * A[il] * U[Var];
			goto NextIl;
		}
    if ( TypeDeVariable[Var] != ENTIER ) {
      /* Variable entiere elle ne doit pas etre non bornee */
      if ( TypeDeBorne[Var] == VARIABLE_NON_BORNEE ) {
			  NbVarEntieres = -1;
			  goto FinPreparation;
			}
			/* Si la coefficient est > on comptabilise la variable */			
      ai = Signe * A[il];						
			Xu = U[Var];
			DeltaSecondMembre = 0.0;
      TypeDeSubstitution = PAS_DE_SUBSTITUTION;
			BorneDeSubstitution = 0.0;
			if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
        if ( Xu > 0.5 * Umax[Var] ) {
	        /* On remplace par U~ =  Umax - U <=> U = Umax - U~ */
          DeltaSecondMembre = ai * Umax[Var];					
          Xu = Umax[Var] - U[Var];
					ai *= -1.0;
					TypeDeSubstitution = SUBSTITUTION_BORNE_SUP;
					BorneDeSubstitution = Umax[Var];
				}
			}
			if ( ai > 0.0 ) {
			  ValDeVarContinue += ai * Xu;
        SecondMembre -= DeltaSecondMembre;
			  /* On enregistre le variable */	 				
				CoefficientDeLaVariableContinue[IndexVariableContinue] = ai;
			  NumeroDeLaVariableContinue[IndexVariableContinue] = Var;
        TypeDeSubstitutionContinue[IndexVariableContinue] = TypeDeSubstitution;
        BorneDeSubstitutionContinue[IndexVariableContinue] = BorneDeSubstitution;			
			  IndexVariableContinue--;
			  NbVarContinues++;
			}
			goto NextIl;
		}		
    if ( TypeDeVariable[Var] == ENTIER ) {
      ai = Signe * A[il];						
			Xu = U[Var];
			DeltaSecondMembre = 0.0;
      TypeDeSubstitution = PAS_DE_SUBSTITUTION;
			BorneDeSubstitution = 0.0;			
			if ( U[Var] != Umin[Var] && U[Var] != Umax[Var] ) {
			  CoeffDiviseur += fabs( ai );
				Nn++;
			}
      /* Substitution eventuelle de borne */
	    if ( U[Var] > 0.5 * Umax[Var] ) {
	      /* On remplace par U~ =  Umax - U <=> U = Umax - U~ */
				DeltaSecondMembre = ai * Umax[Var];
        Xu = Umax[Var] - U[Var];
				ai *= -1.0;
				TypeDeSubstitution = SUBSTITUTION_BORNE_SUP;
				BorneDeSubstitution = Umax[Var];				
			}
      SecondMembre -= DeltaSecondMembre;
			/* On enregistre le variable */	 				
			CoeffDeLaVariableEntiere[NbVarEntieres] = ai;
			ValeurDeLaVariableEntiere[NbVarEntieres] = Xu;		
		  NumeroDeLaVariableEntiere[NbVarEntieres] = Var;
      TypeDeSubstitutionEntiere[NbVarEntieres] = TypeDeSubstitution;
      BorneDeSubstitutionEntiere[NbVarEntieres] = BorneDeSubstitution;						
		  NbVarEntieres++;
    }
		NextIl:
		il++;
	}
	
	FinPreparation:  
	if ( NbVarEntieres < 1 || CoeffDiviseur <= 0.0 ) continue;

	if ( Nn > 1 ) CoeffDiviseur /= Nn;

	/* On change CoeffDiviseur pour ne pas avoir toujours la meme valeur */
	
  X = fabs( Pne->Critere / NombreDeContraintes );
	u = fabs( VariableDuale[Cnt] );
	if ( X < u ) CoeffDiviseur += 1/(1+X);
	else CoeffDiviseur += 1/(1+u);

	if ( CoeffDiviseur < 1.e-2 ) CoeffDiviseur = 1.e-2;
	else if ( CoeffDiviseur > 1.e+2 ) CoeffDiviseur = 1.e+2;
		
	/*printf("CoeffDiviseur = %e  Cnt %d\n",CoeffDiviseur,Cnt);*/
	 
	CoefficientDeLaVariableContinue = &CoefficientDeLaVariableContinue[IndexVariableContinue+1];
	NumeroDeLaVariableContinue = &NumeroDeLaVariableContinue[IndexVariableContinue+1];
  TypeDeSubstitutionContinue = &TypeDeSubstitutionContinue[IndexVariableContinue+1];
  BorneDeSubstitutionContinue = &BorneDeSubstitutionContinue[IndexVariableContinue+1];
	
	/* On divise tout par le coeff d'une variable entiere qui n'est pas sur borne */	
  for ( i = 0 ; i < NbVarEntieres ; i++ ) CoeffDeLaVariableEntiere[i] /= CoeffDiviseur;	
  for ( i = 0 ; i < NbVarContinues ; i++ ) CoefficientDeLaVariableContinue[i] /= CoeffDiviseur;
	
	SecondMembre /= CoeffDiviseur;
	ValDeVarContinue /= CoeffDiviseur;

	/*
  printf("Tentative 2stepMIR  Cnt %d ValDeVarContinue %e sens %c CoeffDiviseur %e SecondMembre %e\n",
	        Cnt,ValDeVarContinue,SensContrainte[Cnt],CoeffDiviseur,SecondMembre);
  */	
	
  PNE_RechercheTwoStepMir( Pne, ValDeVarContinue, NbVarEntieres, CoeffDeLaVariableEntiere,
	                         SecondMembre, ValeurDeLaVariableEntiere, NumeroDeLaVariableEntiere,
													 TypeDeSubstitutionEntiere, BorneDeSubstitutionEntiere,
													 NbVarContinues, NumeroDeLaVariableContinue, TypeDeSubstitutionContinue,
													 BorneDeSubstitutionContinue, CoefficientDeLaVariableContinue,
													 CoeffCoupe, IndiceCoupe, T, NormaliserLaCoupe );  
	
}

free( Buff );

return;
}

/*----------------------------------------------------------------------------*/
/* On essaie de trouver une 2 step MIR sur une ligne du tableau su simplexe */
void PNE_TwoStepMirSurTableau( PROBLEME_PNE * Pne, int VariablePneFractionnaire,
                               double ValeurDuZero )
{
double Signe; double ai; int * TypeDeVariable; int * TypeDeBorne;
int LallocTas; double * U; char NormaliserLaCoupe; char * T;
double ValDeVarContinue; double SecondMembre; double * CoeffDeLaVariableEntiere;
double * ValeurDeLaVariableEntiere; char * pt; int * NumeroDeLaVariableEntiere;
char * Buff; /* int NombreDeVariables;*/  int NbVarEntieres; int VariableFractionnaireSpx;
double Xu; double DeltaSecondMembre; int IndexVariableContinue; int NbVarContinues;
int * NumeroDeLaVariableContinue; double AlphaI0; int VarSpx; 
int VarPne; char * LaVariableSpxEstEntiere; double * CoeffCoupe; int * IndiceCoupe;
double * CoefficientDeLaVariableContinue; double * B; int NombreDeTermes; int i;
int * IndiceDeLaVariableSpx; double * CoefficientSpx; char CodeRet;
char * TypeDeSubstitutionEntiere; char * TypeDeSubstitutionContinue; char TypeDeSubstitution;
double * BorneDeSubstitutionEntiere; double * BorneDeSubstitutionContinue; double BorneDeSubstitution;
int NbVar; int Cnt; int il; int ilMax; int Var2Spx;

DONNEES_POUR_COUPES_DE_GOMORY * DonneesPourCoupesDeGomory;
DONNEES_POUR_COUPES_DINTERSECTION * DonneesPourCoupesDIntersection; 
PROBLEME_SPX * Spx;

BB * Bb;

Bb = (BB *) Pne->ProblemeBbDuSolveur;
/* Uniquement au noeud racine */
if ( Bb->NoeudEnExamen != Bb->NoeudRacine ) return;

Spx = (PROBLEME_SPX *) Pne->ProblemeSpxDuSolveur;
if ( Spx == NULL ) return;
DonneesPourCoupesDIntersection = Spx->DonneesPourCoupesDIntersection;
if ( DonneesPourCoupesDIntersection == NULL ) return;
DonneesPourCoupesDeGomory = Spx->DonneesPourCoupesDeGomory;
if ( DonneesPourCoupesDeGomory == NULL ) return;
LaVariableSpxEstEntiere = DonneesPourCoupesDeGomory->LaVariableSpxEstEntiere;
B = DonneesPourCoupesDeGomory->B;
  
/* NombreDeVariables = Pne->NombreDeVariablesTrav; */
U = Pne->UTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;

if ( Spx->NombreDeVariables > Pne->NombreDeVariablesTrav ) NbVar = Spx->NombreDeVariables;
else NbVar = Pne->NombreDeVariablesTrav;


LallocTas = 0; 
LallocTas += NbVar * sizeof( double ); /* CoeffDeLaVariableEntiere */
LallocTas += NbVar * sizeof( double ); /* ValeurDeLaVariableEntiere */
LallocTas += NbVar * sizeof( int ); /* NumeroDeLaVariableEntiere */
LallocTas += NbVar * sizeof( int ); /* IndiceDeLaVariableSpx */
LallocTas += NbVar * sizeof( double ); /* CoefficientSpx */
LallocTas += NbVar * sizeof( char ); /* TypeDeSubstitutionEntiere */
LallocTas += NbVar * sizeof( double ); /* BorneDeSubstitutionEntiere */
LallocTas += NbVar * sizeof( double ); /* CoeffCoupe */
LallocTas += NbVar * sizeof( int ); /* IndiceCoupe */
LallocTas += NbVar * sizeof( char ); /* T */

Buff = (char *) malloc( LallocTas );
if ( Buff == NULL )	return;

pt = Buff;
CoeffDeLaVariableEntiere = (double *) pt;
pt += NbVar * sizeof( double );
ValeurDeLaVariableEntiere = (double *) pt;
pt += NbVar * sizeof( double );
NumeroDeLaVariableEntiere = (int *) pt;
pt += NbVar * sizeof( int );
IndiceDeLaVariableSpx = (int *) pt;
pt += NbVar * sizeof( int );
CoefficientSpx = (double *) pt;
pt += NbVar * sizeof( double );
TypeDeSubstitutionEntiere = (char *) pt;
pt += NbVar * sizeof( char );
BorneDeSubstitutionEntiere = (double *) pt;
pt += NbVar * sizeof( double );
CoeffCoupe = (double *) pt;
pt += NbVar * sizeof( double );
IndiceCoupe = (int *) pt;
pt += NbVar * sizeof( int );  
T = (char *) pt;
pt += NbVar * sizeof( char );

SPX_GetTableauRow( Spx, VariablePneFractionnaire, ValeurDuZero,
                   /* En retour, la ligne */
                   &NombreDeTermes, CoefficientSpx, IndiceDeLaVariableSpx, 
                   &AlphaI0, &CodeRet );
if ( CodeRet == NON_PNE ) {
  printf("coderet GetTableauRow non \n");
  goto Fin2MirSurTableau;
}

SecondMembre = AlphaI0;

/* Important: dans la papier de Dash, la contrainte de depart est du type >= . Comme il s'agit d'une
   contrainte d'egalite on pourra etudier le cas > et le cas < c'est a dire Signe = 1 et Signe = -1 */

/* On ajoute tout de suite la variable fractionaire avec un coefficient egal a 1 */
VariableFractionnaireSpx = Spx->CorrespondanceVarEntreeVarSimplexe[VariablePneFractionnaire];
CoefficientSpx[NombreDeTermes] = 1.0;
IndiceDeLaVariableSpx[NombreDeTermes] = VariableFractionnaireSpx;
NombreDeTermes++;

Signe = 1.0;	
SecondMembre = Signe * AlphaI0;
for ( i = 0 ; i < NombreDeTermes ; i++ ) CoefficientSpx[i] *= Signe;

memset( (char *) TypeDeSubstitutionEntiere, PAS_DE_SUBSTITUTION, NbVar * sizeof( char ) );

ValDeVarContinue = 0.0;
NbVarEntieres = 0;

CoefficientDeLaVariableContinue = &CoeffDeLaVariableEntiere[NbVar - 1];
NumeroDeLaVariableContinue = &NumeroDeLaVariableEntiere[NbVar - 1];
	
TypeDeSubstitutionContinue = &TypeDeSubstitutionEntiere[NbVar - 1];
BorneDeSubstitutionContinue = &BorneDeSubstitutionEntiere[NbVar - 1];
	
IndexVariableContinue = 0;
NbVarContinues = 0;

memset( (double *) CoeffCoupe, 0, NbVar * sizeof( double ) );
memset( (char *) T, 0, NbVar * sizeof( char ) );

goto AAA;

for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  CoeffCoupe[IndiceDeLaVariableSpx[i]] = CoefficientSpx[i];
  T[IndiceDeLaVariableSpx[i]] = 1;
}

/* Test on fait tout de suite la substitution des variables d'ecart */
for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  VarSpx = IndiceDeLaVariableSpx[i];
	/* Attention aux variables d'ecart */
	if ( Spx->OrigineDeLaVariable[VarSpx] != ECART ) continue;
  ai = CoefficientSpx[i];
  Cnt = Spx->NumeroDeContrainte[Spx->Cdeb[VarSpx]];	
  /* Remplacement de la variable d'ecart */
  il    = Spx->Mdeb[Cnt]; 
  ilMax = il + Spx->NbTerm[Cnt];
  while ( il < ilMax ) {
    Var2Spx = Spx->Indcol[il];
		if ( Spx->OrigineDeLaVariable[Var2Spx] == NATIVE ) {
      if ( T[Var2Spx] == 0 ) {
			  CoeffCoupe[Var2Spx] = -ai * Spx->A[il] * Spx->ScaleX[VarSpx] / Spx->ScaleX[Var2Spx];				
        T[Var2Spx] = 1;
			}
			else {
				CoeffCoupe[Var2Spx] -= ai * Spx->A[il] * Spx->ScaleX[VarSpx] / Spx->ScaleX[Var2Spx];
			}
		}
    il++;
  }   
  /* Attention a la translation de Xminentree */
	SecondMembre -= ai * B[Cnt] * Spx->ScaleX[VarSpx] * Spx->SupXmax;
  CoeffCoupe[VarSpx] = 0.0;
  T[VarSpx] = 0;
}

NombreDeTermes = 0;
for ( VarSpx = 0 ; VarSpx < Spx->NombreDeVariables ; VarSpx++ ) {
  if ( T[VarSpx] != 0 ) {
	  if ( CoeffCoupe[VarSpx] != 0.0 ) {
      CoefficientSpx[NombreDeTermes] = CoeffCoupe[VarSpx];
      IndiceDeLaVariableSpx[NombreDeTermes] = VarSpx;
		  NombreDeTermes++;
		}
		T[VarSpx] = 0;
		CoeffCoupe[VarSpx] = 0.0;
	}
}

AAA:
printf("NombreDeTermes %d\n",NombreDeTermes);

for ( i = 0 ; i < NombreDeTermes ; i++ ) {
  VarSpx = IndiceDeLaVariableSpx[i];
	/* Attention aux variables d'ecart */
	if ( Spx->OrigineDeLaVariable[VarSpx] == ECART ) {	
	  /* A ce stade si la variable d'ecart n'est pas nulle il y a un pb car elle est hors base */
		if ( Spx->X[VarSpx] != 0.0 ) goto Fin2MirSurTableau;		
    ai = CoefficientSpx[i];
		Xu = 0.;				
		if ( ai > 0.0 ) {
			ValDeVarContinue += ai * Xu;
			/* On enregistre le variable */
			CoefficientDeLaVariableContinue[IndexVariableContinue] = ai;
			/* Negatif si variable d'ecart */
			NumeroDeLaVariableContinue[IndexVariableContinue] = - VarSpx - 1; 
	    TypeDeSubstitutionContinue[IndexVariableContinue] = PAS_DE_SUBSTITUTION;
		  BorneDeSubstitutionContinue[IndexVariableContinue] = 0.0;						
			IndexVariableContinue--;
			NbVarContinues++;
		}
		continue;		  
	}

	VarPne = Spx->CorrespondanceVarSimplexeVarEntree[VarSpx];
  if ( TypeDeVariable[VarPne] != ENTIER ) {	
    /* Variable continue elle ne doit pas etre non bornee */
    if ( TypeDeBorne[VarPne] == VARIABLE_NON_BORNEE ) {
			NbVarEntieres = -1;
			goto FinPreparationTableau;
		}
		/* Si le coefficient est > on comptabilise la variable */
    ai = CoefficientSpx[i];
		Xu = U[VarPne];
		DeltaSecondMembre = 0.0;
    TypeDeSubstitution = PAS_DE_SUBSTITUTION;
		BorneDeSubstitution = 0.0;		
		if ( TypeDeBorne[VarPne] == VARIABLE_BORNEE_DES_DEUX_COTES ) {
      if ( Xu > 0.5 * Pne->UmaxTravSv[VarPne] ) {
			  if ( Pne->UminTravSv[VarPne] != 0.0 ) printf("pb Umin variable continue %e\n",Pne->UminTrav[VarPne]);
	      /* On remplace par U~ =  Umax - U <=> U = Umax - U~ */       
				DeltaSecondMembre = ai * Pne->UmaxTravSv[VarPne];
				ai *= -1.0;			
        Xu = Pne->UmaxTravSv[VarPne] - U[VarPne];
				TypeDeSubstitution = SUBSTITUTION_BORNE_SUP;
				BorneDeSubstitution = Pne->UmaxTravSv[VarPne];				
			}
		}
		if ( ai > 0.0 ) {
			ValDeVarContinue += ai * Xu;
      SecondMembre -= DeltaSecondMembre;
			/* On enregistre le variable */
			CoefficientDeLaVariableContinue[IndexVariableContinue] = ai;			
			NumeroDeLaVariableContinue[IndexVariableContinue] = VarPne;
	    TypeDeSubstitutionContinue[IndexVariableContinue] = TypeDeSubstitution;
		  BorneDeSubstitutionContinue[IndexVariableContinue] = BorneDeSubstitution;			
			IndexVariableContinue--;
			NbVarContinues++;
		}
		continue;
	}
	
  if ( TypeDeVariable[VarPne] == ENTIER ) {
    if ( Pne->UminTrav[VarPne] == Pne->UmaxTrav[VarPne] && 0 ) {
		  /* La variable a ete instanciee en entree */
      if ( Pne->UminTrav[VarPne] == Pne->UmaxTravSv[VarPne] ) {      
	      /* Faire le changement de variable U = Umin + U~ et U~ >= */
	      CoeffDeLaVariableEntiere[NbVarEntieres] = CoefficientSpx[i];
			  SecondMembre -= CoeffDeLaVariableEntiere[NbVarEntieres] * Pne->UminTrav[VarPne];				
		    ValeurDeLaVariableEntiere[NbVarEntieres] = 0.0;				
        TypeDeSubstitutionEntiere[NbVarEntieres] = SUBSTITUTION_BORNE_INF;
        BorneDeSubstitutionEntiere[NbVarEntieres] = Pne->UminTrav[VarPne];		
				NumeroDeLaVariableEntiere[NbVarEntieres] = VarPne;			
		    NbVarEntieres++;
		    continue;					 
			}
		}
    ai = CoefficientSpx[i];
		Xu = U[VarPne];
		DeltaSecondMembre = 0.0;
    TypeDeSubstitution = PAS_DE_SUBSTITUTION;
		BorneDeSubstitution = 0.0;	
    /* Substitution eventuelle de borne */
	  if ( U[VarPne] > 0.5 * Pne->UmaxTravSv[VarPne] ) {
	    /* On remplace par U~ =  Umax - U <=> U = Umax - U~ */
			DeltaSecondMembre = ai * Pne->UmaxTravSv[VarPne];
      Xu = Pne->UmaxTravSv[VarPne] - U[VarPne];
			ai *= -1.0;
			TypeDeSubstitution = SUBSTITUTION_BORNE_SUP;
			BorneDeSubstitution =  Pne->UmaxTravSv[VarPne];
		}
    SecondMembre -= DeltaSecondMembre;
		/* On enregistre le variable */	 				
		CoeffDeLaVariableEntiere[NbVarEntieres] = ai;
		ValeurDeLaVariableEntiere[NbVarEntieres] = Xu;		
    TypeDeSubstitutionEntiere[NbVarEntieres] = TypeDeSubstitution;
    BorneDeSubstitutionEntiere[NbVarEntieres] = BorneDeSubstitution;							
		NumeroDeLaVariableEntiere[NbVarEntieres] = VarPne;
		NbVarEntieres++;
  }
	
}

FinPreparationTableau:

/*if ( NbVarContinues != 0 ) goto Fin2MirSurTableau;*/

if ( NbVarEntieres < 1 ) goto Fin2MirSurTableau;

  
  /* printf("NbVarContinues %d NbVarEntieres %d\n",NbVarContinues,NbVarEntieres); */
  
	CoefficientDeLaVariableContinue = &CoefficientDeLaVariableContinue[IndexVariableContinue+1];
	NumeroDeLaVariableContinue = &NumeroDeLaVariableContinue[IndexVariableContinue+1];
  TypeDeSubstitutionContinue = &TypeDeSubstitutionContinue[IndexVariableContinue+1];
  BorneDeSubstitutionContinue = &BorneDeSubstitutionContinue[IndexVariableContinue+1];
		
	
  /*printf("Tentative 2stepMIR sur tableau ValDeVarContinue %e SecondMembre %e\n", ValDeVarContinue,SecondMembre);*/
  
  NormaliserLaCoupe = NON_PNE;
  PNE_RechercheTwoStepMir( Pne, ValDeVarContinue, NbVarEntieres, CoeffDeLaVariableEntiere,
	                         SecondMembre, ValeurDeLaVariableEntiere, NumeroDeLaVariableEntiere,
													 TypeDeSubstitutionEntiere, BorneDeSubstitutionEntiere,
													 NbVarContinues, NumeroDeLaVariableContinue, TypeDeSubstitutionContinue,
													 BorneDeSubstitutionContinue, CoefficientDeLaVariableContinue,
													 CoeffCoupe, IndiceCoupe, T, NormaliserLaCoupe );				

Fin2MirSurTableau:	
free( Buff );

return;
}

# endif
