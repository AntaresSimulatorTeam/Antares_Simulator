#include "antares/solver/optimisation/post_process_commands.h"
#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"
#include "antares/writer/in_memory_writer.h"

#include "in-memory-study.h"

#define BOOST_TEST_MODULE post process

#include <boost/test/unit_test.hpp>

namespace
{
Benchmarking::DurationCollector gDurationCollector;
constexpr unsigned int gNumSpace = 0;
constexpr unsigned int gNumberTimeSteps = 168;
} // namespace

using namespace Antares::Solver::Simulation;

BOOST_AUTO_TEST_CASE(test_adq_patch_areas)
{
    StudyBuilder builder;
    builder.addAreaToStudy("FR");
    builder.addAreaToStudy("ES");

    Antares::Solver::InMemoryWriter writer(gDurationCollector);
    const std::string fileLabel = "label";
    PROBLEME_HEBDO pb;
    SIM_AllocationProblemeHebdo(*builder.study, pb, gNumberTimeSteps);

    WriteDebugAdequacyPatch cmd(&pb, builder.study->areas, gNumSpace, writer, fileLabel);
    const optRuntimeData opt_runtime_data(/* year*/ 0,
                                          /*week*/ 0,
                                          /*hour in year*/ 5);
    // FR, h=2
    pb.ResultatsHoraires[1].ValeursHorairesDENS[2] = 4.4;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositive[2] = 6.6;
    pb.ResultatsHoraires[1].ValeursHorairesDeDefaillancePositiveCSR[2] = 7.6;

    cmd.execute(opt_runtime_data);
    const auto& contents = writer.getMap();
    BOOST_REQUIRE(contents.contains("adequacy-patch-areas-label-0-0.csv"));
    const std::string& areaResults = contents.at("adequacy-patch-areas-label-0-0.csv");
    // Header
    BOOST_CHECK(areaResults.find("Area Hour DENS UnsuppliedEnergy UnsuppliedEnergyCSR MRGPrice "
                                 "MRGPriceCSR DTGmrgCSR SpilledEnergy\n")
                != std::string::npos);
    // Hour 2
    BOOST_CHECK(areaResults.find("FR 2 4.4 6.6 7.6 -0 -0 0 0\n") != std::string::npos);
    // Hour 3
    BOOST_CHECK(areaResults.find("ES 3 0 0 0 -0 -0 0 0\n") != std::string::npos);
    BOOST_CHECK(areaResults.find("FR 3 0 0 0 -0 -0 0 0\n") != std::string::npos);
}
