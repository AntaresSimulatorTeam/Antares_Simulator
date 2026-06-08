// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Data
{
class Study;
class Area;
class AreaLink;
} // namespace Antares::Data

namespace Antares::Solver::Variable
{

class State;

/*!
** \brief Empty default implementations of the per-scope lifecycle hooks
**
** Top-level container wrappers (SetsOfAreas, ...) are not Variable::IVariable
** subclasses, so unlike leaf variables they have no base supplying empty
** defaults for the lifecycle hooks they do not use. They would otherwise have
** to spell out a no-op version of every such hook. Deriving from this struct
** provides those no-ops; a wrapper only redefines the hooks it actually needs.
*/
struct ContainerScopeDefaults
{
    void initializeFromArea(Data::Study*, Data::Area*)
    {
    }

    void initializeFromLink(Data::Study*, Data::AreaLink*)
    {
    }

    void simulationBegin()
    {
    }

    void simulationEnd()
    {
    }

    void yearBegin(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void buildThermalClusterYearEndResults(State&, unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void yearEnd(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void computeSummary(unsigned int /*year*/, unsigned int /*numSpace*/)
    {
    }

    void hourBegin(unsigned int /*hourInTheYear*/)
    {
    }

    void hourForEachArea(State&, unsigned int /*numSpace*/)
    {
    }

    void hourForEachLink(State&)
    {
    }

    void hourEnd(State&, unsigned int /*hourInTheYear*/)
    {
    }

    void weekBegin(State&)
    {
    }

    void weekForEachArea(State&, unsigned int /*numSpace*/)
    {
    }

    void weekEnd(State&)
    {
    }
};

} // namespace Antares::Solver::Variable
