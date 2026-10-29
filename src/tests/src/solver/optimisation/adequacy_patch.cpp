#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <tuple>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <antares/exception/LoadingError.hpp>
#include <antares/solver/simulation/adequacy_patch_runtime_data.h>
#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"
#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/study/parameters/adq-patch-params.h"

#include "in-memory-study.h"

using namespace Antares::Data::AdequacyPatch;
namespace tt = boost::test_tools;

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
    problem.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive = std::vector<double>(1);
    problem.ValeursDeNTC = std::vector<VALEURS_DE_NTC_ET_RESISTANCES>(1);
    problem.ValeursDeNTC[0].ValeurDuFlux = std::vector<double>(3);
    problem.IndexSuivantIntercoOrigine = std::vector<int>(3);
    problem.IndexSuivantIntercoExtremite = std::vector<int>(3);
    problem.IndexDebutIntercoOrigine = std::vector<int>(1);
    problem.IndexDebutIntercoExtremite = std::vector<int>(1);

    // input values
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
    std::tie(netPositionInit, densNew, std::ignore) = calculateAreaFlowBalance(
      &problem,
      !includeFlowsOutsideAdqPatchToDensNew,
      Area,
      hour);

    return std::make_pair(netPositionInit, densNew);
}

AdqPatchParams createParams()
{
    AdqPatchParams p;
    p.enabled = true;
    p.curtailmentSharing.includeHurdleCost = true;
    p.curtailmentSharing.priceTakingOrder = AdqPatchPTO::isDens;

    return p;
}

BOOST_AUTO_TEST_SUITE(_AdequacyPatch_)

BOOST_AUTO_TEST_CASE(curtailment_sharing_uses_effective_capacities_in_both_directions)
{
    for (const bool direct: {true, false})
    {
        for (const double legacyCapacity: {0., 100.})
        {
            for (const double effectiveCapacity:
                 {10., 100., std::numeric_limits<double>::quiet_NaN()})
            {
                Antares::Data::Study study;
                auto* origin = addAreaToListOfAreas(study.areas, "origin");
                auto* destination = addAreaToListOfAreas(study.areas, "destination");
                origin->adequacyPatchMode = physicalAreaInsideAdqPatch;
                destination->adequacyPatchMode = physicalAreaInsideAdqPatch;
                study.areas.rebuildIndexes();

                PROBLEME_HEBDO problem;
                problem.NombreDePays = 2;
                problem.NombreDInterconnexions = 1;
                problem.NombreDePasDeTemps = 1;
                problem.NombreDePasDeTempsPourUneOptimisation = 1;
                problem.NomsDesPays = {"origin", "destination"};
                problem.PaysOrigineDeLInterconnexion = {0};
                problem.PaysExtremiteDeLInterconnexion = {1};
                problem.IndexDebutIntercoOrigine = {0, -1};
                problem.IndexDebutIntercoExtremite = {-1, 0};
                problem.IndexSuivantIntercoOrigine = {-1};
                problem.IndexSuivantIntercoExtremite = {-1};
                problem.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>(
                  study.areas,
                  std::vector<Antares::Data::AreaLink*>{});
                problem.adequacyPatchRuntimeData->originAreaMode = {physicalAreaInsideAdqPatch};
                problem.adequacyPatchRuntimeData->extremityAreaMode = {physicalAreaInsideAdqPatch};
                problem.CorrespondanceVarNativesVarOptim.resize(1);
                problem.ResultatsHoraires.resize(2);
                for (unsigned area = 0; area < 2; ++area)
                {
                    auto& results = problem.ResultatsHoraires[area];
                    results.ValeursHorairesDENS = {100.};
                    results.ValeursHorairesDeDefaillancePositive = {(area == 1) == direct ? 100.
                                                                                          : 0.};
                    results.ValeursHorairesDeDefaillanceNegative = {0.};
                    results.ProductionThermique.resize(1);
                }
                problem.PaliersThermiquesDuPays.resize(2);
                problem.DefaillanceNegativeUtiliserPMinThermique = {false, false};
                problem.DefaillanceNegativeUtiliserHydro = {false, false};
                problem.DefaillanceNegativeUtiliserConsoAbattue = {false, false};
                problem.ConsommationsAbattues.resize(1);
                problem.ConsommationsAbattues[0].ConsommationAbattueDuPays = {100., 100.};
                problem.AllMustRunGeneration.resize(1);
                problem.AllMustRunGeneration[0].AllMustRunGenerationOfArea = {0., 0.};
                problem.ValeursDeNTC.resize(1);
                auto& ntc = problem.ValeursDeNTC[0];
                ntc.ValeurDuFlux = {0.};
                ntc.ValeurDeNTCOrigineVersExtremite = {legacyCapacity};
                ntc.ValeurDeNTCExtremiteVersOrigine = {legacyCapacity};
                ntc.CapaciteEffectiveOrigineVersExtremite = {effectiveCapacity};
                ntc.CapaciteEffectiveExtremiteVersOrigine = {effectiveCapacity};

                auto params = createParams();
                params.curtailmentSharing.includeHurdleCost = false;
                params.curtailmentSharing.checkCsrCostFunction = false;
                params.curtailmentSharing.thresholdVarBoundsRelaxation = 6;
                Antares::Optimization::OptimizationOptions options;
                options.quadraticOptimOptions.solverName = "sirius";
                HourlyCSRProblem csr(params, &problem, options);
                csr.setHour(0);
                csr.run(0, 0);

                const double capacity = std::isnan(effectiveCapacity) ? legacyCapacity
                                                                      : effectiveCapacity;
                const double expectedFlow = (direct ? 1. : -1.) * std::min(50., capacity);
                BOOST_CHECK_SMALL(ntc.ValeurDuFlux[0] - expectedFlow, 1e-3);
                BOOST_CHECK_EQUAL(ntc.ValeurDeNTCOrigineVersExtremite[0], legacyCapacity);
                BOOST_CHECK_EQUAL(ntc.ValeurDeNTCExtremiteVersOrigine[0], legacyCapacity);
            }
        }
    }
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
      0.0,
      false,
      virtualArea,
      virtualArea,
      flowArea0toArea1_positive,
      flowArea2toArea0_positive);
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      positiveEnsInit,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      0.0,
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
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow_initEnsEqualTo50)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      positiveEnsInit,
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
BOOST_AUTO_TEST_CASE(
  calculateAreaFlowBalanceForOneTimeStep_outside_inside_IncludeOut_negativeFlow_initEnsEqualTo0)
{
    double netPositionInit;
    double densNew;
    std::tie(netPositionInit, densNew) = calculateAreaFlowBalanceForOneTimeStep(
      2.0,
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
    BOOST_CHECK_NO_THROW(
      p.checkAdqPatchSimulationModeEconomyOnly(Antares::Data::SimulationMode::Economy));
    BOOST_CHECK_NO_THROW(p.checkAdqPatchIncludeHurdleCost(true));
}

BOOST_AUTO_TEST_CASE(check_adq_param_wrong_mode)
{
    auto p = createParams();
    BOOST_CHECK_THROW(p.checkAdqPatchSimulationModeEconomyOnly(
                        Antares::Data::SimulationMode::Adequacy),
                      Antares::Error::IncompatibleSimulationModeForAdqPatch);
}

BOOST_AUTO_TEST_CASE(check_adq_param_wrong_hurdle_cost)
{
    auto p = createParams();
    BOOST_CHECK_THROW(p.checkAdqPatchIncludeHurdleCost(false),
                      Antares::Error::IncompatibleHurdleCostCSR);
}

BOOST_AUTO_TEST_SUITE_END()
