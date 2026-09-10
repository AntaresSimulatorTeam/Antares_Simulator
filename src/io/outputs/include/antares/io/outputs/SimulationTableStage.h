// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace Antares::IO::Outputs
{
/**
 * \brief A stage of the weekly resolution that can get its own simulation table.
 *
 * Enumerators are ordered as the weekly resolution reaches them, which is also
 * the iteration order of a std::map keyed by Stage.
 */
enum class Stage
{
    firstOptim,
    secondOptim,
    peakShaving,
    /// The whole CSR treatment: curtailment sharing, DTG netting and the
    /// marginal price update.
    adequacyPatch,
};

/// \brief Every stage, in the order the weekly resolution reaches them.
inline constexpr std::array allStages = {Stage::firstOptim,
                                         Stage::secondOptim,
                                         Stage::peakShaving,
                                         Stage::adequacyPatch};

/// \brief Wire name of a stage: used in output file names and the
/// `simulation-table-stages` generaldata.ini value, so these strings are stable.
constexpr std::string_view stageName(Stage stage)
{
    switch (stage)
    {
    case Stage::firstOptim:
        return "optim-nb-1";
    case Stage::secondOptim:
        return "optim-nb-2";
    case Stage::peakShaving:
        return "shave-peaks";
    case Stage::adequacyPatch:
        return "adq-patch";
    }
    return {};
}

/// \brief The stage that goes by `name`, or std::nullopt when none does.
constexpr std::optional<Stage> stageFromName(std::string_view name)
{
    for (const auto stage: allStages)
    {
        if (stageName(stage) == name)
        {
            return stage;
        }
    }
    return std::nullopt;
}
} // namespace Antares::IO::Outputs
