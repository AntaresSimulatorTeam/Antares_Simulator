// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/InactiveComponentsAnalyzer.h"

#include <algorithm>

namespace Antares::Optimization
{

bool InactiveComponentsAnalyzer::getFlag(const std::vector<bool>& flags, uint32_t pays)
{
    return pays < flags.size() && flags[pays];
}

void InactiveComponentsAnalyzer::setFlag(std::vector<bool>& flags, uint32_t pays, bool value)
{
    if (pays >= flags.size())
    {
        flags.resize(pays + 1, false);
    }
    flags[pays] = value;
}

void InactiveComponentsAnalyzer::setLoadAllZero(uint32_t pays, bool value)
{
    setFlag(loadAllZero_, pays, value);
}

void InactiveComponentsAnalyzer::setRorAllZero(uint32_t pays, bool value)
{
    setFlag(rorAllZero_, pays, value);
}

void InactiveComponentsAnalyzer::setSolarAllZero(uint32_t pays, bool value)
{
    setFlag(solarAllZero_, pays, value);
}

void InactiveComponentsAnalyzer::setWindAllZero(uint32_t pays, bool value)
{
    setFlag(windAllZero_, pays, value);
}

void InactiveComponentsAnalyzer::setMiscGenColumnAllZero(uint32_t pays, unsigned column, bool value)
{
    if (pays >= miscGenColumnAllZero_.size())
    {
        miscGenColumnAllZero_.resize(pays + 1);
    }
    auto& columns = miscGenColumnAllZero_[pays];
    if (column >= columns.size())
    {
        columns.resize(column + 1, false);
    }
    columns[column] = value;
}

void InactiveComponentsAnalyzer::setHydroInflowAllZero(uint32_t pays, bool value)
{
    setFlag(hydroInflowAllZero_, pays, value);
}

void InactiveComponentsAnalyzer::setLinkAllZero(uint32_t interco, bool value)
{
    setFlag(linkAllZero_, interco, value);
}

bool InactiveComponentsAnalyzer::loadIsAllZero(uint32_t pays) const
{
    return getFlag(loadAllZero_, pays);
}

bool InactiveComponentsAnalyzer::rorIsAllZero(uint32_t pays) const
{
    return getFlag(rorAllZero_, pays);
}

bool InactiveComponentsAnalyzer::solarIsAllZero(uint32_t pays) const
{
    return getFlag(solarAllZero_, pays);
}

bool InactiveComponentsAnalyzer::windIsAllZero(uint32_t pays) const
{
    return getFlag(windAllZero_, pays);
}

bool InactiveComponentsAnalyzer::miscGenColumnIsAllZero(uint32_t pays, unsigned column) const
{
    if (pays >= miscGenColumnAllZero_.size())
    {
        return false;
    }
    const auto& columns = miscGenColumnAllZero_[pays];
    return column < columns.size() && columns[column];
}

bool InactiveComponentsAnalyzer::hydroInflowIsAllZero(uint32_t pays) const
{
    return getFlag(hydroInflowAllZero_, pays);
}

bool InactiveComponentsAnalyzer::linkIsAllZero(uint32_t interco) const
{
    return getFlag(linkAllZero_, interco);
}

} // namespace Antares::Optimization
