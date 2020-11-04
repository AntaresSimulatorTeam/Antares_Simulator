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






#include <math.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include <yuni/io/file.h>
#include "opt_fonctions.h"



double OPT_CalculerAireMaxPminJour( int PremierPdt, int DernierPdt, int MUTetMDT, int NombreDePasDeTemps,
                                    int * NbGrpCourbeGuide, int * NbGrpOpt )
{
	double Cout; int NbMx; int T; int i;

	Cout = 0.0;


	NbMx = 0;
	for ( T = 0 ; T < PremierPdt ; T++ )
	{
		if ( NbGrpCourbeGuide[T] > NbMx )
			NbMx = NbGrpCourbeGuide[T];
	}

	for ( T = DernierPdt ; T < NombreDePasDeTemps ; T++ ) 
	{
		if ( NbGrpCourbeGuide[T] > NbMx )
			NbMx = NbGrpCourbeGuide[T];
	}

	for ( T = 0 ; T < PremierPdt ; T++ )
	{
		NbGrpOpt[T] = NbMx;
		Cout+= (double) ( NbGrpOpt[T] - NbGrpCourbeGuide[T] );
	}

	for ( T = DernierPdt ; T < NombreDePasDeTemps ; T++ )
	{
		NbGrpOpt[T] = NbMx;
		Cout+= (double) ( NbGrpOpt[T] - NbGrpCourbeGuide[T] );
	}


	T = PremierPdt;
	while ( T < DernierPdt )
	{
		NbMx = 0;
		for ( i = 0 ; i < MUTetMDT && T < DernierPdt ; i++ , T++ )
		{
			if ( NbGrpCourbeGuide[T] > NbMx ) NbMx = NbGrpCourbeGuide[T];
		}

		T-= i ;
		for ( i = 0 ; i < MUTetMDT && T < DernierPdt ; i++ , T++ )
		{
			NbGrpOpt[T] = NbMx;		
			Cout+= (double) ( NbGrpOpt[T] - NbGrpCourbeGuide[T] );
		}
	}

	return( Cout );
}




void OPT_CalculerLesPminThermiquesEnFonctionDeMUTetMDT( PROBLEME_HEBDO * ProblemeHebdo )
{
	int Pays; int Palier; int Pdt; double P    ; int NombreDePasDeTemps; int MUTetMDT           ;
	double * PminDuPalierThermiquePendantUneHeure ; double * TailleUnitaireDUnGroupeDuPalierThermique;
	double * PuissanceMinDuPalierThermique        ; double * PuissanceDisponibleDuPalierThermique    ;
	int * NbGrpCourbeGuide; int * NbGrpOpt      ; double EcartOpt; int iOpt; int i; double Ecart ;
	int Mediane; int PetitDelta; int PremierPdt; int IntervalleDAjustement; int DernierPdt      ;
 
	RESULTATS_HORAIRES *                 ResultatsHoraires;
	PRODUCTION_THERMIQUE_OPTIMALE **     ProductionThermiqueOptimale; 
	PDISP_ET_COUTS_HORAIRES_PAR_PALIER * PuissanceDispoEtCout ;
	PALIERS_THERMIQUES *                 PaliersThermiquesDuPays;
	int * minUpDownTime;
  
	NombreDePasDeTemps = ProblemeHebdo->NombreDePasDeTemps;
	NbGrpCourbeGuide   = ProblemeHebdo->NbGrpCourbeGuide;
	NbGrpOpt           = ProblemeHebdo->NbGrpOpt;

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays; ++Pays)
	{
		ResultatsHoraires       = ProblemeHebdo->ResultatsHoraires[Pays];
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		PminDuPalierThermiquePendantUneHeure     = PaliersThermiquesDuPays->PminDuPalierThermiquePendantUneHeure;
		TailleUnitaireDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique;
		minUpDownTime = PaliersThermiquesDuPays->minUpDownTime;

		ProductionThermiqueOptimale = ResultatsHoraires->ProductionThermique;

		for ( Palier = 0 ; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Palier++ )
		{

			PuissanceDispoEtCout    = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
			PuissanceMinDuPalierThermique        = PuissanceDispoEtCout->PuissanceMinDuPalierThermique;
			PuissanceDisponibleDuPalierThermique = PuissanceDispoEtCout->PuissanceDisponibleDuPalierThermique;

		
			if ( fabs( PminDuPalierThermiquePendantUneHeure[Palier] ) < ZERO_PMIN )
				continue;
			
			for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ )
			{
				P = ProductionThermiqueOptimale[Pdt]->ProductionThermiqueDuPalier[Palier];
			
				NbGrpCourbeGuide[Pdt] = 0;			
				if ( fabs( P ) < ZERO_PMIN ) continue;
			
				if ( TailleUnitaireDUnGroupeDuPalierThermique[Palier] > ZERO_PMIN )
					NbGrpCourbeGuide[Pdt] = (int) ceil( P / TailleUnitaireDUnGroupeDuPalierThermique[Palier] );
				else
					NbGrpCourbeGuide[Pdt] = (int) ceil( P );
			}		
				
    
			EcartOpt = LINFINI_ANTARES;
			MUTetMDT = minUpDownTime[Palier];
	
			iOpt     = -1;
    
			PetitDelta = NombreDePasDeTemps - ( MUTetMDT * ( (int) floor((double)(NombreDePasDeTemps / MUTetMDT) ) ) );
			Mediane = NombreDePasDeTemps >> 1;
		
			if ( MUTetMDT > Mediane )
				IntervalleDAjustement = NombreDePasDeTemps - PetitDelta;
			else
				IntervalleDAjustement = MUTetMDT + PetitDelta;

	
			IntervalleDAjustement = PetitDelta;

			IntervalleDAjustement = MUTetMDT;
			if ( NombreDePasDeTemps - MUTetMDT < IntervalleDAjustement )
				IntervalleDAjustement = NombreDePasDeTemps - MUTetMDT;

			if ( IntervalleDAjustement < 0 )
				IntervalleDAjustement = 0;

			for ( i = 0 ; i <= IntervalleDAjustement ; i++ )
			{
				PremierPdt = i;
				DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + i;
				Ecart = OPT_CalculerAireMaxPminJour( PremierPdt, DernierPdt, MUTetMDT, NombreDePasDeTemps, NbGrpCourbeGuide, NbGrpOpt );			
				if ( Ecart < EcartOpt )
				{
					EcartOpt = Ecart;
					iOpt     = i;
				}
			}

			if ( iOpt < 0 ) continue;
		
			PremierPdt = iOpt;
			DernierPdt = NombreDePasDeTemps - IntervalleDAjustement + iOpt;
		
			Ecart = OPT_CalculerAireMaxPminJour( PremierPdt, DernierPdt, MUTetMDT, NombreDePasDeTemps, NbGrpCourbeGuide, NbGrpOpt );
		
			for ( Pdt = 0 ; Pdt < NombreDePasDeTemps ; Pdt++ )
			{
				if ( PminDuPalierThermiquePendantUneHeure[Palier] * NbGrpOpt[Pdt] > PuissanceMinDuPalierThermique[Pdt] ) 
					PuissanceMinDuPalierThermique[Pdt] = PminDuPalierThermiquePendantUneHeure[Palier] * NbGrpOpt[Pdt];

				if ( PuissanceMinDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermique[Pdt] )											 
					PuissanceMinDuPalierThermique[Pdt] = PuissanceDisponibleDuPalierThermique[Pdt];				
			}
		}
	}

	return;
}

