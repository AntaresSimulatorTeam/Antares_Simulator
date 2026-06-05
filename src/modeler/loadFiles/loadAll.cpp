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

namespace
{
/// Returns the path to the shipped default variable-names.yml.
/// For installed builds this is the FHS data-dir location configured at CMake time.
/// For development builds the source-tree copy is used as a fallback.
std::filesystem::path defaultVariableNamesFilePath()
{
    const std::filesystem::path installPath(ANTARES_VARIABLE_NAMES_INSTALL_PATH);
    if (std::filesystem::exists(installPath))
    {
        return installPath;
    }
    return std::filesystem::path(ANTARES_VARIABLE_NAMES_SOURCE_PATH);
}
} // namespace

std::optional<ModelerData> loadAll(const std::filesystem::path& studyPath)
{
    Utils::TimeMeasurement measure;
    logs.info() << "Loading modeler files...";
    ModelerData data;
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
    logs.info() << "Scenario groups loaded";

    data.variableNameMapper = VariableNameMapper(studyPath, defaultVariableNamesFilePath());
    measure.tick();
    logs.info() << "Variable names loaded";

    logs.info() << "Modeler loaded in " << measure.toStringInSeconds();

    Checks::checkLocations(data);
    logs.info() << "Locations validity OK";

    return data;
}

} // namespace Antares::Solver::LoadFiles
