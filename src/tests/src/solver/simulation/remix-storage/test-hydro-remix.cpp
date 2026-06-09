#define BOOST_TEST_MODULE hydro remix

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"
#include "antares/utils/vector-utils.h"

using namespace Antares::Utils;
using namespace Antares::Solver::Simulation;

template<unsigned int size>
struct InputFixture
{
    InputFixture()
    {
        TotalGenNoHydro.assign(size, 0.);
        Load.assign(size, 0.);
        HydroGen.assign(size, 0.);
        UnsupE.assign(size, 0.);
        levels.assign(size, 0.);
        pmax.assign(size, std::numeric_limits<double>::max());
        pmin.assign(size, 0.);
        inflows.assign(size, 0.);
        ovf.assign(size, 0.);
        pump.assign(size, 0.);
        Spillage.assign(size, 0.);
        DTG_MRG.assign(size, 0.);
    }

    std::shared_ptr<IStorageForRemix> createHydroForRemix()
    {
        return makeHydroForRemix(HydroGen,
                                 UnsupE,
                                 levels,
                                 pmax,
                                 pmin,
                                 inflows,
                                 ovf,
                                 pump,
                                 init_level,
                                 capacity,
                                 pumpEff,
                                 reservoirManagement,
                                 "hydro");
    }

    void callRemixStorageAlgorithm()
    {
        Load = TotalGenNoHydro + UnsupE + HydroGen;
        storagesForRemix.clear();
        storagesForRemix.push_back(createHydroForRemix());
        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    std::vector<double> TotalGenNoHydro, Load, HydroGen, UnsupE, levels, pmax, pmin, inflows, ovf,
      pump, Spillage, DTG_MRG;
    ListStorageForRemix storagesForRemix;
    double init_level = 0.;
    double capacity = std::numeric_limits<double>::max();
    const double pumpEff = 1.;
    const bool reservoirManagement = true;

    std::string err_msg;
};

BOOST_FIXTURE_TEST_CASE(input_vectors_of_different_sizes__exception_raised, InputFixture<1>)
{
    HydroGen = {0., 0.};
    err_msg = "Remix hydro input : arrays of different sizes";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_init_level_exceeds_capacity__exception_raised, InputFixture<1>)
{
    init_level = 2.;
    capacity = 1.;
    err_msg = "Remix hydro input : initial level > reservoir capacity";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(all_input_arrays_of_size_0__exception_raised, InputFixture<0>)
{
    init_level = 0.;
    capacity = 1.;
    err_msg = "Remix hydro input : all arrays of sizes 0";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_not_smaller_than_pmax__exception_raised, InputFixture<5>)
{
    HydroGen = {1., 2., 3., 4., 5.};
    pmax = {2., 2., 2., 4., 5.};
    init_level = 0.;
    capacity = 1.;
    err_msg = "Remix hydro input : Storage withdrawal not smaller than Pmax everywhere";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_not_greater_than_pmin__exception_raised, InputFixture<5>)
{
    HydroGen = {1., 2., 3., 4., 5.};
    pmin = {0., 0., 4., 0., 0.};
    init_level = 0.;
    capacity = 1.;
    err_msg = "Remix hydro input : Storage withdrawal not greater than Pmin everywhere";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_is_acceptable__no_exception_raised, InputFixture<1>)
{
    init_level = 0.;
    capacity = 1.;
    Load = TotalGenNoHydro + UnsupE + HydroGen;

    BOOST_CHECK_NO_THROW(storagesForRemix.push_back(createHydroForRemix()));
    BOOST_CHECK_NO_THROW(
      shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix));
}

BOOST_FIXTURE_TEST_CASE(
  hydro_increases_and_pmax_40mwh___Hydro_gen_is_flattened_to_mean_Hydro_gen_20mwh,
  InputFixture<5>)
{
    std::ranges::fill(pmax, 40.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    HydroGen = {0., 10., 20., 30., 40.}; // we have Pmin <= HydroGen <= Pmax
    UnsupE = {80., 60., 40., 20., 0.};
    init_level = 500.;
    capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains flat
    std::vector<double> expected_UnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(HydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(same_test_as_above___we_just_raise_pmax___same_results, InputFixture<5>)
{
    std::ranges::fill(pmax, 50.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    HydroGen = {0., 10., 20., 30., 40.};
    UnsupE = {80., 60., 40., 20., 0.};
    init_level = 500.;
    capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains constant at each hour
    std::vector<double> expected_UnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(HydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(
  hydro_decreases_and_pmax_40mwh___Hydro_gen_is_flattened_to_mean_Hydro_gen_20mwh,
  InputFixture<5>)
{
    std::ranges::fill(pmax, 40.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    HydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    init_level = 500.;
    capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains constant at each hour
    std::vector<double> expected_UnsupE = {20., 30., 40., 50., 60.};
    BOOST_CHECK(HydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmax, InputFixture<5>, *boost::unit_test::tolerance(0.01))
{
    // TotalGenNoHydro decreases
    TotalGenNoHydro = {100., 80., 60., 40., 20.};

    // HydroGen is flat and must respect HydroGen <= Pmax everywhere
    HydroGen = {20., 20., 20., 20., 20.};
    UnsupE = {50., 50., 50., 50., 50.};
    init_level = 500.;
    capacity = 1000.;

    // 1. Algorithm tends to flatten TotalGenNoHydro + HydroGen, so it would require HydroGen to
    // increase. Proof :
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_1 = {0., 0., 13.33, 33.33, 53.33};
    BOOST_TEST(HydroGen == expected_HydroGen_1, boost::test_tools::per_element());

    // 2. But HydroGen is limited by pmax. So Algo does nothing in the end.
    // Proof :
    pmax = {20., 20., 20., 20., 20.};
    HydroGen = {20., 20., 20., 20., 20.}; // Reset hydro generation
    UnsupE = {50., 50., 50., 50., 50.};   // Reset unsupplied energy

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_2 = {20., 20., 20., 20., 20.};
    std::vector<double> expected_UnsupE_2 = {50., 50., 50., 50., 50.};
    BOOST_CHECK(HydroGen == expected_HydroGen_2);
    BOOST_CHECK(UnsupE == expected_UnsupE_2);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmin, InputFixture<5>, *boost::unit_test::tolerance(0.01))
{
    // TotalGenNoHydro decreases
    TotalGenNoHydro = {100., 80., 60., 40., 20.};

    // HydroGen is flat and must respect  Pmin <= HydroGen <= Pmax everywhere
    HydroGen = {20., 20., 20., 20., 20.};
    UnsupE = {50., 50., 50., 50., 50.};
    init_level = 500.;
    capacity = 1000.;

    // 1. Algorithm tends to flatten TotalGenNoHydro + HydroGen, so it would require HydroGen to
    // increase.
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_1 = {0., 0., 13.33, 33.33, 53.33};
    BOOST_TEST(HydroGen == expected_HydroGen_1, boost::test_tools::per_element());

    // 2. But HydroGen is low bounded by pmin. So Algo does nothing in the end.
    pmin = {20., 20., 20., 20., 20.};
    HydroGen = {20., 20., 20., 20., 20.}; // Reset hydro generation
    UnsupE = {50., 50., 50., 50., 50.};   // Reset unsupplied energy

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_2 = {20., 20., 20., 20., 20.};
    std::vector<double> expected_UnsupE_2 = {50., 50., 50., 50., 50.};
    BOOST_CHECK(HydroGen == expected_HydroGen_2);
    BOOST_CHECK(UnsupE == expected_UnsupE_2);
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_is_already_flat___remix_is_useless__level_easily_computed,
                        InputFixture<5>)
{
    init_level = 500.;
    std::ranges::fill(ovf, 25.);      // Cause levels to decrease
    std::ranges::fill(HydroGen, 20.); // Cause levels to decrease
    std::ranges::fill(inflows, 15.);  // Cause levels to increase
    std::ranges::fill(pump, 10.);     // Cause levels to increase

    callRemixStorageAlgorithm();

    std::vector<double> expected_levels = {480., 460., 440., 420., 400.};
    BOOST_TEST(levels == expected_levels, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(input_leads_to_levels_over_capacity___exception_raised, InputFixture<5>)
{
    init_level = 500.;
    capacity = 550.;
    std::ranges::fill(ovf, 15);      // Cause levels to decrease
    std::ranges::fill(HydroGen, 10); // Cause levels to decrease
    std::ranges::fill(inflows, 25);  // Cause levels to increase
    std::ranges::fill(pump, 20);     // Cause levels to increase
    err_msg = "Remix hydro input : levels computed from input don't respect reservoir bounds";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_leads_to_levels_less_than_zero___exception_raised, InputFixture<5>)
{
    init_level = 50.;
    std::ranges::fill(ovf, 30);      // Cause levels to decrease
    std::ranges::fill(HydroGen, 10); // Cause levels to decrease
    std::ranges::fill(inflows, 5);   // Cause levels to increase
    std::ranges::fill(pump, 10);     // Cause levels to increase
    err_msg = "Remix hydro input : levels computed from input don't respect reservoir bounds";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(influence_of_capacity_on_algorithm___case_where_no_influence,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    HydroGen = {10., 20., 10., 20., 10., 20., 10., 20., 10., 20.};
    UnsupE.assign(HydroGen.size(), 20.); // Not important for this test

    // First inflows > HydroGen, then inflows < HydroGen. Consequence : levels first increase, then
    // decrease.
    inflows = {25., 25., 25., 25., 25., 5., 5., 5., 5., 5.};
    init_level = 100.;

    // HydroGen and inflows result in :
    //   input_levels = {115, 120, 135, 140, 155, 140, 135, 120, 115, 100}
    // Note that : sup(input_levels) = 155

    // Case 1 :
    //   capacity relaxed (infinite by default) ==> leads to optimal solution (HydroGen is flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // Levels associated to new HydroGen are such as sup(levels) = 150. < sup(input_levels) = 155
    std::vector<double> expected_L = {110., 120., 130., 140., 150., 140., 130., 120., 110., 100.};
    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(levels == expected_L, boost::test_tools::per_element());

    // Case 2 : now, if we lower capacity to sup(input_levels) = 155, we should
    // have HydroGen and levels identical to previously : this value of capacity should
    // not have an influence on HydroGen and levels as results of the algorithm.
    capacity = 155.;
    HydroGen = {10., 20., 10., 20., 10., 20., 10., 20., 10., 20.}; // Reset hydro generation
    UnsupE.assign(HydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(levels == expected_L, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(lowering_capacity_too_low_leads_to_suboptimal_solution_for_GplusH,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    HydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.};
    UnsupE.assign(HydroGen.size(), 20.); // Not important for this test

    // First inflows > HydroGen, then inflows < HydroGen. Consequence : levels first increase, then
    // decrease.
    inflows = {25., 25., 25., 25., 25., 5., 5., 5., 5., 5.};
    init_level = 100.;

    // HydroGen and inflows lead to have :
    // input_levels = {105, 120, 125, 140, 145, 140, 125, 120, 105, 100}
    // Note sup(input_levels) = 145

    // Case 1 : capacity relaxed (infinite by default) ==> leads to optimal solution (HydroGen is
    // flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // Levels associated to new HydroGen are such as sup(L) = 150. > sup(input_levels) = 145
    std::vector<double> expected_L = {110., 120., 130., 140., 150., 140., 130., 120., 110., 100.};
    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(levels == expected_L, boost::test_tools::per_element());

    // Case 2 : we lower capacity to sup(input_levels) = 145.
    // This makes input acceptable for algo : levels computed from input have an
    // up bound <= capacity
    // But this time levels can not increase up to sup(L) = 150., as it would if capacity
    // was infinite. Therefore, solution found is suboptimal : we expect to get an
    // output HydroGen flat by interval, not flat on the whole domain.
    capacity = 145.;
    HydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.}; // Reset hydro generation
    UnsupE.assign(HydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat by interval
    std::vector<double> expected_HydroGen_2 = {16., 16., 16., 16., 16., 14., 14., 14., 14., 14.};
    BOOST_TEST(HydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(lowering_initial_level_too_low_leads_to_suboptimal_solution_for_GplusH,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 20 and 30 (new HydroGen will be flattened to 25 everywhere)
    HydroGen = {20., 30., 20., 30., 20., 30., 20., 30., 20., 30.};
    UnsupE.assign(HydroGen.size(), 20.); // Not important for this test

    // First inflows < HydroGen, then inflows > HydroGen. Consequence : levels first decrease, then
    // increase.
    inflows = {5., 5., 5., 5., 5., 45., 45., 45., 45., 45.};
    capacity = std::numeric_limits<double>::max();
    init_level = 100.;

    // HydroGen and inflows result in : input_levels = {85, 60, 45, 20, 5, 20, 45, 60, 85, 100}
    // Note : inf(input_levels) = 5

    // Case 1 : init level (== 100) is high enough so that input levels (computed from input data)
    // are acceptable for algorithm (input levels >= 0.), and running algorithm leads to optimal
    // solution (OutHydroGen is flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 25. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 25.);
    // Levels associated to new HydroGen are such as inf(L) = 0. > inf(input_levels) = 5
    std::vector<double> expected_L = {80., 60., 40., 20., 0., 20., 40., 60., 80., 100.};
    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(levels == expected_L, boost::test_tools::per_element());

    // Case 2 : we lower initial level. Input data are still acceptable
    // for algorithm (despite the new init level), algorithm will have to take the levels lower
    // bound (0.) into account. As the levels change, the solution OutHydroGen will be suboptimal,
    // that is flat by interval (not flat on the whole domain).
    init_level = 95.;
    HydroGen = {20., 30., 20., 30., 20., 30., 20., 30., 20., 30.}; // Reset hydro generation
    UnsupE.assign(HydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat by interval
    std::vector<double> expected_HydroGen_2 = {24., 24., 24., 24., 24., 26., 26., 26., 26., 26.};
    BOOST_TEST(HydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(influence_of_initial_level_on_algorithm___case_where_no_influence,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    HydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.};
    UnsupE.assign(HydroGen.size(), 20.); // Not important for this test

    // First inflows < HydroGen, then inflows > HydroGen. Consequence : levels first decrease, then
    // increase.
    inflows = {5., 5., 5., 5., 5., 25., 25., 25., 25., 25.};
    init_level = 100.;
    // HydroGen and inflows are such as inf(input_levels) = 45

    // Case 1 : init level (== 100) is high enough so that input levels (computed from input data)
    // are acceptable by algorithm, and levels computed by algorithm (output) are optimal, that
    // is computed from a optimal (that is flat) OutHydroGen.
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // Levels associated to new HydroGen are such as inf(L) = 50 > inf(input_levels) = 45
    std::vector<double> expected_L = {90., 80., 70., 60., 50., 60., 70., 80., 90., 100.};
    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(levels == expected_L, boost::test_tools::per_element());

    // Case 2 : now we lower initial level down to 55.
    // In this way, input data is still acceptable for algorithm
    // and algorithm won't have to take the levels lower bound (0.) into account.
    // The solution OutHydroGen will be optimal, that is flat by interval.
    init_level = 55.;
    HydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.}; // Reset hydro generation
    UnsupE.assign(HydroGen.size(), 20.);                           // Reset unsuppied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat (and optimal)
    std::vector<double> expected_HydroGen_2(10, 15.);
    BOOST_TEST(HydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(spillage_positive_at_hour_0___no_change_at_this_hour, InputFixture<5>)
{
    std::ranges::fill(TotalGenNoHydro, 100.);
    HydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    init_level = 500.;
    capacity = 1000.;
    // At this stage, DTG_MRG is filled with zeros. Running the algorithm would flatten
    // HydroGen to 20 everywhere : HydroGen = {20, 20, 20, 20, 20}
    // But :
    Spillage[0] = 1.;
    // Now, we expect no change for HydroGen at hour 0
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {40., 15., 15., 15., 15.};
    BOOST_CHECK(HydroGen == expected_HydroGen);
}

BOOST_FIXTURE_TEST_CASE(DTG_MRG_positive_on_hour_4___no_change_at_this_hour, InputFixture<5>)
{
    std::ranges::fill(TotalGenNoHydro, 100.);
    HydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    init_level = 500.;
    capacity = 1000.;
    // At this stage, DTG_MRG is filled with zeros. Running the algorithm would flatten
    // HydroGen to 20 everywhere : HydroGen = {20, 20, 20, 20, 20}
    // But :
    DTG_MRG[4] = 1.;
    // Now, we expect no change for HydroGen at hour 4
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {25., 25., 25., 25., 0.};
    BOOST_CHECK(HydroGen == expected_HydroGen);
}

BOOST_FIXTURE_TEST_CASE(comparison_of_results_with_python_algo,
                        InputFixture<20>,
                        *boost::unit_test::tolerance(0.01))
{
    std::vector<double> load = {46, 81, 89, 42, 69, 55, 88, 46, 84, 94,
                                66, 93, 68, 39, 91, 89, 94, 93, 91, 38};
    HydroGen = {10, 40, 36, 8, 13, 33, 9, 0, 24, 18, 5, 47, 29, 6, 7, 54, 49, 11, 63, 21};
    UnsupE = {34, 32, 33, 23, 9, 8, 20, 40, 30, 3, 50, 27, 12, 1, 35, 31, 2, 58, 20, 4};
    // Computing total generation without hydro generation
    TotalGenNoHydro = load;
    std::ranges::transform(TotalGenNoHydro,
                           HydroGen,
                           TotalGenNoHydro.begin(),
                           std::minus<double>());
    std::ranges::transform(TotalGenNoHydro, UnsupE, TotalGenNoHydro.begin(), std::minus<double>());

    pmax = {43, 48, 36, 43, 13, 44, 13, 31, 49, 35, 47, 47, 37, 41, 21, 54, 49, 28, 63, 49};
    pmin = {10, 22, 17, 8, 7, 15, 8, 0, 9, 2, 5, 18, 22, 6, 4, 11, 1, 0, 23, 6};
    init_level = 13.6;
    capacity = 126.;
    inflows = {37, 27, 41, 36, 7, 14, 38, 23, 17, 35, 20, 24, 17, 46, 1, 10, 10, 12, 46, 30};

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {42.3, 35.3,  27.,  31.,   7.,    33.,   8.,
                                             31.,  19.55, 2.,   38.55, 30.55, 22.55, 7.,
                                             4.,   45.55, 6.55, 25.55, 41.55, 25.};
    BOOST_TEST(HydroGen == expected_HydroGen, boost::test_tools::per_element());
}
