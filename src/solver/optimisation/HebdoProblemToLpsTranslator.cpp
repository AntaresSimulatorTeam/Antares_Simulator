
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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

template<class T, class U>
void copy(const SparseVector<T>& in, U& out)
{
    copy(in.extract(), out);
}

} // namespace

WeeklyDataFromAntares HebdoProblemToLpsTranslator::translate(
  const PROBLEME_ANTARES_A_RESOUDRE* problem,
  std::string_view name) const
{
    if (problem == nullptr)
    {
        return {};
    }
    auto ret = WeeklyDataFromAntares();

    copy(problem->CoutLineaire, ret.LinearCost);
    copy(problem->Xmax, ret.Xmax);
    copy(problem->Xmin, ret.Xmin);
    copy(problem->NomDesVariables, ret.variables);
    copy(problem->NomDesContraintes, ret.constraints);
    copy(problem->SecondMembre, ret.RHS);
    copy(problem->Sens, ret.Direction);

    copy(name, ret.name);

    return ret;
}

ConstantDataFromAntares HebdoProblemToLpsTranslator::commonProblemData(
  const PROBLEME_ANTARES_A_RESOUDRE* problem) const
{
    if (problem == nullptr)
    {
        return ConstantDataFromAntares();
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

    copy(problem->TypeDeVariable, ret.VariablesType);

    copy(problem->CoefficientsDeLaMatriceDesContraintes, ret.ConstraintsMatrixCoeff);
    ret.ConstraintsMatrixCoeff.resize(ret.CoeffCount);
    copy(problem->IndicesColonnes, ret.ColumnIndexes);
    ret.ColumnIndexes.resize(ret.CoeffCount);
    copy(problem->IndicesDebutDeLigne, ret.Mdeb);
    ret.Mdeb.push_back(ret.CoeffCount);
    return ret;
}

WeeklyProblemTranslationException::WeeklyProblemTranslationException(
  const std::string& string) noexcept:
    std::runtime_error{string}
{
}
} // namespace Antares::Solver
