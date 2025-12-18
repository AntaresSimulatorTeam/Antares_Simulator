/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#define WIN32_LEAN_AND_MEAN

#include <ortools/linear_solver/linear_solver.h>
#include <ortools/linear_solver/wrappers/model_builder_helper.h>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "antares/io/outputs/MPSWriter.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/modeler/fileWriter/FileWriter.h"
#include "antares/solver/modeler/loadFiles/Fileloader.h"
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;

using namespace std;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemDataImpl;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::IO::Outputs; //
namespace fs = std::filesystem;

BOOST_AUTO_TEST_SUITE(ValidateMps)

struct MpsWriterTestFixture
{
    const fs::path resources = std::filesystem::path(CMAKE_SOURCE_DIR) / "tests" / "resources"
                               / "modeler";

    static void run(const fs::path& studyPath)
    {
        Antares::Solver::LoadFiles::FileLoader loader(studyPath);
        Antares::Modeler::FileWriter writer(studyPath);
        Antares::Solver::Modeler modeler(loader, writer);
        modeler.run();

        checkProblem(modeler.masterProblem(), studyPath / "output" / "master.mps");
        checkProblem(modeler.subproblems().at(0), studyPath / "output" / "1-1.mps");
    }

    static void checkProblemType(const unique_ptr<ILinearProblem>& originalProblem,
                                 const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        bool isLp = true;
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            isLp &= !fromMps.VarIsIntegral(vi);
        }
        BOOST_CHECK(isLp == originalProblem->isLP());
    }

    static void checkVariables(const unique_ptr<ILinearProblem>& originalProblem,
                               const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        BOOST_CHECK(originalProblem->variableCount() == fromMps.num_variables());
        const auto& origVariables = originalProblem->getVariables();
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            const auto& origVariable = origVariables.at(vi);
            BOOST_CHECK(origVariable->getName() == fromMps.VarName(vi));
            BOOST_CHECK(origVariable->isInteger() == fromMps.VarIsIntegral(vi));
            BOOST_CHECK(origVariable->getLb() == fromMps.VarLowerBound(vi));
            BOOST_CHECK(origVariable->getUb() == fromMps.VarUpperBound(vi));
        }
    }

    static void checkConstraints(const unique_ptr<ILinearProblem>& originalProblem,
                                 const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        BOOST_CHECK(originalProblem->constraintCount() == fromMps.num_constraints());
        const auto& origVariables = originalProblem->getVariables();
        const auto& origConstraints = originalProblem->getConstraints();

        for (int ci = 0; ci < fromMps.num_constraints(); ++ci)
        {
            const auto& origConstraint = origConstraints.at(ci);
            BOOST_CHECK(origConstraint->getName() == fromMps.ConstraintName(ci));
            BOOST_CHECK(origConstraint->getLb() == fromMps.ConstraintLowerBound(ci));
            BOOST_CHECK(origConstraint->getUb() == fromMps.ConstraintUpperBound(ci));

            const auto fromMpsConstraintCoef = fromMps.ConstraintCoefficients(ci);
            for (int vi = 0; vi < fromMps.num_variables(); ++vi)
            {
                const auto& origVariable = origVariables.at(vi);

                BOOST_CHECK(origConstraint->getCoefficient(origVariable.get())
                            == fromMpsConstraintCoef.at(vi));
            }
        }
    }

    static void checkObjective(const unique_ptr<ILinearProblem>& originalProblem,
                               const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        const auto& origVariables = originalProblem->getVariables();
        BOOST_CHECK(originalProblem->getObjectiveOffset() == fromMps.ObjectiveOffset());
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            const auto& origVariable = origVariables.at(vi);

            BOOST_CHECK(originalProblem->getObjectiveCoefficient(origVariable.get())
                        == fromMps.VarObjectiveCoefficient(vi));
        }
    }

    static void checkProblem(const std::unique_ptr<ILinearProblem>& originalProblem,
                             const fs::path& mpsPath)
    {
        operations_research::mb::ModelBuilderHelper fromMps;
        fromMps.ImportFromMpsFile(mpsPath.string());
        checkProblemType(originalProblem, fromMps);
        checkVariables(originalProblem, fromMps);
        checkConstraints(originalProblem, fromMps);
        checkObjective(originalProblem, fromMps);
    }
};

BOOST_FIXTURE_TEST_CASE(TestALLModelerStudiesMps, MpsWriterTestFixture)
{
    auto dirFilter = std::views::filter(static_cast<bool (*)(const fs::path&)>(&fs::is_directory));
    for (const auto& entry: fs::directory_iterator(resources) | dirFilter)
    {
        if (entry.path().stem() == "epic_2")
        {
            for (const auto& subEntry:
                 fs::directory_iterator(resources / "epic_2" / "us2.5") | dirFilter)
            {
                run(subEntry.path());
            }
        }
        run(entry.path());
    }
}

BOOST_AUTO_TEST_SUITE_END()
