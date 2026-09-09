// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <array>
#include <optional>
#include <string_view>

// Kept in Antares::Data, not Antares::IO::Outputs: this header is pulled in by
// study/parameters.h, which lands in a great many translation units -- several
// of them legacy tools that do `using namespace Yuni; using namespace Antares;`
// and then write a bare `IO::File::...`. Opening Antares::IO here would make
// that `IO` ambiguous with Yuni::IO. OptimisationsSimulationTable re-exports the
// names below into its own namespace for the code that already expects them.
namespace Antares::IO::Outputs
{
// A stage of the weekly resolution that can get its own simulation table. The
// first two are produced during the weekly solve, the others once a
// post-process has moved the results. The enumerators are ordered as the weekly
// resolution reaches them, which is also the order a std::map keyed by Stage
// iterates in.
enum class Stage
{
    firstOptim,
    secondOptim,
    remixHydro,
    // The whole CSR treatment, not just the patch: curtailment sharing, DTG
    // netting and the marginal price update.
    adequacyPatchCsr,
};

// Every stage, in the order the weekly resolution reaches them.
inline constexpr std::array allStages = {Stage::firstOptim,
                                                   Stage::secondOptim,
                                                   Stage::remixHydro,
                                                   Stage::adequacyPatchCsr};

// The wire name of a stage: it is part of the output file names and of the
// `simulation-table-stages` generaldata.ini value, so these strings must not
// change.
constexpr std::string_view stageName(Stage stage)
{
    switch (stage)
    {
    case Stage::firstOptim:
        return "optim-nb-1";
    case Stage::secondOptim:
        return "optim-nb-2";
    case Stage::remixHydro:
        return "remix-hydro";
    case Stage::adequacyPatchCsr:
        return "adq-patch-csr";
    }
    return {};
}

// The stage that goes by `name`, or std::nullopt when no stage does.
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
} // namespace Antares::Data
