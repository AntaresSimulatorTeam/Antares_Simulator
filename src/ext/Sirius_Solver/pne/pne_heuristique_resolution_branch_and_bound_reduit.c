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

   FONCTION: Resolution d'un branch and bound reduit pour les heuristiques.				 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"   
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PNE_HeuristiqueArchivageSolutionEtMajBranchAndBound( PROBLEME_PNE * Pne, double Critere ) 
{
int Var; BB * Bb; double X; int NbNonFix; int * TypeDeVariable; int * TypeDeBorne;
double * Xmin; double * Xmax; double EcartBorneInf;

Bb = (BB *) Pne->ProblemeBbDuSolveur;

/* On archive la solution: attention il faudra aussi y mettre VariablesDualesDesContraintesTravEtDesCoupes */

PNE_ArchiverLaSolutionCourante( Pne );		
Bb->CoutDeLaMeilleureSolutionEntiere = Critere;
Bb->UtiliserCoutDeLaMeilleureSolutionEntiere = OUI_SPX;
Bb->NombreDeProblemesResolus++;

/* Si on met SolutionEntiereTrouveeParHeuristique = OUI, la recherche en profondeur s'arrete.
	 Sinon elle continue tant qu'elle ne trouve pas de solution entiere par elle meme dans la limite
	 d'un certain nombre de problemes */

BB_RechercherLeMeilleurMinorant( Bb, RECHERCHER_LE_PLUS_PETIT );
X = fabs( Bb->ValeurDuMeilleurMinorant );
X = 0.01 * ( X + 1.e-7 );
EcartBorneInf = (Bb->CoutDeLaMeilleureSolutionEntiere - Bb->ValeurDuMeilleurMinorant) / X;
EcartBorneInf = fabs( EcartBorneInf );	

if ( EcartBorneInf > 100 ) {
  /* La solution heuristique est mauvaise on prefere continuer dans la recherche en profondeur */
  Bb->SolutionEntiereTrouveeParHeuristique = NON;
}
else {
  Bb->SolutionEntiereTrouveeParHeuristique = OUI;	
}

/* Pour evaluer une profondeur on compte le nombre de variables entieres qui n'etaient pas fixees */

TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;

NbNonFix = 0;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( TypeDeVariable[Var] != ENTIER ) continue;
  if ( TypeDeBorne[Var] != VARIABLE_FIXE && Xmin[Var] != Xmax[Var] ) NbNonFix++;
}


if ( Bb->NombreDeSolutionsEntieresTrouvees == 0 ) {
	Bb->NombreDeSolutionsEntieresTrouvees++;	 
	Bb->ProfondeurMoyenneDesSolutionsEntieres = NbNonFix;
}
else {				
	X = 0.;
	if ( Bb->ProfondeurMoyenneDesSolutionsEntieres >= 0 ) {
    X = Bb->ProfondeurMoyenneDesSolutionsEntieres * Bb->NombreDeSolutionsEntieresTrouvees;
	}
	X += NbNonFix;
	Bb->NombreDeSolutionsEntieresTrouvees++;
	X /= Bb->NombreDeSolutionsEntieresTrouvees;
	Bb->ProfondeurMoyenneDesSolutionsEntieres = (int) ceil( X );									
}			
			  
Bb->ForcerAffichage = OUI;  

return;
}

/*----------------------------------------------------------------------------*/

char PNE_HeuristiqueResolutionBranchAndBoundReduit( PROBLEME_PNE * Pne,
                                                    MATRICE_DE_CONTRAINTES * Contraintes ) 
{
int Var; double Critere; char OK; PROBLEME_A_RESOUDRE Probleme; int NbMaxSol; CONTROLS Controls;
int Cnt; int il; int ilMax; double ToleranceViolation; double S;

Probleme.NombreDeVariables       = Pne->NombreDeVariablesTrav;
Probleme.TypeDeVariable          = Pne->TypeDeVariableTrav;
Probleme.TypeDeBorneDeLaVariable = Pne->TypeDeBorneTrav;
Probleme.X                       = Pne->UTrav;
Probleme.Xmax                    = Pne->UmaxTrav;
Probleme.Xmin                    = Pne->UminTrav;
Probleme.CoutLineaire            = Pne->LTrav;
Probleme.NombreDeContraintes                   = Contraintes->NombreDeContraintes;
Probleme.SecondMembre                          = Contraintes->SecondMembre;
Probleme.Sens                                  = Contraintes->Sens;
Probleme.IndicesDebutDeLigne                   = Contraintes->IndexDebut;
Probleme.NombreDeTermesDesLignes               = Contraintes->NombreDeTermes;
Probleme.CoefficientsDeLaMatriceDesContraintes = Contraintes->Coefficient;
Probleme.IndicesColonnes                       = Contraintes->Colonne;
Probleme.VariablesDualesDesContraintes         = NULL;
Probleme.SortirLesDonneesDuProbleme = OUI_PNE;
Probleme.AlgorithmeDeResolution     = SIMPLEXE; /* SIMPLEXE ou POINT_INTERIEUR */  
Probleme.CoupesLiftAndProject       = NON_PNE;

Probleme.AffichageDesTraces = OUI_PNE;

Probleme.FaireDuPresolve = NON_PNE /* OUI_PNE */;        

Probleme.TempsDExecutionMaximum = 10;  

NbMaxSol = -1 /*1*/;
  
Probleme.NombreMaxDeSolutionsEntieres = NbMaxSol;

Probleme.ToleranceDOptimalite = 1.e-0; /* C'est en % donc 1.e-4 ca fait 1.e-6 */

Controls.Pne = Pne;
Controls.PneFils = NULL;
/*Controls.PresolveFils = NULL;*/
Controls.PresolveUniquement = NON_PNE;
Controls.FaireDuVariableProbing = OUI_PNE;
Controls.RechercherLesCliques = OUI_PNE;

/*printf("NombreDeVariablesAValeurFractionnaire %d NombreDeVariablesEntieresTrav %d Nombre de contraintes %d \n",
        Pne->NombreDeVariablesAValeurFractionnaire,Pne->NombreDeVariablesEntieresTrav,Contraintes->NombreDeContraintes);*/

PNE_SolveurProblemeReduit( &Probleme, &Controls );

/*printf("Probleme.ExistenceDUneSolution %d\n",Probleme.ExistenceDUneSolution);*/

OK = NON_PNE;
if ( Probleme.ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE ) {
  /*printf("Solution optimale trouvee\n");*/
  OK = OUI_PNE;
}
if ( Probleme.ExistenceDUneSolution == ARRET_PAR_LIMITE_DE_TEMPS_AVEC_SOLUTION_ADMISSIBLE_DISPONIBLE ) OK = OUI_PNE;

/* Controle du cout de la solution */
if ( OK == OUI_PNE ) {
  Critere = Pne->Z0;
	for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) Critere += Pne->LTrav[Var] * Pne->UTrav[Var];
	
  if ( Critere >= Pne->CoutOpt + 1.e-4 && Pne->YaUneSolutionEntiere == OUI_PNE ) {
		/* Solution plus chere */
		OK = NON_PNE;
		/*printf("!!!! Warning dans l'Heuristique Critere %e  Pne->CoutOpt %e --------------------\n",
                 Critere,Pne->CoutOpt);*/
		/*exit(0);*/
  }
}

if ( OK == NON_PNE ) goto Termine;


/* Verification admissibilite */
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;
  if ( Pne->UTrav[Var] > Pne->UmaxTrav[Var] + 1.e-6 ) {
	  /*printf("!!!!!!! Var %d UTrav %e UmaxTrav %e\n",Var,Pne->UTrav[Var],Pne->UmaxTrav[Var]);*/
		/*exit(0);*/
		OK = NON_PNE;
		break;  
	}
  if ( Pne->UTrav[Var] < Pne->UminTrav[Var] - 1.e-6 ) {
	  /*printf("!!!!!!! Var %d UTrav %e UminTrav %e\n",Var,Pne->UTrav[Var],Pne->UminTrav[Var]);*/
		/*exit(0);*/
		OK = NON_PNE;
		break;
	}
}

for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
   /* Verifier les violations de contraintes */
  S = 0.;
  il    = Pne->MdebTrav[Cnt];
  ilMax = il + Pne->NbTermTrav[Cnt];
  while ( il < ilMax ) {
      S += Pne->ATrav[il] * Pne->UTrav[Pne->NuvarTrav[il]];
      il++;
  }
	ToleranceViolation = 1.e-6;
  if ( Pne->SensContrainteTrav[Cnt] == '=' ) {
    if ( fabs( S - Pne->BTrav[Cnt] ) > ToleranceViolation ) {
      /*printf("!!!!!!!!! Contrainte d'egalite %d violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					        Pne->BTrav[Cnt],fabs( S - Pne->BTrav[Cnt] ));*/
		  /*exit(0);*/
		  OK = NON_PNE;
		  break;									
    }
  }
  else if ( Pne->SensContrainteTrav[Cnt] == '<' ) {
    if ( S > Pne->BTrav[Cnt] + ToleranceViolation ) {
      /*printf("!!!!!!!!!!!!!!! Contrainte %d de type < ou = violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					    Pne->BTrav[Cnt],fabs( S - Pne->BTrav[Cnt] ));*/
		  /*exit(0);*/
		  OK = NON_PNE;
		  break;							
    }
  }
  else if ( Pne->SensContrainteTrav[Cnt] == '>' ) {
    if ( S < Pne->BTrav[Cnt] - ToleranceViolation ) {
      /*printf("Contrainte %d de type > ou = violee: valeur calculee %e second membre %e violation %e\n",Cnt,S,
					    Pne->BTrav[Cnt],fabs( S - Pne->BTrav[Cnt] ));*/
		  /*exit(0);*/
		  OK = NON_PNE;
		  break;								
    }
  }
}

if ( OK == NON_PNE ) goto Termine;
/* Solution trouvee */

/* On archive la solution: attention il faudra aussi y mettre VariablesDualesDesContraintesTravEtDesCoupes */

PNE_HeuristiqueArchivageSolutionEtMajBranchAndBound( Pne, Critere ); 

Termine:

return( OK );

}

