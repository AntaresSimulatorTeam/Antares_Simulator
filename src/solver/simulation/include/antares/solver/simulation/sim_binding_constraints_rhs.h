// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Data
{
class BindingConstraintGroupRepository;
class BindingConstraintsRepository;
} // namespace Antares::Data

struct PROBLEME_HEBDO;

namespace Simulation
{
void setBindingConstraintsRHS(PROBLEME_HEBDO& problem,
                              const Antares::Data::BindingConstraintsRepository& bindingConstraints,
                              const Antares::Data::BindingConstraintGroupRepository& bcgroups,
                              const unsigned PasDeTempsDebut,
                              const unsigned weekFirstDay);
} // namespace Simulation
