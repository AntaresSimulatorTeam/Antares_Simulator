#define BOOST_TEST_MODULE multi storage remix

#define WIN32_LEAN_AND_MEAN

#include <unit_test_utils.h>
#include <vector>

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

template<unsigned int nb_hours>
struct InputFixture
{
    InputFixture():
        Load(nb_hours, 0.),
        TotaGenWithoutStorage(nb_hours, 0.),
        UnsupE(nb_hours, 0.),
        Spillage(nb_hours, 0.),
        DTG_MRG(nb_hours, 0.)
    {
    }

    void runRemixStorageAlgorithm()
    {
        Load = TotaGenWithoutStorage + UnsupE + sts_1.withdrawal + sts_2.withdrawal;

        storagesForRemix.clear();
        storagesForRemix.push_back(sts_1.createSTS(UnsupE));
        storagesForRemix.push_back(sts_2.createSTS(UnsupE));

        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    STS_holder<nb_hours> sts_1;
    STS_holder<nb_hours> sts_2;

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

// ================================================
// Note :
//  G : Total generation without storage
//  H : Total storage production (or withdrawal)
// ================================================

BOOST_FIXTURE_TEST_CASE(G_is_flat___H_increases___G_plus_H_gets_flat, InputFixture<5>)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {80., 60., 40., 20., 0.};

    // Storage 1
    std::ranges::fill(sts_1.pmax, 40.);
    sts_1.withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    sts_1.capacity = 1000.;
    sts_1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts_2.pmax, 10.);
    sts_2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts_2.capacity = 400.;
    sts_2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts_1.withdrawal + sts_2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(same_test_as_above___we_just_raise_pmax___same_results, InputFixture<5>)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {80., 60., 40., 20., 0.};

    // Storage 1
    std::ranges::fill(sts_1.pmax, 50.);
    sts_1.withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    sts_1.capacity = 1000.;
    sts_1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts_2.pmax, 20.);
    sts_2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts_2.capacity = 400.;
    sts_2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts_1.withdrawal + sts_2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(G_is_flat___H_decreases___G_plus_H_gets_flat, InputFixture<5>)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {0., 20., 40., 60., 80.};

    // Storage 1
    std::ranges::fill(sts_1.pmax, 40.);
    sts_1.withdrawal = {40., 30., 20., 10., 0.}; // We have : withdrawal <= Pmax
    sts_1.capacity = 1000.;
    sts_1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts_2.pmax, 10.);
    sts_2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts_2.capacity = 400.;
    sts_2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts_1.withdrawal + sts_2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts_1.withdrawal + sts_2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {20., 30., 40., 50., 60.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(influence_of_pmax, InputFixture<5>, *boost::unit_test::tolerance(0.01))
{
    TotaGenWithoutStorage = {100., 80., 60., 40., 20.}; // Decreases
    UnsupE = {50., 50., 50., 50., 50.};

    // Withdrawals are flat and must respect HydroGen <= Pmax everywhere

    // 1. Algorithm tends to flatten TotaGenWithoutStorage + Withdrawals, so it would require
    //    withdrawals to increase.

    // Storage 1
    sts_1.withdrawal = {20., 20., 20., 20., 20.}; // We have : withdrawal <= Pmax
    sts_1.capacity = 1000.;
    sts_1.initLevel = 500.;

    // Storage 2
    sts_2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts_2.capacity = 400.;
    sts_2.initLevel = 200.;

    runRemixStorageAlgorithm();

    std::vector<double> expectedTotalWithdrawal = {0., 7.5, 27.5, 47.5, 67.5};
    std::vector<double> actualTotalWithdrawal = sts_1.withdrawal + sts_2.withdrawal;
    BOOST_TEST(actualTotalWithdrawal == expectedTotalWithdrawal, boost::test_tools::per_element());

    // 2. But withdrawal_1 and widrawal_2 is limited by pmax_1 and pmax_2. So Algo does nothing
    // in the end.
    UnsupE = {50., 50., 50., 50., 50.};

    std::ranges::fill(sts_1.pmax, 20.);
    sts_1.withdrawal = {20., 20., 20., 20., 20.}; // We have : withdrawal <= Pmax

    std::ranges::fill(sts_2.pmax, 10.);
    sts_2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax

    runRemixStorageAlgorithm();

    std::vector<double> expectedWithdrawal_1 = {20., 20., 20., 20., 20.};
    std::vector<double> expectedWithdrawal_2 = {10., 10., 10., 10., 10.};
    expectedTotalWithdrawal = expectedWithdrawal_1 + expectedWithdrawal_2;
    BOOST_CHECK(sts_1.withdrawal + sts_2.withdrawal == expectedTotalWithdrawal);

    std::vector<double> expectedUnsupE = {50., 50., 50., 50., 50.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}
