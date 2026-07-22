// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace Antares::Data
{
/*!
** \brief Selects which output families to write
**
** This class centralizes the output selection logic. It can be used both
** in study parameters (for INI-based defaults) and in command-line settings
** (for overrides).
*/
class OutputSelection
{
public:
    enum Value
    {
        All,            // all outputs (Monte-Carlo + simulation tables)
        None,           // no outputs
        MonteCarlo,     // Monte-Carlo results only (mc-all, mc-ind)
        SimulationTable // simulation table(s) only
    };

    explicit OutputSelection(Value v = MonteCarlo):
        value_(v)
    {
    }

    void fromString(const std::string& input)
    {
        if (input.empty() || input == "monte-carlo")
        {
            value_ = MonteCarlo;
        }
        else if (input == "all")
        {
            value_ = All;
        }
        else if (input == "none")
        {
            value_ = None;
        }
        else if (input == "simulation-tables")
        {
            value_ = SimulationTable;
        }
        else
        {
            throw std::runtime_error("Invalid value for --output: '" + input
                                     + "' (expected all, none, monte-carlo or simulation-tables)");
        }
    }

    //! Get if Monte-Carlo result files (mc-all, mc-ind) must be written
    bool shouldExportMonteCarloResults() const
    {
        return value_ == All || value_ == MonteCarlo;
    }

    //! Get if simulation table(s) must be written
    bool shouldExportSimulationTable() const
    {
        return value_ == All || value_ == SimulationTable;
    }

    bool operator==(Value v) const
    {
        return value_ == v;
    }

    //! Get the current value
    Value value() const
    {
        return value_;
    }

private:
    Value value_ = MonteCarlo;
};

} // namespace Antares::Data
