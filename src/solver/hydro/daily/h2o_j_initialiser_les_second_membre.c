/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/






# include "h2o_j_donnees_mensuelles.h"
  


void H2O_J_InitialiserLeSecondMembre( DONNEES_MENSUELLES * DonneesMensuelles, int NumeroDeProbleme )
{
int Pdt; int Cnt; int NbPdt; double * TurbineCible; 

PROBLEME_HYDRAULIQUE * ProblemeHydraulique; 
CORRESPONDANCE_DES_CONTRAINTES * CorrespondanceDesContraintes;
PROBLEME_LINEAIRE_PARTIE_VARIABLE * ProblemeLineairePartieVariable;

int NumeroDeContrainteDEnergieMensuelle; int * NumeroDeContrainteSurXi; double * SecondMembre;

TurbineCible = DonneesMensuelles->TurbineCible;

ProblemeHydraulique = DonneesMensuelles->ProblemeHydraulique;

NbPdt = ProblemeHydraulique->NbJoursDUnProbleme[NumeroDeProbleme];

CorrespondanceDesContraintes = ProblemeHydraulique->CorrespondanceDesContraintes[NumeroDeProbleme];

NumeroDeContrainteDEnergieMensuelle = CorrespondanceDesContraintes->NumeroDeContrainteDEnergieMensuelle;

ProblemeLineairePartieVariable = ProblemeHydraulique->ProblemeLineairePartieVariable[NumeroDeProbleme];
SecondMembre = ProblemeLineairePartieVariable->SecondMembre;


SecondMembre[NumeroDeContrainteDEnergieMensuelle] = DonneesMensuelles->TurbineDuMois;


NumeroDeContrainteSurXi = CorrespondanceDesContraintes->NumeroDeContrainteSurXi;

for ( Pdt = 0 ; Pdt < NbPdt ; Pdt++ ) {
  
  Cnt = NumeroDeContrainteSurXi[Pdt];	
  SecondMembre[Cnt] = TurbineCible[Pdt];		
}

return;
}
