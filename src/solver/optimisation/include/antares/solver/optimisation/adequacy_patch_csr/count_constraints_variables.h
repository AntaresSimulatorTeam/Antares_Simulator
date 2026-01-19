// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Data::AdequacyPatch
{
int countConstraints(const PROBLEME_HEBDO* problemeHebdo);
int countVariables(const PROBLEME_HEBDO* problemeHebdo);
} // namespace Antares::Data::AdequacyPatch
