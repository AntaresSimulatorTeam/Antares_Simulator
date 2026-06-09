// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/tuple_variable_list.h"

namespace Antares::Solver::Variable::Common
{

// Variadic composition of output variables.
//
// Each variable is a standalone class; ComposeAll flattens them into a
// tuple-based dispatcher.
template<class... Vs>
struct ComposeAll
{
    using type = Container::TupleVariableList<Vs...>;
};

} // namespace Antares::Solver::Variable::Common
