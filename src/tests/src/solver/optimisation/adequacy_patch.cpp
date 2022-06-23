#define BOOST_TEST_MODULE test adequacy patch functions

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include "adequacy_patch.h"

#include <vector>
#include <tuple>

static double origineExtremite = -1;
static double extremiteOrigine = 5;

using namespace Antares::Data::AdequacyPatch;

// NOTE
// Xmax limits the flux origin -> extremity (direct)
// -Xmin limits the flux extremity -> origin (indirect)

std::pair<double, double> setNTCboundsForOneTimeStep(AdequacyPatchMode originType,
                                                     AdequacyPatchMode extremityType,
                                                     bool SetNTCOutsideToOutsideToZero,
                                                     bool SetNTCOutsideToInsideToZero)
{
    PROBLEME_HEBDO problem;
    problem.adequacyPatchRuntimeData.originAreaMode.resize(1);
    problem.adequacyPatchRuntimeData.extremityAreaMode.resize(1);

    problem.adequacyPatchRuntimeData.originAreaMode[0] = originType;
    problem.adequacyPatchRuntimeData.extremityAreaMode[0] = extremityType;
    problem.adqPatchParams
      = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
    auto& adqPatchParams = problem.adqPatchParams;

    adqPatchParams->AdequacyFirstStep = true;
    adqPatchParams->SetNTCOutsideToOutsideToZero = SetNTCOutsideToOutsideToZero;
    adqPatchParams->SetNTCOutsideToInsideToZero = SetNTCOutsideToInsideToZero;

    VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
    ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
    ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

    double Xmin;
    double Xmax;

    setNTCbounds(Xmax, Xmin, &ValeursDeNTC, 0, &problem);

    return std::make_pair(Xmin, Xmax);
}

// Virtual -> Virtual (0 -> 0)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_virtual_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, virtualArea, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_TEST(Xmax == origineExtremite);
    BOOST_TEST(Xmin == -extremiteOrigine);
}

// Virtual -> physical area inside adq-patch (0 -> 2)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_inside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, physicalAreaInsideAdqPatch, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_TEST(Xmax == origineExtremite);
    BOOST_TEST(Xmin == -extremiteOrigine);
}

// Virtual -> physical area outside adq-patch (0 -> 1)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_outside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, physicalAreaOutsideAdqPatch, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_TEST(Xmax == origineExtremite);
    BOOST_TEST(Xmin == -extremiteOrigine);
}

// physical area outside adq-patch -> physical area outside adq-patch (1 -> 1)
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_outside_zero_expected_both_directions)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(physicalAreaOutsideAdqPatch,
                                                      physicalAreaOutsideAdqPatch,
                                                      true /*SetNTCOutsideToOutsideToZero*/,
                                                      false);
    BOOST_TEST(Xmax == 0);
    BOOST_TEST(Xmin == 0);
}

// physical area outside adq-patch -> physical area outside adq-patch (1 -> 1)
// SetNTCOutsideToOutsideToZero = true
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_outside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaOutsideAdqPatch, physicalAreaOutsideAdqPatch, false, false);

    BOOST_TEST(Xmax == origineExtremite);
    BOOST_TEST(Xmin == -extremiteOrigine);
}

// physical area inside adq-patch -> physical area outside adq-patch (2 -> 1)
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_inside_outside_zero_expected_both_directions)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaInsideAdqPatch, physicalAreaOutsideAdqPatch, false, false);
    BOOST_TEST(Xmax == 0);
    BOOST_TEST(Xmin == 0);
}

// physical area outside adq-patch -> physical area inside adq-patch (1 -> 2)
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_inside_zero_expected_both_directions)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(physicalAreaOutsideAdqPatch,
                                                      physicalAreaInsideAdqPatch,
                                                      false,
                                                      true /*SetNTCOutsideToInsideToZero*/);
    BOOST_TEST(Xmax == 0);
    BOOST_TEST(Xmin == 0);
}

// physical area outside adq-patch -> physical area inside adq-patch (1 -> 2)
// NTC should be unchanged in direction origin->extremity (direct)
// NTC should be set to 0 in direction extremity->origin (indirect)
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_inside_change_expected_one_direction)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaOutsideAdqPatch, physicalAreaInsideAdqPatch, false, false);
    BOOST_TEST(Xmax == origineExtremite);
    BOOST_TEST(Xmin == 0);
}
