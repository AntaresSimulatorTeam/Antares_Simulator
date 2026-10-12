#include "antares/solver/optimisation/post_process_commands.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"
#include "antares/writer/in_memory_writer.h"

#include "in-memory-study.h"

#define BOOST_TEST_MODULE post process

#include <boost/test/unit_test.hpp>

namespace
{
Benchmarking::DurationCollector gDurationCollector;
const std::string fileLabel = "label";
const optRuntimeData opt_runtime_data(/* year*/ 0,
                                      /*week*/ 0,
                                      /*hour in year*/ 5);

constexpr unsigned int gNumSpace = 0;
constexpr unsigned int gNumberTimeSteps = 168;

// TODO Use C++23's std::string::contains
bool contains(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}
} // namespace

using namespace Antares::Solver::Simulation;

BOOST_AUTO_TEST_CASE(test_adq_patch_areas)
{
    StudyBuilder builder;
    builder.addAreaToStudy("FR");
    builder.addAreaToStudy("ES");
    Antares::Solver::InMemoryWriter writer(gDurationCollector);

    PROBLEME_HEBDO pb;
    SIM_AllocationProblemeHebdo(*builder.study, pb, gNumberTimeSteps);

    WriteDebugAdequacyPatch cmd(&pb, builder.study->areas, gNumSpace, writer, fileLabel);
    // FR, h=2
    pb.ResultatsHoraires[1].ValeursHorairesDENS[2] = 4.4;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[2] = 6.6;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositiveCSR[2] = 7.6;

    cmd.execute(opt_runtime_data);
    const auto& contents = writer.getMap();
    BOOST_REQUIRE(contents.contains("adequacy-patch-areas-label-0-0.csv"));
    const std::string& areaResults = contents.at("adequacy-patch-areas-label-0-0.csv");
    // Header
    BOOST_CHECK(contains(areaResults,
                         "Area Hour DENS UnsuppliedEnergy UnsuppliedEnergyCSR MRGPrice "
                         "MRGPriceCSR DTGmrgCSR SpilledEnergy\n"));
    // Hour 2
    BOOST_CHECK(contains(areaResults, "FR 2 4.4 6.6 7.6 -0 -0 0 0\n"));
    // Hour 3
    BOOST_CHECK(contains(areaResults, "ES 3 0 0 0 -0 -0 0 0\n"));
    BOOST_CHECK(contains(areaResults, "FR 3 0 0 0 -0 -0 0 0\n"));
}

BOOST_AUTO_TEST_CASE(test_adq_patch_links)
{
    StudyBuilder builder;
    auto* a1 = builder.addAreaToStudy("FR");
    auto* a2 = builder.addAreaToStudy("ES");
    auto link = AreaAddLinkBetweenAreas(a1, a2);

    Antares::Solver::InMemoryWriter writer(gDurationCollector);

    PROBLEME_HEBDO pb;
    builder.study->initializeRuntimeInfos(); // Required for study.runtime.interconnectionsCount()
    SIM_AllocationProblemeHebdo(*builder.study, pb, gNumberTimeSteps);

    WriteDebugAdequacyPatch cmd(&pb, builder.study->areas, gNumSpace, writer, fileLabel);
    // ES / FR, h=2
    pb.ValeursDeNTC[2].ValeurDuFlux[0] = 4.4;

    cmd.execute(opt_runtime_data);
    const auto& contents = writer.getMap();
    BOOST_REQUIRE(contents.contains("adequacy-patch-links-label-0-0.csv"));
    const std::string& linkResults = contents.at("adequacy-patch-links-label-0-0.csv");
    // Header
    BOOST_CHECK(contains(linkResults, "Link Hour Flow\n"));
    // Hour 2
    BOOST_CHECK(contains(linkResults, "ES/FR 2 4.4\n"));
}

BOOST_AUTO_TEST_CASE(test_adq_patch_ens_below_threshold_is_zeroed_per_area)
{
    StudyBuilder builder;
    builder.addAreaToStudy("FR");
    builder.addAreaToStudy("ES");
    builder.addAreaToStudy("DE");
    builder.study->initializeRuntimeInfos();

    PROBLEME_HEBDO pb;
    SIM_AllocationProblemeHebdo(*builder.study, pb, gNumberTimeSteps);
    pb.NombreDePays = builder.study->areas.size();
    pb.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>(
      builder.study->areas, builder.study->runtime.areaLink);
    pb.adequacyPatchRuntimeData->areaMode = {
      Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch,
      Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch,
      Antares::Data::AdequacyPatch::physicalAreaOutsideAdqPatch};

    constexpr unsigned int hour = 2;
    pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[hour] = 50.0;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[hour] = 150.0;
    pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositive[hour] = 25.0;
    pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[hour + 1] = 100.0;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[hour + 1] = 25.0;
    pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositive[hour + 1] = 25.0;
    pb.ResultatsHoraires[0].CoutsMarginauxHoraires[hour] = -40.0;
    pb.ResultatsHoraires[1].CoutsMarginauxHoraires[hour] = -50.0;
    pb.ResultatsHoraires[2].CoutsMarginauxHoraires[hour] = -60.0;
    pb.ResultatsHoraires[0].CoutsMarginauxHoraires[hour + 1] = -70.0;
    pb.ResultatsHoraires[1].CoutsMarginauxHoraires[hour + 1] = -80.0;
    pb.ResultatsHoraires[2].CoutsMarginauxHoraires[hour + 1] = -90.0;
    pb.adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(0, hour);
    pb.adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(1, hour);
    pb.adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(2, hour);
    pb.adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(0, hour + 1);

    builder.study->areas[0]->scratchpad[gNumSpace].dispatchableGenerationMargin[hour] = 10.0;
    builder.study->areas[1]->scratchpad[gNumSpace].dispatchableGenerationMargin[hour] = 10.0;
    builder.study->areas[2]->scratchpad[gNumSpace].dispatchableGenerationMargin[hour] = 10.0;

    AdqPatchParams adqPatchParams;
    adqPatchParams.curtailmentSharing.thresholdRun = 100.0;
    for (unsigned int area = 0; area < pb.NombreDePays; ++area)
    {
        for (unsigned int currentHour = 0; currentHour < gNumberTimeSteps; ++currentHour)
        {
            pb.adequacyPatchRuntimeData->setMarginalCostBeforeAdqPatch(
              area,
              currentHour,
              pb.ResultatsHoraires[area].CoutsMarginauxHoraires[currentHour]);
        }
    }
    DTGnettingAfterCSRcmd cmd(adqPatchParams, &pb, builder.study->areas, gNumSpace);
    cmd.execute(opt_runtime_data);
    UpdateMrgPriceAfterCSRcmd updatePrice(&pb, builder.study->areas, gNumSpace);
    updatePrice.execute(opt_runtime_data);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[hour], 0.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositiveCSR[hour], 0.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].ValeursHorairesDtgMrgCsr[hour], 10.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].CoutsMarginauxHoraires[hour], -40.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].CoutsMarginauxHorairesCSR[hour], -40.0);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[hour], 150.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositiveCSR[hour], 140.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].ValeursHorairesDtgMrgCsr[hour], 0.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].CoutsMarginauxHoraires[hour], -1000.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].CoutsMarginauxHorairesCSR[hour], -1000.0);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositive[hour], 25.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositiveCSR[hour], 25.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].ValeursHorairesDtgMrgCsr[hour], 10.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].CoutsMarginauxHoraires[hour], -60.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].CoutsMarginauxHorairesCSR[hour], -60.0);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositive[hour + 1], 100.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[0].ValeursHorairesDeDefaillancePositiveCSR[hour + 1], 100.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].ValeursHorairesDtgMrgCsr[hour + 1], 0.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].CoutsMarginauxHoraires[hour + 1], -1000.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[0].CoutsMarginauxHorairesCSR[hour + 1], -1000.0);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[hour + 1], 25.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositiveCSR[hour + 1], 25.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].ValeursHorairesDtgMrgCsr[hour + 1], 0.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].CoutsMarginauxHoraires[hour + 1], -80.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[1].CoutsMarginauxHorairesCSR[hour + 1], -1000.0);

    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositive[hour + 1], 25.0);
    BOOST_CHECK_EQUAL(
      pb.ResultatsHoraires[2].ValeursHorairesDeDefaillancePositiveCSR[hour + 1], 25.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].ValeursHorairesDtgMrgCsr[hour + 1], 0.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].CoutsMarginauxHoraires[hour + 1], -90.0);
    BOOST_CHECK_EQUAL(pb.ResultatsHoraires[2].CoutsMarginauxHorairesCSR[hour + 1], -90.0);
}
