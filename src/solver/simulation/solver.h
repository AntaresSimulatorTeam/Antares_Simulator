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
#ifndef __SOLVER_SIMULATION_SOLVER_H__
# define __SOLVER_SIMULATION_SOLVER_H__

# include <vector>
# include <yuni/yuni.h>
# include <antares/study/memory-usage.h>
# include <antares/study.h>
# include <antares/logs.h>
# include <yuni/core/string.h>
# include "../variable/state.h"
# include "../misc/options.h"
# include "solver.data.h"
# include "../hydro/management/management.h"

# include "../../libs/antares/study/fwd.h"	// Added for definition of type PowerFluctuations


namespace Antares
{
namespace Solver
{
namespace Simulation
{

	struct setOfParallelYears;
	class yearRandomNumbers;
	class randomNumbers;
	template<class Impl> class yearJob;

	template<class Impl>
	class ISimulation : public Impl
	{
		friend class yearJob<Impl>;
	public:
		//! The real implementation for the simulation
		typedef Impl ImplementationType;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor (with a given study)
		*/
		ISimulation(Data::Study& study, const ::Settings& settings);
		//! Destructor
		~ISimulation();
		//@}


		/*!
		** \brief Run the simulation
		*/
		void run();

		/*!
		** \brief Export the results to disk
		**
		** \tparam ResultsForYearByYear False to write down the rglobal esults of the simulation,
		**   true for the results of the current year (year-by-year mode)
		** \param year The current year, if applicable
		*/
		void writeResults(bool synthesis, uint year = 0, uint numSpace = 9999);

		static void estimateMemoryUsage(Antares::Data::StudyMemoryUsage& u);
		static void estimateMemoryForRandomNumbers(Antares::Data::StudyMemoryUsage& u);
		static void estimateMemoryForWeeklyPb(Antares::Data::StudyMemoryUsage& u);
		static void estimateMemoryForOptimizationPb(Antares::Data::StudyMemoryUsage& u, int & nbVars, int & nbConstraints);
		static void estimateMemoryForSplxPb(Antares::Data::StudyMemoryUsage& u, int & nbVars, int & nbConstraints);

	public:
		//! Reference to the current study
		Data::Study& study;
		//! The global settings
		const ::Settings& settings;


	private:
		/*!
		** \brief Regenerate time-series if required for a given year
		*/
		template<bool PreproOnly> void regenerateTimeSeries(uint year);

		/*!
		** \brief Builds sets of parallel years
		**
		** \return The max number of years in a set of parallel years (to be executed or not)
		*/
		template<bool PerformCalculationsT>
		uint buildSetsOfParallelYears(	uint firstYear,
										uint endYear, 
										std::vector<setOfParallelYears> & setsOfParallelYears
										);

		/*!
		** \brief Allocate storage for random numbers of parallel years
		**
		** \param	randomParallelYears	... to be finished ...
		*/		
		void allocateMemoryForRandomNumbers(randomNumbers & randomForParallelYears);

		/*!
		** \brief Computes random numbers for each years of a list
		**
		** \param	randomForYears	Storage for random numbers for years in the list
		** \param	years			List of years	
		*/
		void computeRandomNumbers(	randomNumbers & randomForYears,
									std::vector<uint> & years,
									std::map<unsigned int, bool> & isYearPerformed	);

		/*!
		** \brief Iterate through all MC years
		**
		** \param firstYear The first real MC year
		** \param endYear   The last MC year
		*/
		template<bool PerformCalculationsT>
		void loopThroughYears(uint firstYear, uint endYear, std::vector<Variable::State> & state);


	private:
		//! Some temporary to avoid performing useless complex checks
		Solver::Private::Simulation::CacheData pData;
		//!
		uint pNbYearsReallyPerformed;
		//! Max number of years performed in parallel
		uint pNbMaxPerformedYearsInParallel;
		//! Year by year output results
		bool pYearByYear;
		//! Hydro management
		HydroManagement pHydroManagement;

	}; // class ISimulation


	struct setOfParallelYears
	{
		/*
			Un lot d'année à exécuter en parallèle.
			En fonction d'une éventuelle play-list, certaines seront jouées et d'autres non.
		*/
		public:
			// Numeros des annees en parallele pour ce lot (certaines ne seront pas jouées en cas de play-list "trouée")
			std::vector<unsigned int> yearsIndices;

			// Une annee doit-elle être rejouée ?
			std::map<uint, bool> yearFailed;

			// Associe le numero d'une année jouée à l'indice de l'espace
			std::map<unsigned int, unsigned int> performedYearToSpace;

			// L'inverse : pour une année jouée, associe l'indice de l'espace au numero de l'année
			std::map<unsigned int, unsigned int> spaceToPerformedYear;

			// Pour chaque année, est-elle la première à devoir être jouée dans son lot d'années ?
			std::map<unsigned int, bool> isFirstPerformedYearOfASet;

			// Pour chaque année du lot, est-elle jouée ou non ? 
			std::map<unsigned int, bool> isYearPerformed;

			// Nbre d'années en parallele vraiment jouées pour ce lot
			unsigned int nbPerformedYears;

			// Nbre d'années en parallele jouées ou non pour ce lot
			unsigned int nbYears;

			// Regenere-t-on des times series avant de jouer les annees du lot courant
			bool regenerateTS;

			// Annee a passer a la fonction "regenerateTimeSeries<false>(y /* year */)" (si regenerateTS is "true")
			unsigned int yearForTSgeneration;

	}; 

	
	class yearRandomNumbers
	{
		public:
			yearRandomNumbers() 
			{
				pThermalNoisesByArea = nullptr;
				pNbClustersByArea = nullptr;
				pNbAreas = 0;
			}

			~yearRandomNumbers() 
			{
				// General
				delete[] pNbClustersByArea;
				
				// Thermal noises 
				for(uint a = 0; a != pNbAreas; a++)
					delete[] pThermalNoisesByArea[a];
				delete[] pThermalNoisesByArea;
				
				// Reservoir levels and unsupplied energy
				delete[] pReservoirLevels;
				delete[] pUnsuppliedEnergy;

				// Hydro costs noises
				switch(pPowerFluctuations)
				{
					case Data::lssFreeModulations:
					{
						for(uint a = 0; a != pNbAreas; a++)
							delete[] pHydroCostsByArea_freeMod[a];
						delete[] pHydroCostsByArea_freeMod;
						break;
					}

					case Data::lssMinimizeRamping:
					case Data::lssMinimizeExcursions:
					{
						delete[] pHydroCosts_rampingOrExcursion;
						break;
					}
				}
			}

			void setNbAreas(uint nbAreas) { pNbAreas = nbAreas; }

			void setPowerFluctuations(Data::PowerFluctuations powerFluctuations)
			{ pPowerFluctuations = powerFluctuations;}

			void reset()
			{
				// General
				memset(pNbClustersByArea, 0, pNbAreas * sizeof(size_t));
				
				// Thermal noises
				for(uint a = 0; a != pNbAreas; a++)
					memset(pThermalNoisesByArea[a], 0, pNbClustersByArea[a] * sizeof(double));

				// Reservoir levels and unsupplied energy
				memset(pReservoirLevels, 0, pNbAreas * sizeof(double));
				memset(pUnsuppliedEnergy, 0, pNbAreas * sizeof(double));

				// Hydro costs noises
				switch(pPowerFluctuations)
				{
					case Data::lssFreeModulations:
					{
						for(uint a = 0; a != pNbAreas; a++)
							memset(pHydroCostsByArea_freeMod[a], 0, 8784 * sizeof(double));
						break;
					}

					case Data::lssMinimizeRamping:
					case Data::lssMinimizeExcursions:
					{
						memset(pHydroCosts_rampingOrExcursion, 0, pNbAreas * sizeof(double));
						break;
					}
				}
			}

		public:
			// General data
			uint pNbAreas;
			Data::PowerFluctuations pPowerFluctuations;

			// Data for thermal noises
			double ** pThermalNoisesByArea;
			size_t * pNbClustersByArea;

			// Data for reservoir levels
			double * pReservoirLevels;

			// Data for unsupplied energy
			double * pUnsuppliedEnergy;

			// Hydro costs noises
			double ** pHydroCostsByArea_freeMod;
			double *  pHydroCosts_rampingOrExcursion;
	};
	
	class randomNumbers
	{
		public:
			randomNumbers(uint maxNbPerformedYearsInAset, Data::PowerFluctuations powerFluctuations) 
				: pMaxNbPerformedYears(maxNbPerformedYearsInAset)
			{ 
				// Allocate a table of parallel years structures
				pYears = new yearRandomNumbers[maxNbPerformedYearsInAset];

				// Tells these structures their power fluctuations mode 
				for(uint y = 0; y < maxNbPerformedYearsInAset; ++y)
					pYears[y].setPowerFluctuations(powerFluctuations);
			}

			~randomNumbers() { delete[] pYears; }

			void reset()
			{
				for(uint i = 0; i < pMaxNbPerformedYears; i++)	pYears[i].reset();

				yearNumberToIndex.clear();

			}

		public:
			uint pMaxNbPerformedYears;
			yearRandomNumbers * pYears;

			// Associates : 
			//		year number (0, ..., total nb of years to compute - 1) --> index of the year's space (0, ..., max nb of parallel years - 1)
			std::map<uint,uint> yearNumberToIndex;

	};
	



	/*!
	** \brief Re-Generate the time-series numbers
	*/
	void GenerateTimeseriesNumbers(Data::Study& study);



} // namespace Simulation
} // namespace Solver
} // namespace Antares

# include "solver.hxx"

#endif // __SOLVER_SIMULATION_SOLVER_H__
