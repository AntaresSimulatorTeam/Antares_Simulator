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

   FONCTION: Presolve simplifie. Appele par le reduced cost fixing au noeud
	           racine et par le node presolve.
						 On instancie les variables a 0 et a 1 a la recherche
						 d'infaisabilites afn de pouvoir fixer la variable.
                
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

# define TRACES 1
# define MARGE 1.e-6
# define INSTANCATION_A_XMIN 1
# define INSTANCATION_A_XMAX 2

void PNE_PresolveSimplifieVariableProbingSauvegardeConditionsInitiales( PROBLEME_PNE * , double * , double * );
void PNE_PresolveSimplifieVariableProbingReinitConditionsInitiales( PROBLEME_PNE * , double * , double * );							

/*----------------------------------------------------------------------------*/
/* Appele a chaque fois qu'on trouve une solution entiere */
void PNE_PresolveSimplifieVariableProbing( PROBLEME_PNE * Pne, int * Faisabilite, char * RefaireUnCycle )														
{
int Var; int * TypeDeVariable; double * Bmin; double * Bmax; char * BminValide;
char * BmaxValide; char BmnValide; char BmxValide; char SensCnt; double Bmn; double Bmx;
double BCnt; int NombreDeVariables; int i; double NouvelleValeur; char * SensContrainte;
double * B; int Cnt; int NombreDeContraintes; char * BorneSupConnue;double * ValeurDeBorneSup;
char * BorneInfConnue; double * ValeurDeBorneInf; char BrnInfConnue; char BorneMiseAJour;
char UneVariableAEteFixee; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve;
int NombreDeVariablesNonFixes; int * NumeroDesVariablesNonFixes; char ProblemeInfaisable;
double ValeurDeVar; int j; char TypeDInstanciation; int NombreDeContraintesAAnalyser;
int *  NumeroDeContrainteAAnalyser ; char * BorneInfConnueSv; char * BorneSupConnueSv;
double * ValeurDeBorneInfSv; double * ValeurDeBorneSupSv; double * BminSv; double * BmaxSv;
int * NumeroDeContrainteModifiee; int * NbFoisContrainteModifiee; char * ContrainteAAnalyser;

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
NombreDeVariablesNonFixes = Pne->NombreDeVariablesNonFixes;
NumeroDesVariablesNonFixes = Pne->NumeroDesVariablesNonFixes;

TypeDeVariable = Pne->TypeDeVariableTrav;

BorneSupConnueSv = ProbingOuNodePresolve->BorneSupConnueSv;
BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneSupSv = ProbingOuNodePresolve->ValeurDeBorneSupSv;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;

BorneInfConnueSv = ProbingOuNodePresolve->BorneInfConnueSv;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInfSv = ProbingOuNodePresolve->ValeurDeBorneInfSv;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

/* On recupere les bornes des contraintes au noeud racine */
Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;
BminValide = ProbingOuNodePresolve->BminValide;
BmaxValide = ProbingOuNodePresolve->BmaxValide;

SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;

/* Attention ce n'est pas dans qu'il faut sauvegarder : free a faire */
BminSv = (double *) malloc( NombreDeContraintes * sizeof( double ) );
BmaxSv = (double *) malloc( NombreDeContraintes * sizeof( double ) );

PNE_PresolveSimplifieVariableProbingSauvegardeConditionsInitiales( Pne, BminSv, BmaxSv );

ProbingOuNodePresolve->NbParcours = 0;
ProbingOuNodePresolve->NbContraintesModifiees = 0;
NumeroDeContrainteModifiee = ProbingOuNodePresolve->NumeroDeContrainteModifiee;
NumeroDeContrainteAAnalyser = ProbingOuNodePresolve->NumeroDeContrainteAAnalyser;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;

NbFoisContrainteModifiee = ProbingOuNodePresolve->NbFoisContrainteModifiee;
ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;

/* Variable probing simplifie: on fixe alternativement a 0 et a 1 en tenant compte du graphe de conflits
   puis on compare Bmin et Bmax aux seconds membres */

for ( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];
	if ( TypeDeVariable[Var] != ENTIER ) continue;

  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;

	/* Fixation a 0 et etablissement d'une liste de Bmin Bmax modifies */
  TypeDInstanciation = INSTANCATION_A_XMAX;

	Instanciation:
	
  /* Reinit des donnees */
	
  PNE_PresolveSimplifieVariableProbingReinitConditionsInitiales( Pne, BminSv, BmaxSv );									
	
	if ( TypeDInstanciation == INSTANCATION_A_XMAX ) ValeurDeVar = 1;
	else 	if ( TypeDInstanciation == INSTANCATION_A_XMIN ) ValeurDeVar = 0;
	else break;

  ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;
	ProbingOuNodePresolve->VariableInstanciee = Var;
  ProbingOuNodePresolve->Faisabilite = OUI_PNE;
  ProbingOuNodePresolve->ValeurDeLaVariableInstanciee = ValeurDeVar;
	
	ProblemeInfaisable = NON_PNE;
	
  PNE_VariableProbingPreparerInstanciation( Pne, Var, ValeurDeVar );
  if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
	  /* On doit fixer la variable a l'oppose */
	   printf("Apres VariableProbingPreparerInstanciation Pne->ProbingOuNodePresolve->Faisabilite %d\n",Pne->ProbingOuNodePresolve->Faisabilite);
		 ProblemeInfaisable = OUI_PNE;
		 Pne->ProbingOuNodePresolve->Faisabilite = OUI_PNE;
	}
	
  if ( ProblemeInfaisable == OUI_PNE ) {
    UneVariableAEteFixee = NON_PNE;
    BorneMiseAJour = NON_PNE;	
	  if ( TypeDInstanciation == INSTANCATION_A_XMIN ) {
			NouvelleValeur = 1;
			UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
		}
	  else 	if ( TypeDInstanciation == INSTANCATION_A_XMAX ) {
			NouvelleValeur = 0;
			UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
		}
		else goto Fin;
			
	  /* On modifie les bornes des contraintes */		
		# if TRACES == 1  
		  printf("Variable %d  NouvelleValeur de borne %e\n",Var,NouvelleValeur);
		# endif

    PNE_PresolveSimplifieVariableProbingReinitConditionsInitiales( Pne, BminSv, BmaxSv );									
		
    PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) *Faisabilite = NON_PNE;
  	if ( *Faisabilite == NON_PNE ) {
		  # if TRACES == 1
			  printf("Pas de solution dans le presolve simplifie\n");
			# endif				
		  goto Fin;
		}
			
		/* Il faut a nouveau sauvegarder les Bmin Bmax et la fixation des variables puisqu'ils
			 sont modifies par l'instanciation */
    PNE_PresolveSimplifieVariableProbingSauvegardeConditionsInitiales( Pne, BminSv, BmaxSv );

		*RefaireUnCycle = OUI_PNE;
		
    /* Inutile d'analyser les contraintes suivante, on peut passer aux instanciations des autres variables */
		break;
	}			 					
	
  /* Si on a fixe une variable, inutile de faire l'autre instanciation */
  if ( ProblemeInfaisable == NON_PNE ) {
	  if ( TypeDInstanciation == INSTANCATION_A_XMAX ) {
	    TypeDInstanciation = INSTANCATION_A_XMIN;
		  goto Instanciation;
	  }
  }

}

Fin:

if ( *RefaireUnCycle == OUI_PNE ) {
  PNE_PresolveSimplifieVariableProbingReinitConditionsInitiales( Pne, BminSv, BmaxSv );									
}

free( BminSv );
free( BmaxSv );

return;
}  

/*-------------------------------------------------------------------------------*/

void PNE_PresolveSimplifieVariableProbingReinitConditionsInitiales( PROBLEME_PNE * Pne, double * BminSv,
                                                                    double * BmaxSv )														
{
double * Bmin; double * Bmax; int NombreDeVariables; int Cnt; int * NumeroDeContrainteAAnalyser; 
PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; int j; int * NumeroDeContrainteModifiee;
int * NbFoisContrainteModifiee; char * ContrainteAAnalyser;

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

NombreDeVariables = Pne->NombreDeVariablesTrav;

memcpy( (char *) ProbingOuNodePresolve->BorneInfConnue, (char *) ProbingOuNodePresolve->BorneInfConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->BorneSupConnue, (char *) ProbingOuNodePresolve->BorneSupConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneInf, (char *) ProbingOuNodePresolve->ValeurDeBorneInfSv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneSup, (char *) ProbingOuNodePresolve->ValeurDeBorneSupSv, NombreDeVariables * sizeof( double ) );

Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;

NumeroDeContrainteModifiee = ProbingOuNodePresolve->NumeroDeContrainteModifiee;
NumeroDeContrainteAAnalyser = ProbingOuNodePresolve->NumeroDeContrainteAAnalyser;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;
NbFoisContrainteModifiee = ProbingOuNodePresolve->NbFoisContrainteModifiee;

for ( j = 0 ; j < ProbingOuNodePresolve->NbContraintesModifiees ; j++ ) {
  Cnt = NumeroDeContrainteModifiee[j];
	Bmin[Cnt] = BminSv[Cnt];
	Bmax[Cnt] = BmaxSv[Cnt];
  NbFoisContrainteModifiee[Cnt] = 0;
}
ProbingOuNodePresolve->NbContraintesModifiees = 0;

for ( j = 0 ; j < ProbingOuNodePresolve->NombreDeContraintesAAnalyser ; j++ ) {
  ContrainteAAnalyser[NumeroDeContrainteAAnalyser[j]] = NON_PNE;
}	
ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;

return;
}

/*-------------------------------------------------------------------------------*/

void PNE_PresolveSimplifieVariableProbingSauvegardeConditionsInitiales( PROBLEME_PNE * Pne, double * BminSv,
                                                                        double * BmaxSv )
{
PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; int NombreDeVariables; int NombreDeContraintes;
int Cnt; int * NbFoisContrainteModifiee; char * ContrainteAAnalyser;
   
ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;
NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

memcpy( (char *) ProbingOuNodePresolve->BorneInfConnueSv, (char *) ProbingOuNodePresolve->BorneInfConnue, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->BorneSupConnueSv, (char *) ProbingOuNodePresolve->BorneSupConnue, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneInfSv, (char *) ProbingOuNodePresolve->ValeurDeBorneInf, NombreDeVariables * sizeof( double ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneSupSv, (char *) ProbingOuNodePresolve->ValeurDeBorneSup, NombreDeVariables * sizeof( double ) );
memcpy( (char *) BminSv, (char *) ProbingOuNodePresolve->Bmin, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) BmaxSv, (char *) ProbingOuNodePresolve->Bmax, NombreDeContraintes * sizeof( double ) );

NbFoisContrainteModifiee = ProbingOuNodePresolve->NbFoisContrainteModifiee;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;

for ( Cnt = 0; Cnt < NombreDeContraintes ; Cnt++ ) {
  NbFoisContrainteModifiee[Cnt] = 0;
  ContrainteAAnalyser[Cnt] = NON_PNE;
}

ProbingOuNodePresolve->NbContraintesModifiees = 0;
ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;

return;
}

/*-------------------------------------------------------------------------------*/
