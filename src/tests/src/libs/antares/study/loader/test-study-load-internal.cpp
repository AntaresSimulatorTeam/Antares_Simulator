#define BOOST_TEST_MODULE test study load internal

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>
 	
#include <fstream>
#include <string>
#include <filesystem>

#include <fwd.h>
#include <study.h>
#include <area/constants.h>
#include <wrappers/study-wrapper.h>
// #include "files-generator.h"

#include <yuni/io/file.h>

using namespace Antares::Data;


Area* addArea(Study::Ptr pStudy, const std::string& areaName, int nbTS)
{
	Area* pArea = pStudy->areaAdd(areaName);

	BOOST_CHECK(pArea != NULL);

	//Need to add unsupplied energy cost constraint so load is respected
	pArea->thermal.unsuppliedEnergyCost = 10000.0;
	pArea->spreadUnsuppliedEnergyCost	= 0.01;

	//Define default load
	pArea->load.series->series.resize(nbTS, HOURS_PER_YEAR);
	pArea->load.series->series.fill(0.0);

	return pArea;
}

struct Fixture
{
	Fixture(const Fixture& f) = delete;
	Fixture(const Fixture&& f) = delete;
	Fixture& operator= (const Fixture& f) = delete;
	Fixture& operator= (const Fixture&& f) = delete;
	Fixture()
	{
		//Define study parameters
		pStudy->parameters.reset();
		pStudy->parameters.years(1);

		//Prepare parameters for simulation
		pStudy->parameters.prepareForSimulation(options);
	}

	Data::StudyLoadOptions options;
    Study::Ptr pStudy = std::make_shared<Study>(true);
	StudyWrapper::Ptr pStudyWrapper = std::make_shared<StudyWrapper>(pStudy);

};


BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(test_init_internal_data_not_latest_version)
{
	pStudy->header.version = version820;
	pStudy->parameters.timeSeriesToImport = true;
	BOOST_CHECK(!pStudyWrapper->initializeInternalData(options));
}

BOOST_AUTO_TEST_CASE(test_init_internal_data_latest_version)
{
	pStudy->header.version = versionLatest;
	pStudy->parameters.timeSeriesToImport = true;
	BOOST_CHECK(pStudyWrapper->initializeInternalData(options));
}

BOOST_AUTO_TEST_CASE(test_init_internal_data_hot_start_bad_nb_parallel_years)
{
	pStudy->parameters.initialReservoirLevels.iniLevels = irlHotStart;
	pStudy->maxNbYearsInParallel = 2;
	pStudy->parameters.allSetsHaveSameSize = false;
	BOOST_CHECK(!pStudyWrapper->initializeInternalData(options));
}

BOOST_AUTO_TEST_CASE(test_init_internal_data_hot_start_not_whole_year)
{
	pStudy->parameters.initialReservoirLevels.iniLevels = irlHotStart;
	pStudy->parameters.simulationDays.end = 300;
	BOOST_CHECK(!pStudyWrapper->initializeInternalData(options));
}

BOOST_AUTO_TEST_CASE(test_init_internal_data_hot_start_bad_first_day)
{
	int nbTS = 2;
	Area* pArea = addArea(pStudy, "Area 1", nbTS);
	pStudy->parameters.initialReservoirLevels.iniLevels = irlHotStart;
	pArea->hydro.initializeReservoirLevelDate = 11;

	pArea->hydro.reservoirManagement = true;
	
	BOOST_CHECK(!pStudyWrapper->initializeInternalData(options));
}

BOOST_AUTO_TEST_SUITE_END()
