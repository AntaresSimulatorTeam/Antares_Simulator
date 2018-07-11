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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <antares/study.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/area/constants.h>
#include <antares/logs.h>
#include "simulation.h"

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;




void SIM_AllocationProblemeHoraireAdequation()
{
	long i;

	
	auto& study = *Data::Study::Current::Get();

	
	ValeursAnnuellesAdequation = (VALEURS_ANNUELLES **) MemAlloc( study.areas.size() * sizeof(VALEURS_ANNUELLES *));

	for (i = 0; i < (long) study.areas.size(); i++)
	{
		ValeursAnnuellesAdequation[i] = (VALEURS_ANNUELLES *) MemAlloc( sizeof(VALEURS_ANNUELLES) );

		ValeursAnnuellesAdequation[i]->Consommation              = (double *) MemAlloc( study.runtime->nbHoursPerYear * sizeof(double) );
		ValeursAnnuellesAdequation[i]->PuissanceThermiqueCumulee = (double *) MemAlloc( study.runtime->nbHoursPerYear * sizeof(double) );
		ValeursAnnuellesAdequation[i]->Reserve                   = (double *) MemAlloc( study.runtime->nbHoursPerYear * sizeof(double) );
	}

	
	ProblemeHoraireAdequation.NombreDElementsChainage = 2 * study.runtime->interconnectionsCount + 4 *study.areas.size() ;
	ProblemeHoraireAdequation.IndicesDesVoisins       = (long *)                           MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(long) );
	ProblemeHoraireAdequation.PointeurSurIndices      = (long *)                           MemAlloc( (study.areas.size() + 3)                                * sizeof(long) );
	ProblemeHoraireAdequation.RangDansVoisinage       = (long *)                           MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(long) );
	ProblemeHoraireAdequation.QuellesNTCPrendre       = (double *)                         MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double) );
	ProblemeHoraireAdequation.ValeursEffectivesNTC    = (double *)                         MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double) );
	ProblemeHoraireAdequation.FluxDeSecours           = (double *)                         MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double) );
	ProblemeHoraireAdequation.NTCDeReference          = (double *)                         MemAlloc( ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double) );
	ProblemeHoraireAdequation.S                       = (double *)                         MemAlloc( (study.areas.size() + 2)                                * sizeof(double) );
	ProblemeHoraireAdequation.A                       = (long *)                           MemAlloc( (study.areas.size() + 2)                                * sizeof(long) );
	ProblemeHoraireAdequation.B                       = (long *)                           MemAlloc( (study.areas.size() + 2)                                * sizeof(long) );
	ProblemeHoraireAdequation.X                       = (long *)                           MemAlloc( (study.areas.size() + 2)                                * sizeof(long) );
	ProblemeHoraireAdequation.DonneesParPays          = (DONNEES_ADEQUATION **)            MemAlloc( study.areas.size()                                      * sizeof(DONNEES_ADEQUATION *) );
	ProblemeHoraireAdequation.ResultatsParPays       =  (RESULTATS_HORAIRES_ADEQUATION **) MemAlloc( study.areas.size()                                      * sizeof(RESULTATS_HORAIRES_ADEQUATION *) );

	for (i = 0; i < (long)study.areas.size(); i++)
	{
		ProblemeHoraireAdequation.DonneesParPays[i]   = (DONNEES_ADEQUATION *)             MemAlloc( sizeof(DONNEES_ADEQUATION) );
		ProblemeHoraireAdequation.ResultatsParPays[i] = (RESULTATS_HORAIRES_ADEQUATION *)  MemAlloc( sizeof(RESULTATS_HORAIRES_ADEQUATION) );
	}

	memset(ProblemeHoraireAdequation.IndicesDesVoisins , 0, ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(long));
	memset(ProblemeHoraireAdequation.PointeurSurIndices, 0, (study.areas.size() + 3)                                * sizeof(long));
	memset(ProblemeHoraireAdequation.RangDansVoisinage , 0, ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(long));
	memset(ProblemeHoraireAdequation.QuellesNTCPrendre , 0, ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double));
}





void SIM_InitialisationChainagePourAdequation()
{
	
	auto& study = *Data::Study::Current::Get();

	
	long k ;
	long i;
	long j;
	MATRICE_2D ** MatriceDuReseauAugmente;
	MATRICE_2D ** AdjacentTemporaire;
	uint nbAreasPlus2 = study.areas.size() + 2;
	long * NombreDeVoisins ;
	MatriceDuReseauAugmente                = (MATRICE_2D **) MemAlloc( nbAreasPlus2 * sizeof(MATRICE_2D*) );
	AdjacentTemporaire                     = (MATRICE_2D **) MemAlloc( nbAreasPlus2 * sizeof(MATRICE_2D*) );
	NombreDeVoisins                        = (long *)        MemAlloc( nbAreasPlus2 * sizeof(long) );
	AreaLink* lnk;

	for (i = 0; i < (long)nbAreasPlus2; i++)
	{
		AdjacentTemporaire[i]      = (MATRICE_2D *) MemAlloc( sizeof(MATRICE_2D) );
		MatriceDuReseauAugmente[i] = (MATRICE_2D *) MemAlloc( sizeof(MATRICE_2D) );

		MatriceDuReseauAugmente[i]->ParLigne = (double *) MemAlloc( (nbAreasPlus2) * sizeof(double) );
		AdjacentTemporaire[i]->ParLigne      = (double *) MemAlloc( (nbAreasPlus2) * sizeof(double) );
		memset(MatriceDuReseauAugmente[i]->ParLigne, 0, (nbAreasPlus2)*sizeof(double));
		memset(AdjacentTemporaire[i]->ParLigne     , 0, (nbAreasPlus2)*sizeof(double));
	}

	memset(NombreDeVoisins,0 ,(nbAreasPlus2)*sizeof(long));
	memset(ProblemeHoraireAdequation.QuellesNTCPrendre,0 ,ProblemeHoraireAdequation.NombreDElementsChainage*sizeof(double));

	











	for (i = 0; i < (long)study.runtime->interconnectionsCount; i++)
	{
		lnk = study.runtime->areaLink[i];
		assert(lnk != NULL);
		assert(1 + lnk->from->index < nbAreasPlus2);
		assert(1 + lnk->with->index < nbAreasPlus2);
		MatriceDuReseauAugmente[1+lnk->from->index]->ParLigne[1+lnk->with->index] = (double)(i+1);
		MatriceDuReseauAugmente[1+lnk->with->index]->ParLigne[1+lnk->from->index] = (double)(-i-1);
	}

	for (i = 1; i < (long)study.areas.size() + 1; i++)
	{
		MatriceDuReseauAugmente[0]->ParLigne[i]                = 0.5;
		MatriceDuReseauAugmente[study.areas.size() + 1]->ParLigne[i] = 0.5;
		MatriceDuReseauAugmente[i]->ParLigne[0]                = 0.5;
		MatriceDuReseauAugmente[i]->ParLigne[study.areas.size() + 1] = 0.5;
	}

	
	ProblemeHoraireAdequation.PointeurSurIndices[0] = 0;

	for (i = 0 ; i < (long)nbAreasPlus2 ; i++)
	{
		k = 0;
		for (j = 0 ; j < (long)nbAreasPlus2 ; j++)
		{
			if (fabs(MatriceDuReseauAugmente[i]->ParLigne[j]) > 0.1)
			{
				NombreDeVoisins[i] += 1;
				assert((uint)(ProblemeHoraireAdequation.PointeurSurIndices[i] + k) < (uint)ProblemeHoraireAdequation.NombreDElementsChainage);
				ProblemeHoraireAdequation.IndicesDesVoisins[ProblemeHoraireAdequation.PointeurSurIndices[i] + k] = j;
				AdjacentTemporaire[j]->ParLigne[i] = (double) k;
				k++;
			}
		}
		ProblemeHoraireAdequation.PointeurSurIndices[i+1] = ProblemeHoraireAdequation.PointeurSurIndices[i] + NombreDeVoisins[i];
	}

	for (i = 0; i < (long)nbAreasPlus2; i++)
	{
		for (j = ProblemeHoraireAdequation.PointeurSurIndices[i]; j < ProblemeHoraireAdequation.PointeurSurIndices[i+1]; j++)
		{
			assert((uint)j < (uint)ProblemeHoraireAdequation.NombreDElementsChainage);
			assert((uint)ProblemeHoraireAdequation.IndicesDesVoisins[j] < nbAreasPlus2);

			ProblemeHoraireAdequation.RangDansVoisinage[j] = (long)(AdjacentTemporaire[i]->ParLigne[ProblemeHoraireAdequation.IndicesDesVoisins[j]]); 
			ProblemeHoraireAdequation.QuellesNTCPrendre[j] = MatriceDuReseauAugmente[i]->ParLigne[ProblemeHoraireAdequation.IndicesDesVoisins[j]];
		}
	}
	
	MemFree(NombreDeVoisins);

	for (i = 0; i < (long)nbAreasPlus2; i++)
	{
		MemFree( MatriceDuReseauAugmente[i]->ParLigne );
		MemFree( AdjacentTemporaire[i]->ParLigne );
		MemFree( MatriceDuReseauAugmente[i] );
		MemFree( AdjacentTemporaire[i] );
	}

	MemFree(MatriceDuReseauAugmente);
	MemFree(AdjacentTemporaire);
}



void SIM_InitialisationProblemeHoraireAdequation()
{
	SIM_AllocationProblemeHoraireAdequation();
	SIM_InitialisationChainagePourAdequation();
}




void SIM_RenseignementValeursPourTouteLAnnee(const Antares::Data::Study& study, uint numSpace)
{
	uint startTime = study.calendar.days[study.parameters.simulationDays.first].hours.first;
	uint finalHour = study.calendar.days[study.parameters.simulationDays.end - 1].hours.end - 1;

	for (uint i = 0; i != study.areas.size(); ++i)
	{
		auto& Pt               = * ValeursAnnuellesAdequation[i];
		auto& tsIndex          = * NumeroChroniquesTireesParPays[numSpace][i];
		auto& area             = * study.areas[i];
		auto& scratchpad       = *(area.scratchpad[numSpace]);
		auto& primaryReserve   = area.reserves[fhrPrimaryReserve];
		auto& strategicReserve = area.reserves[fhrStrategicReserve];
		auto& ror              = area.hydro.series->ror;
		auto& calendar         = study.calendar;

		memset(Pt.Consommation,              0, study.runtime->nbHoursPerYear * sizeof(double));
		memset(Pt.PuissanceThermiqueCumulee, 0, study.runtime->nbHoursPerYear * sizeof(double));
		memset(Pt.Reserve                  , 0, study.runtime->nbHoursPerYear * sizeof(double));

		auto& avgMaxPower = area.hydro.maxPower.entry[1]; 
		for (uint h = startTime; h < finalHour; ++h)
		{
			assert((uint) tsIndex.Consommation < scratchpad.ts.load.width);
			assert((uint) tsIndex.Eolien < scratchpad.ts.wind.width);
			assert((uint) tsIndex.Solar < scratchpad.ts.solar.width);

			Pt.Consommation[h] =
				+ scratchpad.ts.load[tsIndex.Consommation][h]
				+ primaryReserve[h]
				- scratchpad.ts.wind[tsIndex.Eolien][h]
				- scratchpad.miscGenSum[h]
				- scratchpad.ts.solar[tsIndex.Solar][h]
				- ror[(uint)tsIndex.Hydraulique < ror.width ? tsIndex.Hydraulique : 0][h]
				
				- avgMaxPower[calendar.hours[h].dayYear];
			Pt.Reserve[h] = strategicReserve[h];
		}
		for (uint j = 0; j < area.thermal.list.size(); ++j)
		{
			auto& cluster = *(area.thermal.list.byIndex[j]);
			assert(&cluster != NULL);
			assert(cluster.series != NULL);
			assert((uint)tsIndex.ThermiqueParPalier[cluster.areaWideIndex] < cluster.series->series.width);
			assert(cluster.series->series.jit == NULL);

			auto& col = cluster.series->series[tsIndex.ThermiqueParPalier[cluster.areaWideIndex]];
			for (uint h = startTime; h < finalHour; ++h)
				Pt.PuissanceThermiqueCumulee[h] +=  col[h];
		}
	}
}




void SIM_RenseignementProblemeHoraireAdequation(long Heure)
{
	
	auto& study = *Data::Study::Current::Get();

	for (uint i = 0 ; i < study.areas.size(); i++)
	{
		DONNEES_ADEQUATION& PtHoraire      = *(ProblemeHoraireAdequation.DonneesParPays[i]);
		const VALEURS_ANNUELLES& PtAnnuel  = *(ValeursAnnuellesAdequation[i]);

		PtHoraire.Consommation              = PtAnnuel.Consommation[Heure];
		PtHoraire.PuissanceThermiqueCumulee = PtAnnuel.PuissanceThermiqueCumulee[Heure];
		PtHoraire.Reserve                   = PtAnnuel.Reserve[Heure];
	}

	
	
	
	memset(ProblemeHoraireAdequation.ValeursEffectivesNTC, 0 ,
		ProblemeHoraireAdequation.NombreDElementsChainage * sizeof(double));

	for (long i = 0; i < ProblemeHoraireAdequation.NombreDElementsChainage; ++i)
	{
		double NTCAPrendre = ProblemeHoraireAdequation.QuellesNTCPrendre[i];
		if (fabs(NTCAPrendre) > 0.5) 
		{
			if (NTCAPrendre > 0.) 
			{
				ProblemeHoraireAdequation.ValeursEffectivesNTC[i] = study.runtime->areaLink[(long)(fabs(NTCAPrendre)-1)]->data.entry[fhlNTCIndirect][Heure];
			}
			if (NTCAPrendre < 0.) 
			{
				ProblemeHoraireAdequation.ValeursEffectivesNTC[i] = study.runtime->areaLink[(long)(fabs(NTCAPrendre)-1)]->data.entry[fhlNTCDirect][Heure];
			}
		}
	}
}





void SIM_CalculFlotHoraireAdequation()
{
	
	auto& study = *Data::Study::Current::Get();

	ProblemeHoraireAdequation.DefaillanceEuropeenneHorsReseau  = NON_ANTARES;
	ProblemeHoraireAdequation.DefaillanceEuropeenneAvecReseau  = NON_ANTARES;

	for (uint i = 0; i < study.areas.size(); i++)
	{
		const DONNEES_ADEQUATION&  dataAdequacy = *(ProblemeHoraireAdequation.DonneesParPays[i]);

		auto& Pt                  = * ProblemeHoraireAdequation.ResultatsParPays[i];
		Pt.DefaillanceHorsReseau = NON_ANTARES;
		Pt.DefaillanceAvecReseau = NON_ANTARES;
		Pt.MargeHorsReseau       = dataAdequacy.PuissanceThermiqueCumulee - dataAdequacy.Consommation;
		Pt.Offre                 = 0.;
		if (Pt.MargeHorsReseau < 0.)
		{
			
			Pt.Offre                                                 = ProblemeHoraireAdequation.ResultatsParPays[i]->MargeHorsReseau;
			ProblemeHoraireAdequation.DefaillanceEuropeenneHorsReseau = OUI_ANTARES;
			Pt.DefaillanceHorsReseau                                 = OUI_ANTARES;
		}
		if (Pt.MargeHorsReseau > dataAdequacy.Reserve)
		{
			
			Pt.Offre = Pt.MargeHorsReseau - dataAdequacy.Reserve;
		}
	}

	if (ProblemeHoraireAdequation.DefaillanceEuropeenneHorsReseau > 0)
	{
		
		SIM_AlgorithmeDeFlot();
		for (uint i = 0 ; i < study.areas.size() ; i++ )
		{
			auto& Pt = * ProblemeHoraireAdequation.ResultatsParPays[i];
			if (Pt.MargeAvecReseau < 0.)
			{
				ProblemeHoraireAdequation.DefaillanceEuropeenneAvecReseau = OUI_ANTARES;
				Pt.DefaillanceAvecReseau                                  = OUI_ANTARES;
			}
		}
	}
	else 
	{
		for (uint i = 0 ; i < study.areas.size(); i++)
			ProblemeHoraireAdequation.ResultatsParPays[i]->MargeAvecReseau = ProblemeHoraireAdequation.ResultatsParPays[i]->MargeHorsReseau;
	}
}



void SIM_DesallocationProblemeHoraireAdequation()
{
	
	auto& study = *Data::Study::Current::Get();

	
	for (uint i = 0 ; i < study.areas.size(); i++)
	{
		MemFree( ValeursAnnuellesAdequation[i]->Consommation);
		MemFree( ValeursAnnuellesAdequation[i]->PuissanceThermiqueCumulee);
		MemFree( ValeursAnnuellesAdequation[i]->Reserve);
		MemFree( ValeursAnnuellesAdequation[i] );
	}
	MemFree( ValeursAnnuellesAdequation );
	ValeursAnnuellesAdequation      = NULL;
	
	MemFree( ProblemeHoraireAdequation.IndicesDesVoisins );
	MemFree( ProblemeHoraireAdequation.PointeurSurIndices );
	MemFree( ProblemeHoraireAdequation.RangDansVoisinage );
	MemFree( ProblemeHoraireAdequation.QuellesNTCPrendre );
	MemFree( ProblemeHoraireAdequation.ValeursEffectivesNTC );
	MemFree( ProblemeHoraireAdequation.FluxDeSecours );
	MemFree( ProblemeHoraireAdequation.NTCDeReference );
	MemFree( ProblemeHoraireAdequation.S );
	MemFree( ProblemeHoraireAdequation.A );
	MemFree( ProblemeHoraireAdequation.B );
	MemFree( ProblemeHoraireAdequation.X );
	ProblemeHoraireAdequation.IndicesDesVoisins       = NULL;
	ProblemeHoraireAdequation.PointeurSurIndices      = NULL;
	ProblemeHoraireAdequation.RangDansVoisinage       = NULL;
	ProblemeHoraireAdequation.QuellesNTCPrendre       = NULL;
	ProblemeHoraireAdequation.ValeursEffectivesNTC    = NULL;
	ProblemeHoraireAdequation.FluxDeSecours           = NULL;
	ProblemeHoraireAdequation.NTCDeReference          = NULL;
	ProblemeHoraireAdequation.S                       = NULL;
	ProblemeHoraireAdequation.A                       = NULL;
	ProblemeHoraireAdequation.B                       = NULL;
	ProblemeHoraireAdequation.X                       = NULL;
	for (uint i = 0 ; i < study.areas.size(); i++)
	{
		MemFree( ProblemeHoraireAdequation.DonneesParPays[i] );
		MemFree( ProblemeHoraireAdequation.ResultatsParPays[i] );
	}
	MemFree( ProblemeHoraireAdequation.DonneesParPays );
	MemFree( ProblemeHoraireAdequation.ResultatsParPays );
	ProblemeHoraireAdequation.DonneesParPays   = NULL;
	ProblemeHoraireAdequation.ResultatsParPays = NULL;
}



