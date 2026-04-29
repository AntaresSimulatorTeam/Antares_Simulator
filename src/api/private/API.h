// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include <antares/optimization-options/options.h>
#include <antares/study-loader/IStudyLoader.h>
#include "antares/api/SimulationResults.h"

namespace Antares::Data
{
class Study;
}

namespace Antares::API
{

/**
 * @class APIInternal
 * @brief The APIInternal class is used to run simulations.
 */
class APIInternal
{
public:
    /**
     * @brief The run method is used to run the simulation.
     * @param study_loader A pointer to an IStudyLoader object. The IStudyLoader object is used to
     * load the study that will be simulated.
     * @return SimulationResults object which contains the results of the simulation.
     */
    SimulationResults run(const IStudyLoader& study_loader,
                          const std::filesystem::path& output,
                          const Antares::Solver::Optimization::OptimizationOptions& optOptions);

private:
    std::shared_ptr<Antares::Data::Study> study_;
    SimulationResults execute(
      const std::filesystem::path& output,
      const Antares::Solver::Optimization::OptimizationOptions& optOptions) const;
};

} // namespace Antares::API
