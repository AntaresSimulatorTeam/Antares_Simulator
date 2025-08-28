#define BOOST_TEST_MODULE shave peaks by remix help

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <ranges>
#include <set>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-help.h"
#include "antares/solver/simulation/remix-storage/storage-for-remix.h"

using namespace Antares::Solver::Simulation;

template<size_t size>
struct StorageFixture
{
    StorageFixture()
    {
        maxCapacity = std::numeric_limits<double>::max();

        withdrawal.assign(size, 0.);
        injection.assign(size, 0.);
        unsupE.assign(size, 0.);
        levels.assign(size, 0.);
        pmax.assign(size, 0.);
        inflows.assign(size, 0.);
        lowRuleCurve.assign(size, 0.);
        upRuleCurve.assign(size, maxCapacity);
    }

    std::shared_ptr<StorageForRemixWithLevels> createSTSstorage()
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

    std::vector<double> withdrawal, unsupE, levels, pmax, inflows, injection;
    std::vector<double> lowRuleCurve, upRuleCurve;
    double maxCapacity;
    double initLevel = 0;
    double withdrawalEff = 1;
    double injectionEff = 1;
};

template<size_t size>
struct FindExchangeFixture: public StorageFixture<size>
{
    FindExchangeFixture():
        StorageFixture<size>()
    {
        auto iotaView = std::views::iota(0, (int)size);
        validHours = {iotaView.begin(), iotaView.end()};

        totalGen.assign(size, 0);
    }

    std::set<unsigned> validHours;
    std::vector<double> totalGen;
};

BOOST_FIXTURE_TEST_SUITE(find_exhange, FindExchangeFixture<5>)

BOOST_AUTO_TEST_CASE(dummy)
{
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

template<size_t size>
struct ComputeExchangeFixture: public StorageFixture<size>
{
    ComputeExchangeFixture():
        StorageFixture<size>()
    {
        totalGen.assign(size, 0);
    }

    std::vector<double> totalGen;
};

BOOST_FIXTURE_TEST_SUITE(compute_exchange, StorageFixture<5>)

BOOST_AUTO_TEST_CASE(dummy)
{
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 4;
    const double maxVariationGen = 10;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
