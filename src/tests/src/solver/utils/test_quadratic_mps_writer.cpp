// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <pi_constantes_externes.h>
#include <unit_test_utils.h>

#include <boost/test/unit_test.hpp>

#include <antares/solver/optimisation/opt_structure_probleme_a_resoudre.h>
#include <antares/solver/utils/quadratic_mps_writer.h>
#include <antares/writer/null_result_writer.h>

namespace
{
struct CapturingWriter final: Antares::Solver::NullResultWriter
{
    void addEntryFromBuffer(const std::filesystem::path&, std::string& mpsToWrite) override
    {
        mps = mpsToWrite;
    }

    std::string mps;
};

void buildProblem(PROBLEME_ANTARES_A_RESOUDRE& problem)
{
    problem.NombreDeVariables = 2;
    problem.NombreDeContraintes = 1;

    problem.NomDesVariables = {"a", "b"};
    problem.NomDesContraintes = {"c1"};

    problem.Sens = ">";
    problem.IndicesDebutDeLigne = {0};
    problem.NombreDeTermesDesLignes = {2};
    problem.IndicesColonnes = {0, 1};
    problem.CoefficientsDeLaMatriceDesContraintes = {1.0, 1.0};
    problem.SecondMembre = {10.0};

    problem.CoutLineaire = {1.0, 1.0};
    problem.CoutQuadratique = {1.0, 7.0};

    problem.TypeDeVariable = {VARIABLE_BORNEE_INFERIEUREMENT, VARIABLE_BORNEE_DES_DEUX_COTES};
    problem.Xmin = {0.0, 0.0};
    problem.Xmax = {0.0, 10.0};
    problem.VariablesEntieres = {false, false};
}

} // namespace

BOOST_AUTO_TEST_SUITE(quadratic_mps_writer)

BOOST_AUTO_TEST_CASE(writes_expected_qmatrix_section)
{
    PROBLEME_ANTARES_A_RESOUDRE problem;
    buildProblem(problem);
    CapturingWriter writer;

    writeQuadraticMps(problem, writer, "problem-csr-y1-w2-h3.mps");

    const auto expectedMps = R"(* Antares Simulator QuadraticMPSWriter
* Number of variables: 2
* Number of constraints: 1
NAME problem-csr-y1-w2-h3.mps
ROWS
    N  OBJ
    G  c1
COLUMNS
    a  OBJ  1
    a  c1  1
    b  OBJ  1
    b  c1  1
RHS
    RHS1  c1  10
RANGES
BOUNDS
    UP BND1 b 10
QMATRIX
    a  a  2
    b  b  14
ENDATA
)";

    BOOST_CHECK_EQUAL(expectedMps, writer.mps);
}

BOOST_AUTO_TEST_CASE(falls_back_to_generated_names_when_names_are_disabled)
{
    PROBLEME_ANTARES_A_RESOUDRE problem;
    buildProblem(problem);
    CapturingWriter writer;

    writeQuadraticMps(problem, writer, "problem.mps", false);

    BOOST_CHECK(writer.mps.find("    x0  OBJ  1\n") != std::string::npos);
    BOOST_CHECK(writer.mps.find("    x1  x1  14\n") != std::string::npos);
    BOOST_CHECK(writer.mps.find("    G  r0\n") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
