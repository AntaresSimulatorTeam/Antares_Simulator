#define BOOST_TEST_MODULE test adequacy patch functions

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include "adequacy_patch.h"

#include <vector>
#include <string>

BOOST_AUTO_TEST_CASE(one_timestep_virtual_virtual_no_change_expected)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmVirtualArea;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmVirtualArea;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = true;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_virtual_inside_no_change_expected)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmVirtualArea;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaInsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = true;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_virtual_outside_no_change_expected)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmVirtualArea;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = true;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_outside_zero_expected_both_directions)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = true;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_outside_no_change_expected)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = false;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == -extremiteOrigine);
}

BOOST_AUTO_TEST_CASE(one_timestep_inside_outside_zero_expected_both_directions)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmPhysicalAreaInsideAdqPatch;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = false;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;

  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}

BOOST_AUTO_TEST_CASE(one_timestep_outside_inside_zero_expected_both_directions)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaInsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = false;
  adqPatchParams->SetNTCOutsideToInsideToZero = true;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;
  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == 0);
  BOOST_TEST(Xmin == 0);
}


BOOST_AUTO_TEST_CASE(one_timestep_outside_inside_change_expected_one_direction)
{
  using namespace Antares::Data::AdequacyPatch;
  PROBLEME_HEBDO problem;
  problem.adequacyPatchRuntimeData.originAreaType.resize(1);
  problem.adequacyPatchRuntimeData.extremityAreaType.resize(1);

  problem.adequacyPatchRuntimeData.originAreaType[0] = adqmPhysicalAreaOutsideAdqPatch;
  problem.adequacyPatchRuntimeData.extremityAreaType[0] = adqmPhysicalAreaInsideAdqPatch;
  problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
  auto& adqPatchParams = problem.adqPatch;

  adqPatchParams->AdequacyFirstStep = true;
  adqPatchParams->SetNTCOutsideToOutsideToZero = false;
  adqPatchParams->SetNTCOutsideToInsideToZero = false;

  VALEURS_DE_NTC_ET_RESISTANCES ValeursDeNTC;
  double origineExtremite = -1, extremiteOrigine = 5;
  ValeursDeNTC.ValeurDeNTCOrigineVersExtremite = &origineExtremite;
  ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine = &extremiteOrigine;

  double Xmin, Xmax;
  setBoundsAdqPatch(Xmax,
                    Xmin,
                    &ValeursDeNTC,
                    0,
                    &problem);

  BOOST_TEST(Xmax == origineExtremite);
  BOOST_TEST(Xmin == 0);
}
