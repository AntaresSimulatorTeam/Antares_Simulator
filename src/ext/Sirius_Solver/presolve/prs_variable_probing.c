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

   FONCTION: "Probing" des variables entieres. On les fixe a 0 ou a 1 
             pour en deduire des implications logiques sur les autres 
             variables. 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

# define SEUIL  1.e-4
# define NOMBRE_MAX_DE_CYCLES  10
# define VARIABLE_ENTIERE_FIXEE_SUR_UMIN  0
# define VARIABLE_ENTIERE_FIXEE_SUR_UMAX  1

# define COMPILE_VARIABLE_PROBING NON_PNE

# if COMPILE_VARIABLE_PROBING == OUI_PNE

void PRS_VariableProbing( PRESOLVE * , char , int );
void PRS_EtudierUneInstanciationDeVariable( PRESOLVE * , int    , char   * , char * , char * ,
                                                   double * ,  char * ,  double * , int * , char * );
void PRS_BornerUneContrainte( PRESOLVE * , int , char * , double * , char * , double * );
void PRS_BornerUneVariable( PRESOLVE * , double * , double * , char   * , char * , int ,
                                   char * , double * , char   * , double * ) ;
void PRS_TesterLesBornesDUneVariableContinue( PRESOLVE * , int , char , double , char ,
                                              double , char * );  
void PRS_TesterLesBornesDUneVariableEntiere( PRESOLVE * , int   , char   , double , char , double , 
                                             char * , int * , int * , char * ); 

/*----------------------------------------------------------------------------*/

void PRS_VariableProbing( PRESOLVE * Presolve, char TesterToutesLesVariablesEntieres, int VariableEntiereATester )
{
int Cnt; int Var; int i; char * ContrainteABorner; char *   VariableABorner; int Var2; int Cnt2;
int * NumeroDesVariablesEntieresFixee              ; char *   VariableEntiereFixeeSurUminOuSurUmax ;
char * TypeDeBorneTravSv; double * UTravSv          ; double * UminTravSv; double * UmaxTravSv      ;
char * MinContrainteCalcule  ; char * MaxContrainteCalcule  ; double * MinContrainte  ; double * MaxContrainte  ;
char * MinContrainteCalculeSv; char * MaxContrainteCalculeSv; double * MinContrainteSv; double * MaxContrainteSv;
char   BorneInfValide        ; double BorneInf              ; char     BorneSupValide ; double   BorneSup       ;
int   DebutDeBoucle         ; int   FinDeBoucle           ; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

if ( TesterToutesLesVariablesEntieres == NON_PNE ) {
  if ( VariableEntiereATester < 0 || VariableEntiereATester >= Pne->NombreDeVariablesTrav ) return; 
}

ContrainteABorner = (char *) malloc( Pne->NombreDeContraintesTrav * sizeof( char ) );
VariableABorner   = (char *) malloc( Pne->NombreDeVariablesTrav   * sizeof( char ) );

NumeroDesVariablesEntieresFixee      = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
VariableEntiereFixeeSurUminOuSurUmax = (char *) malloc( Pne->NombreDeVariablesTrav * sizeof( char ) );

TypeDeBorneTravSv = (char *)   malloc( Pne->NombreDeVariablesTrav * sizeof( char   ) );
UTravSv           = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UminTravSv        = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
UmaxTravSv        = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );

MinContrainteCalcule = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) );
MaxContrainteCalcule = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) );
MinContrainte        = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) );
MaxContrainte        = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) );

MinContrainteCalculeSv = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) );
MaxContrainteCalculeSv = (char *)   malloc( Pne->NombreDeContraintesTrav * sizeof( char   ) );
MinContrainteSv        = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) );
MaxContrainteSv        = (double *) malloc( Pne->NombreDeContraintesTrav * sizeof( double ) );

if ( ContrainteABorner               == NULL || VariableABorner                      == NULL ||
     NumeroDesVariablesEntieresFixee == NULL || VariableEntiereFixeeSurUminOuSurUmax == NULL || 
     TypeDeBorneTravSv		     == NULL || UTravSv                              == NULL || 
     UminTravSv                      == NULL || UmaxTravSv                           == NULL ||
     MinContrainteCalcule            == NULL || MaxContrainteCalcule                 == NULL ||
     MinContrainte                   == NULL || MaxContrainte                        == NULL ||
     MinContrainteCalculeSv          == NULL || MaxContrainteCalculeSv               == NULL ||
     MinContrainteSv                 == NULL || MaxContrainteSv                      == NULL ) { 
  printf(" Solveur PNE , memoire insuffisante. Sous-programme: PRS_VariableProbing\n");
  Pne->AnomalieDetectee = OUI_PNE;
  longjmp( Pne->Env , Pne->AnomalieDetectee ); 
}

/* Sauvegarde des donnees des variables du probleme */
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  TypeDeBorneTravSv[Var] = (char) Pne->TypeDeBorneTrav[Var];
  UTravSv          [Var] = Pne->UTrav[Var];
  UminTravSv       [Var] = Pne->UminTrav[Var];
  UmaxTravSv       [Var] = Pne->UmaxTrav[Var];
}
/* On restaure les types de bornes et les valeurs de bornes obtenues en fin de 
   presolve avant liberation des bornes lorsque c'etait possible */
for ( i = 0 ; i < Pne->NombreDeVariablesLiberees ; i++ ) {
  Var = Pne->VariableLiberee[i];
  TypeDeBorneTravSv[Var] = Pne->TypeDeBorneVariableLiberee[i]; 
  UminTravSv       [Var] = Pne->UminVariableLiberee[i];
  UmaxTravSv       [Var] = Pne->UmaxVariableLiberee[i];
}

/* Premiere initialisation */
for ( Var2 = 0 ; Var2 < Pne->NombreDeVariablesTrav ; Var2++ ) {
  Pne->TypeDeBorneTrav[Var2] = TypeDeBorneTravSv[Var2];
  Pne->UTrav          [Var2] = UTravSv[Var2];
  Pne->UminTrav       [Var2] = UminTravSv[Var2];
  Pne->UmaxTrav       [Var2] = UmaxTravSv[Var2];
}
/* Calcul de toutes les bornes des contraintes */
for ( Cnt2 = 0 ; Cnt2 < Pne->NombreDeContraintesTrav  ; Cnt2++ ) {
  PRS_BornerUneContrainte( Presolve, Cnt2, &BorneInfValide, &BorneInf, &BorneSupValide, &BorneSup );
  MinContrainteCalcule  [Cnt2] = BorneInfValide;
  MinContrainteCalculeSv[Cnt2] = BorneInfValide;
  MaxContrainteCalcule  [Cnt2] = BorneSupValide;
  MaxContrainteCalculeSv[Cnt2] = BorneSupValide;
  MinContrainte         [Cnt2] = BorneInf;
  MinContrainteSv       [Cnt2] = BorneInf;
  MaxContrainte         [Cnt2] = BorneSup;
  MaxContrainteSv       [Cnt2] = BorneSup;
}

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav  ; Cnt++ ) ContrainteABorner[Cnt] = NON_PNE;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav    ; Var++ ) VariableABorner  [Var] = NON_PNE;

DebutDeBoucle = 0; 
FinDeBoucle   = Pne->NombreDeVariablesTrav;
if ( TesterToutesLesVariablesEntieres == NON_PNE ) {
 DebutDeBoucle = VariableEntiereATester; 
 FinDeBoucle   = DebutDeBoucle + 1;
}
for ( Var = DebutDeBoucle ; Var < FinDeBoucle ; Var++ ) {
  if ( Pne->TypeDeVariableTrav[Var] != ENTIER ) continue;
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;

printf("-> Probing de la variable entiere %d \n",Var); 

  /* Fixation de la variable a 0 */
  Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;
  Pne->UTrav          [Var] = Pne->UminTrav[Var];	
  /*Pne->UmaxTrav       [Var] = Pne->UminTrav[Var];*/
  /* Etude de l'impact de la fixation de la variable a 0 */
  PRS_EtudierUneInstanciationDeVariable( Presolve, Var, ContrainteABorner, VariableABorner,
		       			 MinContrainteCalcule  , MinContrainte  ,
      					 MaxContrainteCalcule  , MaxContrainte  ,
                                         NumeroDesVariablesEntieresFixee        ,
                                         VariableEntiereFixeeSurUminOuSurUmax );
  /* On restaure les donnees */
  for ( Var2 = 0 ; Var2 < Pne->NombreDeVariablesTrav ; Var2++ ) {
    Pne->TypeDeBorneTrav[Var2] = TypeDeBorneTravSv[Var2];
    Pne->UTrav          [Var2] = UTravSv[Var2];
    Pne->UminTrav       [Var2] = UminTravSv[Var2];
    Pne->UmaxTrav       [Var2] = UmaxTravSv[Var2];
  }
  for ( Cnt2 = 0 ; Cnt2 < Pne->NombreDeContraintesTrav  ; Cnt2++ ) {
    MinContrainteCalcule[Cnt2] = MinContrainteCalculeSv[Cnt2];
    MaxContrainteCalcule[Cnt2] = MaxContrainteCalculeSv[Cnt2];
    MinContrainte       [Cnt2] = MinContrainteSv[Cnt2]; 
    MaxContrainte       [Cnt2] = MaxContrainteSv[Cnt2];
  }

  /* Fixation de la variable a 1 */
  Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;
  Pne->UTrav          [Var] = Pne->UmaxTrav[Var];	 
  /*Pne->UminTrav       [Var] = Pne->UmaxTrav[Var];*/
  /* Etude de l'impact de la fixation de la variable a 1 */
  PRS_EtudierUneInstanciationDeVariable( Presolve, Var, ContrainteABorner, VariableABorner,
				       	 MinContrainteCalcule  , MinContrainte  ,
      				       	 MaxContrainteCalcule  , MaxContrainte  ,
                                         NumeroDesVariablesEntieresFixee        ,
                                         VariableEntiereFixeeSurUminOuSurUmax );
  /* On restaure les donnees */
  for ( Var2 = 0 ; Var2 < Pne->NombreDeVariablesTrav ; Var2++ ) {
    Pne->TypeDeBorneTrav[Var2] = TypeDeBorneTravSv[Var2];
    Pne->UTrav          [Var2] = UTravSv[Var2];
    Pne->UminTrav       [Var2] = UminTravSv[Var2];
    Pne->UmaxTrav       [Var2] = UmaxTravSv[Var2];
  }
  for ( Cnt2 = 0 ; Cnt2 < Pne->NombreDeContraintesTrav  ; Cnt2++ ) {
    MinContrainteCalcule[Cnt2] = MinContrainteCalculeSv[Cnt2];
    MaxContrainteCalcule[Cnt2] = MaxContrainteCalculeSv[Cnt2];
    MinContrainte       [Cnt2] = MinContrainteSv[Cnt2]; 
    MaxContrainte       [Cnt2] = MaxContrainteSv[Cnt2];
  }

}

/* Restauration des variables du probleme (avant liberation des variables) */
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  Pne->TypeDeBorneTrav[Var] = TypeDeBorneTravSv[Var];
  Pne->UTrav          [Var] = UTravSv[Var];
  Pne->UminTrav       [Var] = UminTravSv[Var]  ;
  Pne->UmaxTrav       [Var] = UmaxTravSv[Var]  ;
}
/* Restauration des variables liberees */
for ( i = 0 ; i < Pne->NombreDeVariablesLiberees ; i++ ) {
  Var = Pne->VariableLiberee[i];
  Pne->TypeDeBorneTrav[Var] = VARIABLE_NON_BORNEE;
  Pne->UmaxTrav       [Var] =  LINFINI_PNE;
  Pne->UminTrav       [Var] = -LINFINI_PNE;
}

free( ContrainteABorner );
free( VariableABorner );
free( NumeroDesVariablesEntieresFixee );
free( VariableEntiereFixeeSurUminOuSurUmax );
free( TypeDeBorneTravSv );
free( UTravSv );
free( UminTravSv );
free( UmaxTravSv );
free( MinContrainteCalcule );
free( MaxContrainteCalcule );
free( MinContrainte );
free( MaxContrainte );
free( MinContrainteCalculeSv );
free( MaxContrainteCalculeSv );
free( MinContrainteSv );
free( MaxContrainteSv );

return;
}

/*----------------------------------------------------------------------------*/

void PRS_EtudierUneInstanciationDeVariable( PRESOLVE * Presolve,
                                            int   VariableQuiAEteFixee,
                                            char * ContrainteABorner,
                                            char * VariableABorner,  
					    char * MinContrainteCalcule,
					    double * MinContrainte,
      					    char * MaxContrainteCalcule,
					    double * MaxContrainte,
                                            int * NumeroDesVariablesEntieresFixee, 
                                            char * VariableEntiereFixeeSurUminOuSurUmax ) 
{
int il; int ilMax; int Cnt; int Var  ; char   RecalculerLesBornesSurLesVariables  ; char BorneModifiee ;
char RecalculerLesBornesSurLesContraintes; char   BorneAmelioree; char   VariableFixee; int NombreDeCycles;
char BorneInfValide; char BorneSupValide ; double BorneInf      ; double BorneSup     ;
int NombreDeVariablesEntieresFixee      ;
char MinVariableCalcule; double MinVariable;
char MaxVariableCalcule; double MaxVariable; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeVariablesEntieresFixee = 0;

/* Initialisation de la liste des contraintes a borner */
il  = Pne->CdebTrav[VariableQuiAEteFixee];
while ( il >= 0 ) {
  ContrainteABorner[Pne->NumContrainteTrav[il]] = OUI_PNE;
  il = Pne->CsuiTrav[il]; 
}
RecalculerLesBornesSurLesContraintes = OUI_PNE;

NombreDeCycles = 0;

DebutDeCycle:

RecalculerLesBornesSurLesVariables = NON_PNE;

if ( RecalculerLesBornesSurLesContraintes == OUI_PNE ) {
  for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav  ; Cnt++ ) {
    if ( ContrainteABorner[Cnt] == NON_PNE ) continue;
    ContrainteABorner[Cnt] = NON_PNE;

    PRS_BornerUneContrainte( Presolve, Cnt, &BorneInfValide, &BorneInf, &BorneSupValide, &BorneSup );

    MinContrainteCalcule[Cnt] = BorneInfValide;
    MaxContrainteCalcule[Cnt] = BorneSupValide;

    BorneAmelioree = NON_PNE;	   
    if ( BorneInfValide == OUI_PNE && BorneInf > MinContrainte[Cnt] + SEUIL ) {
/*
printf("  borne sur contrainte %d ancienne borne min %lf nouvelle borne min %lf\n",Cnt,MinContrainte[Cnt],BorneInf);
*/
      MinContrainte[Cnt] = BorneInf;
      BorneAmelioree     = OUI_PNE;
    }
    if ( BorneSupValide == OUI_PNE && BorneSup < MaxContrainte[Cnt] - SEUIL ) {
/*
printf("  borne sur contrainte %d ancienne borne max %lf nouvelle borne max %lf\n",Cnt,MaxContrainte[Cnt],BorneSup);
*/
      MaxContrainte[Cnt] = BorneSup;
      BorneAmelioree     = OUI_PNE;
    }
    if ( BorneAmelioree == OUI_PNE ) {    
      RecalculerLesBornesSurLesVariables = OUI_PNE;
      il    = Pne->MdebTrav[Cnt];
      ilMax = il + Pne->NbTermTrav[Cnt];
      while ( il < ilMax ) {
        Var = Pne->NuvarTrav[il];
        if ( Pne->TypeDeBorneTrav[Var] != VARIABLE_FIXE ) VariableABorner[Var] = OUI_PNE;
        il++;
      }       
    }
  }
}

RecalculerLesBornesSurLesContraintes = NON_PNE;

if ( RecalculerLesBornesSurLesVariables == OUI_PNE ) {
  for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) { 
    if ( VariableABorner[Var] == NON_PNE ) continue;
    VariableABorner[Var] = NON_PNE;

    /* Compte tenu des nouvelles valeurs des bornes sur les contraintes, on recalcule  les bornes de 
       la variable */
    PRS_BornerUneVariable( Presolve,
                           MinContrainte      , MaxContrainte, MinContrainteCalcule, MaxContrainteCalcule, Var, 
                           &MinVariableCalcule, &MinVariable , &MaxVariableCalcule , &MaxVariable );

    /* On controle si la variable a des bornes moins larges */
    if ( Pne->TypeDeVariableTrav[Var] != ENTIER ) {
      PRS_TesterLesBornesDUneVariableContinue( Presolve,
                                               Var        , MinVariableCalcule, MinVariable, MaxVariableCalcule, 
                                               MaxVariable, &BorneAmelioree ); 
    }
    else if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) { 
      PRS_TesterLesBornesDUneVariableEntiere( Presolve,
                                              Var        , MinVariableCalcule, MinVariable, MaxVariableCalcule, 
                                              MaxVariable, &VariableFixee    , &NombreDeVariablesEntieresFixee, 
                                              NumeroDesVariablesEntieresFixee, VariableEntiereFixeeSurUminOuSurUmax ); 
    }

    /* Si la variable a des bornes plus petites, on recalcule les bornes des contraintes dans lesquelles 
       elle intervient */
    if ( BorneAmelioree == OUI_PNE || VariableFixee == OUI_PNE ) {   
      RecalculerLesBornesSurLesContraintes = OUI_PNE;
      il = Pne->CdebTrav[Var];
      while ( il >= 0 ) {
        ContrainteABorner[Pne->NumContrainteTrav[il]] = OUI_PNE; 
        il = Pne->CsuiTrav[il];
      }
    }
  }
}

NombreDeCycles++;
if ( RecalculerLesBornesSurLesContraintes == OUI_PNE && NombreDeCycles < NOMBRE_MAX_DE_CYCLES ) goto DebutDeCycle;

return;
}

/*----------------------------------------------------------------------------*/
/* Calcul des bornes sur une contrainte (a ce stade on suppose que toutes les 
   contraintes sont actives) */

void PRS_BornerUneContrainte( PRESOLVE * Presolve,
                              int     Cnt, 
                              char   * BorneInfValide, 
                              double * BorneInf, 
                              char   * BorneSupValide,
                              double * BorneSup )
{
int il; int ilMax; int Var; double Smin; double Smax; double X; 
PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*BorneInfValide = OUI_PNE;
*BorneSupValide = OUI_PNE;

Smin  = 0.;
Smax  = 0.;
il    = Pne->MdebTrav[Cnt];
ilMax = il + Pne->NbTermTrav[Cnt];
while ( il < ilMax ) {

  Var = Pne->NuvarTrav[il];

  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {
    *BorneInfValide = NON_PNE;
    *BorneSupValide = NON_PNE;
    return;
  }
  else if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) {
    X = Pne->ATrav[il] * Pne->UTrav[Var];
    Smax+= X;
    Smin+= X;
  }
  else {
    if ( Pne->ATrav[il] > 0. ) {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) *BorneSupValide = NON_PNE; 
      else Smax+= Pne->ATrav[il] * Pne->UmaxTrav[Var];
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) *BorneInfValide = NON_PNE; 
      else Smin+= Pne->ATrav[il] * Pne->UminTrav[Var];
    }
    else {
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) *BorneSupValide = NON_PNE;
      else Smax+= Pne->ATrav[il] * Pne->UminTrav[Var];
      if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) *BorneInfValide = NON_PNE;
      else Smin+= Pne->ATrav[il] * Pne->UmaxTrav[Var];
    }
  }
  il++;
}

*BorneInf = Smin;
*BorneSup = Smax;

return;
}

/*----------------------------------------------------------------------------*/
/* Calcul des bornes sur une variable (a ce stade on suppose que toutes les 
   contraintes sont actives) */

void PRS_BornerUneVariable( PRESOLVE * Presolve,
                            double * MinContrainte, 
                            double * MaxContrainte, 
                            char   * MinContrainteCalcule, 
                            char   * MaxContrainteCalcule,
                            int     Var, 
                            char   * MinVariableCalcule, 
                            double * MinVariable,
                            char   * MaxVariableCalcule, 
                	    double * MaxVariable )
{
int   il         ; int Cnt          ; double CoeffDeVar; double Smin    ; double Smax ;  
double AxiMin     ; double AxiMax     ; double XiMin     ; double XiMax   ;  
double InfDesXiMax; double SupDesXiMin;     
char   InfDesXiMaxEstInitialise       ; char SupDesXiMinEstInitialise     ;
char   SminEstValide                  ; char SmaxEstValide                ;    
char   XiMinEstValide                 ; char XiMaxEstValide               ;
PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;  

InfDesXiMaxEstInitialise = NON_PNE;
SupDesXiMinEstInitialise = NON_PNE;

il = Pne->CdebTrav[Var];
while ( il >= 0 ) {

  Cnt = Pne->NumContrainteTrav[il];

  CoeffDeVar = Pne->ATrav[il];

  SmaxEstValide  = OUI_PNE;
  SminEstValide  = OUI_PNE;
  XiMinEstValide = OUI_PNE;
  XiMaxEstValide = OUI_PNE;

  if ( Pne->SensContrainteTrav[Cnt] == '=' ) { 
    /* Dans le cas d'une contrainte d'egalite on peut calculer une borne 
       min et une borne max */
    if ( CoeffDeVar > ZERO_PRESOLVE ) { 
      /* Coeff positif */
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_INFERIEUREMENT ||    
           MaxContrainteCalcule[Cnt] == NON_PNE                           ) SmaxEstValide = NON_PNE;  
      else Smax = MaxContrainte[Cnt] - ( CoeffDeVar * Pne->UmaxTrav[Var] );
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_SUPERIEUREMENT ||
           MinContrainteCalcule[Cnt] == NON_PNE                           ) SminEstValide = NON_PNE;  
      else Smin = MinContrainte[Cnt] - ( CoeffDeVar * Pne->UminTrav[Var] );
      AxiMin = Pne->BTrav[Cnt] - Smax;
      AxiMax = Pne->BTrav[Cnt] - Smin;
      if ( SmaxEstValide == NON_PNE ) XiMinEstValide = NON_PNE;
      else XiMin = AxiMin / CoeffDeVar; 
      if ( SminEstValide == NON_PNE ) XiMaxEstValide = NON_PNE;
      else XiMax = AxiMax / CoeffDeVar; 
    }
    else if ( CoeffDeVar < -ZERO_PRESOLVE ) {
      /* Coeff negatif */
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_SUPERIEUREMENT || 
           MaxContrainteCalcule[Cnt] == NON_PNE                           ) SmaxEstValide = NON_PNE;  
      else Smax = MaxContrainte[Cnt] - ( CoeffDeVar * Pne->UminTrav[Var] );
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_INFERIEUREMENT || 
           MinContrainteCalcule[Cnt] == NON_PNE                           ) SminEstValide = NON_PNE;  
      else Smin = MinContrainte[Cnt] - ( CoeffDeVar * Pne->UmaxTrav[Var] );
      AxiMin = Pne->BTrav[Cnt] - Smax;
      AxiMax = Pne->BTrav[Cnt] - Smin;
      if ( SminEstValide == NON_PNE ) XiMinEstValide = NON_PNE;
      else XiMin  = AxiMax / CoeffDeVar; 
      if ( SmaxEstValide == NON_PNE ) XiMaxEstValide = NON_PNE;
      else XiMax  = AxiMin / CoeffDeVar; 
    }
    else return; /* Car evaluation impossible */
    if ( XiMinEstValide == OUI_PNE ) {
      if ( SupDesXiMinEstInitialise == OUI_PNE ) {
        if ( XiMin > SupDesXiMin ) SupDesXiMin = XiMin; 
      }
      else {
        SupDesXiMin              = XiMin;
        SupDesXiMinEstInitialise = OUI_PNE;
      }
    }
    if ( XiMaxEstValide == OUI_PNE ) {
      if ( InfDesXiMaxEstInitialise == OUI_PNE ) {
        if ( XiMax < InfDesXiMax ) InfDesXiMax = XiMax; 
      }
      else {
        InfDesXiMax              = XiMax;
        InfDesXiMaxEstInitialise = OUI_PNE;
      }
    }
  }
  else {     
    /* Dans le cas d'une contrainte d'inegalite on ne peut calculer qu'un seul type de borne */
    if ( CoeffDeVar > ZERO_PRESOLVE ) { 
      /* Coeff positif */
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_SUPERIEUREMENT || 
           MinContrainteCalcule[Cnt] == NON_PNE                           ) SminEstValide = NON_PNE;  
      if ( SminEstValide == OUI_PNE ) {
        Smin = MinContrainte[Cnt] - ( CoeffDeVar * Pne->UminTrav[Var] );
        AxiMax = Pne->BTrav[Cnt] - Smin;
        XiMax  = AxiMax / CoeffDeVar; 
        if ( InfDesXiMaxEstInitialise == OUI_PNE ) {
          if ( XiMax < InfDesXiMax ) InfDesXiMax = XiMax;  
        }
        else {
          InfDesXiMax              = XiMax;
          InfDesXiMaxEstInitialise = OUI_PNE;
        }
      }
    }
    else if ( CoeffDeVar < -ZERO_PRESOLVE ) {  
      /* Coeff negatif */
      if ( Pne->TypeDeBorneTrav[Var]  == VARIABLE_NON_BORNEE            || 
           Pne->TypeDeBorneTrav[Var]  == VARIABLE_BORNEE_INFERIEUREMENT || 
           MinContrainteCalcule[Cnt] == NON_PNE                           ) SminEstValide = NON_PNE;  
      if ( SminEstValide == OUI_PNE ) {
        Smin   = MinContrainte[Cnt] - ( CoeffDeVar * Pne->UmaxTrav[Var] );
        AxiMax = Pne->BTrav[Cnt] - Smin;
        XiMin  = AxiMax / CoeffDeVar; 
        if ( SupDesXiMinEstInitialise == OUI_PNE ) {
          if ( XiMin > SupDesXiMin ) SupDesXiMin = XiMin;  
        }
        else {
          SupDesXiMin              = XiMin;
          SupDesXiMinEstInitialise = OUI_PNE;
        }
      }
    }
    else return; /* Car evaluation impossible */
  }
  /*ContrainteSuivante:*/
  il = Pne->CsuiTrav[il];

}

*MinVariableCalcule = SupDesXiMinEstInitialise; 
*MinVariable        = SupDesXiMin;
*MaxVariableCalcule = InfDesXiMaxEstInitialise; 
*MaxVariable        = InfDesXiMax;

return;
}

/*----------------------------------------------------------------------------*/

void PRS_TesterLesBornesDUneVariableContinue( PRESOLVE * Presolve,
                                              int   Var, 
                                              char   MinVariableCalcule, 
                                              double MinVariable, 
                                              char   MaxVariableCalcule, 
                                              double MaxVariable, 
                                              char * BorneAmelioree )  
{
PROBLEME_PNE * Pne;
Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*BorneAmelioree = NON_PNE;

if ( MinVariableCalcule == OUI_PNE ) {
  if ( ( MinVariable - Pne->UminTrav[Var] ) > SEUIL                || 
       Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT || 
       Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {  
/*   
    printf("-> Phase de variable probing: variable %d borne inf amelioree valeur %lf nouvelle valeur %lf\n",Var,Pne->UminTrav[Var],MinVariable);
*/     
    *BorneAmelioree   = OUI_PNE;
    Pne->UminTrav[Var] = MinVariable;
    if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) Pne->TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
    else if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) Pne->TypeDeBorneTrav[Var] = VARIABLE_BORNEE_INFERIEUREMENT;     
  }
}

if ( MaxVariableCalcule == OUI_PNE ) {
  if ( ( Pne->UmaxTrav[Var] - MaxVariable ) > SEUIL                || 
       Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT || 
       Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) {    
/* 
    printf("-> Phase de variable probing: variable %d borne sup amelioree valeur %lf nouvelle valeur %lf\n",Var,Pne->UmaxTrav[Var],MaxVariable);  
*/  
    *BorneAmelioree = OUI_PNE;		 
    Pne->UmaxTrav                    [Var] = MaxVariable;
    if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) Pne->TypeDeBorneTrav[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
    else if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_NON_BORNEE ) Pne->TypeDeBorneTrav[Var] = VARIABLE_BORNEE_SUPERIEUREMENT;     
  }
}

return;
}

/*----------------------------------------------------------------------------*/

void PRS_TesterLesBornesDUneVariableEntiere( PRESOLVE * Presolve,
                                             int   Var, 
                                             char   MinVariableCalcule, 
                                             double MinVariable, 
                                             char   MaxVariableCalcule, 
                                             double MaxVariable, 
                                             char * VariableFixee, 
                                             int * NombreDeVariablesEntieresFixee, 
                                             int * NumeroDesVariablesEntieresFixee, 
                                             char * VariableEntiereFixeeSurUminOuSurUmax ) 
{
PROBLEME_PNE * Pne;
Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;  

*VariableFixee = NON_PNE;

if ( MinVariableCalcule == OUI_PNE && ( MinVariable - Pne->UminTrav[Var] ) > SEUIL ) {
  printf("-> Phase de variable probing: la variable entiere %d est fixee a 1 car nouveau min = %lf\n",Var,MinVariable); 
  /*Pne->UminTrav       [Var] = Pne->UmaxTrav[Var];*/
  Pne->UTrav          [Var] = Pne->UmaxTrav[Var];
  Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;
  *VariableFixee = OUI_PNE;
  NumeroDesVariablesEntieresFixee     [*NombreDeVariablesEntieresFixee] = Var;
  VariableEntiereFixeeSurUminOuSurUmax[*NombreDeVariablesEntieresFixee] = VARIABLE_ENTIERE_FIXEE_SUR_UMIN;
  *NombreDeVariablesEntieresFixee = *NombreDeVariablesEntieresFixee + 1; 
}

if ( MaxVariableCalcule == OUI_PNE && ( Pne->UmaxTrav[Var] - MaxVariable ) > SEUIL ) {
  printf("-> Phase de variable probing: la variable entiere %d est fixee a 0 car nouveau max = %lf\n",Var,MaxVariable); 
  /*Pne->UmaxTrav       [Var] = Pne->UminTrav[Var];*/
  Pne->UTrav          [Var] = Pne->UminTrav[Var];
  Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;   
  *VariableFixee = OUI_PNE;
  NumeroDesVariablesEntieresFixee     [*NombreDeVariablesEntieresFixee] = Var;
  VariableEntiereFixeeSurUminOuSurUmax[*NombreDeVariablesEntieresFixee] = VARIABLE_ENTIERE_FIXEE_SUR_UMAX;
  *NombreDeVariablesEntieresFixee = *NombreDeVariablesEntieresFixee + 1;      	       
}

return;
}

# endif


