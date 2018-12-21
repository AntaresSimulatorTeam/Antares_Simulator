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







# ifdef __CPLUSPLUS
  extern "C"
  {
# endif

# include "spx_constantes_externes.h"

# ifdef __CPLUSPLUS
  }
# endif

# include "h2o2_j_donnees_mensuelles.h"



void H2O2_j_ConstruireLesVariables(		int NbPdt,
										DONNEES_MENSUELLES_ETENDUES * DonneesMensuellesEtendues,
										double * Xmin,
										double * Xmax,
										int * TypeDeVariable,
										double ** AdresseOuPlacerLaValeurDesVariablesOptimisees,
										CORRESPONDANCE_DES_VARIABLES_PB_ETENDU * CorrespondanceDesVariables
									)
{
	int Var; int Pdt;

	
	

	Var = 0;

	
	for ( Pdt = 0 ; Pdt < NbPdt ; Pdt++ ) 
	{
		
		CorrespondanceDesVariables->NumeroVar_Turbine[Pdt] = Var;
		Xmin[Var] = 0.0; 
		Xmax[Var] = 0.0; 
		TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
		AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->Turbine[Pdt]);
		Var++;
	}


	
	for (Pdt = 0; Pdt < NbPdt; Pdt++)
	{
		CorrespondanceDesVariables->NumeroVar_niveauxFinJours[Pdt] = Var;
		Xmin[Var] = 0.0;
		Xmax[Var] = 1.0;
		TypeDeVariable[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
		AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->niveauxFinJours[Pdt]);
		Var++;
	}

	
	CorrespondanceDesVariables->NumeroVar_waste = Var;
	Xmin[Var] = 0.0;
	Xmax[Var] = LINFINI;
	TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
	AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->waste);
	Var++;

	
	for (Pdt = 0; Pdt < NbPdt; Pdt++)
	{
		CorrespondanceDesVariables->NumeroVar_overflow[Pdt] = Var;
		Xmin[Var] = 0.0;
		Xmax[Var] = LINFINI;
		TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->overflows[Pdt]);
		Var++;
	}

	
	for (Pdt = 0; Pdt < NbPdt; Pdt++)
	{
		CorrespondanceDesVariables->NumeroVar_deviations[Pdt] = Var;
		Xmin[Var] = 0.0;
		Xmax[Var] = LINFINI;
		TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->deviations[Pdt]);
		Var++;
	}

	
	for (Pdt = 0; Pdt < NbPdt; Pdt++)
	{
		CorrespondanceDesVariables->NumeroVar_violations[Pdt] = Var;
		Xmin[Var] = 0.0;
		Xmax[Var] = LINFINI;
		TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->violations[Pdt]);
		Var++;
	}

	
	CorrespondanceDesVariables->NumeroVar_deviationMax = Var;
	Xmin[Var] = 0.0;
	Xmax[Var] = LINFINI;
	TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
	AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->deviationMax);
	Var++;

	
	CorrespondanceDesVariables->NumeroVar_violationMax = Var;
	Xmin[Var] = 0.0;
	Xmax[Var] = LINFINI;
	TypeDeVariable[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
	AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = &(DonneesMensuellesEtendues->violationMax);
	Var++;

	return;
}
