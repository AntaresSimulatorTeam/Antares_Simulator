
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"

#include "antares/solver/utils/filename.h"

namespace Antares::Solver
{

namespace
{
/**
 * @brief Copies elements from one container to another.
 *
 * This function takes two containers as arguments. It copies elements from the first container to
 * the second one.
 *
 * @param in The container from which to copy elements.
 * @param out The container to which to copy elements.
 */
template<class T, class U>
void copy(const T& in, U& out)
{
    std::ranges::copy(in, std::back_inserter(out));
}

template<class T>
void resizeIfLargerThan(T& in, std::size_t size)
{
    if (in.size() > size)
    {
        in.resize(size);
    }
}
} // namespace

WeeklyDataFromAntares HebdoProblemToLpsTranslator::translate(
  const PROBLEME_ANTARES_A_RESOUDRE* problem,
  std::string_view name) const
{
    WeeklyDataFromAntares ret;
    if (problem == nullptr)
    {
        return ret;
    }

    copy(problem->CoutLineaire, ret.LinearCost);
    copy(problem->Xmax, ret.Xmax);
    copy(problem->Xmin, ret.Xmin);
    copy(problem->SecondMembre, ret.RHS);
    copy(problem->Sens, ret.Direction);
    resizeIfLargerThan(ret.Direction, problem->NombreDeContraintes);
    copy(name, ret.name);

    return ret;
}

ConstantDataFromAntares HebdoProblemToLpsTranslator::commonProblemData(
  const PROBLEME_ANTARES_A_RESOUDRE* problem) const
{
    if (problem == nullptr)
    {
        return {};
    }

    if (problem->NombreDeVariables <= 0)
    {
        throw WeeklyProblemTranslationException("VariablesCount must be strictly positive");
    }
    if (problem->NombreDeContraintes <= 0)
    {
        throw WeeklyProblemTranslationException("ConstraintesCount must be strictly positive");
    }

    if (problem->NombreDeContraintes > (int)problem->IndicesDebutDeLigne.size())
    {
        throw WeeklyProblemTranslationException(
          "ConstraintesCount exceed IndicesDebutDeLigne size");
    }

    if (problem->NombreDeContraintes > (int)problem->NombreDeTermesDesLignes.size())
    {
        throw WeeklyProblemTranslationException(
          "ConstraintesCount exceed NombreDeTermesDesLignes size");
    }

    ConstantDataFromAntares ret;

    ret.VariablesCount = problem->NombreDeVariables;
    ret.ConstraintesCount = problem->NombreDeContraintes;

    ret.CoeffCount = problem->IndicesDebutDeLigne[problem->NombreDeContraintes - 1]
                     + problem->NombreDeTermesDesLignes[problem->NombreDeContraintes - 1];

    copy(problem->CoefficientsDeLaMatriceDesContraintes, ret.ConstraintsMatrixCoeff);
    ret.ConstraintsMatrixCoeff.resize(ret.CoeffCount);
    copy(problem->IndicesColonnes, ret.ColumnIndexes);
    ret.ColumnIndexes.resize(ret.CoeffCount);

    copy(problem->IndicesDebutDeLigne, ret.Mdeb);
    resizeIfLargerThan(ret.Mdeb, ret.ConstraintesCount);
    // Add a final coeff
    ret.Mdeb.push_back(ret.CoeffCount);
    copy(problem->NomDesVariables, ret.VariablesMeaning);

    copy(problem->NomDesContraintes, ret.ConstraintsMeaning);

    // Until the problem is built, the number of variables & constraints is a rough estimate
    // It is then updated to the exact number resp. ret.VariablesCount and ret.ConstraintesCount.
    // To avoid wasting memory and errors, we resize ret.VariablesMeaning and ret.ConstraintsMeaning
    resizeIfLargerThan(ret.VariablesMeaning, ret.VariablesCount);
    resizeIfLargerThan(ret.ConstraintsMeaning, ret.ConstraintesCount);
    return ret;
}

WeeklyProblemTranslationException::WeeklyProblemTranslationException(
  const std::string& string) noexcept:
    std::runtime_error{string}
{
}
} // namespace Antares::Solver
