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
using namespace Antares;
using namespace Antares::Solver;
using namespace Antares::IO::Outputs; //
namespace fs = std::filesystem;
const fs::path resources = std::filesystem::path(CMAKE_SOURCE_DIR) / "tests" / "resources"
                           / "modeler";

const std::set<std::string> ignoreList{"1_3", "1_5"};
BOOST_AUTO_TEST_SUITE(ValidateMps)

bool isProblemEmpty(const std::unique_ptr<ILinearProblem>& problem)
{
    return problem->variableCount() == 0;
}

void checkProblemType(const unique_ptr<ILinearProblem>& originalProblem,
                      const operations_research::mb::ModelBuilderHelper& fromMps)
{
    const bool isMip = std::ranges::any_of(std::views::iota(0, fromMps.num_variables()),
                                           [&fromMps](const int vi)
                                           { return fromMps.VarIsIntegral(vi); });
    BOOST_CHECK_EQUAL(isMip, !originalProblem->isLP());
}

void checkVariables(const unique_ptr<ILinearProblem>& originalProblem,
                    const operations_research::mb::ModelBuilderHelper& fromMps)
{
    BOOST_CHECK_EQUAL(originalProblem->variableCount(), fromMps.num_variables());
    const auto& origVariables = originalProblem->getVariables();
    NameManager nameManager;
    for (int vi = 0; vi < fromMps.num_variables(); ++vi)
    {
        const auto& origVariable = origVariables.at(vi);
        BOOST_CHECK_EQUAL(MakeMpsSafeUniqueName(origVariable->getName(), nameManager),
                          fromMps.VarName(vi));
        BOOST_CHECK_EQUAL(origVariable->isInteger(), fromMps.VarIsIntegral(vi));
        BOOST_CHECK_EQUAL(origVariable->getLb(), fromMps.VarLowerBound(vi));
        BOOST_CHECK_EQUAL(origVariable->getUb(), fromMps.VarUpperBound(vi));
    }
}

void checkConstraints(const unique_ptr<ILinearProblem>& originalProblem,
                      const operations_research::mb::ModelBuilderHelper& fromMps)
{
    BOOST_CHECK_EQUAL(originalProblem->constraintCount(), fromMps.num_constraints());
    const auto& origVariables = originalProblem->getVariables();
    const auto& origConstraints = originalProblem->getConstraints();

    NameManager nameManager;
    for (int ci = 0; ci < fromMps.num_constraints(); ++ci)
    {
        const auto& origConstraint = origConstraints.at(ci);
        BOOST_CHECK_EQUAL(MakeMpsSafeUniqueName(origConstraint->getName(), nameManager),
                          fromMps.ConstraintName(ci));
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

void checkObjective(const unique_ptr<ILinearProblem>& originalProblem,
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

    void checkProblem(const std::unique_ptr<ILinearProblem>& originalProblem,
                      const fs::path& mpsPath)
    {
        operations_research::mb::ModelBuilderHelper fromMps;
        fromMps.ImportFromMpsFile(mpsPath.string());
        checkProblemType(originalProblem, fromMps);
        checkVariables(originalProblem, fromMps);
        checkConstraints(originalProblem, fromMps);
        checkObjective(originalProblem, fromMps);
    }

    void checkMPS(const fs::path& studyPath, Modeler& modeler)
    {
        modeler.run();
        if (const auto& masterProblem = modeler.masterProblem(); !isProblemEmpty(masterProblem))
        {
            checkProblem(masterProblem, studyPath / "output" / "master.mps");
        }
        checkProblem(modeler.subproblems().at(0), studyPath / "output" / "1-1.mps");
    }

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
    };

    void processStudy(const filesystem::path& entry)
    {
        MpsWriterTestFixture fixture(entry);
        auto modeler = fixture.build();
        checkMPS(entry, modeler);
    }

    void checkEpic2Studies()
    {
        for (const auto& subEntry: fs::directory_iterator(resources / "epic2" / "us2.5"))
        {
            if (!subEntry.is_directory())
            {
                continue;
            }
            const auto& path = subEntry.path();
            if (!ignoreList.contains(path.filename().string()))
            {
                processStudy(path);
            }
        }
    }

    BOOST_AUTO_TEST_CASE(TestALLModelerStudiesMps)
    {
        for (const auto& entry:
             fs::directory_iterator(resources, fs::directory_options::skip_permission_denied))
        {
            if (!entry.is_directory())
            {
                continue;
            }
            const auto& path = entry.path();
            if (!ignoreList.contains(path.filename().string()))
            {
                if (path.filename().string() == "epic2")
                {
                    checkEpic2Studies();
                    continue;
                }
                processStudy(path);
            }
        }
    }

    BOOST_AUTO_TEST_SUITE_END()
