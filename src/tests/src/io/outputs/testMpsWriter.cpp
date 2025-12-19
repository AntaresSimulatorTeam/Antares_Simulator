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

#include <algorithm>
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
using namespace Antares;
using namespace Antares::Solver;
using namespace Antares::IO::Outputs; //
namespace fs = std::filesystem;
const fs::path resources = std::filesystem::path(CMAKE_SOURCE_DIR) / "tests" / "resources"
                           / "modeler";
auto dirFilter = std::views::filter([](const fs::directory_entry& entry)
                                    { return entry.is_directory(); });
BOOST_AUTO_TEST_SUITE(ValidateMps)

struct MpsWriterTestFixture
{
    LoadFiles::FileLoader loader;
    FileWriter writer;

    explicit MpsWriterTestFixture(const fs::path& studyPath):
        loader(studyPath),
        writer(studyPath)
    {
    }

    Modeler build()
    {
        return {loader, writer};
    }

    static void checkMPS(const fs::path& studyPath, Modeler& modeler)
    {
        modeler.run();
        checkProblem(modeler.masterProblem(), studyPath / "output" / "master.mps");
        checkProblem(modeler.subproblems().at(0), studyPath / "output" / "1-1.mps");
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

    static void checkProblemType(const unique_ptr<ILinearProblem>& originalProblem,
                                 const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        bool isLp = true;
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            if (fromMps.VarIsIntegral(vi))
            {
                isLp = false;
                break;
            }
        }
        BOOST_CHECK_EQUAL(isLp, originalProblem->isLP());
    }

    static void checkVariables(const unique_ptr<ILinearProblem>& originalProblem,
                               const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        BOOST_CHECK_EQUAL(originalProblem->variableCount(), fromMps.num_variables());
        const auto& origVariables = originalProblem->getVariables();
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            const auto& origVariable = origVariables.at(vi);
            BOOST_CHECK_EQUAL(origVariable->getName(), fromMps.VarName(vi));
            BOOST_CHECK_EQUAL(origVariable->isInteger(), fromMps.VarIsIntegral(vi));
            BOOST_CHECK_EQUAL(origVariable->getLb(), fromMps.VarLowerBound(vi));
            BOOST_CHECK_EQUAL(origVariable->getUb(), fromMps.VarUpperBound(vi));
        }
    }

    static void checkConstraints(const unique_ptr<ILinearProblem>& originalProblem,
                                 const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        BOOST_CHECK_EQUAL(originalProblem->constraintCount(), fromMps.num_constraints());
        const auto& origVariables = originalProblem->getVariables();
        const auto& origConstraints = originalProblem->getConstraints();

        for (int ci = 0; ci < fromMps.num_constraints(); ++ci)
        {
            const auto& origConstraint = origConstraints.at(ci);
            BOOST_CHECK_EQUAL(origConstraint->getName(), fromMps.ConstraintName(ci));
            BOOST_CHECK_EQUAL(origConstraint->getLb(), fromMps.ConstraintLowerBound(ci));
            BOOST_CHECK_EQUAL(origConstraint->getUb(), fromMps.ConstraintUpperBound(ci));

            const auto fromMpsConstraintCoef = fromMps.ConstraintCoefficients(ci);
            const auto fromMpsConstraintVarIndices = fromMps.ConstraintVarIndices(ci);
            BOOST_CHECK_EQUAL(fromMpsConstraintCoef.size(), fromMpsConstraintVarIndices.size());
            for (int vi = 0; vi < fromMps.num_variables(); ++vi)
            {
                const auto& origVariable = origVariables.at(vi);
                const auto origVariableCoef = origConstraint->getCoefficient(origVariable.get());
                if (origVariableCoef == 0) // if the coef in the original constraint is zero then
                                           // variable index should not be in ConstraintVarIndices
                {
                    const bool found = ranges::find(fromMpsConstraintVarIndices, vi)
                                       != fromMpsConstraintVarIndices.end();
                    BOOST_CHECK_EQUAL(found, false);
                    continue;
                }
                auto it = std::ranges::find(fromMpsConstraintVarIndices, vi);
                BOOST_CHECK_EQUAL(it == fromMpsConstraintVarIndices.end(), false);
                const auto position = std::distance(fromMpsConstraintVarIndices.begin(), it);
                BOOST_CHECK_EQUAL(origVariableCoef, fromMpsConstraintCoef.at(position));
            }
        }
    }

    static void checkObjective(const unique_ptr<ILinearProblem>& originalProblem,
                               const operations_research::mb::ModelBuilderHelper& fromMps)
    {
        const auto& origVariables = originalProblem->getVariables();
        BOOST_CHECK_EQUAL(originalProblem->getObjectiveOffset(), fromMps.ObjectiveOffset());
        for (int vi = 0; vi < fromMps.num_variables(); ++vi)
        {
            const auto& origVariable = origVariables.at(vi);

            BOOST_CHECK_EQUAL(originalProblem->getObjectiveCoefficient(origVariable.get()),
                              fromMps.VarObjectiveCoefficient(vi));
        }
    }
};

static void checkEpic2Studies()
{
    for (const auto& subEntry: fs::directory_iterator(resources / "epic_2" / "us2.5") | dirFilter)
    {
        MpsWriterTestFixture fixture(subEntry.path());
        auto modeler = fixture.build();
        MpsWriterTestFixture::checkMPS(subEntry.path(), modeler);
    }
}

BOOST_AUTO_TEST_CASE(TestALLModelerStudiesMps)
{
    for (const auto& entry: fs::directory_iterator(resources) | dirFilter)
    {
        if (entry.path().stem() == "epic_2")
        {
            checkEpic2Studies();
            continue;
        }
        MpsWriterTestFixture fixture(entry.path());
        auto modeler = fixture.build();
        MpsWriterTestFixture::checkMPS(entry.path(), modeler);
    }
}

BOOST_AUTO_TEST_SUITE_END()
