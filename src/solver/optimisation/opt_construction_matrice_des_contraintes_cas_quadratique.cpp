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
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "opt_fonctions.h"

void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique( PROBLEME_HEBDO * ProblemeHebdo )
{
	int Interco; int Pays; int Var; int NombreDeTermes;	double * Pi; int * Colonne;
	CORRESPONDANCES_DES_VARIABLES * CorrespondanceVarNativesVarOptim; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;	

	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

	Pi      = (double *) MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(double));
	Colonne = (int *)    MemAlloc(ProblemeAResoudre->NombreDeVariables * sizeof(int   ));

	
	ProblemeAResoudre->NombreDeContraintes = 0;
	ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
	CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[0];

	
	for (Pays = 0 ; Pays < ProblemeHebdo->NombreDePays - 1 ; Pays++) {
		
		NombreDeTermes = 0;

		Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Pays];
		while (Interco >= 0) {
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
			if (Var >= 0) {
				Pi     [NombreDeTermes] = 1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
			Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
		}
		Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Pays];
		while (Interco >= 0 ) {
			Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
			if (Var >= 0 ) {
				Pi     [NombreDeTermes] = -1.0;
				Colonne[NombreDeTermes] = Var;
				NombreDeTermes++;
			}
			Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
		}

		
		ProblemeHebdo->NumeroDeContrainteDeSoldeDEchange[Pays] = ProblemeAResoudre->NombreDeContraintes;
		
		OPT_ChargerLaContrainteDansLaMatriceDesContraintes( ProblemeAResoudre, Pi, Colonne, NombreDeTermes, '=' );
	}

	MemFree(Pi);
	MemFree(Colonne);
}


