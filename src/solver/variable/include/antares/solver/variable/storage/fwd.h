// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_FORWARD_H__
#define __SOLVER_VARIABLE_STORAGE_FORWARD_H__

#include "../categories.h"

namespace Antares::Solver::Variable
{
// Forward declaration
struct Empty;

namespace R::AllYears
{
template<class NextT = Empty, int FileFilter = Variable::Category::FileLevel::allFile>
struct Raw;

template<class NextT = Empty, int FileFilter = Variable::Category::FileLevel::allFile>
struct Or;

} // namespace R::AllYears
} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STORAGE_FORWARD_H__
