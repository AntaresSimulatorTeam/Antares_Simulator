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
# include "opt_structure_probleme_a_resoudre.h"

# include "../simulation/simulation.h"
# include "../simulation/sim_structure_donnees.h"
# include "../simulation/sim_extern_variables_globales.h"

# include "opt_fonctions.h"



void OPT_ChargerLaContrainteAPartirDePi( PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre, double * Pi , char SensContrainte )
{
	int NombreDeVariables           ; int NombreDeContraintes; int NombreDeTermes;
	int NombreDeTermesDeLaContrainte; int Var                ;

	NombreDeVariables   = ProblemeAResoudre->NombreDeVariables;
	NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
	NombreDeTermes      = ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes;

	NombreDeTermesDeLaContrainte = 0;
	ProblemeAResoudre->IndicesDebutDeLigne[NombreDeContraintes] = NombreDeTermes;
	for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
		if ( Pi[Var] == 0.0 ) continue;
		ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[NombreDeTermes] = Pi[Var];
		ProblemeAResoudre->IndicesColonnes                      [NombreDeTermes] = Var;
		NombreDeTermesDeLaContrainte++;
		NombreDeTermes++;  
		if ( NombreDeTermes == ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes ) {
			OPT_AugmenterLaTailleDeLaMatriceDesContraintes( ProblemeAResoudre );
		}  
	}
	ProblemeAResoudre->NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermesDeLaContrainte;
	
	
	ProblemeAResoudre->Sens[NombreDeContraintes] = SensContrainte;
	NombreDeContraintes++;

	ProblemeAResoudre->NombreDeContraintes                       = NombreDeContraintes;
	ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = NombreDeTermes;

	return;
}



void OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
                             PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre,
                             double * Pi,
														 int   * Colonne,
														 int     NombreDeTermesDeLaContrainte,
														 char     SensContrainte )
{
	 int i;  int NombreDeTermes; int NombreDeContraintes;

	
	

	

















	

	NombreDeContraintes = ProblemeAResoudre->NombreDeContraintes;
	NombreDeTermes      = ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes;

	ProblemeAResoudre->IndicesDebutDeLigne[NombreDeContraintes] = NombreDeTermes;
	for ( i = 0 ; i < NombreDeTermesDeLaContrainte ; i++ ) {		
		ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes[NombreDeTermes] = Pi[i];
		ProblemeAResoudre->IndicesColonnes                      [NombreDeTermes] = Colonne[i];
		NombreDeTermes++;  
		if ( NombreDeTermes == ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes ) {		
			OPT_AugmenterLaTailleDeLaMatriceDesContraintes( ProblemeAResoudre );
		}
	}
	ProblemeAResoudre->NombreDeTermesDesLignes[NombreDeContraintes] = NombreDeTermesDeLaContrainte;
	
	
	ProblemeAResoudre->Sens[NombreDeContraintes] = SensContrainte;
	NombreDeContraintes++;

	ProblemeAResoudre->NombreDeContraintes                       = NombreDeContraintes;
	ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = NombreDeTermes;

	return;
}
