#include <numeric>
#define BOOST_TEST_MODULE hydro remix

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

using namespace Antares::Solver::Simulation;

struct HydroStorageData
{
    HydroStorageData(size_t size):
        hydroGen(size, 0.),
        levels(size, 0.),
        inflows(size, 0.)
    {
    }

    std::vector<double> hydroGen;
    std::vector<double> levels;
    std::vector<double> inflows;
    double init_level = 0;
    double capacity = std::numeric_limits<double>::max();
};

template<unsigned int size>
struct InputFixture
{
    InputFixture():
        s1(size),
        s2(size),
        s3(size)
    {
        TotalGenNoHydro.assign(size, 0.);
        Load.assign(size, 0.);
        UnsupE.assign(size, 0.);
        pmax.assign(size, std::numeric_limits<double>::max());
        pmin.assign(size, 0.);
        ovf.assign(size, 0.);
        pump.assign(size, 0.);
        Spillage.assign(size, 0.);
        DTG_MRG.assign(size, 0.);
    }

    std::shared_ptr<IStorageForRemix> createHydroForRemix(HydroStorageData& storageData)
    {
        return makeHydroForRemix(storageData.hydroGen,
                                 UnsupE,
                                 storageData.levels,
                                 pmax,
                                 pmin,
                                 storageData.inflows,
                                 ovf,
                                 pump,
                                 storageData.init_level,
                                 storageData.capacity,
                                 pumpEff,
                                 reservoirManagement,
                                 "hydro");
    }

    void callRemixStorageAlgorithm(unsigned nbStorage = 1)
    {
        Load = TotalGenNoHydro + UnsupE + s1.hydroGen + s2.hydroGen + s3.hydroGen;
        storagesForRemix.clear();
        storagesForRemix.push_back(createHydroForRemix(s1));
        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);

        if (nbStorage >= 2)
        {
            auto storage2 = createHydroForRemix(s2);
            storagesForRemix.push_back(storage2);

            if (nbStorage == 3)
            {
                auto storage3 = createHydroForRemix(s3);
                storagesForRemix.push_back(storage3);
            }
        }
        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    std::vector<double> TotalGenNoHydro, Load, UnsupE, pmax, pmin, ovf, pump, Spillage, DTG_MRG;
    ListStorageForRemix storagesForRemix;

    const double pumpEff = 1.;
    const bool reservoirManagement = true;

    std::string err_msg;

    HydroStorageData s1, s2, s3;
};

BOOST_FIXTURE_TEST_CASE(input_vectors_of_different_sizes__exception_raised, InputFixture<1>)
{
    s1.hydroGen = {0., 0.};
    err_msg = "Remix hydro input : arrays of different sizes";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_init_level_exceeds_capacity__exception_raised, InputFixture<1>)
{
    s1.init_level = 2.;
    s1.capacity = 1.;
    err_msg = "Remix hydro input : initial level > reservoir capacity";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(all_input_arrays_of_size_0__exception_raised, InputFixture<0>)
{
    s1.init_level = 0.;
    s1.capacity = 1.;
    err_msg = "Remix hydro input : all arrays of sizes 0";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_not_smaller_than_pmax__exception_raised, InputFixture<5>)
{
    s1.hydroGen = {1., 2., 3., 4., 5.};
    pmax = {2., 2., 2., 4., 5.};
    s1.init_level = 0.;
    s1.capacity = 1.;
    err_msg = "Remix hydro input : Storage withdrawal not smaller than Pmax everywhere";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_not_greater_than_pmin__exception_raised, InputFixture<5>)
{
    s1.hydroGen = {1., 2., 3., 4., 5.};
    pmin = {0., 0., 4., 0., 0.};
    s1.init_level = 0.;
    s1.capacity = 1.;
    err_msg = "Remix hydro input : Storage withdrawal not greater than Pmin everywhere";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_is_acceptable__no_exception_raised, InputFixture<1>)
{
    s1.init_level = 0.;
    s1.capacity = 1.;
    Load = TotalGenNoHydro + UnsupE + s1.hydroGen;

    BOOST_CHECK_NO_THROW(storagesForRemix.push_back(createHydroForRemix(s1)));
    BOOST_CHECK_NO_THROW(
      shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix));
}

BOOST_FIXTURE_TEST_CASE(
  hydro_increases_and_pmax_40mwh___Hydro_gen_is_flattened_to_mean_Hydro_gen_20mwh,
  InputFixture<5>)
{
    std::ranges::fill(pmax, 40.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    s1.hydroGen = {0., 10., 20., 30., 40.}; // we have Pmin <= HydroGen <= Pmax
    UnsupE = {80., 60., 40., 20., 0.};
    s1.init_level = 500.;
    s1.capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains flat
    std::vector<double> expected_UnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(same_test_as_above___we_just_raise_pmax___same_results, InputFixture<5>)
{
    std::ranges::fill(pmax, 50.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    s1.hydroGen = {0., 10., 20., 30., 40.};
    UnsupE = {80., 60., 40., 20., 0.};
    s1.init_level = 500.;
    s1.capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains constant at each hour
    std::vector<double> expected_UnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(
  hydro_decreases_and_pmax_40mwh___Hydro_gen_is_flattened_to_mean_Hydro_gen_20mwh,
  InputFixture<5>)
{
    std::ranges::fill(pmax, 40.);
    std::ranges::fill(TotalGenNoHydro, 100.);
    s1.hydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    s1.init_level = 500.;
    s1.capacity = 1000.;

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {20., 20., 20., 20., 20.};
    // UnsupE such as Load = TotalGenNoHydro + HydroGen + UnsupE remains constant at each hour
    std::vector<double> expected_UnsupE = {20., 30., 40., 50., 60.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen);
    BOOST_CHECK(UnsupE == expected_UnsupE);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmax, InputFixture<5>, *boost::unit_test::tolerance(0.01))
{
    // TotalGenNoHydro decreases
    TotalGenNoHydro = {100., 80., 60., 40., 20.};

    // HydroGen is flat and must respect HydroGen <= Pmax everywhere
    s1.hydroGen = {20., 20., 20., 20., 20.};
    UnsupE = {50., 50., 50., 50., 50.};
    s1.init_level = 500.;
    s1.capacity = 1000.;

    // 1. Algorithm tends to flatten TotalGenNoHydro + HydroGen, so it would require HydroGen to
    // increase. Proof :
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_1 = {0., 0., 13.33, 33.33, 53.33};
    BOOST_TEST(s1.hydroGen == expected_HydroGen_1, boost::test_tools::per_element());

    // 2. But HydroGen is limited by pmax. So Algo does nothing in the end.
    // Proof :
    pmax = {20., 20., 20., 20., 20.};
    s1.hydroGen = {20., 20., 20., 20., 20.}; // Reset hydro generation
    UnsupE = {50., 50., 50., 50., 50.};      // Reset unsupplied energy

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_2 = {20., 20., 20., 20., 20.};
    std::vector<double> expected_UnsupE_2 = {50., 50., 50., 50., 50.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen_2);
    BOOST_CHECK(UnsupE == expected_UnsupE_2);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmin, InputFixture<5>, *boost::unit_test::tolerance(0.01))
{
    // TotalGenNoHydro decreases
    TotalGenNoHydro = {100., 80., 60., 40., 20.};

    // HydroGen is flat and must respect  Pmin <= HydroGen <= Pmax everywhere
    s1.hydroGen = {20., 20., 20., 20., 20.};
    UnsupE = {50., 50., 50., 50., 50.};
    s1.init_level = 500.;
    s1.capacity = 1000.;

    // 1. Algorithm tends to flatten TotalGenNoHydro + HydroGen, so it would require HydroGen to
    // increase.
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_1 = {0., 0., 13.33, 33.33, 53.33};
    BOOST_TEST(s1.hydroGen == expected_HydroGen_1, boost::test_tools::per_element());

    // 2. But HydroGen is low bounded by pmin. So Algo does nothing in the end.
    pmin = {20., 20., 20., 20., 20.};
    s1.hydroGen = {20., 20., 20., 20., 20.}; // Reset hydro generation
    UnsupE = {50., 50., 50., 50., 50.};      // Reset unsupplied energy

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen_2 = {20., 20., 20., 20., 20.};
    std::vector<double> expected_UnsupE_2 = {50., 50., 50., 50., 50.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen_2);
    BOOST_CHECK(UnsupE == expected_UnsupE_2);
}

BOOST_FIXTURE_TEST_CASE(Hydro_gen_is_already_flat___remix_is_useless__level_easily_computed,
                        InputFixture<5>)
{
    s1.init_level = 500.;
    std::ranges::fill(ovf, 25.);         // Cause s1.levels to decrease
    std::ranges::fill(s1.hydroGen, 20.); // Cause s1.levels to decrease
    std::ranges::fill(s1.inflows, 15.);  // Cause s1.levels to increase
    std::ranges::fill(pump, 10.);        // Cause s1.levels to increase

    callRemixStorageAlgorithm();

    std::vector<double> expected_s1_levels = {480., 460., 440., 420., 400.};
    BOOST_TEST(s1.levels == expected_s1_levels, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(input_leads_to_s1_levels_over_s1_capacity___exception_raised,
                        InputFixture<5>)
{
    s1.init_level = 500.;
    s1.capacity = 550.;
    std::ranges::fill(ovf, 15);         // Cause s1.levels to decrease
    std::ranges::fill(s1.hydroGen, 10); // Cause s1.levels to decrease
    std::ranges::fill(s1.inflows, 25);  // Cause s1.levels to increase
    std::ranges::fill(pump, 20);        // Cause s1.levels to increase
    err_msg = "Remix hydro input : levels computed from input don't respect reservoir bounds";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(input_leads_to_s1_levels_less_than_zero___exception_raised, InputFixture<5>)
{
    s1.init_level = 50.;
    std::ranges::fill(ovf, 30);         // Cause s1.levels to decrease
    std::ranges::fill(s1.hydroGen, 10); // Cause s1.levels to decrease
    std::ranges::fill(s1.inflows, 5);   // Cause s1.levels to increase
    std::ranges::fill(pump, 10);        // Cause s1.levels to increase
    err_msg = "Remix hydro input : levels computed from input don't respect reservoir bounds";
    BOOST_CHECK_EXCEPTION(createHydroForRemix(s1), std::invalid_argument, checkMessage(err_msg));
}

BOOST_FIXTURE_TEST_CASE(influence_of_s1_capacity_on_algorithm___case_where_no_influence,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    s1.hydroGen = {10., 20., 10., 20., 10., 20., 10., 20., 10., 20.};
    UnsupE.assign(s1.hydroGen.size(), 20.); // Not important for this test

    // First s1.inflows > HydroGen, then s1.inflows < HydroGen. Consequence : s1.levels first
    // increase, then decrease.
    s1.inflows = {25., 25., 25., 25., 25., 5., 5., 5., 5., 5.};
    s1.init_level = 100.;

    // HydroGen and s1.inflows result in :
    //   input_s1.levels = {115, 120, 135, 140, 155, 140, 135, 120, 115, 100}
    // Note that : sup(input_s1.levels) = 155

    // Case 1 :
    //   s1.capacity relaxed (infinite by default) ==> leads to optimal solution (HydroGen is flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // s1.levels associated to new HydroGen are such as sup(s1.levels) = 150. < sup(input_s1.levels)
    // = 155
    std::vector<double> expected_L = {110., 120., 130., 140., 150., 140., 130., 120., 110., 100.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(s1.levels == expected_L, boost::test_tools::per_element());

    // Case 2 : now, if we lower s1.capacity to sup(input_s1.levels) = 155, we should
    // have HydroGen and s1.levels identical to previously : this value of s1.capacity should
    // not have an influence on HydroGen and s1.levels as results of the algorithm.
    s1.capacity = 155.;
    s1.hydroGen = {10., 20., 10., 20., 10., 20., 10., 20., 10., 20.}; // Reset hydro generation
    UnsupE.assign(s1.hydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(s1.levels == expected_L, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(lowering_s1_capacity_too_low_leads_to_suboptimal_solution_for_GplusH,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    s1.hydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.};
    UnsupE.assign(s1.hydroGen.size(), 20.); // Not important for this test

    // First s1.inflows > HydroGen, then s1.inflows < HydroGen. Consequence : s1.levels first
    // increase, then decrease.
    s1.inflows = {25., 25., 25., 25., 25., 5., 5., 5., 5., 5.};
    s1.init_level = 100.;

    // HydroGen and s1.inflows lead to have :
    // input_s1.levels = {105, 120, 125, 140, 145, 140, 125, 120, 105, 100}
    // Note sup(input_s1.levels) = 145

    // Case 1 : s1.capacity relaxed (infinite by default) ==> leads to optimal solution (HydroGen is
    // flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // s1.levels associated to new HydroGen are such as sup(L) = 150. > sup(input_s1.levels) = 145
    std::vector<double> expected_L = {110., 120., 130., 140., 150., 140., 130., 120., 110., 100.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(s1.levels == expected_L, boost::test_tools::per_element());

    // Case 2 : we lower s1.capacity to sup(input_s1.levels) = 145.
    // This makes input acceptable for algo : s1.levels computed from input have an
    // up bound <= s1.capacity
    // But this time s1.levels can not increase up to sup(L) = 150., as it would if s1.capacity
    // was infinite. Therefore, solution found is suboptimal : we expect to get an
    // output HydroGen flat by interval, not flat on the whole domain.
    s1.capacity = 145.;
    s1.hydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.}; // Reset hydro generation
    UnsupE.assign(s1.hydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat by interval
    std::vector<double> expected_HydroGen_2 = {16., 16., 16., 16., 16., 14., 14., 14., 14., 14.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(lowering_initial_level_too_low_leads_to_suboptimal_solution_for_GplusH,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 20 and 30 (new HydroGen will be flattened to 25 everywhere)
    s1.hydroGen = {20., 30., 20., 30., 20., 30., 20., 30., 20., 30.};
    UnsupE.assign(s1.hydroGen.size(), 20.); // Not important for this test

    // First s1.inflows < HydroGen, then s1.inflows > HydroGen. Consequence : s1.levels first
    // decrease, then increase.
    s1.inflows = {5., 5., 5., 5., 5., 45., 45., 45., 45., 45.};
    s1.capacity = std::numeric_limits<double>::max();
    s1.init_level = 100.;

    // HydroGen and s1.inflows result in : input_s1.levels = {85, 60, 45, 20, 5, 20, 45, 60, 85,
    // 100} Note : inf(input_s1.levels) = 5

    // Case 1 : init level (== 100) is high enough so that input s1.levels (computed from input
    // data) are acceptable for algorithm (input s1.levels >= 0.), and running algorithm leads to
    // optimal solution (OutHydroGen is flat)
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 25. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 25.);
    // s1.levels associated to new HydroGen are such as inf(L) = 0. > inf(input_s1.levels) = 5
    std::vector<double> expected_L = {80., 60., 40., 20., 0., 20., 40., 60., 80., 100.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(s1.levels == expected_L, boost::test_tools::per_element());

    // Case 2 : we lower initial level. Input data are still acceptable
    // for algorithm (despite the new init level), algorithm will have to take the s1.levels lower
    // bound (0.) into account. As the s1.levels change, the solution OutHydroGen will be
    // suboptimal, that is flat by interval (not flat on the whole domain).
    s1.init_level = 95.;
    s1.hydroGen = {20., 30., 20., 30., 20., 30., 20., 30., 20., 30.}; // Reset hydro generation
    UnsupE.assign(s1.hydroGen.size(), 20.);                           // Reset unsupplied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat by interval
    std::vector<double> expected_HydroGen_2 = {24., 24., 24., 24., 24., 26., 26., 26., 26., 26.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(influence_of_initial_level_on_algorithm___case_where_no_influence,
                        InputFixture<10>,
                        *boost::unit_test::tolerance(0.001))
{
    // HydroGen oscillates between 10 and 20 (new HydroGen will be flattened to 15 everywhere)
    s1.hydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.};
    UnsupE.assign(s1.hydroGen.size(), 20.); // Not important for this test

    // First s1.inflows < HydroGen, then s1.inflows > HydroGen. Consequence : s1.levels first
    // decrease, then increase.
    s1.inflows = {5., 5., 5., 5., 5., 25., 25., 25., 25., 25.};
    s1.init_level = 100.;
    // HydroGen and s1.inflows are such as inf(input_s1.levels) = 45

    // Case 1 : init level (== 100) is high enough so that input s1.levels (computed from input
    // data) are acceptable by algorithm, and s1.levels computed by algorithm (output) are optimal,
    // that is computed from a optimal (that is flat) OutHydroGen.
    callRemixStorageAlgorithm();

    // HydroGen is flat and is 15. (means of initial HydroGen)
    std::vector<double> expected_HydroGen(10, 15.);
    // s1.levels associated to new HydroGen are such as inf(L) = 50 > inf(input_s1.levels) = 45
    std::vector<double> expected_L = {90., 80., 70., 60., 50., 60., 70., 80., 90., 100.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
    BOOST_TEST(s1.levels == expected_L, boost::test_tools::per_element());

    // Case 2 : now we lower initial level down to 55.
    // In this way, input data is still acceptable for algorithm
    // and algorithm won't have to take the s1.levels lower bound (0.) into account.
    // The solution OutHydroGen will be optimal, that is flat by interval.
    s1.init_level = 55.;
    s1.hydroGen = {20., 10., 20., 10., 20., 10., 20., 10., 20., 10.}; // Reset hydro generation
    UnsupE.assign(s1.hydroGen.size(), 20.);                           // Reset unsuppied energy

    callRemixStorageAlgorithm();

    // OutHydroGen_2 is flat (and optimal)
    std::vector<double> expected_HydroGen_2(10, 15.);
    BOOST_TEST(s1.hydroGen == expected_HydroGen_2, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(spillage_positive_at_hour_0___no_change_at_this_hour, InputFixture<5>)
{
    std::ranges::fill(TotalGenNoHydro, 100.);
    s1.hydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    s1.init_level = 500.;
    s1.capacity = 1000.;
    // At this stage, DTG_MRG is filled with zeros. Running the algorithm would flatten
    // HydroGen to 20 everywhere : HydroGen = {20, 20, 20, 20, 20}
    // But :
    Spillage[0] = 1.;
    // Now, we expect no change for HydroGen at hour 0
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {40., 15., 15., 15., 15.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen);
}

BOOST_FIXTURE_TEST_CASE(DTG_MRG_positive_on_hour_4___no_change_at_this_hour, InputFixture<5>)
{
    std::ranges::fill(TotalGenNoHydro, 100.);
    s1.hydroGen = {40., 30., 20., 10., 0.};
    UnsupE = {0., 20., 40., 60., 80.};
    s1.init_level = 500.;
    s1.capacity = 1000.;
    // At this stage, DTG_MRG is filled with zeros. Running the algorithm would flatten
    // HydroGen to 20 everywhere : HydroGen = {20, 20, 20, 20, 20}
    // But :
    DTG_MRG[4] = 1.;
    // Now, we expect no change for HydroGen at hour 4
    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {25., 25., 25., 25., 0.};
    BOOST_CHECK(s1.hydroGen == expected_HydroGen);
}

BOOST_FIXTURE_TEST_CASE(comparison_of_results_with_python_algo,
                        InputFixture<20>,
                        *boost::unit_test::tolerance(0.01))
{
    std::vector<double> load = {46, 81, 89, 42, 69, 55, 88, 46, 84, 94,
                                66, 93, 68, 39, 91, 89, 94, 93, 91, 38};
    s1.hydroGen = {10, 40, 36, 8, 13, 33, 9, 0, 24, 18, 5, 47, 29, 6, 7, 54, 49, 11, 63, 21};
    UnsupE = {34, 32, 33, 23, 9, 8, 20, 40, 30, 3, 50, 27, 12, 1, 35, 31, 2, 58, 20, 4};
    // Computing total generation without hydro generation
    TotalGenNoHydro = load;
    std::ranges::transform(TotalGenNoHydro,
                           s1.hydroGen,
                           TotalGenNoHydro.begin(),
                           std::minus<double>());
    std::ranges::transform(TotalGenNoHydro, UnsupE, TotalGenNoHydro.begin(), std::minus<double>());

    pmax = {43, 48, 36, 43, 13, 44, 13, 31, 49, 35, 47, 47, 37, 41, 21, 54, 49, 28, 63, 49};
    pmin = {10, 22, 17, 8, 7, 15, 8, 0, 9, 2, 5, 18, 22, 6, 4, 11, 1, 0, 23, 6};
    s1.init_level = 13.6;
    s1.capacity = 126.;
    s1.inflows = {37, 27, 41, 36, 7, 14, 38, 23, 17, 35, 20, 24, 17, 46, 1, 10, 10, 12, 46, 30};

    callRemixStorageAlgorithm();

    std::vector<double> expected_HydroGen = {42.3, 35.3,  27.,  31.,   7.,    33.,   8.,
                                             31.,  19.55, 2.,   38.55, 30.55, 22.55, 7.,
                                             4.,   45.55, 6.55, 25.55, 41.55, 25.};
    BOOST_TEST(s1.hydroGen == expected_HydroGen, boost::test_tools::per_element());
}

BOOST_FIXTURE_TEST_CASE(three_hydros_with_one_dominant_storage, InputFixture<5>)
{
    // 3 hydro units, same time horizon
    s1.hydroGen = {10., 20., 10., 20., 10.};      // Total = 70. Mean = 14.
    s2.hydroGen = {10., 20., 10., 20., 10.};      // Total = 70. Mean = 14.
    s3.hydroGen = {100., 200., 100., 200., 100.}; // Total = 700. Mean = 140.
    // Initial HydroSum = {120., 240., 120., 240., 120.}
    // Total Mean = 168.0

    // *** NEW CONSTRAINTS ***
    // 1. Set s1.inflows to match the mean generation for each reservoir
    // This means if generation is flat, s1.levels are flat.
    std::ranges::fill(s1.inflows, 14.);  // Mean of HydroGen
    std::ranges::fill(s2.inflows, 14.);  // Mean of HydroGen2
    std::ranges::fill(s3.inflows, 140.); // Mean of HydroGen3

    // 2. Set non-binding s1.levels and capacities
    s1.init_level = s2.init_level = s3.init_level = 1000.;
    s1.capacity = s2.capacity = s3.capacity = 2000.;

    // 3. Set driving signals
    std::ranges::fill(TotalGenNoHydro, 0.);  // Flat
    UnsupE.assign(s1.hydroGen.size(), 100.); // Zero UnsupE -> signal to flatten

    callRemixStorageAlgorithm(3);

    // --- Check Results ---

    // 1. Check that total hydro generation is perfectly flat
    std::vector<double> HydroSum(s1.hydroGen.size());
    HydroSum = s1.hydroGen + s2.hydroGen + s3.hydroGen;

    std::vector<double> expected_HydroSum = {168., 168., 168., 168., 168.};
    for (unsigned i = 0; i < HydroSum.size(); ++i)
    {
        BOOST_CHECK_CLOSE(HydroSum[i], expected_HydroSum[i], 1e-3);
    }
}

BOOST_FIXTURE_TEST_CASE(flow_conservation_two_hydro_units, InputFixture<8>)
{
    // ------------------------------
    // SETUP: Two reservoirs with distinct s1.inflows and generations
    // ------------------------------
    s1.hydroGen = {10., 15., 20., 15., 10., 5., 10., 15.};
    s2.hydroGen = {5., 10., 15., 10., 5., 0., 5., 10.};

    s1.inflows = {12., 18., 20., 18., 12., 8., 12., 18.};
    s2.inflows = {6., 12., 16., 12., 6., 2., 6., 12.};

    s1.init_level = 100.;
    s2.init_level = 50.;
    s1.capacity = 500.;
    s2.capacity = 300.;

    UnsupE.assign(s1.hydroGen.size(), 0.); // Not relevant for flow conservation

    // ------------------------------
    // Record pre-call total s1.inflows, generations, and init s1.levels
    // ------------------------------
    double total_inflow_before = std::accumulate(s1.inflows.begin(), s1.inflows.end(), 0.0)
                                 + std::accumulate(s2.inflows.begin(), s2.inflows.end(), 0.0);

    double total_gen_before = std::accumulate(s1.hydroGen.begin(), s1.hydroGen.end(), 0.0)
                              + std::accumulate(s2.hydroGen.begin(), s2.hydroGen.end(), 0.0);

    double total_init_level = s1.init_level + s2.init_level;

    // ------------------------------
    // Run Remix algorithm
    // ------------------------------
    callRemixStorageAlgorithm(2);

    // ------------------------------
    // Compute flow balance after remix
    // ------------------------------
    double total_inflow_after = std::accumulate(s1.inflows.begin(), s1.inflows.end(), 0.0)
                                + std::accumulate(s2.inflows.begin(), s2.inflows.end(), 0.0);

    double total_gen_after = std::accumulate(s1.hydroGen.begin(), s1.hydroGen.end(), 0.0)
                             + std::accumulate(s2.hydroGen.begin(), s2.hydroGen.end(), 0.0);

    double total_final_level = s1.levels.back() + s2.levels.back();

    // ------------------------------
    // Check flow conservation
    // ------------------------------

    // 1. Check that total s1.inflows were not modified
    BOOST_TEST(std::abs(total_inflow_after - total_inflow_before) < 1e-3);

    // 2. Check that total generation was conserved (remixing just *shifts* energy)
    BOOST_TEST(std::abs(total_gen_after - total_gen_before) < 1e-3);

    // 3. Check the main flow balance equation:
    // (Total Inflow - Total Generation) must equal (Change in Storage)
    double imbalance = std::abs((total_inflow_after - total_gen_after)
                                - (total_final_level - total_init_level));

    BOOST_TEST(imbalance < 1e-3); // Flow perfectly conserved within tolerance
}
