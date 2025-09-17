#define BOOST_TEST_MODULE multi storage remix

#define WIN32_LEAN_AND_MEAN

#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

using namespace Antares::Solver::Simulation;

template<unsigned int size>
struct STS_setup
{
    STS_setup()
    {
        withdrawal.assign(size, 0);
        injection.assign(size, 0);
        levels.assign(size, 0.);
        pmax.assign(size, std::numeric_limits<double>::max());
        inflows.assign(size, 0.);
        ovf.assign(size, 0.);
        lowRuleCurve.assign(size, 0.);
        upRuleCurve.assign(size, capacity);
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

template<unsigned int size>
struct InputFixture
{
    InputFixture()
    {
        Load.assign(size, 0.);
        TotaGenWithoutStorage.assign(size, 0.);
        UnsupE.assign(size, 0.);
        Spillage.assign(size, 0.);
        DTG_MRG.assign(size, 0.);
    }

    void runRemixStorageAlgorithm()
    {
        Load = TotaGenWithoutStorage + UnsupE + sts1.withdrawal + sts2.withdrawal;

        storagesForRemix.clear();
        storagesForRemix.push_back(sts1.createSTS(UnsupE));
        storagesForRemix.push_back(sts2.createSTS(UnsupE));
        shavePeaksByRemixingStorageGen(Load, UnsupE, Spillage, DTG_MRG, storagesForRemix);
    }

    STS_setup<size> sts1;
    STS_setup<size> sts2;

    std::vector<double> Load, TotaGenWithoutStorage, UnsupE, Spillage, DTG_MRG;

    ListStorageForRemix storagesForRemix;
};

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
    std::ranges::fill(sts1.pmax, 40.);
    sts1.withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    sts1.capacity = 1000.;
    sts1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts2.pmax, 10.);
    sts2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts2.capacity = 400.;
    sts2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts1.withdrawal + sts2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(same_test_as_above___we_just_raise_pmax___same_results, InputFixture<5>)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {80., 60., 40., 20., 0.};

    // Storage 1
    std::ranges::fill(sts1.pmax, 50.);
    sts1.withdrawal = {0., 10., 20., 30., 40.}; // We have : withdrawal <= Pmax
    sts1.capacity = 1000.;
    sts1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts2.pmax, 20.);
    sts2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts2.capacity = 400.;
    sts2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts1.withdrawal + sts2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {60., 50., 40., 30., 20.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}

BOOST_FIXTURE_TEST_CASE(G_is_flat___H_decreases___G_plus_H_gets_flat, InputFixture<5>)
{
    std::ranges::fill(TotaGenWithoutStorage, 100.);
    UnsupE = {0., 20., 40., 60., 80.};

    // Storage 1
    std::ranges::fill(sts1.pmax, 40.);
    sts1.withdrawal = {40., 30., 20., 10., 0.}; // We have : withdrawal <= Pmax
    sts1.capacity = 1000.;
    sts1.initLevel = 500.;

    // Storage 2
    std::ranges::fill(sts2.pmax, 10.);
    sts2.withdrawal = {10., 10., 10., 10., 10.}; // We have : withdrawal <= Pmax
    sts2.capacity = 400.;
    sts2.initLevel = 200.;

    runRemixStorageAlgorithm();

    // G + H (= TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal) gets flat
    std::vector<double> expectedTotalWithdrawal = {30., 30., 30., 30., 30.};
    std::vector<double> actualTotalWithdrawal = sts1.withdrawal + sts2.withdrawal;
    BOOST_CHECK(actualTotalWithdrawal == expectedTotalWithdrawal);

    // UnsupE such as TotaGenWithoutStorage + sts1.withdrawal + sts2.withdrawal gets flat
    std::vector<double> expectedUnsupE = {20., 30., 40., 50., 60.};
    BOOST_CHECK(UnsupE == expectedUnsupE);
}
