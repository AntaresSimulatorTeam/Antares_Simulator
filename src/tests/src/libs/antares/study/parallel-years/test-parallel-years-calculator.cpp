#define BOOST_TEST_MODULE test-end-to-end tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <antares/study/study.h>
#include <antares/study/parallel-years.h>

#include <iostream>
#include <stdio.h>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

using namespace Yuni;
using namespace Antares::Data;


uint nbLogicalCores_{2};

namespace Yuni
{
namespace System
{
namespace CPU
{
    uint Count()
    {
        // The number of processors online (capable of running processes)
        return nbLogicalCores_;
    }

}
}
}


struct Fixture
{
	Fixture(const Fixture& f) = delete;
	Fixture(const Fixture&& f) = delete;
	Fixture& operator= (const Fixture& f) = delete;
	Fixture& operator= (const Fixture&& f) = delete;
	Fixture() 
	{
        // using default params
        params.reset();
	}

	~Fixture(){}

    //creating a study, maybe not needed
    Study::Ptr study = std::make_shared<Study>();
    Parameters params{};
    Data::StudyLoadOptions options;
	
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(default_params_no_force)
{
    bool thermalTSRefresh = false;
    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);
    
    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 1);
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 1);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 1);
}

BOOST_AUTO_TEST_CASE(hundred_years_no_force)
{
    bool thermalTSRefresh = false;
    params.nbYears = 100;
    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);
    
    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 1);
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 1);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 100);
}

BOOST_AUTO_TEST_CASE(four_mc_years_force_parallel)
{

    bool thermalTSRefresh = false;
    options.forceParallel = true;
    options.enableParallel = true;
    options.maxNbYearsInParallel = 2;
    params.nbYears = 4;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 2);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 2);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 4);
}

BOOST_AUTO_TEST_CASE(hundred_mc_years_force_parallel_sets_size_five_thermal_refresh_on)
{

    bool thermalTSRefresh = true;
    options.forceParallel = true;
    options.enableParallel = true;
    options.maxNbYearsInParallel = 5;
    params.nbYears = 100;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 5);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 5);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 100);
}


BOOST_AUTO_TEST_CASE(hundred_mc_years_no_force_thermal_refresh_on)
{

    bool thermalTSRefresh = true;
    options.forceParallel = false;
    options.enableParallel = false;
    params.nbYears = 100;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 1);
    // BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 1);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 100);
}

BOOST_AUTO_TEST_CASE(hundred_mc_years_no_force_enable_parallel)
{

    bool thermalTSRefresh = false;
    options.forceParallel = false;
    options.enableParallel = true;
    
    // Shouldn't have any effect since force parallel is off
    options.maxNbYearsInParallel = 10;

    params.nbYears = 100;

    nbLogicalCores_ = 16;
    params.nbCores.ncMode = Antares::Data::NumberOfCoresMode::ncHigh;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 12);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 4);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 100);
}

BOOST_AUTO_TEST_CASE(hundred_mc_years_no_force_enable_parallel_thermal_refresh_on)
{

    bool thermalTSRefresh = true;
    options.forceParallel = false;
    options.enableParallel = true;
    
    // Shouldn't have any effect since force parallel is off
    options.maxNbYearsInParallel = 10;

    params.nbYears = 100;

    nbLogicalCores_ = 8;
    params.nbCores.ncMode = Antares::Data::NumberOfCoresMode::ncAvg;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 4);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 4);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 100);
}


BOOST_AUTO_TEST_CASE(hundred_mc_years_force_10_parallel_user_playlist_thermal_refresh_on)
{

    bool thermalTSRefresh = true;
    options.forceParallel = true;
    options.enableParallel = true;
    
    options.maxNbYearsInParallel = 10;

    params.nbYears = 100;

    // Create playlist
    params.userPlaylist = true;
    params.yearsFilter.reserve(params.nbYears);
    for (uint i = 0; i != params.nbYears; ++i)
        params.yearsFilter[i] = false;
    params.yearsFilter[2] = true;
    params.resetYearsWeigth();
    params.setYearWeight(0,4);
    params.setYearWeight(1,10);
    params.setYearWeight(2,3);

    // Override number of raw cores
    nbLogicalCores_ = 4;
    params.nbCores.ncMode = Antares::Data::NumberOfCoresMode::ncLow;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 1);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 1);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 1);
}

BOOST_AUTO_TEST_CASE(hundred_mc_years_enable_parallel_user_playlist_thermal_refresh_on)
{

    bool thermalTSRefresh = true;
    options.forceParallel = false;
    options.enableParallel = true;
    
    // Shouldn't have any effect since force parallel is off
    options.maxNbYearsInParallel = 10;

    params.nbYears = 100;

    // Create playlist
    params.userPlaylist = true;
    params.yearsFilter.reserve(params.nbYears);
    for (uint i = 0; i != params.nbYears; ++i)
    {
        if(i%2 == 0)
            params.yearsFilter[i] = true;
        else
            params.yearsFilter[i] = false;
    }
        
    params.resetYearsWeigth();
    params.setYearWeight(0,4);
    params.setYearWeight(1,10);
    params.setYearWeight(2,3);

    // Override number of raw cores
    nbLogicalCores_ = 6;
    params.nbCores.ncMode = Antares::Data::NumberOfCoresMode::ncHigh;

    SetsOfParallelYearCalculator builder(options.forceParallel, 
                                         options.enableParallel, 
                                         options.maxNbYearsInParallel,
                                         thermalTSRefresh,
                                         params);

    BOOST_CHECK_EQUAL(builder.getForcedNbOfParallelYears(), 4);
    BOOST_CHECK(builder.allSetsParallelYearsHaveSameSize());
    BOOST_CHECK_EQUAL(builder.getMinNbParallelYearsForGUI(), 2);
    BOOST_CHECK_EQUAL(builder.getNbYearsReallyPerformed(), 50);
}

BOOST_AUTO_TEST_SUITE_END()