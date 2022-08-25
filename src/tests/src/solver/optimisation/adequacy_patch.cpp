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

static const double flowArea1toArea2_positive = 10;
static const double flowArea1toArea2_negative = -10;
static const double flowArea3toArea1_positive = 30;
static const double flowArea3toArea1_negative = -30;

std::pair<double, double> calculateAreaFlowBalanceForOneTimeStep(
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
    problem.adequacyPatchRuntimeData.originAreaMode.resize(3);
    problem.adequacyPatchRuntimeData.extremityAreaMode.resize(3);

    problem.adqPatchParams
      = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
    auto& adqPatchParams = problem.adqPatchParams;

    problem.ResultatsHoraires = (RESULTATS_HORAIRES**)malloc(1 * sizeof(void*));
    problem.ResultatsHoraires[0] = (RESULTATS_HORAIRES*)malloc(sizeof(RESULTATS_HORAIRES));
    problem.ResultatsHoraires[0]->ValeursHorairesDeDefaillancePositive
      = (double*)malloc(1 * sizeof(double));
    problem.ValeursDeNTC = (VALEURS_DE_NTC_ET_RESISTANCES**)malloc(1 * sizeof(void*));
    problem.ValeursDeNTC[0]
      = (VALEURS_DE_NTC_ET_RESISTANCES*)malloc(sizeof(VALEURS_DE_NTC_ET_RESISTANCES));
    problem.ValeursDeNTC[0]->ValeurDuFlux = (double*)malloc(3 * sizeof(double));
    problem.IndexSuivantIntercoOrigine = (int*)malloc(3 * sizeof(int));
    problem.IndexSuivantIntercoExtremite = (int*)malloc(3 * sizeof(int));
    problem.IndexDebutIntercoOrigine = (int*)malloc(1 * sizeof(int));
    problem.IndexDebutIntercoExtremite = (int*)malloc(1 * sizeof(int));

    // input values
    adqPatchParams->SetNTCOutsideToInsideToZero = !includeFlowsOutsideAdqPatchToDensNew;
    problem.ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour] = 0.0;
    int Interco = 1;
    problem.IndexDebutIntercoOrigine[Area] = Interco;
    problem.adequacyPatchRuntimeData.extremityAreaMode[Interco] = Area1Mode;
    problem.ValeursDeNTC[hour]->ValeurDuFlux[Interco] = flowToArea1;
    problem.IndexSuivantIntercoOrigine[Interco] = -1;

    Interco = 2;
    problem.IndexDebutIntercoExtremite[Area] = Interco;
    problem.adequacyPatchRuntimeData.originAreaMode[Interco] = Area2Mode;
    problem.ValeursDeNTC[hour]->ValeurDuFlux[Interco] = flowFromArea2;
    problem.IndexSuivantIntercoExtremite[Interco] = -1;

    // get results
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalance(&problem, Area, hour);

    // free memory
    free(problem.IndexDebutIntercoExtremite);
    free(problem.IndexSuivantIntercoExtremite);
    free(problem.IndexDebutIntercoOrigine);
    free(problem.IndexSuivantIntercoOrigine);
    free(problem.ValeursDeNTC[0]->ValeurDuFlux);
    free(problem.ValeursDeNTC[0]);
    free(problem.ValeursDeNTC);
    free(problem.ResultatsHoraires[0]->ValeursHorairesDeDefaillancePositive);
    free(problem.ResultatsHoraires[0]);
    free(problem.ResultatsHoraires);

    // return
    return std::make_pair(netPositionInit, densNew);
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

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 virtual-area, and Area2-virtual area
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_virtual_virtual_NotIncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      false, virtualArea, virtualArea, flowArea1toArea2_positive, flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == 0.0);
    BOOST_TEST(densNew == 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2-virtual area
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_virtual_NotIncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(false,
                                               physicalAreaInsideAdqPatch,
                                               virtualArea,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == -flowArea1toArea2_positive);
    BOOST_TEST(densNew == 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_outside_NotIncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(false,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == -flowArea1toArea2_positive);
    BOOST_TEST(densNew == 0.0);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter should NOT include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_inside_inside_NotIncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(false,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == -flowArea1toArea2_positive + flowArea3toArea1_positive);
    BOOST_TEST(densNew == -flowArea1toArea2_positive + flowArea3toArea1_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_inside_outside_IncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == -flowArea1toArea2_positive);
    BOOST_TEST(densNew == -flowArea1toArea2_positive + flowArea3toArea1_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_outside_IncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == 0.0);
    BOOST_TEST(densNew == flowArea3toArea1_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is positive
// flow from Area2 -> Area0 is positive
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_positiveFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea1toArea2_positive,
                                               flowArea3toArea1_positive);
    BOOST_TEST(netPositionInit == +flowArea3toArea1_positive);
    BOOST_TEST(densNew == +flowArea3toArea1_positive);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area inside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_inside_outside_IncludeOut_negativeFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaInsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea1toArea2_negative,
                                               flowArea3toArea1_negative);
    BOOST_TEST(netPositionInit == -flowArea1toArea2_negative);
    BOOST_TEST(densNew == -flowArea1toArea2_negative);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area outside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_outside_IncludeOut_negativeFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaOutsideAdqPatch,
                                               flowArea1toArea2_negative,
                                               flowArea3toArea1_negative);
    BOOST_TEST(netPositionInit == 0.0);
    BOOST_TEST(densNew == -flowArea1toArea2_negative);
}

// Area 0 is physical area inside adq-patch connected to two areas:
// Area1 physical area outside adq-patch, and Area2 physical area inside adq-patch
// flow from Area0 -> Area1 is negative
// flow from Area2 -> Area0 is negative
// DensNew parameter SHOULD include flows from areas outside adq patch
BOOST_AUTO_TEST_CASE(calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow)
{
    double netPositionInit, densNew;
    std::tie(netPositionInit, densNew)
      = calculateAreaFlowBalanceForOneTimeStep(true,
                                               physicalAreaOutsideAdqPatch,
                                               physicalAreaInsideAdqPatch,
                                               flowArea1toArea2_negative,
                                               flowArea3toArea1_negative);
    BOOST_TEST(netPositionInit == flowArea3toArea1_negative);
    BOOST_TEST(densNew == 0.0);
}
