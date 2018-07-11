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

   FONCTION: Recherche de couverture de sac a dos
	 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define MARGE 1.e-5 /*1.e-6*/ 
# define SEUIL_POUR_REBOUCLAGE 50  
# define MX_NBAR_AJ 50

# define ZERO_X 1.e-12 /*1.e-8*/
# define ZERO_X_UPLIFT 1.e-12 /*1.e-8*/

# define POSSIBLE 1
# define IMPOSSIBLE 0

/* Remarque sur le sequence independant: finalement ce n'est pas si interessant que ca */   
# define KNAPSACK_SEQUENCE_INDEPENDENT_LIFTING  NON_PNE

# define VALEUR_CONTRAINTE_MOINS_A_DE_VARIABLE_A_1 OUI_PNE /* OUI_PNE */



int NbK=0; double SommeViolations=0;



/*----------------------------------------------------------------------------*/
int PNE_PartitionTriRapide( double * Tableau, int * Ordre, int Deb, int Fin, char TypeTri )
{
int Compt; double Pivot; int i; int j; int Var; 

Compt = Deb;
j     = Ordre[Deb];
Pivot = Tableau[j];

if ( TypeTri == DECROISSANT ) {
  /* Ordre decroissant */
  for ( i = Deb + 1 ; i <= Fin ; i++) {
    j = Ordre[i];
    if ( Tableau[j] > Pivot) {
      Compt++;     
      Var = Ordre[Compt];
      Ordre[Compt] = j;
      Ordre[i] = Var; 	     
    }
  }
}
else {
  /* Ordre croissant */
  for ( i = Deb + 1 ; i <= Fin ; i++) {
    j = Ordre[i];
    if ( Tableau[j] < Pivot) {
      Compt++;     
      Var = Ordre[Compt];
      Ordre[Compt] = j;
      Ordre[i] = Var; 	     
    }
  }
}

Var = Ordre[Compt];
Ordre[Compt] = Ordre[Deb];
Ordre[Deb] = Var;

return(Compt);
}

/*----------------------------------------------------------------------------*/
void PNE_TriRapide( double * Tableau, int * Ordre, int Debut, int Fin, char TypeTri )
{
int Pivot;
if ( Debut < Fin ) {
  Pivot = PNE_PartitionTriRapide( Tableau, Ordre, Debut, Fin, TypeTri );  
  PNE_TriRapide( Tableau, Ordre, Debut  , Pivot-1, TypeTri );
  PNE_TriRapide( Tableau, Ordre, Pivot+1, Fin    , TypeTri );
}
return;
}

/*----------------------------------------------------------------------------*/

void PNE_ReclasserCsurA( int VariableLiftee, char * Z, int * Nb, double * CsurA,
                         int MxCoeff, int * C, double * A1, double * A, int * NumVar )
{
int iSupprime; int NbNew; double CdivA; int i;
NbNew = *Nb;
Z[VariableLiftee] = 1;
CdivA = (double) MxCoeff / A[VariableLiftee];
iSupprime = NbNew;	
for ( i = 0 ; i < NbNew ; i++ ) {
  if ( CsurA[i] < CdivA ) {
    iSupprime = i;
    break;
	}
}	
for( i = NbNew ; i > iSupprime ; i-- ) {
  CsurA[i] = CsurA[i-1];
  C[i] = C[i-1];
  A1[i] = A1[i-1];
  NumVar[i] = NumVar[i-1];		
}	
C[iSupprime] = MxCoeff;
A1[iSupprime] = A[VariableLiftee];
CsurA[iSupprime] = CdivA;	
NumVar[iSupprime] = VariableLiftee;
NbNew++;
*Nb = NbNew;
return; 
}

/*----------------------------------------------------------------------------*/

void PNE_RechercherUneCouvertureMinimale( int NombreDeVariables, double * ValeurDeX, double * A, int * C,
                                          char * CouvertureTrouvee , double ValeurDeLaContrainte ,
																					char * Z , double * CsurA, int * OrdreVar ,
		                              			  double * SecondMembreDeLaKnapsak, int * NombreDeVariablesDeLaKnapsak,
					                                double * CoefficientsDeLaKnapsack, int * VariableBooleenneDeLaKnapsack,
																					double * A1, int * NumVar, char LesCoeffSontEntiers )
{
double Bmax; int i; int Var; double NouvelleValeurDeB; double VB;  int j ;
char TypeTri; char OnEnleve; int Nb; double B0; double B;  int SecondMembreCouverture;
int Xi; int Co; int YaDesVariablesDansC2; char OnPeutDownLifter; double Zdyn;

*CouvertureTrouvee = NON_PNE;

/* On essaie d'abord de trouver une couverture minimale par empilements puis on lifte
   les variables.
	 A noter: j'ai essaye de trouver une couverture minimale violee par la solution
	 courante par programmation dynamique. Ce n'est pas une bonne methode car tres souvent
	 la programmation dynamique ne trouve pas de couverture violee.
	 L'heuristique ci-dessous (proposee par Zongao Gu) part d'une couverture minimale
	 trouvee par un empilement un peu "intelligent" ce qui permet de lifter les variables
	 et de trouver une inegalite valide violee par la solution courante.
	 Demander a partir d'un couverture violee pour faire du lifting est trop
	 restictif */
  
/* Calcul de la valeur max de la somme (toutes les variables sont a 1) */
Bmax = 0.0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) Bmax+= A[Var];
if ( Bmax <= ValeurDeLaContrainte ) goto FinCouverture;


/********************** Test **************************/
Zdyn = -1.;
/*
if ( LesCoeffSontEntiers == OUI_PNE ) {
  if ( ValeurDeLaContrainte <= SEUIL_POUR_PROGRAMMATION_DYNAMIQUE ) {
    for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	    CsurA[Var] = 1. - ValeurDeX[Var];
    }
    Zdyn = PNE_ProgrammationDynamiqueKnapsackSeparation( NombreDeVariables, CsurA, A, ValeurDeLaContrainte );
		if ( Zdyn < 1 ) printf("Couverture minimale trouvee Zdyn %e\n",Zdyn);
	}
}
*/
/************** Fin test ***********************/

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  OrdreVar[Var] = Var;
	CsurA[Var] = (1. - ValeurDeX[Var]) / A[Var];
  Z[Var] = 0;
}

/* On empile */
/* On ne tient pas compte des variables a 0, lesquelles seront upliftees */
NouvelleValeurDeB = 0.0;
Nb = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {	
  if ( ValeurDeX[Var] > ZERO_X ) {
    NouvelleValeurDeB += A[Var];	
    if ( ValeurDeX[Var] == 1.0 ) Z[Var] = 2; /* mettre 1 pour ne pas downlifter */  
		else Z[Var] = 1;
		Nb++;
	}
}

/* Si on peut tout empiler sans contrainte on sort */
if ( NouvelleValeurDeB <= ValeurDeLaContrainte + MARGE ) {
  goto FinCouverture;
}

/* Ordre decroissant */
TypeTri = DECROISSANT;
PNE_TriRapide( CsurA, OrdreVar, 0, NombreDeVariables - 1, TypeTri);

# if DEBUG_PNE
  /* Verification */
  for ( j = 0 ; j < NombreDeVariables - 1 ; j++ ) {
	  Var = OrdreVar[j];
	  Var1 = OrdreVar[j+1];
	  if ( (1. - ValeurDeX[Var]) / A[Var] < (1. - ValeurDeX[Var1]) / A[Var1]) {
	    printf("Mauvais classement\n");
		  exit(0);
	  }
  }
# endif

/* On enleve les objets tout en respectant la contrainte */
for ( j = 0 ; j < NombreDeVariables ; j++ ) {
  Var = OrdreVar[j];
	if ( Z[Var] != 0 ) {
	  /* On essaie d'enlever l'objet */
		if ( NouvelleValeurDeB - A[Var] > ValeurDeLaContrainte + MARGE ) {
		  /* On peut enlever l'objet */
      NouvelleValeurDeB -= A[Var];
      Z[Var] = 0;
			Nb--;
		}  
	}
}

if ( Nb == 0 ) goto FinCouverture;
if ( NouvelleValeurDeB < ValeurDeLaContrainte + MARGE ) {
  goto FinCouverture;
}

/* Couverture minimale : si on peut enlever une variable en etant superieur a ContrainteMoinsVariablesAUn, on l'enleve */

OnEnleve = OUI_PNE;
while ( OnEnleve == OUI_PNE ) {
  OnEnleve = NON_PNE;
  NouvelleValeurDeB = 0.0;
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
	  if ( Z[Var] != 0 ) NouvelleValeurDeB += A[Var];
	}
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    if ( Z[Var] != 0 ) {
      if ( NouvelleValeurDeB - A[Var] > ValeurDeLaContrainte + MARGE ) {
        OnEnleve = OUI_PNE;
	      Nb--;								 
        Z[Var] = 0;
        break;
      }
    }
  }
}

if ( Nb == 0 ) goto FinCouverture;

if ( NouvelleValeurDeB < ValeurDeLaContrainte + MARGE ) goto FinCouverture;

/* On verifie que la couverture est minimale */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 0 ) {
    if ( NouvelleValeurDeB - A[Var] > ValeurDeLaContrainte + MARGE ) {
      printf("La couverture n'est pas minimale NouvelleValeurDeB %e A[Var] %e ValeurDeLaContrainte %e\n",
			        NouvelleValeurDeB,A[Var],ValeurDeLaContrainte);
			goto FinCouverture;
		}
	}
}

if ( Nb == 0 ) goto FinCouverture;

# if KNAPSACK_SEQUENCE_INDEPENDENT_LIFTING == OUI_PNE 
  if ( LesCoeffSontEntiers == OUI_PNE ) {
    /* On classe les coefficients de la couverture dans l'ordre decroissant */
    for ( i = 0 ; i < NombreDeVariables ; i++ ) OrdreVar[i] = i;		
    TypeTri = DECROISSANT;
    PNE_TriRapide( A, OrdreVar, 0, NombreDeVariables - 1, TypeTri);		
    Nb = 0;
    B0 = ValeurDeLaContrainte;		
    for ( i = 0 ; i < NombreDeVariables ; i++ ) {
      Var = OrdreVar[i];
			/*if ( Z[Var] == 2 ) Z[Var] = 1;*/ /* Ca ne marche pas si on ne met pas les variable de C2 dans la couverture de depart */
      if ( Z[Var] == 1 ) {
	      A1[Nb] = A[Var];
        CoefficientsDeLaKnapsack[Nb] = 1;
        VariableBooleenneDeLaKnapsack[Nb] = Var;			
        Nb++;   
	    }
	    else if ( Z[Var] == 2 ) {
	      B0 -= A[Var];
	    }			
	  }		
    *SecondMembreDeLaKnapsak = Nb - 1;		
    *NombreDeVariablesDeLaKnapsak = Nb;
		/* La couverture passee n'est constituee que de variables dans C1 */
    PNE_SequenceIndependantCoverKnapsackLifting( /* La contrainte de sac a dos */
		                                             NombreDeVariables, ValeurDeX, A, B0,
		                                             /* La couverture */
                                                 NombreDeVariablesDeLaKnapsak, CoefficientsDeLaKnapsack,
																								 VariableBooleenneDeLaKnapsack, SecondMembreDeLaKnapsak, A1,
																								 /* Les variables a lifter */
																								 Z, CsurA, OrdreVar,
																								 /* Code retour */
																								 CouvertureTrouvee );      
    return;
  }
# endif

/* On constitue C1 */
/* Z[Var] = 1 si Var est dans C1 , Z[Var] = 2 si Var est dans C2 */
YaDesVariablesDansC2 = NON_PNE;
Nb = 0;
B0 = ValeurDeLaContrainte;
NouvelleValeurDeB = 0;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] == 1 ) {			
		NouvelleValeurDeB += A[Var];
	  CsurA[Nb] = 1. / A[Var];
    NumVar[Nb] = Var;
	  OrdreVar[Nb] = Nb;
    Nb++;
	}
	else if ( Z[Var] == 2 ) {
	  B0 -= A[Var];
    YaDesVariablesDansC2 = OUI_PNE;		
	}
}

OnPeutDownLifter = NON_PNE;
if ( YaDesVariablesDansC2 == OUI_PNE ) OnPeutDownLifter = OUI_PNE;

SecondMembreCouverture = Nb - 1;

if ( NouvelleValeurDeB < B0 ) {
  printf("Erreur NouvelleValeurDeB %e B0 %e apres avoir enleve les variables dans C2\n",NouvelleValeurDeB,B0);
}

if ( Nb == 0 ) goto FinCouverture;

/* Classement de C/A dans l'ordre decroissant */
TypeTri = DECROISSANT;
PNE_TriRapide( CsurA, OrdreVar, 0, Nb - 1, TypeTri);

for ( i = 0 ; i < Nb ; i++ ) {
  j = OrdreVar[i];
	Var = NumVar[j];
	C[i] = 1;
	A1[i] = A[Var];
	CsurA[i] = 1. / A1[i];
	OrdreVar[i] = Var;
}

memcpy( (char *) NumVar, (char *) OrdreVar, Nb * sizeof( int ) );

/* On classe les variables dans l'ordre decroissant de X */
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) OrdreVar[Var] = Var;
TypeTri = DECROISSANT;
PNE_TriRapide( ValeurDeX, OrdreVar, 0, NombreDeVariables - 1, TypeTri);

# if DEBUG_PNE
  /* Verification */
  for ( j = 0 ; j < NombreDeVariables - 1 ; j++ ) {
    if ( ValeurDeX[OrdreVar[j]] < ValeurDeX[OrdreVar[j+1]] ) {
	    printf("Mauvais classement\n");
		  exit(0);
	  }
  }
# endif

/* On uplifte toutes les variables qui ne sont pas dans la K */
/* On downlift les variables qui sont dans C2 */

/* On uplifte dans F */

/* On parcours la table dans le sens de ValeurDeX decroissant */

# if VALEUR_CONTRAINTE_MOINS_A_DE_VARIABLE_A_1 == NON_PNE
  B0 = ValeurDeLaContrainte;  /* Non: quand on uplifte on a enleve les variable dans C2 donc il faut enlever leurs
                                      coefficients dans le second membre */
# endif

for ( j = 0 ; j < NombreDeVariables ; j++ ) {
  Var = OrdreVar[j];
  if ( Z[Var] != 0 ) continue; /* La variable est dans C1 ou C2 */
	if ( ValeurDeX[Var] <= ZERO_X_UPLIFT  ) continue;	
  B = B0 - A[Var];
	if ( B < 0.0 ) {
    # if VALEUR_CONTRAINTE_MOINS_A_DE_VARIABLE_A_1 == OUI_PNE	
	    /* Lifting impossible: les ensembles ont ete mal choisis, tant pis */
      goto FinCouverture;
		# else
		  continue;
		# endif
  }
  Xi = PNE_MajorantKnapsack( Nb, C, A1, B, LesCoeffSontEntiers );																		
  Co = SecondMembreCouverture - Xi;  
	if ( Co > 0 ) {
    PNE_ReclasserCsurA( Var, Z, &Nb, CsurA, Co, C, A1, A, NumVar );
  }
}

/* If the resulting cover is not violated, the cover will not be violated */
VB = 0.0;
for ( i = 0 ; i < Nb ; i++ ) VB += C[i] * ValeurDeX[NumVar[i]];  

/* Verification: normalement la couverture est violee par rapport a Nb */
if ( VB < (double) SecondMembreCouverture ) {
  /*printf(" VB %e SecondMembreCouverture %d\n",VB,SecondMembreCouverture);*/
  goto FinCouverture;
}

/* On downlift les variables qui sont dans C2 */
if ( OnPeutDownLifter == NON_PNE ) goto OnUpLifteDansR;

B0 = ValeurDeLaContrainte;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 2 ) continue;	
	/* La variable est dans C2 */
  B = B0;
	Xi = PNE_MajorantKnapsack( Nb, C, A1, B, LesCoeffSontEntiers );
	Co = Xi - SecondMembreCouverture;

  # if VALEUR_CONTRAINTE_MOINS_A_DE_VARIABLE_A_1 == OUI_PNE
	 /* Lifting impossible: les ensembles ont ete mal choisis, tant pis */	  
	  if ( Co <= 0 ) goto FinCouverture; 
  # endif
	
	if ( Co > 0 ) {
	  /* Insertion de la variable et modification du Classement de C/A */
    PNE_ReclasserCsurA( Var, Z, &Nb, CsurA, Co, C, A1, A, NumVar );	
	  SecondMembreCouverture += Co;
	}
}

OnUpLifteDansR:

B0 = ValeurDeLaContrainte;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( Z[Var] != 0 ) continue; /* La variable est dans C1 ou dans C2 */
	if ( ValeurDeX[Var] > ZERO_X_UPLIFT ) continue;
	
  B = B0 - A[Var];
	if ( B < 0.0 ) {
    # if VALEUR_CONTRAINTE_MOINS_A_DE_VARIABLE_A_1 == OUI_PNE
	    /* Lifting impossible. En principe ici ca ne peut pas arriver car sinon cela voudrait dire
		     qu'il y a une couverture a 1 seul element */
      goto FinCouverture;
		# else
		  continue;
		# endif
	}
  Xi = PNE_MajorantKnapsack( Nb, C, A1, B, LesCoeffSontEntiers );																		
  Co = SecondMembreCouverture - Xi;		
	if ( Co > 0 ) {
	  /*printf("Uplift de %d dans R\n",Var);*/
	  /* Insertion de la variable et modification du Classement de C/A */
    PNE_ReclasserCsurA( Var, Z, &Nb, CsurA, Co, C, A1, A, NumVar );	
  }
}

*SecondMembreDeLaKnapsak = SecondMembreCouverture;
*NombreDeVariablesDeLaKnapsak = 0;
for ( i = 0 ; i < Nb ; i++ ) {
  if ( C[i] == 0 ) continue;
	if ( Z[NumVar[i]] != 1 ) {
	  printf("Attention C[i] = %d Z[NumVar[i]] = %d  OnPeutDownLifter %d\n",C[i],Z[NumVar[i]],OnPeutDownLifter);
		goto FinCouverture;		
	}
  CoefficientsDeLaKnapsack     [*NombreDeVariablesDeLaKnapsak] = C[i];
  VariableBooleenneDeLaKnapsack[*NombreDeVariablesDeLaKnapsak] = NumVar[i];
  *NombreDeVariablesDeLaKnapsak = *NombreDeVariablesDeLaKnapsak + 1;
}

*CouvertureTrouvee = OUI_PNE;
 
FinCouverture:

return;
}

/*----------------------------------------------------------------------------*/
/* On essaie d'en faire une contrainte avec des coefficients entiers */
void PNE_KnapsackCoeffEntiers( int NbBool, double * Coeff, double * SecondMembre,
                               double * FacteurMultiplicatifSurPGCD )
{
int Kref; int i; int a; int b; int c; char Bfait; double B; double Min; 
char TousLesCoeffSontEntiers;

Min = ldexp( 2, 4 ); /*2^4=16 2^5=32 etc..*/

*FacteurMultiplicatifSurPGCD = 1.;

B = *SecondMembre;
/* Si les coefficients et le second membre sont entiers au passe a l'etape du PGCD */
TousLesCoeffSontEntiers = OUI_PNE;
if ( B - floor( B ) != 0.0 ) TousLesCoeffSontEntiers = NON_PNE;
for ( i = 0 ; i < NbBool && TousLesCoeffSontEntiers == OUI_PNE ; i++ ) {
  if ( Coeff[i] - floor( Coeff[i] ) != 0.0 ) TousLesCoeffSontEntiers = NON_PNE;
}
if ( TousLesCoeffSontEntiers == NON_PNE ) {
  Kref = 3; /* Pour esperer se deplacer d'au moins un chiffre apres la virgule */
  while ( ldexp( B, Kref ) < Min ) Kref++;
  for ( i = 0 ; i < NbBool ; i++ ) {
    while ( ldexp( Coeff[i], Kref ) < Min ) Kref++;
  }	
  for ( i = 0 ; i < NbBool ; i++ ) {
	  Coeff[i] = floor( ldexp( Coeff[i], Kref ) );
  }
  /*B = floor( ldexp( B, Kref ) );*/
  B = ceil( ldexp( B, Kref ) );	
}
else {
  /* Il n'y a rien a faire */
	return;
}

/* Calcul du GCD des coeffs de la contrainte */
if ( Coeff[0] > Coeff[1] ) { a = (int) Coeff[0]; b = (int) Coeff[1]; }
else                       { a = (int) Coeff[1]; b = (int) Coeff[0]; }

i = 1;
Bfait = NON_PNE;

GCD:
if ( a == 0 && b == 0 ) c = 0;
else if ( b == 0 ) c = a;
else if ( a == 0 ) c = b;
else {
  c = a % b;
  while ( c != 0 ) { 
    a = b;
    b = c;
    c = a % b;
  }
  c = b;
}

i++;
if ( i < NbBool ) {
  if ( (int) Coeff[i] > c ) { a = (int) Coeff[i]; b = c; }
  else                      { a = c; b = (int) Coeff[i]; }
	goto GCD;
}
if ( Bfait == NON_PNE ) {
  Bfait = OUI_PNE;
	if ( (int) B > c ) { a = (int) B; b = c; }
	else               { a = c; b = (int) B; }
	goto GCD;
}

if ( c != 0 ) {
  for ( i = 0 ; i < NbBool ; i++ ) Coeff[i] /= c;
  B /= c;
}

*SecondMembre = B;

/*printf("Kref %d  PGCD %d  FacteurMultiplicatifSurPGCD %e\n",Kref,c,ldexp( 1.0, Kref ) / (double) c);*/
*FacteurMultiplicatifSurPGCD = ldexp( 1.0, Kref ) / (double) c;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_GreedyCoverKnapsack( PROBLEME_PNE * Pne, int Mdeb, int NbTerm, int * Nuvar,
                              double * A, double B, char RendreLesCoeffsEntiers,
															char * CouvertureTrouvee,
												      /* Pour pouvoir lifter la variable continue s'il sagit d'une mixed 0-1 knapsack */
															char Mixed_0_1_Knapsack, /* OUI_PNE ou NON_PNE */												
                              double sBarre,													 
												      int NombreDeVariablesSubstituees,
															int * NumeroDesVariablesSubstituees,
												      char * TypeDeSubsitution,
															double * CoefficientDeLaVariableSubstituee 
														)
{
double * Coefficients; int il; int ilMax; double X; int Var; int NbBooleens;
int i; char * Complementee; int NombreDeTermes; double SecondMembre;
int * C; double a; char * Z; double * CsurA; int * OrdreVar; double * ValeurDeX;
int * NumVarDeVarBooleenne; double SecondMembreCouverture; int NombreDeVariablesCouverture;
double * CoefficientCouverture; int * VariableBooleenneCouverture;
int NumVarBooleenne; int * IndiceColonne; double * UTrav; int * TypeDeBorneTrav;
int * TypeDeVariableTrav; double S; double * A1; int * NumVar; double ValB;
char * TasPourKnapsack; char * pt; int LallocTas; char LesCoeffSontEntiers;
double CoeffDeLaVariableContinue; double FacteurMultiplicatifSurPGCD;
double Seuil;

*CouvertureTrouvee = NON_PNE;

UTrav = Pne->UTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;

ValeurDeX = Pne->ValeurLocale;
C = Pne->IndiceLocal;
Coefficients = Pne->Coefficient_CG;
OrdreVar = Pne->IndiceDeLaVariable_CG;

LallocTas  = NbTerm * sizeof( double ); /* Pour CsurA */
LallocTas  += NbTerm * sizeof( double ); /* Pour CoefficientCouverture */
LallocTas  += NbTerm * sizeof( double ); /* Pour A1 */

LallocTas += NbTerm * sizeof( int ); /* Pour NumVarDeVarBooleenne */
LallocTas += NbTerm * sizeof( int ); /* Pour VariableBooleenneCouverture */
LallocTas += NbTerm * sizeof( int ); /* Pour NumVar */

LallocTas += NbTerm * sizeof( char );/* Pour Complementee */
LallocTas += NbTerm * sizeof( char );/* Pour Z */

TasPourKnapsack = (char *) malloc( LallocTas );
if ( TasPourKnapsack == NULL ) {
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PNE_GreedyCoverKnapsack \n");
	goto FinGreedy;
}

pt = TasPourKnapsack;
CsurA = (double *) pt;
pt += NbTerm * sizeof( double );
CoefficientCouverture = (double *) pt;
pt += NbTerm * sizeof( double );
A1 = (double *) pt;
pt += NbTerm * sizeof( double );
NumVarDeVarBooleenne = (int *) pt;
pt += NbTerm * sizeof( int );
VariableBooleenneCouverture = (int *) pt;
pt += NbTerm * sizeof( int );
NumVar = (int *) pt;
pt += NbTerm * sizeof( int );
Complementee = (char *) pt;
pt += NbTerm * sizeof( char );
Z  = (char *) pt;
pt += NbTerm * sizeof( char );

il = Mdeb;
ilMax = il + NbTerm;
while ( il < ilMax ) {
  Var = Nuvar[il];
  if ( TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
    B -= A[il] * UTrav[Var];
  }
  il++;
}

il = Mdeb;
ilMax = il + NbTerm;
NbBooleens = 0;
while ( il < ilMax ) {
  /* Attention A est dans Pne->ValeurLocale */
  a   = A[il];
  /* Attention Nuvar est dans Pne->IndiceLocal */
  Var = Nuvar[il];
  if ( TypeDeBorneTrav[Var] != VARIABLE_FIXE && a != 0.0 ) {
    if ( TypeDeVariableTrav[Var] == ENTIER ) {
      X = UTrav[Var];
      Complementee [NbBooleens] = NON_PNE;
      if ( a < 0.0 ) {			  
        Complementee[NbBooleens] = OUI_PNE;
        B -= a;
        a *= -1.0;
        X = 1.0 - X;
      }
      Coefficients[NbBooleens] = a;
      C           [NbBooleens] = 0; /* Sera initialise ensuite */
      ValeurDeX           [NbBooleens] = X;
      NumVarDeVarBooleenne[NbBooleens] = Var;
      NbBooleens++;    
    }
    else {
      /* La variable est reelle */
      printf("---------------------------  La variable est reelle non fixe \n");
      goto FinGreedy; 
    }
  }
  il++;
}

if ( B < 0 ) goto FinGreedy;

/* En retour on recoit la knapsack */
/*printf("--------------------------- RechercherUneCouvertureMinimale\n");*/

/* Rendre les coefficients entiers n'est pas toujours benefique */
LesCoeffSontEntiers = NON_PNE;
FacteurMultiplicatifSurPGCD = 1.;
# if COEFFS_ENTIERS_DANS_KNAPSACK == OUI_PNE
  if ( RendreLesCoeffsEntiers == OUI_PNE && NbBooleens >= 2 ) {
	  PNE_KnapsackCoeffEntiers( NbBooleens, Coefficients, &B, &FacteurMultiplicatifSurPGCD );
		LesCoeffSontEntiers = OUI_PNE;
	}
# else
  RendreLesCoeffsEntiers = NON_PNE; /* Pour ne pas avoir de warning a la compilation */
# endif

PNE_RechercherUneCouvertureMinimale( NbBooleens, ValeurDeX, Coefficients, C, CouvertureTrouvee, B, Z,
                                     CsurA, OrdreVar, &SecondMembreCouverture, &NombreDeVariablesCouverture,
				                             CoefficientCouverture, VariableBooleenneCouverture,
																		 A1, NumVar, LesCoeffSontEntiers );

if ( *CouvertureTrouvee == NON_PNE ) goto FinGreedy;
if ( NombreDeVariablesCouverture <= 0 ) goto FinGreedy;

/*
printf("Knapsack trouvee  NombreDeVariablesCouverture %d  NbBooleens %d\n",NombreDeVariablesCouverture,NbBooleens);
*/

/* S'il s'agit d'une 0-1 mixed Knapsack on lifte la variable continue */
if ( Mixed_0_1_Knapsack == OUI_PNE ) {
  ValB = 0.0;
  for ( i = 0 ; i < NombreDeVariablesCouverture ; i++ ) {
    ValB += CoefficientCouverture[i] * ValeurDeX[VariableBooleenneCouverture[i]];
  }
	S = ValB - SecondMembreCouverture;
  if ( S < 1.e-6 ) goto FinGreedy;
  printf(" Avant Lift \n");
  printf("NbBooleens %d Violation de la K %e        SecondMembreCouverture %e\n",NbBooleens,S,SecondMembreCouverture);

  PNE_KnapsackLifterLaVariableContinue( CsurA,
                                        /* La knapsack */
                                        NbBooleens,
																				Coefficients,
																			  B,
																				/* La coupe sur knapsack */
																			  NombreDeVariablesCouverture,
																				VariableBooleenneCouverture,
																				CoefficientCouverture,
																				SecondMembreCouverture,
																				/* La valeur prise par s pour le calcul de la coupe sur knapsack */
                                        sBarre,
																				/* En retour le coeff de la variable continue liftee */
																				&CoeffDeLaVariableContinue );
  CoeffDeLaVariableContinue *= FacteurMultiplicatifSurPGCD;
	if ( FacteurMultiplicatifSurPGCD != 1.0 ) {
	  printf("FacteurMultiplicatifSurPGCD %e\n",FacteurMultiplicatifSurPGCD);
	}
	printf("CoeffDeLaVariableContinue %e\n",CoeffDeLaVariableContinue);
	
}

/* On prepare la contrainte a stocker et on calcule la violation */
B = SecondMembreCouverture;

NombreDeTermes = 0;
IndiceColonne = OrdreVar;
S = 0.0;
for ( i = 0 ; i < NombreDeVariablesCouverture ; i++ ) {
  X = CoefficientCouverture[i];
  NumVarBooleenne = VariableBooleenneCouverture[i];
  if ( Complementee[NumVarBooleenne] == OUI_PNE ) {
	  Complementee[NumVarBooleenne] = NON_PNE;
    B -= X;
    X *= -1.0;
  }
	if ( X != 0.0 ) {
    Coefficients[NombreDeTermes] = X;		
		Var = NumVarDeVarBooleenne[NumVarBooleenne];
    IndiceColonne[NombreDeTermes] = Var;
		S += X * UTrav[Var];
    NombreDeTermes++;
	}
}
SecondMembre = B;
S -= SecondMembre;

if ( Mixed_0_1_Knapsack == OUI_PNE ) printf("Violation de la K apres retour aux variables entieres initiales %e\n",S);

PNE_MiseAJourSeuilCoupes( Pne, COUPE_KNAPSACK, &Seuil );

if ( S >= Seuil ) {
  Pne->SommeViolationsK += S;
  Pne->NombreDeK++;
}

if ( S >= Pne->SeuilDeViolationK ) { 
  if ( Mixed_0_1_Knapsack == NON_PNE ) {
    /* On peut tout de suite stocker */		
    PNE_EnrichirLeProblemeCourantAvecUneCoupe( Pne, 'K', NombreDeTermes, SecondMembre, S, Coefficients, IndiceColonne );
	}
	else {
    /* Il faut redecomposer la variable continue, puis stocker la coupe */

		/* Attention: Coefficients est remis a 0 dans SyntheseEtStockageMIR car
		   NuVarCoupe = Pne->IndiceDeLaVariable_CG;
       CoeffCoupe = Pne->Coefficient_CG;*/
		double * Co; int * Nu;
		Co = (double *) malloc( NombreDeTermes * sizeof(double) );
		Nu = (int *) malloc( NombreDeTermes * sizeof(int) );
		for ( i = 0 ; i < NombreDeTermes ; i++ ) {
		  Co[i] = Coefficients[i];
			Nu[i] = IndiceColonne[i];
		}
		
    PNE_SyntheseEtStockageMIR( Pne, NombreDeTermes, Nu, Co, SecondMembre, CoeffDeLaVariableContinue,
															 NombreDeVariablesSubstituees, NumeroDesVariablesSubstituees,
														   TypeDeSubsitution, CoefficientDeLaVariableSubstituee );
    free(Co);
		free(Nu);

															 
	}
}
else {
  *CouvertureTrouvee = NON_PNE;
}

FinGreedy:

PNE_FreeTasGreedyCoverKnapsack( TasPourKnapsack ); 

return;
}

/*----------------------------------------------------------------------------*/

void PNE_FreeTasGreedyCoverKnapsack( char * TasPourKnapsack )
{
free( TasPourKnapsack );
return;
}
