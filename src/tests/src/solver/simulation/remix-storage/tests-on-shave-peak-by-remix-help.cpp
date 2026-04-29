#define BOOST_TEST_MODULE shave peaks by remix help

#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <ranges>
#include <set>
#include <unit_test_utils.h>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-help.h"

using namespace Antares::Solver::Simulation;

constexpr double largeValue = 1000;

template<size_t size>
struct StorageFixture
{
    StorageFixture()
    {
        maxCapacity = largeValue;

        // Garantees that bound from the 'no level' part of the storage is infinite
        pmax.assign(size, largeValue);
        withdrawal.assign(size, largeValue / 2);
        unsupE.assign(size, largeValue);
        levels.assign(size, maxCapacity);

        // Garantees that bound from 'level' part of the storage is infinite
        initLevel = maxCapacity / 2;
        levels.assign(size, initLevel);
        inflows = withdrawal; // compensate withdrawal (levels stay flat)
        upRuleCurve.assign(size, maxCapacity);
        lowRuleCurve.assign(size, 0.);

        injection.assign(size, 0.);
        ovf.assign(size, 0.);
    }

    std::shared_ptr<IStorageForRemix> createSTSstorage()
    {
        return makeSTSforRemix(withdrawal,
                               unsupE,
                               levels,
                               pmax,
                               inflows,
                               ovf,
                               injection,
                               lowRuleCurve,
                               upRuleCurve,
                               initLevel,
                               withdrawalEff,
                               injectionEff);
    }

    std::vector<double> withdrawal, unsupE, levels, pmax, inflows, ovf, injection;
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
        maxVariationGen = largeValue;
    }

    double maxVariationGen;
};

BOOST_FIXTURE_TEST_SUITE(compute_exchange, ComputeExchangeFixture<5>)

BOOST_AUTO_TEST_CASE(default_values_of_function_arguments_leads_to_a_large_exchange)
{
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 3;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    BOOST_CHECK_EQUAL(exchange, largeValue / 2);
}

BOOST_AUTO_TEST_CASE(maxVariationGen_is_bounded___exchange_gets_maxVariationGen_value)
{
    const unsigned hourOfMinGen = 2;
    const unsigned hourOfMaxGen = 4;
    maxVariationGen = 10;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    BOOST_CHECK_EQUAL(exchange, maxVariationGen / 2);
}

BOOST_AUTO_TEST_CASE(withdraw_at_hour_of_max_gen_is_bounded___exchange_gets_this_bound_value)
{
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 2;
    withdrawal[hourOfMaxGen] = 9;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

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

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    BOOST_CHECK_EQUAL(exchange, pmax[hourOfMinGen] - withdrawal[hourOfMinGen]);
}

BOOST_AUTO_TEST_CASE(unsup_energy_at_hour_of_min_gen_is_bounded___exchange_gets_this_bound_value)
{
    const unsigned hourOfMinGen = 0;
    const unsigned hourOfMaxGen = 4;
    unsupE[hourOfMinGen] = 7;
    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    BOOST_CHECK_EQUAL(exchange, unsupE[hourOfMinGen]);
}

BOOST_AUTO_TEST_CASE(limit_for_exchange_can_be__min_of_LEVELS__minus__LOW_RULE_CURVE)
{
    // In case hourOfMinGen < hourOfMaxGen, exchange is limited by :
    // min(levels - low rule curve) on subrange [hourOfMinGen, hourOfMaxGen - 1]
    // CAUTION : hourOfMaxGen is not part of subrange.
    const unsigned hourOfMinGen = 1;
    const unsigned hourOfMaxGen = 4;

    // Here levels decrease (but not mandatory for this test) :
    initLevel = 100;
    std::ranges::fill(withdrawal, 95);
    std::ranges::fill(inflows, 80);
    // So levels (computed internally at storage creation) should be : {85, 70, 55, 40, 25}

    // Low rules curve increases :
    lowRuleCurve = {5, 10, 14, 18, 20};

    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    double expectedExchange = levels[hourOfMaxGen - 1] - lowRuleCurve[hourOfMaxGen - 1];
    BOOST_CHECK_EQUAL(exchange, expectedExchange);
}

BOOST_AUTO_TEST_CASE(limit_for_exchange_can_be__min_of_UP_RULE_CURVE__minus__LEVELS)
{
    // In case hourOfMaxGen < hourOfMinGen, exchange is limited by :
    // min(ruleCurveUp - levels) on subrange [hourOfMaxGen, hourOfMinGen - 1]
    // CAUTION : hourOfMinGen is not part of subrange.
    const unsigned hourOfMinGen = 4;
    const unsigned hourOfMaxGen = 1;

    // Here levels increase (but not mandatory for this test) :
    initLevel = 100;
    std::ranges::fill(withdrawal, 25);
    std::ranges::fill(inflows, 35);
    // So levels (computed internally at storage creation) should be : {110, 120, 130, 140, 150}

    // Low rules curve decreases :
    upRuleCurve = {175, 170, 165, 160, 155};

    auto storage = createSTSstorage();

    auto exchange = computeExchange(hourOfMinGen, hourOfMaxGen, maxVariationGen, *storage);

    double expectedExchange = upRuleCurve[hourOfMinGen - 1] - levels[hourOfMinGen - 1];
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

BOOST_AUTO_TEST_SUITE(find_exhange)

BOOST_FIXTURE_TEST_CASE(totalGen_is_zero_everywhere___we_get_undefined_hours,
                        FindExchangeFixture<5>)
{
    auto storage = createSTSstorage();
    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK(!exchange.hourOfMinGen.has_value());
    BOOST_CHECK(!exchange.hourOfMaxGen.has_value());
}

BOOST_FIXTURE_TEST_CASE(totalGen_is_flat___we_get_undefined_hours, FindExchangeFixture<5>)
{
    std::ranges::fill(totalGen, 10);
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK(!exchange.hourOfMinGen.has_value());
    BOOST_CHECK(!exchange.hourOfMaxGen.has_value());
}

BOOST_FIXTURE_TEST_CASE(totalGen_is_decreasing___hourOfMaxGen_is_first_and_hourOfMinGen_last,
                        FindExchangeFixture<5>)
{
    totalGen = {15, 12, 10, 7, 4};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 0);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 4);
}

BOOST_FIXTURE_TEST_CASE(totalGen_is_increasing___hourOfMinGen_is_first_and_hourOfMaxGen_last,
                        FindExchangeFixture<5>)
{
    totalGen = {4, 7, 10, 12, 15};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 4);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 0);
}

BOOST_FIXTURE_TEST_CASE(totalGen_is_not_monotone__output_hours_are_as_expected,
                        FindExchangeFixture<5>)
{
    totalGen = {25, 2, 10, 60, 15};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 3);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 1);
}

BOOST_FIXTURE_TEST_CASE(valid_hours_is_a_strict_subrange__output_hours_are_as_expected,
                        FindExchangeFixture<9>)
{
    // Absolute hours for min and max totalGen are 0 and 8
    totalGen = {0, 25, 2, 10, 60, 15, 6, 75, 80};

    validHours = {2, 3, 4};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 4);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 2);
}

BOOST_FIXTURE_TEST_CASE(valid_hours_is_another_strict_subrange__output_hours_are_as_expected,
                        FindExchangeFixture<9>)
{
    // Absolute hours for min and max totalGen are 0 and 8
    totalGen = {0, 25, 2, 10, 60, 15, 6, 75, 80};

    validHours = {3, 4, 5, 6, 7};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 7);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 6);
}

BOOST_FIXTURE_TEST_CASE(valid_hours_is_not_contuguous__output_hours_are_as_expected,
                        FindExchangeFixture<9>)
{
    // Absolute hours for min and max totalGen are 0 and 8
    totalGen = {0, 25, 2, 10, 60, 15, 6, 75, 80};

    validHours = {1, 3, 6};
    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMaxGen.value_or(100), 1);
    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 6);
}

BOOST_FIXTURE_TEST_CASE(unsupE_is_zero_everywhere__valid_hours_is_empty__output_hours_undefined,
                        FindExchangeFixture<9>)
{
    // Absolute hours for min and max totalGen are 0 and 8
    totalGen = {0, 25, 2, 10, 60, 15, 6, 75, 80};
    std::ranges::fill(unsupE, 0); // Makes all hours invalid for hourOfMinGen

    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK(!exchange.hourOfMinGen.has_value());
    BOOST_CHECK(!exchange.hourOfMaxGen.has_value());
}

BOOST_FIXTURE_TEST_CASE(hourOfMinGen_can_only_be_in_the_subset_where_unsupE_is_non_null,
                        FindExchangeFixture<9>)
{
    // Absolute hours for min and max totalGen are 0 and 8
    totalGen = {0, 25, 2, 10, 60, 15, 6, 75, 80};
    std::ranges::fill(unsupE, 0); // Makes all hours invalid for hourOfMinGen
    unsupE[1] = 1;
    unsupE[3] = 1;
    unsupE[6] = 1;

    auto storage = createSTSstorage();

    auto exchange = searchForExhange(validHours, totalGen, unsupE, *storage);

    BOOST_CHECK_EQUAL(exchange.hourOfMinGen.value_or(100), 6);
}

BOOST_AUTO_TEST_SUITE_END()
