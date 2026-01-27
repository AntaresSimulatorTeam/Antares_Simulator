#define BOOST_TEST_MODULE ModelerBasicsTests
#include <boost/test/unit_test.hpp>

#include "antares/solver/modeler/ILoader.h"
#include "antares/solver/modeler/IWriter.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/modeler/ModelerData.h"

// No using namespace -- use explicit qualifications to avoid lookup issues.

BOOST_AUTO_TEST_CASE(test_modelerdata_default)
{
    Antares::Solver::ModelerData d;
    (void)d; // Default-constructible
}

// Small mock loader/writer implementing the actual interfaces
struct DummyLoader : Antares::Solver::ILoader
{
    Antares::Solver::ModelerParameters loadParameters() override
    {
        return Antares::Solver::ModelerParameters{};
    }

    Antares::Solver::ModelerData loadAll() override
    {
        return Antares::Solver::ModelerData{};
    }
};

struct DummyWriter : Antares::Solver::IWriter
{
    void init(const std::string& /*simulationId*/) override
    {
    }

    void writeSimulationTable(
      const Antares::Optimisation::LinearProblemApi::ILinearProblem& /*linearProblem*/,
      const Antares::Optimisation::LinearProblemApi::IMipSolution& /*solution*/,
      const Antares::Solver::ModelerData& /*modelerData*/,
      const Antares::Optimisation::OptimEntityContainer& /*variableContainer*/,
      const Antares::Optimisation::LinearProblemApi::FillContext& /*fillContext*/) const override
    {
    }

    const std::filesystem::path& outputPath() const override
    {
        static std::filesystem::path p("/tmp");
        return p;
    }
};

BOOST_AUTO_TEST_CASE(test_modeler_constructor_stores_references)
{
    DummyLoader loader;
    DummyWriter writer;

    Antares::Solver::Modeler m(loader, writer);

    // The Modeler stores references to loader/writer passed to constructor
    BOOST_TEST(&m.loader_ == &loader);
    BOOST_TEST(&m.writer_ == &writer);
}
