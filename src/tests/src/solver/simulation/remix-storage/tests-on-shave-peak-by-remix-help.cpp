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

constexpr double infinity = 1000;

template<size_t size>
struct StorageFixture
{
    StorageFixture()
    {
        maxCapacity = infinity;

        // Garantees that bound from the 'no level' part of the storage is infinite
        pmax.assign(size, infinity);
        withdrawal.assign(size, infinity / 2);
        unsupE.assign(size, infinity);
        levels.assign(size, maxCapacity);

        // Garantees that bound from 'level' part of the storage is infinite
        initLevel = maxCapacity / 2;
        levels.assign(size, initLevel);
        inflows = withdrawal; // compensate withdrawal (levels stay flat)
        upRuleCurve.assign(size, maxCapacity);
        lowRuleCurve.assign(size, 0.);

        injection.assign(size, 0.);
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
    double initLevel;
    double withdrawalEff = 1;
    double injectionEff = 1;
};

template<size_t size>
struct ComputeExchangeFixture: public StorageFixture<size>
{
    ComputeExchangeFixture():
        StorageFixture<size>()
    {
        maxVariationGen = infinity;
    }

    double maxVariationGen;
};

BOOST_FIXTURE_TEST_SUITE(compute_exchange, ComputeExchangeFixture<5>)

BOOST_AUTO_TEST_CASE(default_values_of_function_arguments_leads_to_an_infinity)
{
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 3;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK_EQUAL(exchange, infinity / 2);
}

BOOST_AUTO_TEST_CASE(maxVariationGen_is_bounded___exchange_gets_maxVariationGen_value)
{
    const unsigned hourOfMinGen = 2;
    const unsigned hourOfMaxGen = 4;
    maxVariationGen = 10;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK_EQUAL(exchange, maxVariationGen / 2);
}

BOOST_AUTO_TEST_CASE(withdraw_at_hour_of_max_gen_is_bounded___exchange_gets_this_bound_value)
{
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 2;
    withdrawal[hourOfMaxGen] = 9;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK_EQUAL(exchange, withdrawal[hourOfMaxGen]);
}

BOOST_AUTO_TEST_CASE(
  pmax_minus_withdrawal_at_hour_of_min_gen_is_bounded___exchange_gets_this_bound_value)
{
    const unsigned hourOfMinGen = 0;
    const unsigned hourOfMaxGen = 4;
    pmax[hourOfMinGen] = 10;
    withdrawal[hourOfMinGen] = 5;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK_EQUAL(exchange, pmax[hourOfMinGen] - withdrawal[hourOfMinGen]);
}

BOOST_AUTO_TEST_CASE(unsup_energy_at_hour_of_min_gen_is_bounded___exchange_gets_this_bound_value)
{
    const unsigned hourOfMinGen = 0;
    const unsigned hourOfMaxGen = 4;
    unsupE[hourOfMinGen] = 7;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    BOOST_CHECK_EQUAL(exchange, unsupE[hourOfMinGen]);
}

BOOST_AUTO_TEST_CASE(
  limit_for_exchange_can_be__min_of_levels_minus_low_rule_curve__at_hour_of_min_gen)
{
    // In case hourOfMinGen < hourOfMaxGen, exchange is limited by :
    // min(levels - low rule curve) on subrange [hourOfMinGen, hourOfMaxGen]
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 4;

    // Levels decrease :
    initLevel = 100;
    std::ranges::fill(withdrawal, 50);
    std::ranges::fill(inflows, 40);
    // So levels (computed internally at storage creation) should be : {90, 80, 70, 60, 50}

    // Low rules curve increases :
    lowRuleCurve = {5, 10, 15, 20, 25};

    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, storage);

    double expectedExchange = levels[hourOfMaxGen] - lowRuleCurve[hourOfMaxGen];
    BOOST_CHECK_EQUAL(exchange, expectedExchange);
}

BOOST_AUTO_TEST_SUITE_END()

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
