// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/registerThematicTrimmingVariables.h"

#include <antares/solver/variable/economy/all.h>
#include <antares/study/variable-print-info.h>

namespace Antares::Solver::Variable
{
void RegisterThematicTrimmingVariables()
{
    Antares::Data::ThematicTrimmingVariableRegistry::setProvider(
      [](Antares::Data::variablePrintInfoCollector& collector)
      { Economy::AllVariables::RetrieveVariableList(collector); });
}
} // namespace Antares::Solver::Variable
