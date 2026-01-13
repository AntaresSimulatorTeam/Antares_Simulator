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

#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include "antares/io/outputs/MPSGenerator.h"
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
    return !problem || problem->variableCount() == 0;
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

void checkProblem(const std::unique_ptr<ILinearProblem>& originalProblem, const fs::path& mpsPath)
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

// ==================== ExportableName Tests ====================
BOOST_AUTO_TEST_SUITE(ExportableNameTests)

BOOST_AUTO_TEST_CASE(MakeExportableName_EmptyString)
{
    NameManager nameManager;
    std::string result = MakeMpsSafeUniqueName("", nameManager);
    BOOST_CHECK(!result.empty());
    BOOST_CHECK_EQUAL(result.front(), '_');
}

BOOST_AUTO_TEST_CASE(MakeExportableName_StartsWithForbiddenChar)
{
    NameManager nameManager;

    // Test all forbidden first chars
    std::vector<std::string> forbiddenStarts = {"$test", ".test", "0test", "1test", "9test"};
    for (const auto& name: forbiddenStarts)
    {
        std::string result = MakeMpsSafeUniqueName(name, nameManager);
        BOOST_CHECK_EQUAL(result.front(), '_');
    }
}

BOOST_AUTO_TEST_CASE(MakeExportableName_ContainsForbiddenChars)
{
    NameManager nameManager;

    // Test all forbidden chars
    std::string name = "test +name-with*forbidden/chars<>=:\\";
    std::string result = MakeMpsSafeUniqueName(name, nameManager);

    // Check that forbidden chars are replaced with underscores
    BOOST_CHECK(result.find('+') == std::string::npos);
    BOOST_CHECK(result.find('-') == std::string::npos);
    BOOST_CHECK(result.find('*') == std::string::npos);
    BOOST_CHECK(result.find('/') == std::string::npos);
    BOOST_CHECK(result.find('<') == std::string::npos);
    BOOST_CHECK(result.find('>') == std::string::npos);
    BOOST_CHECK(result.find('=') == std::string::npos);
    BOOST_CHECK(result.find(':') == std::string::npos);
    BOOST_CHECK(result.find('\\') == std::string::npos);
}

BOOST_AUTO_TEST_CASE(MakeExportableName_ValidName)
{
    NameManager nameManager;
    std::string validName = "validName123_ABC";
    std::string result = MakeMpsSafeUniqueName(validName, nameManager);
    BOOST_CHECK_EQUAL(result, validName);
}

BOOST_AUTO_TEST_CASE(NameManager_UniqueNames)
{
    NameManager nameManager;

    std::string name1 = nameManager.MakeUniqueName("test");
    std::string name2 = nameManager.MakeUniqueName("test");
    std::string name3 = nameManager.MakeUniqueName("test");

    BOOST_CHECK_EQUAL(name1, "test");
    BOOST_CHECK_NE(name1, name2);
    BOOST_CHECK_NE(name2, name3);
    BOOST_CHECK_NE(name1, name3);
}

BOOST_AUTO_TEST_CASE(NameManager_DifferentNames)
{
    NameManager nameManager;

    std::string name1 = nameManager.MakeUniqueName("test1");
    std::string name2 = nameManager.MakeUniqueName("test2");

    BOOST_CHECK_EQUAL(name1, "test1");
    BOOST_CHECK_EQUAL(name2, "test2");
}

BOOST_AUTO_TEST_CASE(NameManager_EmptyName)
{
    NameManager nameManager;
    std::string result = nameManager.MakeUniqueName("");
    BOOST_CHECK_EQUAL(result, "");
}

BOOST_AUTO_TEST_CASE(MakeMpsSafeUniqueName_Integration)
{
    NameManager nameManager;
    std::string result1 = MakeMpsSafeUniqueName("$invalid", nameManager);
    std::string result2 = MakeMpsSafeUniqueName("$invalid", nameManager);

    BOOST_CHECK_EQUAL(result1.front(), '_');
    BOOST_CHECK_NE(result1, result2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MPSGeneratorTests)

BOOST_AUTO_TEST_CASE(MPSGenerator_EmptyProblem)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");
    fs::path tempPath = fs::temp_directory_path() / "empty_problem.mps";

    BOOST_CHECK_NO_THROW({
        auto mps = MPSGenerator(*problem, "EmptyProblem").run();
        MPSFileWriter::write(tempPath, mps);
    });

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_SimpleProblem)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    // min x + 2*y
    // s.t. x + y >= 1
    //      x, y >= 0

    auto x = problem->addNumVariable(0.0, problem->infinity(), "x");
    auto y = problem->addNumVariable(0.0, problem->infinity(), "y");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setObjectiveCoefficient(y, 2.0);
    problem->setMinimization();

    auto c = problem->addConstraint(1.0, problem->infinity(), "c1");
    c->setCoefficient(x, 1.0);
    c->setCoefficient(y, 1.0);

    fs::path tempPath = fs::temp_directory_path() / "simple_problem.mps";

    BOOST_CHECK_NO_THROW({
        auto mps = MPSGenerator(*problem, "SimpleProblem").run();
        MPSFileWriter::write(tempPath, mps);
    });

    BOOST_CHECK(fs::exists(tempPath));

    // Verify file contains expected sections
    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    BOOST_CHECK(content.find("NAME SimpleProblem") != std::string::npos);
    BOOST_CHECK(content.find("ROWS") != std::string::npos);
    BOOST_CHECK(content.find("COLUMNS") != std::string::npos);
    BOOST_CHECK(content.find("RHS") != std::string::npos);
    BOOST_CHECK(content.find("BOUNDS") != std::string::npos);
    BOOST_CHECK(content.find("ENDATA") != std::string::npos);

    // Validate the MPS file can be read back and matches original problem
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_MixedIntegerProblem)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(true, "scip");

    // Create a MIP problem
    auto x = problem->addIntVariable(0, 10, "x");
    auto y = problem->addNumVariable(0.0, 5.0, "y");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setObjectiveCoefficient(y, 1.0);
    problem->setMinimization();

    auto c = problem->addConstraint(-problem->infinity(), 15.0, "c1");
    c->setCoefficient(x, 2.0);
    c->setCoefficient(y, 3.0);

    fs::path tempPath = fs::temp_directory_path() / "mip_problem.mps";

    auto mps = MPSGenerator(*problem, "MIPProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_BinaryVariable)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(true, "highs");

    auto b = problem->addIntVariable(0, 1, "binary_var");
    problem->addNumVariable(0.5, 1.981, "num_var");

    problem->setObjectiveCoefficient(b, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "binary_problem.mps";
    auto mps = MPSGenerator(*problem, "BinaryProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain BV (binary variable) bound
    BOOST_CHECK(content.find(" BV ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_FreeVariable)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(-problem->infinity(), problem->infinity(), "free_var");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "free_var_problem.mps";

    auto mps = MPSGenerator(*problem, "FreeVarProblem").run();
    MPSFileWriter::write(tempPath, mps);
    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain FR (free variable) bound
    BOOST_CHECK(content.find(" FR ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_FixedVariable)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(5.0, 5.0, "fixed_var");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "fixed_var_problem.mps";

    auto mps = MPSGenerator(*problem, "FixedVarProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain FX (fixed variable) bound
    BOOST_CHECK(content.find(" FX ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_EqualityConstraint)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(0.0, 10.0, "x");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setMinimization();

    // Equality constraint
    auto c = problem->addConstraint(5.0, 5.0, "eq_constraint");
    c->setCoefficient(x, 1.0);

    fs::path tempPath = fs::temp_directory_path() / "eq_constraint_problem.mps";

    auto mps = MPSGenerator(*problem, "EqConstraintProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain E (equality) constraint
    BOOST_CHECK(content.find(" E  ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_ObjectiveOffset)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(0.0, 10.0, "x");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setObjectiveOffset(100.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "offset_problem.mps";

    auto mps = MPSGenerator(*problem, "OffsetProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain RHS for objective offset
    BOOST_CHECK(content.find("RHS1  OBJ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_MaximizationProblem)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(0.0, 10.0, "x");

    problem->setObjectiveCoefficient(x, 2.0);
    problem->setMaximization();

    fs::path tempPath = fs::temp_directory_path() / "max_problem.mps";

    auto mps = MPSGenerator(*problem, "MaxProblem").run();
    MPSFileWriter::write(tempPath, mps);
    BOOST_CHECK(fs::exists(tempPath));

    // Verify the objective coefficient is negated for maximization
    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    BOOST_CHECK(content.find("COLUMNS") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_InvalidPath)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    fs::path invalidPath = "/invalid/path/that/does/not/exist/problem.mps";

    BOOST_CHECK_THROW(
      {
          auto mps = MPSGenerator(*problem, "InvalidPathProblem").run();
          MPSFileWriter::write(invalidPath, mps);
      },
      std::runtime_error);
}

BOOST_AUTO_TEST_CASE(MPSGenerator_VariableNamesWithForbiddenChars)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(0.0, 10.0, "$invalid-name+test");
    auto y = problem->addNumVariable(0.0, 10.0, ".another/bad*name");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setObjectiveCoefficient(y, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "invalid_names_problem.mps";

    auto mps = MPSGenerator(*problem, "InvalidNamesProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    // Verify file is valid
    std::ifstream file(tempPath);
    BOOST_CHECK(file.is_open());
    file.close();
    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);
    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_DuplicateVariableNames)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x1 = problem->addNumVariable(0.0, 10.0, "duplicate");
    auto x2 = problem->addNumVariable(0.0, 10.0, "duplicate");
    auto x3 = problem->addNumVariable(0.0, 10.0, "duplicate");

    problem->setObjectiveCoefficient(x1, 1.0);
    problem->setObjectiveCoefficient(x2, 1.0);
    problem->setObjectiveCoefficient(x3, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "duplicate_names_problem.mps";

    auto mps = MPSGenerator(*problem, "DuplicateNamesProblem").run();
    MPSFileWriter::write(tempPath, mps);
    BOOST_CHECK(fs::exists(tempPath));

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_ZeroCoefficients)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(false,
                                                                                     "highs");

    auto x = problem->addNumVariable(0.0, 10.0, "x");
    auto y = problem->addNumVariable(0.0, 10.0, "y");

    // Zero coefficient in objective
    problem->setObjectiveCoefficient(x, 0.0);
    problem->setObjectiveCoefficient(y, 1.0);
    problem->setMinimization();

    auto c = problem->addConstraint(0.0, 10.0, "c1");
    c->setCoefficient(x, 0.0); // Zero coefficient
    c->setCoefficient(y, 1.0);

    fs::path tempPath = fs::temp_directory_path() / "zero_coef_problem.mps";

    auto mps = MPSGenerator(*problem, "ZeroCoefProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_NegativeIntegerBounds)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(true, "highs");

    auto x = problem->addIntVariable(-10, 5, "neg_int");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "neg_int_problem.mps";

    auto mps = MPSGenerator(*problem, "NegIntProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Should contain LI (lower integer) and UI (upper integer)
    BOOST_CHECK(content.find(" LI ") != std::string::npos);
    BOOST_CHECK(content.find(" UI ") != std::string::npos);
    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_CASE(MPSGenerator_UnboundedIntegerVariable)
{
    std::unique_ptr<ILinearProblem> problem = std::make_unique<OrtoolsLinearProblem>(true, "highs");

    auto x = problem->addIntVariable(-problem->infinity(), problem->infinity(), "unbounded_int");

    problem->setObjectiveCoefficient(x, 1.0);
    problem->setMinimization();

    fs::path tempPath = fs::temp_directory_path() / "unbounded_int_problem.mps";

    auto mps = MPSGenerator(*problem, "UnboundedIntProblem").run();
    MPSFileWriter::write(tempPath, mps);

    BOOST_CHECK(fs::exists(tempPath));

    std::ifstream file(tempPath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // Should contain MI (minus infinity)
    BOOST_CHECK(content.find(" MI ") != std::string::npos);

    // Validate the MPS file
    ValidateMps::checkProblem(problem, tempPath);

    if (fs::exists(tempPath))
    {
        fs::remove(tempPath);
    }
}

BOOST_AUTO_TEST_SUITE_END()
