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
#ifndef __MAIN_H__
# define __MAIN_H__

# include "misc/options.h"
# include <antares/study.h>
# include <antares/study/load-options.h>
# include "simulation/simulation.h"
# include "simulation/sim_extern_variables_globales.h"
# include "aleatoire/alea_fonctions.h"
# include "optimisation/opt_fonctions.h"


#define SOLVER_ERR_LICENSE	1
#define SOLVER_ERR_OTHER	2
#define SOLVER_CHECKLICENCE_OK	3


/*!
** \brief Common variables used in the main loops
*/
# define COMMON_VARIABLES_SIMULATION \
		const int haveToRefreshTSHydro = (pRuntimeInfos->parameters->timeSeriesToRefresh & timeSeriesHydro); \
		const int haveToRefreshTSWind = (pRuntimeInfos->parameters->timeSeriesToRefresh & timeSeriesWind); \
		const int haveToRefreshTSThermal = (pRuntimeInfos->parameters->timeSeriesToRefresh & timeSeriesThermal)
		//const int haveToRefreshTSLoad = (pRuntimeInfos->parameters->timeSeriesToRefresh & timeSeriesLoad)


/*!
** \brief Re-generate time-series if needed
**
** From time to time, the time-series must be regenerated (often
** every 500 years).
**
** \warning The order _must_ remain
**
** \param IT The current monte-carlo year to process
*/
# define REGENERATE_TIMESERIES(IT) \
		do { \
		if (haveToRefreshTSHydro && (IT % pRuntimeInfos->parameters->refreshIntervalHydro == 0)) \
			PreproGenerateTimeSeriesHydro(IT); \
		if (haveToRefreshTSWind && (IT % pRuntimeInfos->parameters->refreshIntervalWind == 0)) \
			PreproGenerateTimeSeriesWind(IT); \
		if (haveToRefreshTSThermal && (IT % pRuntimeInfos->parameters->refreshIntervalThermal == 0)) \
			PreproGenerateTimeSeriesThermal(IT); \
		} while (0)





class SolverApplication :
	public Yuni::IEventObserver<SolverApplication, Yuni::Policy::SingleThreaded>
{
public:
	//! \name Constructor & Destructor
	//@{
	/*!
	** \brief Default Constructor
	*/
	SolverApplication();
	/*!
	** \brief Destructor
	*/
	~SolverApplication();
	//@}

	/*!
	** \brief Initialize data before launching the simulation
	**
	** \return False if the operation failed.
	*/
	bool prepare(int argc, char* argv[]);

	/*!
	** \brief Execute the simulation
	**
	** \return Exit status
	*/
	int execute();

	/*!
	** \brief Reset the process priority (Windows only)
	*/
	void resetProcessPriority();

private:
	/*!
	** \brief Reset the log filename and open it
	*/
	void resetLogFilename();

	/*!
	* \brief Load data of the study from a local or remote folder
	*/
	bool readDataForTheStudy(Antares::Data::StudyLoadOptions& options);

	void runSimulationInAdequacyMode();
	void runSimulationInAdequacyDraftMode();
	void runSimulationInEconomicMode();

	void initializeRandomNumberGenerators();

	void onLogMessage(int level, const YString& message);

	/*!
	** \brief Run an arbitrary simulation
	**
	** see main/{adequacy.cpp, economy.cpp, adequacy-draft.cpp} for specific
	** instanciation
	*/
	template<class SimulationT> void runSimulation();

	void processCaption(const AnyString& caption);

	/*!
	* \brief check the proxy with the commend line entries
	*/
	bool completeWithOnlineCheck();

private:
	//! The settings given from the command line
	Settings pSettings;
	//! The current Antares study
	Antares::Data::Study::Ptr pStudy;
	//! General data related to the current study
	Antares::Data::Parameters* pParameters;
	//! The total number of errors which have been generated
	uint pErrorCount;
	//! The total muber of warnings which have been generated
	uint pWarningCount;

	int pArgc;
	char** pArgv;

}; // class SolverApplication





# include "main.hxx"

#endif // __MAIN_H__
