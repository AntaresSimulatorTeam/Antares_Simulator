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

   FONCTION: Pilotage du branch and bound  
 
   Appele comme un sous-programme du programme fortran maitre. Ce 
   sous-programme orchestre le déroulement de la recherche arborescente. 
   Les etapes sont les suivantes:
   1- Creation du noeud racine.
   2- Recherche en profondeur a partie du noeud racine pour trouver une  
      solution entiere admissible.
   3- Recherche en largeur a partir du noeud racine pour trouver la  
      solution optimale.
       
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "bb_sys.h"
# include "bb_define.h"
# include "bb_fonctions.h"

# include "pne_define.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/

int BB_BranchAndBound( void * PneE,
                       int TempsDexecutionMaximum, int NombreMaxDeSolutionsEntieres, double ToleranceDOptimalite, 
                       int NbVar, int NbContr   , int NbVarEntieres               , char AffichageDesTraces    ,
			                 int * NumVarEntieres )			
{
BB * Bb; PROBLEME_PNE * Pne; int YaUneSolution; void * Tas;

Bb = NULL;

if ( Bb == NULL ) {
  # ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
    Tas = MEM_Init(); 
    Bb = (BB *) MEM_Malloc( Tas, sizeof( BB ) );
    if ( Bb == NULL ) {
      printf("Saturation memoire, impossible d'allouer un objet BB\n");
      YaUneSolution = BB_ERREUR_INTERNE;
      return( YaUneSolution );    
    }		
	  Bb->Tas = Tas;
  # else		 
    Bb = (BB *) malloc( sizeof( BB ) );
    if ( Bb == NULL ) {
      printf("Saturation memoire, impossible d'allouer un objet BB\n");
      YaUneSolution = BB_ERREUR_INTERNE;
      return( YaUneSolution );    
    }
		Tas = NULL;
	  Bb->Tas = Tas;
	# endif
}

Pne = (PROBLEME_PNE *) PneE;
/* Initialisation de ProblemeBbDuSolveur pour la partie PNE */
Pne->ProblemeBbDuSolveur = (void *) Bb;
Bb->ProblemePneDuBb = PneE;

Bb->AnomalieDetectee = NON;
setjmp( Bb->EnvBB );  
if ( Bb->AnomalieDetectee == OUI ) {
  YaUneSolution = BB_ERREUR_INTERNE;
  /* Liberation du probleme */ 
  BB_BranchAndBoundDesallouerProbleme( Bb );
  Pne->ProblemeBbDuSolveur = NULL;
} 
else {
  /* Recherche arborescente */
  YaUneSolution = BB_BranchAndBoundCalculs( Bb, TempsDexecutionMaximum, NombreMaxDeSolutionsEntieres, ToleranceDOptimalite,
                                            NbVar, NbContr, NbVarEntieres, AffichageDesTraces, NumVarEntieres );   
  Pne->ProblemeBbDuSolveur = NULL;
}
  
return( YaUneSolution ); 

}

