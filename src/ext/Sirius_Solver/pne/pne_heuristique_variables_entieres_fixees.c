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

   FONCTION: Heuristique, on fixe des variables fractionnaires qui ont pris
	           une valeur entiere dans le probleme courant et
             on resout le probleme par un branch and bound de probleme
						 reduit dans l'espoir de trouver une solution
             entiere.
						 Attention, lorsque ce module est appele les variables
						 UminTrav UmaxTrav et TypeDeBorneTrav on ete remises a leur
						 valeur du noeud racine. Donc leurs valeurs n'inclut pas
						 les instanciations faites en amont du noeud a partir duquel
						 on demarre.
						 Par contre les structures du simplexe se trouvent dans l'etat
						 du dernier noeud resolu (bornes et types de bornes).						 
                
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

# define TRACES 1

/*----------------------------------------------------------------------------*/

char PNE_HeuristiqueVariablesEntieresFixees( PROBLEME_PNE * Pne )    
{
int i; BB * Bb; double * UTravSv; int NbFix; char OK; MATRICE_DE_CONTRAINTES * Contraintes;
char ProblemeTropGros;

ProblemeTropGros = NON_PNE;

Bb = (BB *) Pne->ProblemeBbDuSolveur;
Contraintes = NULL;

UTravSv = Pne->ValeurLocale;
memcpy( (char *) UTravSv, (char *) Pne->UTrav, Pne->NombreDeVariablesTrav * sizeof( double ) );

memcpy( (char *) Pne->TypeDeBorneTrav , (char *) Pne->TypeDeBorneTravSv, Pne->NombreDeVariablesTrav * sizeof( int ) );

NbFix = 0;

for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {

  if ( Pne->TypeDeVariableTrav[i] != ENTIER ) continue;
  if ( Pne->LaVariableAUneValeurFractionnaire[i] == OUI_PNE ) continue;
	if ( fabs( Pne->CoutsReduits[i] ) < 1.e-8 ) continue; /* Cas hors base degeneree */
	/* La variable a pris une valeur entiere */

	/* Probleme: UmaxTrav et UminTrav ne sont pas ceux du noeud resolu mais ceux du noeud racine */
  if ( fabs( Pne->UmaxTrav[i] - Pne->UTrav[i] ) < 10 * Pne->SeuilDeFractionnalite[i] ) {				  
    Pne->TypeDeBorneTrav[i] = VARIABLE_FIXE;
		NbFix++;  
		Pne->UTrav[i] = Pne->UmaxTrav[i];
		Pne->UminTrav[i] = Pne->UmaxTrav[i];
	}
	else if ( fabs( Pne->UTrav[i] - Pne->UminTrav[i] ) < 10 * Pne->SeuilDeFractionnalite[i] ) {	
    Pne->TypeDeBorneTrav[i] = VARIABLE_FIXE;  
		NbFix++;
		Pne->UTrav[i] = Pne->UminTrav[i];
		Pne->UmaxTrav[i] = Pne->UminTrav[i];
	}							
	
}   

OK = PNE_HeuristiqueEvaluerTailleDuProbleme( Pne );
if ( OK == NON_PNE ) ProblemeTropGros = OUI_PNE;

if ( OK == OUI_PNE ) {

  if ( Pne->AffichageDesTraces == OUI_PNE ) {
    printf("Starting heuristic with %d binaries fixed over %d\n",NbFix,Pne->NombreDeVariablesEntieresNonFixes);
  }

  /* Construction de ma matrice des contraintes */
  Contraintes = PNE_HeuristiqueConstruireMatriceDeContraintes( Pne );
  if ( Contraintes == NULL ) goto Termine;

  /* Resolution du branch and bound reduit */
  OK = PNE_HeuristiqueResolutionBranchAndBoundReduit( Pne, Contraintes );
}

if ( OK == NON_PNE ) {
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
    printf("Heuristic failed ...\n");
  }
  Pne->NombreDEchecsSuccessifsHeuristiqueFixation++;
	if ( Pne->NombreDEchecsSuccessifsHeuristiqueFixation >= NB_MAX_ECHECS_SUCCESSIFS_HEURISTIQUE ) {
    Pne->NombreDeRefusSuccessifsHeuristiqueFixation = 0;
    Pne->FaireHeuristiqueFixation = NON_PNE;
    if ( Pne->NombreDeSolutionsHeuristiqueFixation <= 0 ) Pne->NombreDeReactivationsSansSuccesHeuristiqueFixation++;
		if ( Pne->NombreDeReactivationsSansSuccesHeuristiqueFixation > NB_MAX_REACTIVATION_SANS_SUCCES ) {
      Pne->StopHeuristiqueFixation = OUI_PNE;
			/*printf("****************** arret definitif HeuristiqueFixation \n");*/
		}		
		/*printf(" !!!!!!!!!!!! On positionne a FaireHeuristiqueFixation NON_PNE \n");*/
	}	
	goto Termine;	
}
else {
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
    printf("Heuristic found ...\n");
  }
}

/* On a trouve une solution et elle a ete controlee et archivee */

Pne->NombreDEchecsSuccessifsHeuristiqueFixation = 0;	
Pne->NombreDeSolutionsHeuristiqueFixation++;
Pne->NombreDeReactivationsSansSuccesHeuristiqueFixation = 0;

Termine:
    
Pne->CestTermine = NON_PNE;

memcpy( (char *) Pne->UTrav, (char *) UTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) Pne->TypeDeBorneTrav, (char *) Pne->TypeDeBorneTravSv, Pne->NombreDeVariablesTrav * sizeof( int ) );
memcpy( (char *) Pne->UmaxTrav, (char *) Pne->UmaxTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) Pne->UminTrav, (char *) Pne->UminTravSv, Pne->NombreDeVariablesTrav * sizeof( double ) );

PNE_HeuristiqueLibererMatriceDeContraintes( Contraintes ); 

return( ProblemeTropGros );

}

