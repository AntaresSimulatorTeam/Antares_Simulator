#define BOOST_TEST_MODULE test adequacy patch functions

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include "adequacy_patch_local_matching/adq_patch_local_matching.h"
#include "adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include <adequacy_patch_runtime_data.h>
#include "antares/study/parameters/adq-patch-params.h"
#include <antares/exception/LoadingError.hpp>
#include <fstream>

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
    problem.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>();
    problem.adequacyPatchRuntimeData->originAreaMode.resize(1);
    problem.adequacyPatchRuntimeData->extremityAreaMode.resize(1);

    problem.adequacyPatchRuntimeData->originAreaMode[0] = originType;
    problem.adequacyPatchRuntimeData->extremityAreaMode[0] = extremityType;
    problem.adequacyPatchRuntimeData->AdequacyFirstStep = true;

    AdqPatchParams adqPatchParams;
    adqPatchParams.enabled = true;
    adqPatchParams.localMatching.setToZeroOutsideOutsideLinks = SetNTCOutsideToOutsideToZero;
    adqPatchParams.localMatching.setToZeroOutsideInsideLinks = SetNTCOutsideToInsideToZero;

    VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
    ValeursDeNTC.ValeurDeNTCOrigineVersExtremite.assign(1, 0.);
    ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine.assign(1, 0.);
    ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[0] = origineExtremite;
    ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[0] = extremiteOrigine;

    double Xmin(0.);
    double Xmax(0.);

    setNTCbounds(Xmax, Xmin, ValeursDeNTC, 0, &problem, adqPatchParams);

    return std::make_pair(Xmin, Xmax);
}

static const double flowArea0toArea1_positive = 10;
static const double flowArea0toArea1_negative = -10;
static const double flowArea2toArea0_positive = 30;
static const double flowArea2toArea0_negative = -30;
static const double positiveEnsInit = 50.0;

std::pair<double, double> calculateAreaFlowBalanceForOneTimeStep(
  double ensInit,
  bool includeFlowsOutsideAdqPatchToDensNew,
  AdequacyPatchMode Area1Mode,
  AdequacyPatchMode Area2Mode,
  double flowToArea1,
  double flowFromArea2)
{
    PROBLEME_HEBDO problem;
    int Area = 0;
    uint hour = 0;

    // allocate memory
    problem.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>();
    problem.adequacyPatchRuntimeData->originAreaMode.resize(3);
    problem.adequacyPatchRuntimeData->extremityAreaMode.resize(3);

    AdqPatchParams adqPatchParams;

    problem.ResultatsHoraires.resize(1);
    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive = new double;
    problem.ValeursDeNTC = std::vector<VALEURS_DE_NTC_ET_RESISTANCES>(1);
    problem.ValeursDeNTC[0].ValeurDuFlux = std::vector<double>(3);
    problem.IndexSuivantIntercoOrigine = std::vector<int>(3);
    problem.IndexSuivantIntercoExtremite = std::vector<int>(3);
    problem.IndexDebutIntercoOrigine = std::vector<int>(1);
    problem.IndexDebutIntercoExtremite = std::vector<int>(1);

    // input values
    adqPatchParams.localMatching.setToZeroOutsideInsideLinks = !includeFlowsOutsideAdqPatchToDensNew;
    problem.ResultatsHoraires[Area].ValeursHorairesDeDefaillancePositive[hour] = ensInit;
    int Interco = 1;
    problem.IndexDebutIntercoOrigine[Area] = Interco;
    problem.adequacyPatchRuntimeData->extremityAreaMode[Interco] = Area1Mode;
    problem.ValeursDeNTC[hour].ValeurDuFlux[Interco] = flowToArea1;
    problem.IndexSuivantIntercoOrigine[Interco] = -1;

    Interco = 2;
    problem.IndexDebutIntercoExtremite[Area] = Interco;
    problem.adequacyPatchRuntimeData->originAreaMode[Interco] = Area2Mode;
    problem.ValeursDeNTC[hour].ValeurDuFlux[Interco] = flowFromArea2;
    problem.IndexSuivantIntercoExtremite[Interco] = -1;

    // get results
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew, std::ignore)
        = calculateAreaFlowBalance(&problem, adqPatchParams.localMatching.setToZeroOutsideInsideLinks, Area, hour);

    // free memory
    delete problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive;

    return std::make_pair(netPositionInit, densNew);
}

AdqPatchParams createParams()
{
    AdqPatchParams p;
    p.enabled = true;
    p.curtailmentSharing.includeHurdleCost = true;
    p.localMatching.enabled = true;
    p.curtailmentSharing.priceTakingOrder = AdqPatchPTO::isDens;

    return p;
}

// Virtual -> Virtual (0 -> 0)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_virtual_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, virtualArea, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_CHECK_EQUAL(Xmax, origineExtremite);
    BOOST_CHECK_EQUAL(Xmin, -extremiteOrigine);
}

// Virtual -> physical area inside adq-patch (0 -> 2)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_inside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, physicalAreaInsideAdqPatch, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_CHECK_EQUAL(Xmax, origineExtremite);
    BOOST_CHECK_EQUAL(Xmin, -extremiteOrigine);
}

// Virtual -> physical area outside adq-patch (0 -> 1)
// No change in bounds is expected
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_virtual_outside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      virtualArea, physicalAreaOutsideAdqPatch, true /*SetNTCOutsideToOutsideToZero*/, false);
    BOOST_CHECK_EQUAL(Xmax, origineExtremite);
    BOOST_CHECK_EQUAL(Xmin, -extremiteOrigine);
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
    BOOST_CHECK_EQUAL(Xmax, 0);
    BOOST_CHECK_EQUAL(Xmin, 0);
}

// physical area outside adq-patch -> physical area outside adq-patch (1 -> 1)
// SetNTCOutsideToOutsideToZero = true
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_outside_no_change_expected)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaOutsideAdqPatch, physicalAreaOutsideAdqPatch, false, false);

    BOOST_CHECK_EQUAL(Xmax, origineExtremite);
    BOOST_CHECK_EQUAL(Xmin, -extremiteOrigine);
}

// physical area inside adq-patch -> physical area outside adq-patch (2 -> 1)
// NTC should be set to 0 in both directions
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_inside_outside_zero_expected_both_directions)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaInsideAdqPatch, physicalAreaOutsideAdqPatch, false, false);
    BOOST_CHECK_EQUAL(Xmax, 0);
    BOOST_CHECK_EQUAL(Xmin, 0);
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
    BOOST_CHECK_EQUAL(Xmax, 0);
    BOOST_CHECK_EQUAL(Xmin, 0);
}

// physical area outside adq-patch -> physical area inside adq-patch (1 -> 2)
// NTC should be unchanged in direction origin->extremity (direct)
// NTC should be set to 0 in direction extremity->origin (indirect)
BOOST_AUTO_TEST_CASE(setNTCboundsForOneTimeStep_outside_inside_change_expected_one_direction)
{
    double Xmin, Xmax;
    std::tie(Xmin, Xmax) = setNTCboundsForOneTimeStep(
      physicalAreaOutsideAdqPatch, physicalAreaInsideAdqPatch, false, false);
    BOOST_CHECK_EQUAL(Xmax, origineExtremite);
    BOOST_CHECK_EQUAL(Xmin, 0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 virtual-area, and Area2-virtual area
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_virtual_virtual_NotIncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0, false, virtualArea, virtualArea, flowArea0toArea1_positive, flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, 0.0);
    BOOST_CHECK_EQUAL(densNew, 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2-virtual area
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_virtual_NotIncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               false,
                                               physicalAreaInsideAdqPatch,
                                               virtualArea,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive);
    BOOST_CHECK_EQUAL(densNew, 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2-virtual area
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
// ensInit = 50.0
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_virtual_NotIncludeOut_positiveFlow_ensInitGraterThanZero)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(positiveEnsInit,
                                               false,
                                               physicalAreaInsideAdqPatch,
                                               virtualArea,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive);
    BOOST_CHECK_EQUAL(densNew, positiveEnsInit - flowArea0toArea1_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_outside_NotIncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               false,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive);
    BOOST_CHECK_EQUAL(densNew, 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_inside_NotIncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               false,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive + flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(densNew, -flowArea0toArea1_positive + flowArea2toArea0_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_inside_outside_IncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive);
    BOOST_CHECK_EQUAL(densNew, -flowArea0toArea1_positive + flowArea2toArea0_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_outside_IncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, 0.0);
    BOOST_CHECK_EQUAL(densNew, flowArea2toArea0_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_positiveFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(netPositionInit, +flowArea2toArea0_positive);
    BOOST_CHECK_EQUAL(densNew, +flowArea2toArea0_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_inside_outside_IncludeOut_negativeFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea0toArea1_negative,
                                               flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_negative);
    BOOST_CHECK_EQUAL(densNew, -flowArea0toArea1_negative);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_outside_IncludeOut_negativeFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea0toArea1_negative,
                                               flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(netPositionInit, 0.0);
    BOOST_CHECK_EQUAL(densNew, -flowArea0toArea1_negative);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(0.0,
                                               true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea0toArea1_negative,
                                               flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(netPositionInit, flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(densNew, 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positiive
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
// ensInit = 50.0
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow_initEnsEqualTo50)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(positiveEnsInit,
                                               true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive + flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(densNew, positiveEnsInit + netPositionInit);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positiive
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
// ensInit = 2.0
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow_initEnsEqualTo0)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(2.0,
                                               true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea0toArea1_positive,
                                               flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(netPositionInit, -flowArea0toArea1_positive + flowArea2toArea0_negative);
    BOOST_CHECK_EQUAL(densNew, 0.0);
}

BOOST_AUTO_TEST_CASE(check_valid_adq_param)
{
    auto p = createParams();
    BOOST_CHECK_NO_THROW(p.checkAdqPatchStudyModeEconomyOnly(Antares::Data::stdmEconomy));
    BOOST_CHECK_NO_THROW(p.checkAdqPatchIncludeHurdleCost(true));
    BOOST_CHECK_NO_THROW(p.checkAdqPatchDisabledLocalMatching());
}

BOOST_AUTO_TEST_CASE(check_adq_param_wrong_mode)
{
    auto p = createParams();
    BOOST_CHECK_THROW(p.checkAdqPatchStudyModeEconomyOnly(Antares::Data::stdmAdequacy),
            Error::IncompatibleStudyModeForAdqPatch);
}

BOOST_AUTO_TEST_CASE(check_adq_param_wrong_hurdle_cost)
{
    auto p = createParams();
    BOOST_CHECK_THROW(p.checkAdqPatchIncludeHurdleCost(false), Error::IncompatibleHurdleCostCSR);
}

BOOST_AUTO_TEST_CASE(check_adq_param_wrong_lmr_disabled)
{
    auto p = createParams();
    p.localMatching.enabled = false;
    BOOST_CHECK_THROW(p.checkAdqPatchDisabledLocalMatching(), Error::AdqPatchDisabledLMR);
}
