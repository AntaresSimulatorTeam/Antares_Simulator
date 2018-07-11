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
#include <antares/study/area/constants.h>
#include <antares/study/area/scratchpad.h>

#include "simulation.h"

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"
#include "../optimisation/opt_fonctions.h"
#include "../optimisation/opt_structure_probleme_a_resoudre.h"



using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;


void SIM_InitialisationProblemeHebdo(Data::Study& study, PROBLEME_HEBDO& problem, int NombreDePasDeTemps, uint numSpace)
{
	int NombreClassesActives;
	int NombrePaliers;
	CONTRAINTES_COUPLANTES   * PtMat;
	char                       ClasseDefinie[168 + 1];

	
	auto& parameters = study.parameters;


	for (uint i = 0; i < 169; i++)
		ClasseDefinie[i] = NON_ANTARES;

	
	NombreClassesActives = 1;
	ClasseDefinie[1] = OUI_ANTARES;

	for (uint i = 0; i != study.areas.size(); i++)
	{
		
		auto& area = *(study.areas.byIndex[i]);

		for (uint j = 0; j < area.thermal.list.size(); j++) 
		{
			const uint minUpDownTime = area.thermal.list.byIndex[j]->minUpDownTime;
			if (ClasseDefinie[minUpDownTime] == NON_ANTARES) 
			{
				ClasseDefinie[minUpDownTime] = OUI_ANTARES;
				++NombreClassesActives;
			}
		}
	}

	problem.ClasseDeManoeuvrabiliteActive = (CLASSE_DE_MANOEUVRABILITE *) MemAlloc(NombreClassesActives * sizeof(int));

	{
		int j = 0;
		for (int k = 168; k >= 0; k--)
		{
			if (ClasseDefinie[k] == OUI_ANTARES)
			{
				problem.ClasseDeManoeuvrabiliteActive[j] = (CLASSE_DE_MANOEUVRABILITE) k;
				++j;
			}
		}
	}
	problem.NombreDeClassesDeManoeuvrabiliteActives = NombreClassesActives;

	
	SIM_AllocationProblemeHebdo(problem, NombreDePasDeTemps);

	
	
	problem.NombreDePasDeTemps            = NombreDePasDeTemps;
	
	problem.NombreDePasDeTempsDUneJournee = (int)(NombreDePasDeTemps / 7);
	
	problem.NombreDePays                  = study.areas.size();
	
	problem.NombreDInterconnexions        = study.runtime->interconnectionsCount;
	
	problem.NombreDeContraintesCouplantes = study.runtime->bindingConstraintCount;

	
	problem.ExportMPS					  = study.parameters.include.exportMPS; 

	
	problem.OptimisationAvecCoutsDeDemarrage = (study.parameters.unitCommitment.ucMode == Antares::Data::UnitCommitmentMode::ucMILP) ? OUI_ANTARES : NON_ANTARES ;

	
	
	
	
	problem.BorneDeLaDefaillancePositive  = (parameters.mode == Data::stdmEconomy) ? EGOISTE : ALTRUISTE;

	
	switch (parameters.power.fluctuations)
	{
		case Data::lssFreeModulations:
			problem.TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
			break;
		case Data::lssMinimizeRamping:
			problem.TypeDeLissageHydraulique = LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS;
			break;
		case Data::lssMinimizeExcursions:
			problem.TypeDeLissageHydraulique = LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX;
			break;
		case Data::lssUnknown:
			problem.TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
			assert(false && "invalid power fluctuations");
			break;
	}


	for (uint i = 0; i != study.areas.size(); i++)
	{
		
		auto& area = *(study.areas[i]);

		problem.NomsDesPays[i] = area.id.c_str();

		
		
		problem.CoutDeDefaillancePositive[i] = area.thermal.unsuppliedEnergyCost;
		
		problem.CoutDeDefaillanceNegative[i] = area.thermal.spilledEnergyCost;

		problem.CoutDeDefaillanceEnReserve[i] = area.thermal.unsuppliedEnergyCost;
		problem.CaracteristiquesHydrauliques[i]->PresenceDHydrauliqueModulable  =
			(area.scratchpad[numSpace]->hydroHasMod ? OUI_ANTARES : NON_ANTARES);

		
		problem.DefaillanceNegativeUtiliserPMinThermique[i] = ((anoOtherDispatchPower & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;
		problem.DefaillanceNegativeUtiliserHydro[i]         = ((anoDispatchHydroPower & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;
		problem.DefaillanceNegativeUtiliserConsoAbattue[i]  = ((anoNonDispatchPower   & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;

		
		assert(area.hydro.intraDailyModulation >= 1. && "Intra-daily modulation must be >= 1.0");
		problem.CoefficientEcretementPMaxHydraulique[i]  = area.hydro.intraDailyModulation;
	}

	
	for (uint i = 0; i < study.runtime->interconnectionsCount; ++i)
	{
		
		auto& link = *(study.runtime->areaLink[i]);
		problem.PaysOrigineDeLInterconnexion[i]   = link.from->index;
		problem.PaysExtremiteDeLInterconnexion[i] = link.with->index;
	}

	
	for (uint i = 0; i < study.runtime->bindingConstraintCount; ++i)
	{
		
		BindingConstraintRTI& bc = study.runtime->bindingConstraint[i];

		PtMat      = problem.MatriceDesContraintesCouplantes[i];
		PtMat->NombreDInterconnexionsDansLaContrainteCouplante = bc.linkCount;
		
		
		
		
		switch (bc.type)
		{
			case BindingConstraint::typeHourly:
				PtMat->TypeDeContrainteCouplante = CONTRAINTE_HORAIRE;
				break;
			case BindingConstraint::typeDaily:
				PtMat->TypeDeContrainteCouplante = CONTRAINTE_JOURNALIERE;
				break;
			case BindingConstraint::typeWeekly:
				PtMat->TypeDeContrainteCouplante = CONTRAINTE_HEBDOMADAIRE;
				break;
			case BindingConstraint::typeUnknown:
			case BindingConstraint::typeMax:
				assert(false && "Invalid constraint");
				break;
		}
		PtMat->SensDeLaContrainteCouplante = bc.operatorType;

		for (uint j = 0 ; j < bc.linkCount; ++j)
		{
			PtMat->NumeroDeLInterconnexion[j] = bc.linkIndex[j];
			PtMat->PoidsDeLInterconnexion[j]  = bc.weight[j];
			
			PtMat->OffsetTemporelSurLInterco[j] = bc.offset[j];
		}
	}

	
	NombrePaliers = 0;
	for (uint i = 0; i < study.areas.size(); ++i)
	{
		
		auto& area = *(study.areas.byIndex[i]);

		auto& pbPalier  = *(problem.PaliersThermiquesDuPays[i]);
		pbPalier.NombreDePaliersThermiques = area.thermal.list.size();

		for (uint l = 0; l != area.thermal.list.size(); ++l)
		{
			auto& cluster = *(area.thermal.list.byIndex[l]);
			pbPalier.NumeroDuPalierDansLEnsembleDesPaliersThermiques[l] = NombrePaliers + l;
			pbPalier.TailleUnitaireDUnGroupeDuPalierThermique[l]        = cluster.nominalCapacityWithSpinning;
			pbPalier.PminDuPalierThermiquePendantUneHeure[l]            = cluster.minStablePower;
			pbPalier.PminDuPalierThermiquePendantUnJour[l]              = 0; 
			pbPalier.PminDuPalierThermiquePendantUneSemaine[l]          = 0; 
			pbPalier.ClasseDeManoeuvrabilite[l]                       = (CLASSE_DE_MANOEUVRABILITE) cluster.minUpDownTime;
			
			
			 	pbPalier.CoutDeDemarrageDUnGroupeDuPalierThermique[l]			= cluster.startupCost;
				pbPalier.CoutDArretDUnGroupeDuPalierThermique[l]				= 0;
				pbPalier.CoutFixeDeMarcheDUnGroupeDuPalierThermique[l]			= cluster.fixedCost;
				pbPalier.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[l]		= cluster.minUpTime;
				pbPalier.DureeMinimaleDArretDUnGroupeDuPalierThermique[l]		= cluster.minDownTime;

			
				pbPalier.PmaxDUnGroupeDuPalierThermique[l]						= cluster.nominalCapacityWithSpinning;
				pbPalier.PminDUnGroupeDuPalierThermique[l]						= (pbPalier.PmaxDUnGroupeDuPalierThermique[l] < cluster.minStablePower) ? pbPalier.PmaxDUnGroupeDuPalierThermique[l] : cluster.minStablePower;
			

		}
		
		NombrePaliers += area.thermal.list.size();
	}

	problem.LeProblemeADejaEteInstancie = NON_ANTARES;

	
	problem.OptimisationAuPasHebdomadaire =
		(parameters.simplexOptimizationRange == Data::sorWeek) ? OUI_ANTARES : NON_ANTARES;

	
	problem.OptimisationMUTetMDT = OUI_ANTARES;
}






void SIM_InitialisationResultats()
{
	auto& study = *Data::Study::Current::Get();
	const size_t sizeOfNbHoursDouble = study.runtime->nbHoursPerYear * sizeof(double);
	const size_t sizeOfNbHoursLong   = study.runtime->nbHoursPerYear * sizeof(int);

	for (uint i = 0 ; i < study.runtime->interconnectionsCount ; ++i)
	{
		auto& interconnexion = *ResultatsParInterconnexion[i];
		memset(interconnexion.VariablesDualesMoyennes, 0, sizeOfNbHoursDouble);
		memset(interconnexion.TransitMoyen,            0, sizeOfNbHoursDouble);
		memset(interconnexion.TransitStdDev,           0, sizeOfNbHoursDouble);
		memset(interconnexion.TransitMinimumNo,        0, sizeOfNbHoursLong);
		memset(interconnexion.TransitMaximumNo,        0, sizeOfNbHoursLong);
		memset(interconnexion.RenteHoraire,            0, sizeOfNbHoursDouble);
		memset(interconnexion.TransitAnnuel,           0, study.runtime->nbYears * sizeof(double));
		for (uint j = 0 ; j < study.runtime->nbHoursPerYear ; j++)
		{
			interconnexion.TransitMinimum[j] = (double) LINFINI_ENTIER;
			interconnexion.TransitMaximum[j] = (double) -LINFINI_ENTIER;
		}
	}

	for (uint i = 0 ; i < study.runtime->bindingConstraintCount; i++)
	{
		memset(ResultatsParContrainteCouplante[i]->VariablesDualesMoyennes, 0, sizeOfNbHoursDouble);
	}
}



void SIM_RenseignementProblemeHebdo(PROBLEME_HEBDO& problem, uint numSpace, const int PasDeTempsDebut)
{
	auto& study = *Data::Study::Current::Get();
	auto& studyruntime = *study.runtime;
	const uint nbPays = study.areas.size();
	const size_t pasDeTempsSizeDouble = problem.NombreDePasDeTemps * sizeof(double);
	const size_t sizeOfIntercoDouble = sizeof(double) * studyruntime.interconnectionsCount;

	
	int indx = PasDeTempsDebut;

	
	problem.ReinitOptimisation = (study.runtime->weekInTheYear[numSpace]==0) ? OUI_ANTARES : NON_ANTARES;

	for (uint k = 0; k < studyruntime.interconnectionsCount; ++k)
	{
		
		auto* lnk = studyruntime.areaLink[k];
		
		if (lnk->useHurdlesCost)
		{
			COUTS_DE_TRANSPORT* couts = problem.CoutDeTransport[k];
			couts->IntercoGereeAvecDesCouts = OUI_ANTARES;
			const double* direct   = ((const double*)((void*)lnk->data[fhlHurdlesCostDirect])) + PasDeTempsDebut;
			const double* indirect = ((const double*)((void*)lnk->data[fhlHurdlesCostIndirect])) + PasDeTempsDebut;
			memcpy(couts->CoutDeTransportOrigineVersExtremite,    direct,   pasDeTempsSizeDouble);
			memcpy(couts->CoutDeTransportOrigineVersExtremiteRef, direct,   pasDeTempsSizeDouble);
			memcpy(couts->CoutDeTransportExtremiteVersOrigine,    indirect, pasDeTempsSizeDouble);
			memcpy(couts->CoutDeTransportExtremiteVersOrigineRef, indirect, pasDeTempsSizeDouble);
		}
		else
			problem.CoutDeTransport[k]->IntercoGereeAvecDesCouts = NON_ANTARES;

		lnk->data.flush();
	}

	
	if (studyruntime.bindingConstraintCount)
	{
		
		const uint day = study.calendar.hours[PasDeTempsDebut].dayYear;
		
		for (uint k = 0; k != studyruntime.bindingConstraintCount; ++k)
		{
			
			auto& bc = studyruntime.bindingConstraint[k];
			switch (bc.type)
			{
				case BindingConstraint::typeHourly:
					{
						
						break;
					}
				case BindingConstraint::typeDaily:
					{
						assert(bc.bounds.width && "Invalid constraint data width");
						assert(day + 6 < bc.bounds.height && "Invalid constraint data height");
						auto& column = bc.bounds[0];
						double* sndMember = problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante;
						double* sndMemberRef = problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef;
						for (uint d = 0; d != 7; ++d)
						{
							sndMember[d]    = column[day + d];
							sndMemberRef[d] = sndMember[d];
						}
						break;
					}
				case BindingConstraint::typeWeekly:
					{
						assert(bc.bounds.width && "Invalid constraint data width");
						assert(day + 6 < bc.bounds.height && "Invalid constraint data height");
						const Matrix<>::ColumnType& column = bc.bounds[0];
						double sum = 0;
						for (uint d = 0; d != 7; ++d)
							sum += column[day + d];

						problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante[0]    = sum;
						problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef[0] = sum;
						break;
					}
				case BindingConstraint::typeUnknown:
				case BindingConstraint::typeMax:
					{
						assert(false && "invalid constraint type");
						logs.error() << "internal error. Please submit a full bug report";
						break;
					}
			}
			bc.bounds.flush();
		}
	}


	
	double PuissanceMinDuPalierThermiqueDeReference = 0;

	for (int j = 0; j < problem.NombreDePasDeTemps; ++j, ++indx)
	{
		VALEURS_DE_NTC_ET_RESISTANCES* ntc = problem.ValeursDeNTC[j];
		assert(NULL != ntc);

		
		{
			uint linkCount = studyruntime.interconnectionsCount;
			for (uint k = 0; k != linkCount; ++k)
			{
				auto& lnk = *(studyruntime.areaLink[k]);
				assert((uint)indx < lnk.data.height);
				assert((uint) fhlNTCDirect < lnk.data.width);
				assert((uint) fhlNTCIndirect < lnk.data.width);
				ntc->ValeurDeNTCOrigineVersExtremite[k] = lnk.data[fhlNTCDirect][indx];
				ntc->ValeurDeNTCExtremiteVersOrigine[k] = lnk.data[fhlNTCIndirect][indx];
			}
		}

		
		memcpy( (char *) problem.ValeursDeNTCRef[j]->ValeurDeNTCOrigineVersExtremite,
				(char *) ntc->ValeurDeNTCOrigineVersExtremite,
				sizeOfIntercoDouble);
		memcpy( (char *) problem.ValeursDeNTCRef[j]->ValeurDeNTCExtremiteVersOrigine,
				(char *) ntc->ValeurDeNTCExtremiteVersOrigine,
				sizeOfIntercoDouble);

		
		
		
		
		
		
		{
			const uint constraintCount = studyruntime.bindingConstraintCount;
			for (uint k = 0; k != constraintCount; ++k)
			{
				auto& bc = studyruntime.bindingConstraint[k];
				
				if (bc.type == BindingConstraint::typeHourly)
				{
					auto& column = bc.bounds[0];
					problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante[j] =
						column[PasDeTempsDebut + j];
					problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef[j] =
						problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante[j];
				}
			}
		}

		
		const uint dayInTheYear = study.calendar.hours[indx].dayYear;

		for (uint k = 0; k < nbPays; ++k)
		{
			auto& tsIndex     = *NumeroChroniquesTireesParPays[numSpace][k];
			auto& area        = *(study.areas.byIndex[k]);
			auto& scratchpad  = *(area.scratchpad[numSpace]);
			auto& ror         = area.hydro.series->ror;

			
			assert(&scratchpad);
			assert((uint) indx < scratchpad.ts.load.height);
			assert((uint) indx < scratchpad.ts.solar.height);
			assert((uint) indx < scratchpad.ts.wind.height);
			assert((uint) tsIndex.Consommation < scratchpad.ts.load.width);
			assert((uint) tsIndex.Eolien < scratchpad.ts.wind.width);
			assert((uint) tsIndex.Solar < scratchpad.ts.solar.width);

			
			uint tsFatalIndex = (uint) tsIndex.Hydraulique < ror.width ? tsIndex.Hydraulique : 0;

			
			problem.ConsommationsAbattues[j]->ConsommationAbattueDuPays[k] =
				+ scratchpad.ts.load  [tsIndex.Consommation][indx]
				- scratchpad.ts.wind  [tsIndex.Eolien][indx]
				- scratchpad.ts.solar [tsIndex.Solar][indx]
				- scratchpad.miscGenSum[indx]
				- ror[tsFatalIndex][indx]
				- scratchpad.mustrunSum[indx];

			
			assert(!Math::NaN(problem.ConsommationsAbattues[j]->ConsommationAbattueDuPays[k])
				&& "NaN detected for 'ConsommationAbattue', probably from miscGenSum/mustrunSum");

			
			area.thermal.list.each([&] (const Data::ThermalCluster& cluster)
			{		
				assert((uint) tsIndex.ThermiqueParPalier[cluster.areaWideIndex] < cluster.series->series.width);
				assert((uint) indx < cluster.series->series.height);
				assert(cluster.series->series.jit == NULL && "No JIT data from the solver");

				auto& Pt = *problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[cluster.index];
				auto& PtValGen = *ValeursGenereesParPays[numSpace][k];
				
				
				
				Pt.PuissanceDisponibleDuPalierThermique[j]     = cluster.series->series[tsIndex.ThermiqueParPalier[cluster.areaWideIndex]] [indx];

				# ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
					if (Pt.PuissanceDisponibleDuPalierThermique[j] == 0.)
					{
						Pt.CoutHoraireDeProductionDuPalierThermique[j] = studyruntime.globalMaximumCost;
					}
					else
						Pt.CoutHoraireDeProductionDuPalierThermique[j] = cluster.marketBidCost * cluster.modulation[thermalModulationMarketBid][indx] + PtValGen.AleaCoutDeProductionParPalier[cluster.areaWideIndex];
				# else
					Pt.CoutHoraireDeProductionDuPalierThermique[j] = cluster.marketBidCost * cluster.modulation[thermalModulationMarketBid][indx] + PtValGen.AleaCoutDeProductionParPalier[cluster.areaWideIndex];
				# endif
				
				
				Pt.PuissanceMinDuPalierThermique[j] = (Pt.PuissanceDisponibleDuPalierThermique[j] < cluster.PthetaInf[indx]) ? Pt.PuissanceDisponibleDuPalierThermique[j] : cluster.PthetaInf[indx];
				
				});

			
			if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
			{
				problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire[j] =
					scratchpad.optimalMaxPower[dayInTheYear];
			}
			


			problem.ReserveJMoins1[k]->ReserveHoraireJMoins1[j] = area.reserves[fhrDayBefore][PasDeTempsDebut + j];
		}
	} 


	
	
	{
		# ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
		double sum;
		for (uint k = 0 ; k < nbPays; ++k)
		{
			studyruntime.hydroCostByAreaShouldBeInfinite[k] = false;
			if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
			{
				sum = 0;
				for (int j = 0; j < 7; ++j)
				{
					uint dayYear = study.calendar.hours[PasDeTempsDebut + j *24].dayYear;
					problem.CaracteristiquesHydrauliques[k]->ContrainteDEnergieHydrauliqueParJour[j] =
						ValeursGenereesParPays[numSpace][k]->HydrauliqueModulableQuotidien[dayYear];
					sum += problem.CaracteristiquesHydrauliques[k]->ContrainteDEnergieHydrauliqueParJour[j];
				}
				if (sum < DBL_EPSILON)
					studyruntime.hydroCostByAreaShouldBeInfinite[k] = true;
			}
		}
		# else
		for (uint k = 0 ; k < nbPays; ++k)
		{
			if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
			{
				for (uint j = 0; j < 7; ++j)
				{
					
					uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

					problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise[j] =
						ValeursGenereesParPays[numSpace][k]->HydrauliqueModulableQuotidien[day];
				}
			}
		}
		# endif
	}

	
	for (uint k = 0; k < nbPays; ++k)
	{
		auto& area = *study.areas.byIndex[k];

		for (uint l = 0; l != area.thermal.list.size(); ++l)
		{
			memcpy( (char *) problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[l]->PuissanceDisponibleDuPalierThermiqueRef,
					(char *) problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[l]->PuissanceDisponibleDuPalierThermique,
					pasDeTempsSizeDouble);
			memcpy( (char *) problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[l]->CoutHoraireDeProductionDuPalierThermiqueRef,
					(char *) problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[l]->CoutHoraireDeProductionDuPalierThermique,
					pasDeTempsSizeDouble);
		}
		
		memcpy( (char *) problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef,
				(char *) problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise,
				7 * sizeof( double ) );
		memcpy( (char *) problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef,
				(char *) problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire,
				pasDeTempsSizeDouble);
		




		memcpy( (char *) problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref,
				(char *) problem.ReserveJMoins1[k]->ReserveHoraireJMoins1,
				pasDeTempsSizeDouble);
	}
	for (int j = 0; j < problem.NombreDePasDeTemps; ++j)
	{
		memcpy( (char *) problem.ConsommationsAbattuesRef[j]->ConsommationAbattueDuPays,
				(char *) problem.ConsommationsAbattues[j]->ConsommationAbattueDuPays,
				nbPays * sizeof( double ));
	}


	# ifdef ANTARES_SWAP_SUPPORT
	
	
	Antares::memory.flushAll();
	# endif
}



