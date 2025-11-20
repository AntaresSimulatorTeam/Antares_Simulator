#define BOOST_TEST_MODULE multi storage remix

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>
#include <vector>
#include <numeric>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

using namespace Antares::Solver::Simulation;

template<unsigned int nb_hours>
struct STS_holder
{
    STS_holder():
        withdrawal(nb_hours, 0),
        injection(nb_hours, 0),
        levels(nb_hours, 0.),
        pmax(nb_hours, std::numeric_limits<double>::max()),
        inflows(nb_hours, 0.),
        ovf(nb_hours, 0.),
        lowRuleCurve(nb_hours, 0.)
    {
        upRuleCurve.assign(nb_hours, capacity);
    }

    std::shared_ptr<IStorageForRemix> createSTS(std::vector<double>& unsupE)
    {
        return makeSTSforRemix(withdrawal,
                               unsupE,
                               levels,
                               pmax,
                               inflows,
                               injection,
                               lowRuleCurve,
                               upRuleCurve,
                               initLevel,
                               withdrawalEff,
                               injectionEff);
    }

    std::vector<double> withdrawal, injection, levels, pmax, inflows, ovf;
    std::vector<double> lowRuleCurve, upRuleCurve;

    double initLevel = 0.;
    double capacity = std::numeric_limits<double>::max();
    const double withdrawalEff = 1.;
    const double injectionEff = 1.;
};

template<unsigned nb_hours, unsigned nb_storage>
struct InputFixture
{
    InputFixture():
        Load(nb_hours, 0.),
        TotaGenWithoutStorage(nb_hours, 0.),
        UnsupE(nb_hours, 0.),
        Spillage(nb_hours, 0.),
        DTG_MRG(nb_hours, 0.)
    {
        STS_holders.resize(nb_storage);
    }

    void runRemixStorageAlgorithm()
    {
        Load = TotaGenWithoutStorage + UnsupE;
        for (const auto& sts: STS_holders)
        {
            Load = Load + sts.withdrawal;
        }

        storagesForRemix.clear();
        for (auto& sts: STS_holders)
        {
            storagesForRemix.push_back(sts.createSTS(UnsupE));
        }

        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    std::vector<STS_holder<nb_hours>> STS_holders;

    std::vector<double> Load, TotaGenWithoutStorage, UnsupE, Spillage, DTG_MRG;

    ListStorageForRemix storagesForRemix;
};

BOOST_FIXTURE_TEST_CASE(default_input_in_STS_setup_does_not_raise_exception, STS_holder<5>)
{
    std::vector<double> unsupE(5, 0.);
    BOOST_CHECK_NO_THROW(createSTS(unsupE));
}

BOOST_AUTO_TEST_CASE(creating_an_STS_with_unsup_having_a_wrong_size_leads_to_exception)
{
    std::vector<double> unsupE(5, 0.);
    STS_holder<4> sts_holder;
    std::string err_msg = "Remix hydro input : arrays of different sizes";
    BOOST_CHECK_EXCEPTION(sts_holder.createSTS(unsupE),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

BOOST_AUTO_TEST_CASE(create_2_STS_for_nb_of_hours_not_equal___check_input_for_algo_raises_exception)
{
    std::vector<double> Load(5);
    std::vector<double> TotaGenWithoutStorage(5);
    std::vector<double> Spillage(5);
    std::vector<double> DTG_MRG(5);

    // Creating sts_1 for 5 hours
    std::vector<double> unsupE_1(5, 0.);
    STS_holder<5> sts_holder_1;
    auto sts_1 = sts_holder_1.createSTS(unsupE_1);

    // Creating sts_2 for 3 hours
    std::vector<double> unsupE_2(3, 0.);
    STS_holder<3> sts_holder_2;
    auto sts_2 = sts_holder_2.createSTS(unsupE_2);

    ListStorageForRemix storagesForRemix;
    storagesForRemix.push_back(sts_1);
    storagesForRemix.push_back(sts_2);

    std::string err_msg = "Remix storage input : arrays of different sizes";
    BOOST_CHECK_EXCEPTION(checkInput(Load, unsupE_1, Spillage, DTG_MRG, storagesForRemix),
                          std::invalid_argument,
                          checkMessage(err_msg));
}

// Using direcly InputFixture<n, m> in fixture test cases does not compile on Win MSVC.
// So we're forced to use typedefs instead.
using InputFixture_5_2 = InputFixture<5, 2>;
using InputFixture_5_3 = InputFixture<5, 3>;
using InputFixture_8_2 = InputFixture<8, 2>;

// ================================================
// Note :
//  G : Total generation without storage
//  H : Total storage production (or withdrawal)
// ================================================

BOOST_FIXTURE_TEST_CASE(G_is_flat___H_increases___G_plus_H_gets_flat, InputFixture_5_2)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {80., 60., 40., 20., 0.};

    // Storage 1
    std::ranges::fill(STS_holders[0].pmax, 40.);
    STS_holders[0].withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    STS_holders[0].capacity = 1000.;
    STS_holders[0].initLevel = 500.;

    // Storage 2
    std::ranges::fill(STS_holders[1].pmax, 10.);
    STS_holders[1].withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    STS_holders[1].capacity = 400.;
    STS_holders[1].initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = STS_holders[0].withdrawal
                                                + STS_holders[1].withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(same_test_as_above___we_just_raise_pmax___same_results, InputFixture_5_2)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {80., 60., 40., 20., 0.};

    // Storage 1
    std::ranges::fill(STS_holders[0].pmax, 50.);
    STS_holders[0].withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    STS_holders[0].capacity = 1000.;
    STS_holders[0].initLevel = 500.;

    // Storage 2
    std::ranges::fill(STS_holders[1].pmax, 20.);
    STS_holders[1].withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    STS_holders[1].capacity = 400.;
    STS_holders[1].initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = STS_holders[0].withdrawal
                                                + STS_holders[1].withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(G_is_flat___H_decreases___G_plus_H_gets_flat, InputFixture_5_2)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {0., 20., 40., 60., 80.};

    // Storage 1
    std::ranges::fill(STS_holders[0].pmax, 40.);
    STS_holders[0].withdrawal = {40., 30., 20., 10., 0.}; // We have : withdrawal <= Pmax
    STS_holders[0].capacity = 1000.;
    STS_holders[0].initLevel = 500.;

    // Storage 2
    std::ranges::fill(STS_holders[1].pmax, 10.);
    STS_holders[1].withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    STS_holders[1].capacity = 400.;
    STS_holders[1].initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = STS_holders[0].withdrawal
                                                + STS_holders[1].withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {20., 30., 40., 50., 60.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmax, InputFixture_5_2, *boost::unit_test::tolerance(0.01))
{
    TotaGenWithoutStorage = {100., 80., 60., 40., 20.}; // Decreases
    UnsupE = {50., 50., 50., 50., 50.};

    // Withdrawals are flat and must respect HydroGen <= Pmax everywhere

    // 1. Algorithm tends to flatten TotaGenWithoutStorage + Withdrawals, so it would require
    //    withdrawals to increase.

    // Storage 1
    STS_holders[0].withdrawal = {20., 20., 20., 20., 20.}; // We have : withdrawal <= Pmax
    STS_holders[0].capacity = 1000.;
    STS_holders[0].initLevel = 500.;

    // Storage 2
    STS_holders[1].withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    STS_holders[1].capacity = 400.;
    STS_holders[1].initLevel = 200.;

    runRemixStorageAlgorithm();

    std::vector<double> expectedTotalWithdrawal = {0., 7.5, 27.5, 47.5, 67.5};
    std::vector<double> actualTotalWithdrawal = STS_holders[0].withdrawal
                                                + STS_holders[1].withdrawal;
    BOOST_TEST(actualTotalWithdrawal == expectedTotalWithdrawal, boost::test_tools::per_element());

    // 2. But withdrawal_1 and widrawal_2 is limited by pmax_1 and pmax_2. So Algo does nothing
    // in the end.
    UnsupE = {50., 50., 50., 50., 50.};

    std::ranges::fill(STS_holders[0].pmax, 20.);
    STS_holders[0].withdrawal = {20., 20., 20., 20., 20.}; // We have : withdrawal <= Pmax

    std::ranges::fill(STS_holders[1].pmax, 10.);
    STS_holders[1].withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax

    runRemixStorageAlgorithm();

    std::vector<double> expectedWithdrawal_1 = {20., 20., 20., 20., 20.};
    std::vector<double> expectedWithdrawal_2 = {10., 10., 10., 10., 10.};
    expectedTotalWithdrawal = expectedWithdrawal_1 + expectedWithdrawal_2;
    BOOST_CHECK(STS_holders[0].withdrawal + STS_holders[1].withdrawal == expectedTotalWithdrawal);

    std::vector<double> expectedUnsupE = {50., 50., 50., 50., 50.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(three_hydros_with_one_dominant_storage, InputFixture_5_3)
{
    STS_holders[0].withdrawal = {10., 20., 10., 20., 10.};      // Total = 70. Mean = 14.
    STS_holders[1].withdrawal = {10., 20., 10., 20., 10.};      // Total = 70. Mean = 14.
    STS_holders[2].withdrawal = {100., 200., 100., 200., 100.}; // Total = 700. Mean = 140.

    // Initial generation sum = {120., 240., 120., 240., 120.}
    // Total Mean = 168.0

    // 1. Set inflows to match the mean generation for each storage
    // This means if generation is flat, levels are flat.
    std::ranges::fill(STS_holders[0].inflows, 14.);
    std::ranges::fill(STS_holders[1].inflows, 14.);
    std::ranges::fill(STS_holders[2].inflows, 140.);

    // 2. Set non-binding levels and capacities
    STS_holders[0].initLevel = STS_holders[1].initLevel = STS_holders[2].initLevel = 1000.;
    STS_holders[0].capacity = STS_holders[1].capacity = STS_holders[2].capacity = 2000.;

    // 3. Set driving signals
    std::ranges::fill(TotaGenWithoutStorage, 0.);          // Flat
    UnsupE.assign(STS_holders[0].withdrawal.size(), 100.); // Zero UnsupE -> signal to flatten

    runRemixStorageAlgorithm();

    // --- Check Results ---

    // 1. Check that total hydro generation is perfectly flat
    std::vector<double> sumGeneration = STS_holders[0].withdrawal + STS_holders[1].withdrawal
                                        + STS_holders[2].withdrawal;

    std::vector<double> expected_sumGeneration = {168., 168., 168., 168., 168.};
    for (unsigned i = 0; i < sumGeneration.size(); ++i)
    {
        BOOST_CHECK_CLOSE(sumGeneration[i], expected_sumGeneration[i], 1e-3);
    }
}


BOOST_FIXTURE_TEST_CASE(flow_conservation_two_hydro_units, InputFixture_8_2)
{
    // ------------------------------
    // SETUP: Two reservoirs with distinct s1.inflows and generations
    // ------------------------------
    STS_holders[0].withdrawal = {10., 15., 20., 15., 10., 5., 10., 15.};
    STS_holders[1].withdrawal = {5., 10., 15., 10., 5., 0., 5., 10.};

    STS_holders[0].inflows = {12., 18., 20., 18., 12., 8., 12., 18.};
    STS_holders[1].inflows = {6., 12., 16., 12., 6., 2., 6., 12.};

    STS_holders[0].initLevel = 100.;
    STS_holders[1].initLevel = 50.;

    STS_holders[0].capacity = 500.;
    STS_holders[1].capacity = 300.;

    UnsupE.assign(STS_holders[0].withdrawal.size(), 0.); // Not relevant for flow conservation

    // ------------------------------
    // Record pre-call total s1.inflows, generations, and init s1.levels
    // ------------------------------
    double total_inflow_before = std::accumulate(STS_holders[0].inflows.begin(), STS_holders[0].inflows.end(), 0.0)
                                 + std::accumulate(STS_holders[1].inflows.begin(), STS_holders[1].inflows.end(), 0.0);

    double total_gen_before = std::accumulate(STS_holders[0].withdrawal.begin(), STS_holders[0].withdrawal.end(), 0.0)
                              + std::accumulate(STS_holders[1].withdrawal.begin(), STS_holders[1].withdrawal.end(), 0.0);

    double total_init_level = STS_holders[0].initLevel + STS_holders[1].initLevel;

    // ------------------------------
    // Run Remix algorithm
    // ------------------------------
    runRemixStorageAlgorithm();

    // ------------------------------
    // Compute flow balance after remix
    // ------------------------------
    double total_inflow_after = std::accumulate(STS_holders[0].inflows.begin(), STS_holders[0].inflows.end(), 0.0)
                                + std::accumulate(STS_holders[1].inflows.begin(), STS_holders[1].inflows.end(), 0.0);

    double total_gen_after = std::accumulate(STS_holders[0].withdrawal.begin(), STS_holders[0].withdrawal.end(), 0.0)
                             + std::accumulate(STS_holders[1].withdrawal.begin(), STS_holders[1].withdrawal.end(), 0.0);

    double total_final_level = STS_holders[0].levels.back() + STS_holders[1].levels.back();

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
