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

   FONCTION: Controle des coupes qui n'avaient pas ete inserees dans le
             probleme. Si necessaire on les introduit dans le probleme
	           en vue d'une nouvelle resolution.
            
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"
#include "pne_fonctions.h"

/*---------------------------------------------------------------------------------------------------------*/

void BB_ControlerLesCoupesNonInsereesInitialement( BB * Bb, char * YaDesCoupesAjoutees )
{
COUPE ** Coupe; int NumeroDeCoupe; int il; double X; double Marge; NOEUD * NoeudCourant; 
int i; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;

NoeudCourant = Bb->NoeudEnExamen;
Marge        = 1.;

*YaDesCoupesAjoutees = NON;

Coupe = Bb->NoeudRacine->CoupesGenereesAuNoeud;  
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
  Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant = NON;
}
for ( i = 0 ; i < Bb->NombreDeCoupesAjoutees ; i++ ) {
  Coupe[Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[i]]->CoupeExamineeAuNoeudCourant = OUI;
}

/* Marquage des coupes que l'on pourra utiliser */
for ( NumeroDeCoupe = 0 ; NumeroDeCoupe < Bb->NoeudRacine->NombreDeCoupesGenereesAuNoeud ; NumeroDeCoupe++ ) {
  if ( Coupe[NumeroDeCoupe]->CoupeExamineeAuNoeudCourant == OUI ) continue;	
  X = 0.;
  for ( il = 0 ; il < Coupe[NumeroDeCoupe]->NombreDeTermes ; il++ ) {  
    X += Coupe[NumeroDeCoupe]->Coefficient[il] *
         Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant[ Coupe[NumeroDeCoupe]->IndiceDeLaVariable[il] ];
  }
  if ( X > Coupe[NumeroDeCoupe]->SecondMembre + Marge ) {  
    *YaDesCoupesAjoutees = OUI;
    /* La coupe est violee => on l'ajoute au probleme courant */
    Bb->NoeudRacine->NumeroDesCoupeAjouteeAuProblemeCourant[Bb->NombreDeCoupesAjoutees] = NumeroDeCoupe; 
    Bb->NombreDeCoupesAjoutees++;
    /* Pour avoir une base inversible */   
    Coupe[NumeroDeCoupe]->CoupeSaturee = NON_PNE;		
    PNE_InsererUneContrainte( Pne,
                              Coupe[NumeroDeCoupe]->NombreDeTermes, 
                              Coupe[NumeroDeCoupe]->Coefficient, 
                              Coupe[NumeroDeCoupe]->IndiceDeLaVariable,  
                              Coupe[NumeroDeCoupe]->SecondMembre,
                              Coupe[NumeroDeCoupe]->CoupeSaturee,
                              Coupe[NumeroDeCoupe]->TypeDeCoupe
															);
    Bb->NombreTotalDeCoupesDuPoolUtilisees++; 
  }			    
}
 
return;
}


 
