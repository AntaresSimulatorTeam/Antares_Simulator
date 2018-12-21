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

   FONCTION: Construction d'une crash base triangulaire avec autant de 
             variables structurelles que possible. 
             Algorithme dual.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "lu_define.h"  /* Pour la valeur de PIVOT_MIN_SIMPLEXE */

# ifdef SPX_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "spx_memoire.h"
# endif

# define VERBOSE_SPX 0

# define SEUIL_DE_PIVOT   ( 100. * PIVOT_MIN_SIMPLEXE ) /* Ce seuil doit etre superieur au pivot nul de la factorisation LU */
# define MX_NBSCAN        2
# define MX_NBTERMES      10

# define UTILISER_LA_VERIFICATION_DU_TRIANGLE
  /*# undef  UTILISER_LA_VERIFICATION_DU_TRIANGLE*/
	
# define VALEUR_SECOND_MEMBRE            0.1 /*0.1*/
# define SEUIL_DADMISSIBILITE_TRIANGLE   ( 1.0 * SEUIL_DADMISSIBILITE )
# define COEFF_DE_DEGRADATION_DE_LERREUR 2.0
# define NB_CALCULS_MOYENNE              10

typedef struct{
 int * Mdeb;
 int * NbTerm;
 int * Indcol;
 double * A; 
 int NombreDeContraintesAParcourir;
 int * ContrainteAParcourir; 
 char * VariableAffectable;   

 int PlusGrandNombreDeTermesParLigne; 
 int * PremLigne;
 int * SuivLigne;
 int * PrecLigne;

 int ContrainteChoisie; 
 int VariableChoisie;
} CB;

/*--------------------------------------------------------------------------------------------------*/

void SPX_CrashBaseChainageDeLaTransposee( PROBLEME_SPX * );
void SPX_DualClasserUneLigne( int , CB * ); 
void SPX_DualDeClasserUneLigne( int , CB * ); 
void SPX_DualClasserToutesLesLignes( PROBLEME_SPX *  , CB * );
void SPX_DualMajChainageParLigne( CB * ); 
void SPX_DualChoisirUneContrainteEtUneVariable( PROBLEME_SPX * , CB * );

int SPX_VerifResolutionCrashBase( PROBLEME_SPX * , CB * , double * , double * );
void SPX_VerifResolutionCrashBaseTransposee( PROBLEME_SPX * , int * );

/*--------------------------------------------------------------------------------------------------*/
/*                 Chainage de la transposee mais sans ACol                                         */

void SPX_CrashBaseChainageDeLaTransposee( PROBLEME_SPX * Spx  )
{
int i; int il; int ilMax; int Var   ; int ilC      ; 
int * NumeroDeContrainte  ; int * Mdeb; int * NbTerm ; int * Indcol;
int * Cdeb; int * CNbTerm; int * IndexCourant        ; double * ACol; double * A;
int NombreDeVariables     ; int NombreDeContraintes   ; 

Mdeb    = Spx->Mdeb;
NbTerm  = Spx->NbTerm;
Indcol  = Spx->Indcol;
Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;

ACol = Spx->ACol;
A    = Spx->A;

NumeroDeContrainte  = Spx->NumeroDeContrainte;
NombreDeVariables   = Spx->NombreDeVariables;
NombreDeContraintes = Spx->NombreDeContraintes;

IndexCourant = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( IndexCourant == NULL ) {
  printf(" Simplexe: memoire insuffisante dans SPX_CrashBaseChainageDeLaTransposee \n");
  Spx->AnomalieDetectee = OUI_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}  
memset( (char *) CNbTerm , 0 , NombreDeVariables * sizeof( int ) );
  
for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
  il    = Mdeb[i];
  ilMax = il + NbTerm[i];
  while ( il < ilMax ) {
    CNbTerm[Indcol[il]]++;
    il++;
  }
} 
for ( ilC = 0 , Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  Cdeb[Var] = ilC;
  IndexCourant[Var] = ilC;
  ilC+= CNbTerm[Var];
}
 
for ( i = 0 ; i < NombreDeContraintes ; i++ ) { 
  il    = Mdeb[i];
  ilMax = il + NbTerm[i];
  while ( il < ilMax ) {
    NumeroDeContrainte[IndexCourant[Indcol[il]]] = i;
		# ifdef UTILISER_LA_VERIFICATION_DU_TRIANGLE
		  ACol[IndexCourant[Indcol[il]]] = A[il];
		# endif
    IndexCourant[Indcol[il]]++;     
    il++;
  } 
}  

free( IndexCourant );

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*                 Classement d'une ligne en fonction de son nombre de termes                       */

void SPX_DualClasserUneLigne( int Cnt, CB * Cb) 
{
int NbTermes; int CntSuiv;

NbTermes            = Cb->NbTerm[Cnt];
CntSuiv             = Cb->PremLigne[NbTermes];
Cb->PremLigne[NbTermes] = Cnt;
Cb->SuivLigne[Cnt]      = CntSuiv;
if ( CntSuiv >= 0 ) Cb->PrecLigne[CntSuiv] = Cnt;

return;
}

/*------------------------------------------------------------------------------------------------*/
/*      Enleve la ligne de son chainage en fonction de son nombre de termes                       */

void SPX_DualDeClasserUneLigne( int Cnt, CB * Cb ) 
{
int NbTermes;  

NbTermes = Cb->NbTerm[Cnt];
if ( Cb->PremLigne[NbTermes] == Cnt ) {
  /* C'est le premier element qu'on enleve */
  Cb->PremLigne[NbTermes] = Cb->SuivLigne[Cnt];
  return;
}
Cb->SuivLigne[Cb->PrecLigne[Cnt]] = Cb->SuivLigne[Cnt];
if ( Cb->SuivLigne[Cnt] >= 0 ) Cb->PrecLigne[Cb->SuivLigne[Cnt]] = Cb->PrecLigne[Cnt];

return;
}

/*--------------------------------------------------------------------------------------------------*/
/*  Classement de toutes les lignes en fonction de leur nombre de termes                            */

void SPX_DualClasserToutesLesLignes( PROBLEME_SPX * Spx, CB * Cb )
{
int Cnt;   

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) SPX_DualClasserUneLigne( Cnt , Cb );
	
return;
}
 
/*----------------------------------------------------------------------------*/

void SPX_DualMajChainageParLigne( CB * Cb ) 
{
int Cnt; int il; int ilMax; int Var; int i; int * Cb_ContrainteAParcourir;
int * Cb_Mdeb; int * Cb_NbTerm; int * Cb_Indcol; char * Cb_VariableAffectable;
double * Cb_A;

Cb_ContrainteAParcourir = Cb->ContrainteAParcourir;
Cb_Mdeb   = Cb->Mdeb;
Cb_NbTerm = Cb->NbTerm;
Cb_Indcol = Cb->Indcol;
Cb_A      = Cb->A;
Cb_VariableAffectable = Cb->VariableAffectable;

for ( i = 0 ; i < Cb->NombreDeContraintesAParcourir ; i++) {
  Cnt = Cb_ContrainteAParcourir[i];
  SPX_DualDeClasserUneLigne( Cnt , Cb ); 
  /* On enleve les colonnes des variables non affectables du chainage par ligne */
  /* Les autres variables de la contraintes ont ContrainteAvecMinTermeLigne a recalculer */
  il    = Cb_Mdeb[Cnt];
  ilMax = il + Cb_NbTerm[Cnt] - 1;
  while ( il <= ilMax ) {
    Var = Cb_Indcol[il];
    if ( Cb_VariableAffectable[Var] == NON_SPX ) {
      /* On remplace il par le dernier */
      Cb_Indcol[il] = Cb_Indcol[ilMax];
      Cb_A     [il] = Cb_A[ilMax];
      ilMax--;
      Cb_NbTerm[Cnt]--;
    }
    else { 
      il++;
    }
  }
  /* Classement des lignes modifiees */
  SPX_DualClasserUneLigne( Cnt , Cb ); 
}

return;
}

/*----------------------------------------------------------------------------*/

int SPX_VerifResolutionCrashBase( PROBLEME_SPX * Spx, CB * Cb, double * Bs,
                                   double * Accumulateur )
{
int Var; int Cnt; int il; int ilMax; int ic; int icMax; int Resolution_OK;
double Pivot; double X; double S;  int * Cb_Mdeb; int * Cb_NbTerm; int * Cb_Indcol;
double * Cb_A; int * Spx_Cdeb; int * Spx_CNbTerm;int * Spx_NumeroDeContrainte;
double * Spx_ACol;

X = 0.0;

Cb_Mdeb   = Cb->Mdeb;
Cb_NbTerm = Cb->NbTerm;
Cb_Indcol = Cb->Indcol;
Cb_A      = Cb->A;

Spx_Cdeb    = Spx->Cdeb;
Spx_CNbTerm = Spx->CNbTerm;
Spx_NumeroDeContrainte = Spx->NumeroDeContrainte;
Spx_ACol               = Spx->ACol;

Resolution_OK = OUI_SPX;

/* On verifie si la nouvelle colonne ne deteriore pas trop le conditionnement de la matrice.
	 Pour cela on simule une resolution */
Var = Cb->VariableChoisie;
Cnt = Cb->ContrainteChoisie;
il    = Cb_Mdeb[Cnt];
ilMax = il + Cb_NbTerm[Cnt];
/* Recherche du pivot */
while ( il < ilMax ) {
  if ( Cb_Indcol[il] == Var ) {
	  /* Pivot trouve */
		Pivot = Cb_A[il];       
    /* Controle de la resolution */
   	X = Bs[Cnt] / Pivot;
	  S = Accumulateur[Cnt] + (Pivot * X);			
	  if ( fabs( S - VALEUR_SECOND_MEMBRE ) > SEUIL_DADMISSIBILITE_TRIANGLE ) {		
		  /*printf("Refus car S - VALEUR_SECOND_MEMBRE = %e\n",S - VALEUR_SECOND_MEMBRE );*/			
      Resolution_OK = NON_SPX;
	  }
    break;
  }
  il++;
}

if ( Resolution_OK == OUI_SPX ) {
  Var   = Cb->VariableChoisie;
  ic    = Spx_Cdeb[Var];
  icMax = ic + Spx_CNbTerm[Var];
  while ( ic < icMax ) {
    Cnt = Spx_NumeroDeContrainte[ic];
	  S = Spx_ACol[ic] * X;
	  Accumulateur[Cnt]+= S;			
	  Bs[Cnt]-= S;
	  ic++;
	}		
}

return( Resolution_OK );			
}

/*----------------------------------------------------------------------------*/
/* La base triangulaire a ete construite. On effectue une resolution avec la
   transposee avant de l'accepter. Si on s'aperçoit que ça se passe mal, on casse
	 une partie de la base et on complete avec des variables additionnelles */
	 
void SPX_VerifResolutionCrashBaseTransposee( PROBLEME_SPX * Spx, int * Ordre )
{
int Kp; int NombreDeContraintes; int NombreDeVariables; int Var; int Cnt;
int il; int il0; int ilMax; double X; double S; char * PositionDeLaVariable;
int * ContrainteDeLaVariableEnBase; int * VariableEnBaseDeLaContrainte;
double * Accumulateur; int * Mdeb; int * NbTerm; int * Indcol; double * A;
double Pivot; double * SecondMembre; char * OrigineDeLaVariable; char * TypeDeVariable;
double CalculPremiereMoyenne; double CalculDeuxiemeMoyenne; int NbPremiereMoyenne;
int NbDeuxiemeMoyenne; double PremiereMoyenne; double DeuxiemeMoyenne;

Pivot = 1.0;

NombreDeContraintes = Spx->NombreDeContraintes;
NombreDeVariables   = Spx->NombreDeVariables;

PositionDeLaVariable         = Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;
	
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
A      = Spx->A;

SecondMembre = Spx->CBarreSurNBarreR;
Accumulateur = Spx->CBarre;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  SecondMembre[Var] = VALEUR_SECOND_MEMBRE;
	Accumulateur[Var] = 0.0;
}

PremiereMoyenne = 0.0;
DeuxiemeMoyenne = 0.0;
NbPremiereMoyenne = 0;
NbDeuxiemeMoyenne = 0;
CalculPremiereMoyenne = NON_SPX;
CalculDeuxiemeMoyenne = NON_SPX;

for ( Kp = NombreDeContraintes - 1 ; Kp >= 0 ; Kp-- ) {
  Var = Ordre[Kp];
	Cnt = ContrainteDeLaVariableEnBase[Var];
  /* Recherche du pivot */
	il0 = Mdeb[Cnt];
	il    = il0;
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
    if ( Indcol[il] == Var ) {
		  Pivot = A[il];
		  break;
		}
	  il++;
	}	
	/* Calcul de la solution */
	X = SecondMembre[Var] / Pivot;
	/* Verification de la resolution */
	S = (X * Pivot) + Accumulateur[Var];
	S = fabs( S  - VALEUR_SECOND_MEMBRE );
	if ( S > SEUIL_DADMISSIBILITE_TRIANGLE ) {
	  if ( CalculPremiereMoyenne == OUI_SPX ) {
	    PremiereMoyenne+= S;
		  NbPremiereMoyenne++;
		  if ( NbPremiereMoyenne >= NB_CALCULS_MOYENNE ) {
        CalculPremiereMoyenne = NON_SPX;
			  CalculDeuxiemeMoyenne = OUI_SPX;
			  PremiereMoyenne/= NbPremiereMoyenne;
		  }
	  }
	  else if ( CalculDeuxiemeMoyenne == OUI_SPX ) {
	    DeuxiemeMoyenne+= S;
		  NbDeuxiemeMoyenne++;
		  if ( NbDeuxiemeMoyenne >= NB_CALCULS_MOYENNE ) {
        CalculDeuxiemeMoyenne = NON_SPX;
			  DeuxiemeMoyenne/= NbDeuxiemeMoyenne;
			  /*printf(" PremiereMoyenne %e  DeuxiemeMoyenne %e\n",PremiereMoyenne,DeuxiemeMoyenne);*/
			  if ( DeuxiemeMoyenne > COEFF_DE_DEGRADATION_DE_LERREUR * PremiereMoyenne ) {
				  /* L'erreur moyenne augmente */
			    break;
			  }
		  	else {
          PremiereMoyenne = 0.0;
          DeuxiemeMoyenne = 0.0;
          NbPremiereMoyenne = 0;
          NbDeuxiemeMoyenne = 0;
          CalculPremiereMoyenne = NON_SPX;
          CalculDeuxiemeMoyenne = NON_SPX;			
			  }
		  }		
	  }	
	  else if ( S > SEUIL_DADMISSIBILITE_TRIANGLE ) {
      /* Resolution imprecise */
		  /*printf("Ecart de resolution transposee %e Kp = %d\n", S, Kp);*/
		  CalculPremiereMoyenne = OUI_SPX;
	  }
	}
  /* Mise a jour de SecondMembre et Accumulateur */	
	il    = il0;
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
    Var = Indcol[il];
		if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) {  
		  S = A[il] * X;
		  SecondMembre[Var]-= S;
		  Accumulateur[Var]+= S;
		}
	  il++;
	}
}

OrigineDeLaVariable = Spx->OrigineDeLaVariable;
TypeDeVariable      = Spx->TypeDeVariable;
for ( ; Kp >= 0 ; Kp-- ) {
  /* Si ce n'est pas deja une variable additionnelle, on la cree */	 
  Var = Ordre[Kp];
	/* 16/4/2012: j'ajoute le test par rapport a ECART car il n'y a pas de difference d'usage par rapport a
	   une variable BASIQUE ARTIFICIELLE */
  if ( OrigineDeLaVariable[Var] == ECART ) continue;
  if ( OrigineDeLaVariable[Var] == BASIQUE_ARTIFICIELLE ) continue;
  if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT || TypeDeVariable[Var] == BORNEE ) {
    PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
	}
	else if (  TypeDeVariable[Var] == BORNEE_SUPERIEUREMENT ) PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_SUP;
	else PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
	Cnt = ContrainteDeLaVariableEnBase[Var];
  ContrainteDeLaVariableEnBase[Var] = -1;
  VariableEnBaseDeLaContrainte[Cnt] = -1;	
  SPX_DualCreerVariableDeBase( Spx, Cnt );
	
}

return;
}

/*----------------------------------------------------------------------------*/
/* Contruction d'une base de depart. La matrice est deja sous forme standard. */

void SPX_DualConstruireUneCrashBase( PROBLEME_SPX * Spx )
{
int Var; int Cnt; int il  ; int ilMax ; int NbC; int ic; int icMax; int i;	
CB * Cb ; double * Bs; double * Accumulateur; int NombreDeRefus; int * Ordre;
int Kp ;
int * Cb_Mdeb; int * Cb_NbTerm; int * Cb_Indcol; char * Cb_VariableAffectable;
int * Spx_Cdeb;int * Spx_CNbTerm; int * Spx_NumeroDeContrainte; int * Cb_ContrainteAParcourir;
char * Spx_PositionDeLaVariable; char * Spx_TypeDeVariable; int * Spx_ContrainteDeLaVariableEnBase;
int * Spx_VariableEnBaseDeLaContrainte;

SPX_CrashBaseChainageDeLaTransposee( Spx  );

Cb = (CB *) malloc( sizeof( CB ) );
if ( Cb == NULL ) {
  printf("Memoire insuffisante dans le sous-programme SPX_DualConstruireUneCrashBase \n");
  Spx->AnomalieDetectee = NON_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
}

Cb->VariableAffectable = (char *) malloc(Spx->NombreDeVariables * sizeof( char   ) );
 
Cb->Mdeb   = (int *)   malloc( Spx->NombreDeContraintesAllouees * sizeof( int   ) );
Cb->NbTerm = (int *)   malloc( Spx->NombreDeContraintesAllouees * sizeof( int   ) );
Cb->Indcol = (int *)   malloc( Spx->NbTermesAlloues             * sizeof( int   ) );
Cb->A      = (double *) malloc( Spx->NbTermesAlloues             * sizeof( double ) );

Cb->ContrainteAParcourir = (int *) malloc( Spx->NombreDeContraintes * sizeof( int ) );

Cb->PremLigne = (int *) malloc( (Spx->NombreDeVariables + 1) * sizeof( int ) ); 
Cb->SuivLigne = (int *) malloc( Spx->NombreDeContraintes     * sizeof( int ) ); 
Cb->PrecLigne = (int *) malloc( Spx->NombreDeContraintes     * sizeof( int ) ); 

if ( Cb->VariableAffectable == NULL || Cb->Mdeb      == NULL || Cb->NbTerm               == NULL || 
     Cb->Indcol             == NULL || Cb->A         == NULL || Cb->ContrainteAParcourir == NULL ||
     Cb->PremLigne          == NULL || Cb->SuivLigne == NULL || Cb->PrecLigne            == NULL  
   ) 
{
  printf("Memoire insuffisante dans le sous-programme SPX_DualConstruireUneCrashBase \n");
  Spx->AnomalieDetectee = NON_SPX;
  longjmp( Spx->EnvSpx , Spx->AnomalieDetectee ); 
} 

memcpy( (char *) Cb->Mdeb  , (char *) Spx->Mdeb  , Spx->NombreDeContraintes * sizeof( int   ) ); 
memcpy( (char *) Cb->NbTerm, (char *) Spx->NbTerm, Spx->NombreDeContraintes * sizeof( int   ) ); 
memcpy( (char *) Cb->Indcol, (char *) Spx->Indcol, Spx->NbTermesAlloues     * sizeof( int   ) ); 
memcpy( (char *) Cb->A     , (char *) Spx->A     , Spx->NbTermesAlloues     * sizeof( double ) ); 


Cb_Mdeb   = Cb->Mdeb;
Cb_NbTerm = Cb->NbTerm;
Cb_Indcol = Cb->Indcol;
Cb_VariableAffectable = Cb->VariableAffectable;
Spx_Cdeb    = Spx->Cdeb;
Spx_CNbTerm = Spx->CNbTerm;
Spx_NumeroDeContrainte  = Spx->NumeroDeContrainte;
Cb_ContrainteAParcourir = Cb->ContrainteAParcourir;

Spx_PositionDeLaVariable = Spx->PositionDeLaVariable;
Spx_TypeDeVariable       = Spx->TypeDeVariable;
Spx_ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
Spx_VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) { 
  Cb_VariableAffectable    [Var] = OUI_SPX;
  Spx_PositionDeLaVariable [Var] = HORS_BASE_SUR_BORNE_INF;
  if ( Spx_TypeDeVariable[Var] == NON_BORNEE ) {
    Spx_PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
	}
  Spx_ContrainteDeLaVariableEnBase[Var] = -1;
  /* Car une ligne peut avoir Spx->NombreDeVariables termes */
  Cb->PremLigne[Var] = -1;  
}
Cb->PremLigne[Spx->NombreDeVariables] = -1;

SPX_DualClasserToutesLesLignes( Spx , Cb );

Cb->PlusGrandNombreDeTermesParLigne = -1;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  if ( Cb->NbTerm[Cnt] > Cb->PlusGrandNombreDeTermesParLigne ) Cb->PlusGrandNombreDeTermesParLigne = Cb->NbTerm[Cnt];
  Spx_VariableEnBaseDeLaContrainte[Cnt] = -1;  
  Cb->ContrainteAParcourir[Cnt] = NON_SPX;
}

Bs = Spx->Bs;
Accumulateur =  Spx->BBarre;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  Bs[Cnt] = VALEUR_SECOND_MEMBRE;
  Accumulateur[Cnt] = 0.0;
}

Ordre = Spx->CdebBase;
Kp = 0;

while ( 1 ) {

  NombreDeRefus = 0; /* Utilite a verifier */

  # ifdef UTILISER_LA_VERIFICATION_DU_TRIANGLE
  DEBUT_WHILE:
	# endif
	
  SPX_DualChoisirUneContrainteEtUneVariable( Spx , Cb );
  if ( Cb->ContrainteChoisie < 0 || Cb->VariableChoisie < 0 ) break;	
	
	/* On verifie si la nouvelle colonne ne deteriore pas trop le conditionnement de la matrice.
	   Pour cela on simule une resolution */
  # ifdef UTILISER_LA_VERIFICATION_DU_TRIANGLE
    if ( SPX_VerifResolutionCrashBase( Spx, Cb, Bs, Accumulateur ) == NON_SPX ) {
	    NombreDeRefus++;
		  if ( NombreDeRefus > 100 ) break;
	    /* On ne devrait refuser que le pivot. Comme c'est complique, on choisi d'eliminer la colonne */
      Cb->VariableAffectable[Cb->VariableChoisie] = NON_SPX;    
      Cb->NombreDeContraintesAParcourir = 0;
      ic    = Spx->Cdeb[Cb->VariableChoisie];
      icMax = ic + Spx->CNbTerm[Cb->VariableChoisie];
      while ( ic < icMax ) {   
        Cb->ContrainteAParcourir[Cb->NombreDeContraintesAParcourir] = Spx->NumeroDeContrainte[ic];
        Cb->NombreDeContraintesAParcourir++;
        ic++;
      }    
      SPX_DualMajChainageParLigne( Cb );				
      goto DEBUT_WHILE;
	}
	# endif
	
  Spx_PositionDeLaVariable        [Cb->VariableChoisie]   = EN_BASE_LIBRE;			  
  Spx_ContrainteDeLaVariableEnBase[Cb->VariableChoisie]   = Cb->ContrainteChoisie;
  Spx_VariableEnBaseDeLaContrainte[Cb->ContrainteChoisie] = Cb->VariableChoisie;

	Ordre[Kp] = Cb->VariableChoisie;
	Kp++;
  
  /* Elimination de toutes les variables qui contiennent un terme dans la ligne 
     de la contrainte choisie y compris la variable choisie bien sur */
  Cb->NombreDeContraintesAParcourir = 0;
	
  il    = Cb_Mdeb[Cb->ContrainteChoisie];
  ilMax = il + Cb_NbTerm[Cb->ContrainteChoisie];
  while ( il < ilMax ) {
    Var = Cb_Indcol[il];
    Cb_VariableAffectable[Var] = NON_SPX;    
    ic    = Spx_Cdeb[Var];
    icMax = ic + Spx_CNbTerm[Var];
    while ( ic < icMax ) {   
      Cnt = Spx_NumeroDeContrainte[ic];
      for ( i = 0 ; i < Cb->NombreDeContraintesAParcourir ; i++ ) {
        if ( Cb_ContrainteAParcourir[i] == Cnt ) goto PasDeMaj;
      }
      Cb_ContrainteAParcourir[Cb->NombreDeContraintesAParcourir] = Cnt;
      Cb->NombreDeContraintesAParcourir++;
      PasDeMaj:
      ic++;
    }    
    il++;
  }  

  SPX_DualMajChainageParLigne( Cb ); 

  SPX_DualDeClasserUneLigne( Cb->ContrainteChoisie, Cb );

}

/* On a fait ce qu'on a pu. Maintenant toutes les variables ajoutees seront
   de type artificiel c'est a dire de cout non nul, meme les variables d'ecart
   ne pourront plus servir (pourquoi pas les variables d'ecart ?) */	 
NbC = 0;
for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  if ( Spx_VariableEnBaseDeLaContrainte[Cnt] >= 0 ) continue;

  /* Le 16/4/2012: je verifie s'il y a une variable d'ecart disponible. Si c'est le cas je l'utilise car je ne
	   vois pas de difference d'usage avec une variable BASIQUE_ARTIFICIELLE */
	/* S'il y a une varible d'ecart on la prend */
  il = Spx->Mdeb[Cnt] + Spx->NbTerm[Cnt] - 1;
	Var = Spx->Indcol[il];
	if ( Spx->OrigineDeLaVariable[Var] == ECART && Spx->PositionDeLaVariable[Var] != EN_BASE_LIBRE ) {
    Spx->PositionDeLaVariable        [Var] = EN_BASE_LIBRE;
    Spx->ContrainteDeLaVariableEnBase[Var] = Cnt; 
    Spx->VariableEnBaseDeLaContrainte[Cnt] = Var;
    Spx->CntVarEcartOuArtif[Var] = Cnt;
    NbC++;		
    Ordre[Kp] = Var;
	  Kp++;
    continue;
	}
	
  /* Il faut creer une variable artificielle */
  /* printf("Complement de base pour la contrainte %d\n",Cnt); */ 
  NbC++;
  SPX_DualCreerVariableDeBase( Spx, Cnt );
  Ordre[Kp] = Spx->NombreDeVariables - 1;
	Kp++;
}

# ifdef UTILISER_LA_VERIFICATION_DU_TRIANGLE
  SPX_VerifResolutionCrashBaseTransposee( Spx, Ordre );
# endif

#if VERBOSE_SPX
  printf("Nombre de contraintes non affectees par crash base DUAL %d sur %d \n",NbC,Spx->NombreDeContraintes);
#endif

free( Cb->VariableAffectable );
free( Cb->Mdeb );
free( Cb->NbTerm );
free( Cb->Indcol );
free( Cb->A );
free( Cb->ContrainteAParcourir );
free( Cb->PremLigne );
free( Cb->SuivLigne );
free( Cb->PrecLigne );

free( Cb );

return;
}

/*-------------------------------------------------------------------------------------------------*/

void SPX_DualChoisirUneContrainteEtUneVariable( PROBLEME_SPX * Spx , CB * Cb )
{
int il; int ilMax ; int Cnt; int CntChoix; int   Var     ; int VarChoix; 
int MinTermesLigne ; int MinTermesColonne  ; int   VChoix  ;
int MinTermesInit  ; char TypeVar           ; double Tie     ; int CNbTerm ;
char TypeVarChoisie ; char TypChoix          ; int   i       ; int NbScan  ;
int NbCntParcourues; int MxCntParcourues   ; 

int * Mdeb  ; int * NbTerm; int * Indcol      ; double * A           ; 
double * Xmin; double * Xmax; int * CNbTermArray; char * TypeDeVariable;

Tie      = 0.0;
TypChoix = 0;

Mdeb   = Cb->Mdeb;
NbTerm = Cb->NbTerm;
A      = Cb->A;
Indcol = Cb->Indcol;

Cb->ContrainteChoisie = -1;
Cb->VariableChoisie   = -1;

/* Choix de la ligne */
MinTermesLigne = Spx->NombreDeVariables   + 100;
MinTermesInit  = Spx->NombreDeContraintes + 100;
CntChoix       = -1;
VarChoix       = -1;
TypeVarChoisie = 128; /* car c'est un char et on peut pas mettre -1 */

TypeDeVariable = Spx->TypeDeVariable;
Xmax = Spx->Xmax;
Xmin = Spx->Xmin;
CNbTermArray = Spx->CNbTerm;

MxCntParcourues = 100;

for ( i = 1 ; i <= Cb->PlusGrandNombreDeTermesParLigne && i < MinTermesLigne ; i++) {
  /* Lignes a i termes */
	NbCntParcourues = 0;
  Cnt = Cb->PremLigne[i];
  while ( Cnt >= 0 )  {
    NbCntParcourues++;
	  /* La contrainte est candidate. On verifie qu'on peut choisir une colonne */
    MinTermesColonne = MinTermesInit;
    VChoix           = -1;		
    NbScan           = 0;
		
    /* Recherche d'une variable non bornee */
		/*
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];		
    while ( il < ilMax ) {		
      Var = Indcol[il];	       
      if ( fabs( A[il] ) > SEUIL_DE_PIVOT ) {
        if ( TypeDeVariable[Var] == NON_BORNEE ) {
          MinTermesColonne = 1;
          VChoix = Var;
          goto FinParcours;      
			  }
			}
		  il++;
    }
    */
    /* Recherche d'une variable d'ecart */
		/*
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
    while ( il < ilMax ) {		
      Var = Indcol[il];	       
      if ( fabs( A[il] ) > SEUIL_DE_PIVOT ) {
        if ( Spx->OrigineDeLaVariable[Var] == ECART ) {
          MinTermesColonne = 1;
          VChoix = Var;
          goto FinParcours;      
			  }
			}
		  il++;
    }
		*/
    /* Recherche d'une variable a 1 terme dans la colonne */
    /*
		il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];		
    while ( il < ilMax ) {		
      Var = Indcol[il];	       
      if ( fabs( A[il] ) > SEUIL_DE_PIVOT ) {
        if ( CNbTermArray[Var] == 1 ) {
          MinTermesColonne = 1;
          VChoix = Var;
          goto FinParcours;      
			  }
			}
		  il++;
    }		
    */
    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];				
    while ( il < ilMax ) {		
      Var = Indcol[il];	       
      if ( fabs( A[il] ) > SEUIL_DE_PIVOT ) { 
        TypeVar = TypeDeVariable[Var];
        CNbTerm = CNbTermArray[Var];
        if ( CNbTerm < MinTermesColonne ) {									
          MinTermesColonne = CNbTerm;
          VChoix           = Var;
          TypChoix         = TypeVar;   
          Tie              = Xmax[Var] - Xmin[Var];
				 	if ( NbCntParcourues > MxCntParcourues ) NbScan = MX_NBSCAN + 1;
          if ( TypeVar == NON_BORNEE ) {
            /* Si la variables est non bornee on la met tout de suite dans la base */
            goto FinParcours;
          }
        }
        /* Plus on aura de variables bornees hors base plus on a de chance que la base soit duale  
           realisable */ 
        else if ( CNbTerm == MinTermesColonne ) {
          /* Si on passe ici ça veut dire qu'on a choisi une variable de la ligne et donc 
             TypChoix est initialise */
          if ( TypeVar == NON_BORNEE ) { 
            MinTermesColonne = CNbTerm;
            VChoix           = Var;
            TypChoix         = TypeVar;   
            Tie              = Xmax[Var] - Xmin[Var];
						if ( NbCntParcourues > MxCntParcourues ) NbScan = MX_NBSCAN + 1;						
            /* Si la variables est non bornee on la met tout de suite dans la base */
            goto FinParcours;
          }
          else if ( TypeVar == BORNEE_INFERIEUREMENT && TypChoix != NON_BORNEE ) {
            MinTermesColonne = CNbTerm;
            VChoix           = Var;
            TypChoix         = TypeVar;   
            Tie              = Xmax[Var] - Xmin[Var];
						if ( NbCntParcourues > MxCntParcourues ) {
						  NbScan = MX_NBSCAN + 1;						
              goto FinParcours;
						}
          }           
          else if( ( Xmax[Var] - Xmin[Var] ) > Tie && TypChoix != NON_BORNEE ) {  
            /* La variable est donc bornee */
            MinTermesColonne = CNbTerm;
            VChoix           = Var;
            TypChoix         = TypeVar;   
            Tie              = Xmax[Var] - Xmin[Var];
						if ( NbCntParcourues > MxCntParcourues ) {
						  NbScan = MX_NBSCAN + 1;						
              goto FinParcours;
						}						
          }	    
        }
      }
      il++;
    }

    FinParcours:
    if ( VChoix >= 0 ) {
      NbScan++;
      MinTermesLigne = i;
      CntChoix       = Cnt;
      VarChoix       = VChoix;
      TypeVarChoisie = TypChoix;
      if ( TypeVarChoisie > NON_BORNEE ) goto FinBoucleDeRecherche;
      if ( NbScan > MX_NBSCAN ) goto FinBoucleDeRecherche;
      if ( MinTermesColonne == 1 ) goto FinBoucleDeRecherche;
      if ( i >= MX_NBTERMES ) goto FinBoucleDeRecherche;
    }

    Cnt = Cb->SuivLigne[Cnt];
  }
}

FinBoucleDeRecherche:

if ( CntChoix < 0 || VarChoix < 0 ) return;
Cb->ContrainteChoisie = CntChoix;
Cb->VariableChoisie   = VarChoix;

return;
} 

/*----------------------------------------------------------------------------*/
/*    Cas d'une contrainte pour laquelle on ne fourni pas de  
      de base. On cree une variable supplementaire a l'emplacement 
      deja reserve.                                                           */

void SPX_DualCreerVariableDeBase( PROBLEME_SPX * Spx, int Cnt )
{
int il; double Seuil; 

Spx->TypeDeVariable[Spx->NombreDeVariables] = BORNEE;
Spx->C[Spx->NombreDeVariables] = 0.;
Spx->X[Spx->NombreDeVariables] = 0.; 

/* Dans le cas de l'algorithme dual on veut faire sortir cette variable de la base au plus vite. On
   met donc un cout nul mais des bornes nulles */
Spx->XminEntree[Spx->NombreDeVariables] = 0.; 
Spx->Xmin      [Spx->NombreDeVariables] = 0.; 
Spx->XmaxEntree[Spx->NombreDeVariables] = 0.; 
Spx->Xmax      [Spx->NombreDeVariables] = 0.; 

Seuil = SEUIL_DE_VIOLATION_DE_BORNE_NON_NATIVE * Spx->ScaleB[Cnt];
if ( Seuil < SEUIL_MIN_DE_VIOLATION_DE_BORNE_NON_NATIVE ) Seuil = SEUIL_MIN_DE_VIOLATION_DE_BORNE_NON_NATIVE;
else if ( Seuil > SEUIL_MAX_DE_VIOLATION_DE_BORNE_NON_NATIVE ) Seuil = SEUIL_MAX_DE_VIOLATION_DE_BORNE_NON_NATIVE;

if ( fabs( Spx->B[Cnt] ) > 1.e+10 ) {
  if ( Seuil < 1.e-3 ) Seuil = 1.e-3;
}
else if ( fabs( Spx->B[Cnt] ) > 1.e+9 ) {
  if ( Seuil < 1.e-4 ) Seuil = 1.e-4;
}
else if ( fabs( Spx->B[Cnt] ) > 1.e+6 ) {
  if ( Seuil < 1.e-5 ) Seuil = 1.e-5;
}

Spx->SeuilDeViolationDeBorne[Spx->NombreDeVariables] = Seuil;

Spx->SeuilDAmissibiliteDuale1[Spx->NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_1;
Spx->SeuilDAmissibiliteDuale2[Spx->NombreDeVariables] = SEUIL_ADMISSIBILITE_DUALE_2;

Spx->PositionDeLaVariable        [Spx->NombreDeVariables] = EN_BASE_LIBRE; /* L'important c'est que ce soit en base */
Spx->ContrainteDeLaVariableEnBase[Spx->NombreDeVariables] = Cnt; 
Spx->VariableEnBaseDeLaContrainte[Cnt]                    = Spx->NombreDeVariables;

/* On la met dans l'équation de la contrainte */
il = Spx->Mdeb[Cnt] + Spx->NbTerm[Cnt]; /* On a deja reserve la place de cette variable a la creation du probleme */
Spx->NbTerm[Cnt]++; 
Spx->Indcol[il] = Spx->NombreDeVariables;
Spx->A     [il] = 1.0;  /* Important: ne pas mettre autre chose que 1 */
 
Spx->Csv                     [Spx->NombreDeVariables] = Spx->C[Spx->NombreDeVariables];
Spx->OrigineDeLaVariable     [Spx->NombreDeVariables] = BASIQUE_ARTIFICIELLE;
Spx->StatutBorneSupCourante  [Spx->NombreDeVariables] = BORNE_NATIVE;
Spx->StatutBorneSupAuxiliaire[Spx->NombreDeVariables] = BORNE_AUXILIAIRE_INVALIDE;

Spx->CntVarEcartOuArtif [Spx->NombreDeVariables] = Cnt;

Spx->CorrespondanceVarSimplexeVarEntree[Spx->NombreDeVariables] = -1;

/* Incrementation du nombre de variables */
Spx->NombreDeVariables++;

return;
} 







                                                                                                                                                                                                                                                                                   
