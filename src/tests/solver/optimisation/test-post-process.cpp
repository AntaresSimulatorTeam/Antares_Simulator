#include "antares/solver/optimisation/post_process_commands.h"
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
