// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares
{
namespace Data
{
class Study;
}

namespace Solver
{

class ScenarioBuilderOwner
{
public:
    explicit ScenarioBuilderOwner(Antares::Data::Study& study);

    void callScenarioBuilder();

private:
    Antares::Data::Study& study_;
};
} // namespace Solver
} // namespace Antares
