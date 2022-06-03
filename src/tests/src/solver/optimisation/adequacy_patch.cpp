#define BOOST_TEST_MODULE test adequacy patch functions

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include "adequacy_patch.h"

#include <vector>
#include <string>

static double origineExtremite = -1;
static double extremiteOrigine = 5;

using namespace Antares::Data::AdequacyPatch;

std::pair<double, double> one_timestep(AdequacyPatchMode originType,
                                       AdequacyPatchMode extremityType,
                                       bool SetNTCOutsideToOutsideToZero,
                                       bool SetNTCOutsideToInsideToZero)
{
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = originType;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = extremityType;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = SetNTCOutsideToOutsideToZero;
  adqPatchParams->SetNTCOutsideToInsideToZero = SetNTCOutsideToInsideToZero;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin;
  double Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  return std::make_pair(Xmin, Xmax);
}

BOOST_AUTO_TEST_CASE(one_timestep_virtual_virtual_no_change_expected)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmVirtualArea,
                                      adqmVirtualArea,
                                      true,
                                      false);
  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_virtual_inside_no_change_expected)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmVirtualArea,
                                      adqmPhysicalAreaInsideAdqPatch,
                                      true,
                                      false);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_virtual_outside_no_change_expected)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmVirtualArea,
                                      adqmPhysicalAreaOutsideAdqPatch,
                                      true,
                                      false);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_outside_zero_expected_both_directions)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmPhysicalAreaOutsideAdqPatch,
                                      adqmPhysicalAreaOutsideAdqPatch,
                                      true,
                                      false);

  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_outside_no_change_expected)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmPhysicalAreaOutsideAdqPatch,
                                      adqmPhysicalAreaOutsideAdqPatch,
                                      false,
                                      false);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_inside_outside_zero_expected_both_directions)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmPhysicalAreaInsideAdqPatch,
                                      adqmPhysicalAreaOutsideAdqPatch,
                                      false,
                                      false);

  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_inside_zero_expected_both_directions)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmPhysicalAreaOutsideAdqPatch,
                                      adqmPhysicalAreaInsideAdqPatch,
                                      false,
                                      true);
  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}


BOOST_AUTO_TEST_CASE(one_timestep_outside_inside_change_expected_one_direction)
{
  double Xmin, Xmax;
  std::tie(Xmin, Xmax) = one_timestep(adqmPhysicalAreaOutsideAdqPatch,
                                      adqmPhysicalAreaInsideAdqPatch,
                                      false,
                                      false);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == 0);
}
