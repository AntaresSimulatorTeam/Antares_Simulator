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

/***********************************************************************************

   FONCTION: 

                   
   AUTEUR: R. GONZALEZ

************************************************************************************/

# include "pi_sys.h"

# include "pi_fonctions.h"
# include "pi_define.h"

/*------------------------------------------------------------------------*/
/*                          Initialisation                                */ 

void PI_MdEqua( PROBLEME_PI * Pi,
                int     NombreDeVariables_E, 
                double * Q_E,
								double * L_E, 
								double * Umin_E, 
								double * Umax_E,
                int   * TypeVar_E, 
                char   * VariableBinaire_E, 
								double * U_E,		
                int     NombreDeContraintes_E, 
								int   * Mdeb_E, 
								int   * Nbter_E,
                int   * Nuvar_E,   
								double * A_E, 
								double * B_E,
								char   * Sens_E )
{
int i    ; int   iq; int iqMax; int VarPi  ; int VarPi2; int iqt; int OkInit;
int jqNew; double SecondMembre  ; double Coeff; int CntPi ;  

Pi->ScaleLigneDesCouts = 1.;
Pi->ScaleLigneDesU     = 1.;

/* Decompte des variables */
Pi->NombreDeVariables         = 0;
Pi->NombreDeVariablesBinaires = 0;
for ( i = 0 ; i < NombreDeVariables_E ; i++ ) {
  if ( TypeVar_E[i] == VARIABLE_FIXE ) {
    /* La variable est fixe, on suppose que la valeur est donnee dans U_E */  
    Pi->CorrespondanceVarEntreeVarPi[i] = -1;
    continue;
  }
  else {
    Pi->Q     [Pi->NombreDeVariables] = Q_E[i];
    Pi->L     [Pi->NombreDeVariables] = L_E[i]; 
    Pi->Umin  [Pi->NombreDeVariables] = Umin_E[i]; 
    Pi->Umax  [Pi->NombreDeVariables] = Umax_E[i]; 
    Pi->U     [Pi->NombreDeVariables] = U_E[i];
    Pi->ScaleU[Pi->NombreDeVariables] = 1.;
    /*   */
    Pi->CorrespondanceVarEntreeVarPi[i] = Pi->NombreDeVariables;
    /*   */
    if ( TypeVar_E[i] == VARIABLE_BORNEE_DES_DEUX_COTES ) Pi->TypeDeVariable[Pi->NombreDeVariables] = BORNEE;
    else if ( TypeVar_E[i] == VARIABLE_BORNEE_INFERIEUREMENT ) { 
      Pi->TypeDeVariable[Pi->NombreDeVariables] = BORNEE_INFERIEUREMENT;
      Pi->Umax          [Pi->NombreDeVariables] = LINFINI_PI;
    }
    else if ( TypeVar_E[i] == VARIABLE_BORNEE_SUPERIEUREMENT ) { 
      Pi->TypeDeVariable[Pi->NombreDeVariables] = BORNEE_SUPERIEUREMENT;
      Pi->Umin          [Pi->NombreDeVariables] = -LINFINI_PI;
    }
    else if ( TypeVar_E[i] == VARIABLE_NON_BORNEE ) {
      Pi->TypeDeVariable[Pi->NombreDeVariables] = NON_BORNEE;
      Pi->Umin          [Pi->NombreDeVariables] = -LINFINI_PI;
      Pi->Umax          [Pi->NombreDeVariables] = LINFINI_PI;
    }
    else {
      printf("Erreur entree du solveur: le seul type de variables reconnues est: \n");
      printf("    VARIABLE_FIXE                  -> valeur de constante: %d\n",VARIABLE_FIXE);
      printf("    VARIABLE_BORNEE_DES_DEUX_COTES -> valeur de constante: %d\n",VARIABLE_BORNEE_DES_DEUX_COTES);
      printf("    VARIABLE_BORNEE_INFERIEUREMENT -> valeur de constante: %d\n",VARIABLE_BORNEE_INFERIEUREMENT);
      printf("    VARIABLE_BORNEE_SUPERIEUREMENT -> valeur de constante: %d\n",VARIABLE_BORNEE_SUPERIEUREMENT);
      printf("    VARIABLE_NON_BORNEE            -> valeur de constante: %d\n",VARIABLE_NON_BORNEE);
      printf("Or la variable %i est du type %d => exit volontaire car pb de mise au point\n",i,TypeVar_E[i]); 
      exit(0);
    }
    Pi->VariableBinaire[Pi->NombreDeVariables] = NON_PI;    
    if ( MPCC == OUI_PI ) {
      if ( VariableBinaire_E[i] == OUI_PI ) {        
        Pi->VariableBinaire[Pi->NombreDeVariables] = OUI_PI;	
        Pi->NombreDeVariablesBinaires++;		
      }
    }
    Pi->NombreDeVariables++;
  }
}
/*
printf("Pi->NombreDeVariablesBinaires %d\n",Pi->NombreDeVariablesBinaires);
*/
/* Second membre et contraintes */
Pi->NombreDeContraintes = 0;
iqt = 0;
for ( i = 0 ; i < NombreDeContraintes_E ; i++ ) {
  Pi->CorrespondanceCntEntreeCntPi[i] = -1;  
  Coeff = 1.;   
  if ( Sens_E[i] == '>' ) Coeff = -1.; /* Pour transformer en < */
  SecondMembre = 0.;
  OkInit = NON_PI;
  iq     = Mdeb_E[i];
  iqMax  = iq + Nbter_E[i];
  while ( iq < iqMax ) {
    VarPi = Pi->CorrespondanceVarEntreeVarPi[Nuvar_E[iq]] ;
    if ( VarPi >= 0 ) {
      if ( OkInit == NON_PI ) { 
	Pi->Mdeb  [Pi->NombreDeContraintes] = iqt;
	Pi->NbTerm[Pi->NombreDeContraintes] = 0;
	OkInit = OUI_PI;
      } 
      Pi->NbTerm[Pi->NombreDeContraintes]++;
      Pi->A    [iqt] = Coeff * A_E[iq];
      Pi->Indcol[iqt] = VarPi;
      iqt++;
    }
    else {
    /* La variable est fixee => on modifie le second membre */
      SecondMembre+= A_E[iq] * U_E[Nuvar_E[iq]];
    }
    iq++;
  }
  if ( OkInit == OUI_PI ) {
    /* Dans le cas des contraintes d'inegalite il va y avoir la variable
       d'ecart */
    if ( Sens_E[i] != '=' ) iqt++;   
    Pi->B     [Pi->NombreDeContraintes] = Coeff * ( B_E[i] - SecondMembre );
    Pi->ScaleB[Pi->NombreDeContraintes] = 1.;
    Pi->CorrespondanceCntEntreeCntPi[i] = Pi->NombreDeContraintes;    
    Pi->NombreDeContraintes++;
  }
}

/* Scaling du probleme avant sa mise sous forme standard (on le fait ici pour eviter 
   de faire un scaling sur les variables additionnelles de mise sous forme standard) */
/* attention, faire en 2 temps comme dans le simplexe */
PI_InitATransposee( Pi , CREUX );
PI_CalculerLeScaling( Pi );
PI_Scaling( Pi );

/* Mise du probleme sous la forme standard */
for ( i = 0 ; i < NombreDeContraintes_E ; i++ ) {
  if ( Sens_E[i] == '=' ) continue;
  CntPi = Pi->CorrespondanceCntEntreeCntPi[i];    
  PI_MettreLaContrainteSousFormeStandard( Pi , CntPi );
  if ( Pi->YaUneSolution == NON_PI ) return; 
}

/* Cas MPCC */
Pi->Rho = 0.0;
if ( MPCC == OUI_PI && Pi->NombreDeVariablesBinaires > 0 ) {

  { double MxC;
    MxC = -1.0;
    for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
      if ( fabs( Pi->L[i] ) > MxC ) MxC = fabs( Pi->L[i] );
    }
    printf("Plus grand cout lineaire: %e\n",MxC);
    Pi->Rho = 1. * MxC;
  }

  /* Creation de la variable Xi */
  Pi->Q     [Pi->NombreDeVariables] = 0.0;
  Pi->L     [Pi->NombreDeVariables] = Pi->Rho; 
  Pi->Umin  [Pi->NombreDeVariables] = 0.0; 
  Pi->Umax  [Pi->NombreDeVariables] = LINFINI_PI; 
  Pi->U     [Pi->NombreDeVariables] = 1.;
  Pi->ScaleU[Pi->NombreDeVariables] = 1.;  
  Pi->TypeDeVariable [Pi->NombreDeVariables] = BORNEE_INFERIEUREMENT;
  Pi->VariableBinaire[Pi->NombreDeVariables] = NON_PI;
  Pi->NumeroDeLaVariableXi = Pi->NombreDeVariables;
  Pi->NombreDeVariables++;    
  
  /* Creation de la ligne de A correspondant a d'inegalite x(1-x) < Xi */
  /* Normalement il faut mettre 1-2x, a ce stade ca n'a pas d'importance */
  iqt = Pi->Mdeb[Pi->NombreDeContraintes-1] + Pi->NbTerm[Pi->NombreDeContraintes-1]; /* Car deja standard */
  for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
    if ( Pi->VariableBinaire[i] == OUI_PI ) {
      printf("VARIABLE BINAIRE %d Umax = %e\n",i,Pi->Umax[i]);
      Pi->NumeroDeLaContrainteDeComplementarite[i] = Pi->NombreDeContraintes;
      Pi->Mdeb  [Pi->NombreDeContraintes] = iqt;
      Pi->NbTerm[Pi->NombreDeContraintes] = 0;
      Pi->A     [iqt] = 1;
      Pi->Indcol[iqt] = i;
      Pi->NbTerm[Pi->NombreDeContraintes]++;
      iqt++;
      /* Variable Xi */    
      Pi->A     [iqt] = -1;
      Pi->Indcol[iqt] = Pi->NumeroDeLaVariableXi;
      Pi->NbTerm[Pi->NombreDeContraintes]++;
      iqt++;
      Pi->B     [Pi->NombreDeContraintes] = 0.0;
      Pi->ScaleB[Pi->NombreDeContraintes] = 1.;
      Pi->NombreDeContraintes++;
      iqt++; /* On laisse une place pour la variable d'ecart */
    }
  }
  for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
    if ( Pi->VariableBinaire[i] == OUI_PI ) {
      CntPi = Pi->NumeroDeLaContrainteDeComplementarite[i];    
      PI_MettreLaContrainteSousFormeStandard( Pi , CntPi );      
      if ( Pi->YaUneSolution == NON_PI ) return; 
    }
  }
}

memset( (char *) Pi->RegulVar , 0 , Pi->NombreDeVariables * sizeof( double ) );
memset( (char *) Pi->RegulContrainte , 0 , Pi->NombreDeContraintes * sizeof( double ) );

for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) Pi->Alpha[i] = 1.0;

#if VERBOSE_PI
  printf("Nombre de variables du probleme %d\n",Pi->NombreDeVariables);
#endif
{int NbBor; int NbBorInf; int NbBorSup; int NbNonBor;
  NbBor=0;NbBorInf=0;NbBorSup=0;NbNonBor=0;
  for ( i = 0 ; i < Pi->NombreDeVariables ; i++ ) {
    if ( Pi->TypeDeVariable[i] == BORNEE ) NbBor++;
    else if ( Pi->TypeDeVariable[i] == BORNEE_INFERIEUREMENT ) NbBorInf++; 
    else if ( Pi->TypeDeVariable[i] == BORNEE_SUPERIEUREMENT ) NbBorSup++;
    else if ( Pi->TypeDeVariable[i] == NON_BORNEE ) {
      NbNonBor++; 
    }
  }
  #if VERBOSE_PI
    printf("  Nombre de variables BORNEE                : %d\n",NbBor);
    printf("  Nombre de variables BORNEE_INFERIEUREMENT : %d\n",NbBorInf);
    printf("  Nombre de variables BORNEE_SUPERIEUREMENT : %d\n",NbBorSup);
    printf("  Nombre de variables NON_BORNEE            : %d\n",NbNonBor);
  #endif
}

return;
}	       

/*----------------------------------------------------------------------------*/
/*       A ce stade, les contraintes d'inegalite sont toujours de signe <  
         et la place pour la variable d'ecart a deja ete reservee             */

void PI_MettreLaContrainteSousFormeStandard( PROBLEME_PI * Pi , int Cnt ) 
{
int il; int ilMax; int Var; double Smin; char OnBorneDesDeuxCotes; double Seuil;  

OnBorneDesDeuxCotes = NON_PI;

il    = Pi->Mdeb[Cnt];
ilMax = il + Pi->NbTerm[Cnt];
Smin  = 0.;
while ( il < ilMax) {
  Var    = Pi->Indcol[il];
  if ( Pi->A[il] <= 0. ) { 
    if ( Pi->TypeDeVariable[Var] == BORNEE_INFERIEUREMENT || Pi->TypeDeVariable[Var] == NON_BORNEE ) {
      OnBorneDesDeuxCotes = NON_PI;
      goto CreationDeLaVariableDEcart;
    }
    Smin+= Pi->A[il] * Pi->Umax[Var];
  }
  else {
    if ( Pi->TypeDeVariable[Var] == NON_BORNEE ) { 
      OnBorneDesDeuxCotes = NON_PI;
      goto CreationDeLaVariableDEcart;
    }
    Smin+= Pi->A[il] * Pi->Umin[Var];
  }
  il++;
}

if ( Smin > ( Pi->B[Cnt] + Pi->SeuilDAdmissibilite) ) { 
  #if VERBOSE_PI 
    printf("Point interieur: impossible de satisfaire la contrainte d'inegalite %d car Smin = %lf B = %lf \n",Cnt,Smin,Pi->B[Cnt]);
  #endif
  Pi->YaUneSolution = NON_PI;
  return;
}

CreationDeLaVariableDEcart:

/* Creation de la variable d'ecart */
Pi->Umin  [Pi->NombreDeVariables] = 0.; 
Pi->U     [Pi->NombreDeVariables] = 0.;
Pi->Q     [Pi->NombreDeVariables] = 0.;
Pi->L     [Pi->NombreDeVariables] = 0.;
Pi->Umax  [Pi->NombreDeVariables] = LINFINI_PI; 
Pi->ScaleU[Pi->NombreDeVariables] = 1.;

Pi->TypeDeVariable [Pi->NombreDeVariables] = BORNEE_INFERIEUREMENT;

Pi->VariableBinaire[Pi->NombreDeVariables] = NON_PI;    

if ( OnBorneDesDeuxCotes == OUI_PI ) {
  Pi->Umax[Pi->NombreDeVariables] = Pi->B[Cnt] - Smin + 1.; 
  Pi->TypeDeVariable[Pi->NombreDeVariables] = BORNEE;	  
  if ( Pi->Umax[Pi->NombreDeVariables] > 1.e+15 ) {	  
    Pi->Umax          [Pi->NombreDeVariables] = LINFINI_PI;
    Pi->TypeDeVariable[Pi->NombreDeVariables] = BORNEE_INFERIEUREMENT;
    printf("creation d'une variable d'ecart non bornee superieurement \n");
    exit(0); /* temporaire, a enlever */
  }
  else {
    /*printf("creation d'une variable d'ecart bornee des 2 cotes \n");*/
  }
}
else {
  /*printf("creation d'une variable d'ecart non bornee superieurement \n");*/
}

/* On la met dans l'équation de la contrainte */
il = Pi->Mdeb[Cnt] + Pi->NbTerm[Cnt]; /* On a deja reserve la place de cette variable a la creation du probleme */
Pi->NbTerm[Cnt]++; 
Pi->Indcol[il] = Pi->NombreDeVariables;
Pi->A    [il] = 1.;

/* Incrementation du nombre de variables */
Pi->NombreDeVariables++;

return;
}

