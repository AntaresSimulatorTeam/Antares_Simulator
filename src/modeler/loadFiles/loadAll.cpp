// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <fmt/format.h>

#include <antares/logs/logs.h>
#include <antares/solver/modeler/ModelerData.h>
#include "antares/solver/modeler/checks/checkLocation.h"
#include "antares/solver/modeler/loadFiles/loadFiles.h"
#include "antares/utils/utils.h"

using namespace Antares::ModelerStudy;

namespace Antares::Solver::LoadFiles
{

std::optional<ModelerData> loadAll(const std::filesystem::path& studyPath)
{
    Utils::TimeMeasurement measure;
    logs.info() << "Loading modeler files...";
    ModelerData data;
    warnOnYamlFiles(studyPath);
    auto res = loadLibraries(studyPath);
    if (!res.has_value())
    {
        return {};
    }
    std::tie(data.libraries, data.resolutionMode) = res.value();
    logs.info() << "Libraries loaded";

    data.system = std::make_unique<SystemModel::System>(loadSystem(studyPath, data.libraries));
    logs.info() << "System loaded";

    data.dataSeries = loadDataSeries(studyPath);
    logs.info() << "Timeseries loaded";

    data.scenarioGroupRepository = loadScenarioGroupRepository(studyPath);
    measure.tick();
    logs.info() << "Scenario groups loaded";

    logs.info() << "Modeler loaded in " << measure.toStringInSeconds();

    Checks::checkLocations(data);
    logs.info() << "Locations validity OK";

    return data;
}

} // namespace Antares::Solver::LoadFiles
