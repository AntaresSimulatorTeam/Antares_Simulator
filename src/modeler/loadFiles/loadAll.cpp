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

void checkIntegerStrategies(const ModelerData& data)
{
    for (const auto& component: data.system->Components())
    {
        const auto heuristicType = component.heuristicType();
        if (!heuristicType)
        {
            continue;
        }
        if (data.resolutionMode == ResolutionMode::BENDERS_DECOMPOSITION)
        {
            throw ErrorLoadingYaml(
              fmt::format("Component '{}': integer strategy 'heuristic' is not compatible with "
                          "the benders-decomposition resolution mode",
                          component.Id()));
        }
        const auto* heuristic = data.heuristicFor(component);
        if (!heuristic)
        {
            const auto* model = component.getModel();
            throw ErrorLoadingYaml(
              fmt::format("Component '{}': heuristic '{}' is not declared in optim-config.yml "
                          "for model '{}.{}'",
                          component.Id(),
                          SystemModel::toString(*heuristicType),
                          model->LibraryId(),
                          model->Id()));
        }
        for (const auto& input: heuristic->inputs)
        {
            if (input.source != SystemModel::HeuristicInputSource::Parameter
                || SystemModel::expectedTimeDependency(input.element)
                     != SystemModel::HeuristicElementTimeDependency::Constant)
            {
                continue;
            }
            const auto& parameter = component.getParameterValues().at(input.id);
            if (LinearProblem::isTimeDependent(parameter.type))
            {
                throw ErrorLoadingYaml(
                  fmt::format("Component '{}': parameter '{}' is used as heuristic input '{}' and "
                              "must not be time-dependent",
                              component.Id(),
                              input.id,
                              SystemModel::toString(input.element)));
            }
        }
    }
}

ModelerData loadAll(const std::filesystem::path& studyPath)
{
    Utils::TimeMeasurement measure;
    logs.info() << "Loading modeler files...";
    ModelerData data;

    auto res = loadLibraries(studyPath);
    data.libraries = std::move(res.libraries);
    data.resolutionMode = res.resolutionMode;
    data.scenarioScope = std::move(res.scenarioScope);
    data.heuristicsByModel = std::move(res.heuristicsByModel);
    logs.info() << "Libraries loaded";

    auto loadedSystem = loadSystem(studyPath, data.libraries);
    data.system = std::make_unique<SystemModel::System>(std::move(loadedSystem.system));
    data.componentProperties = std::move(loadedSystem.componentProperties);
    logs.info() << "System loaded";

    data.dataSeries = loadDataSeries(studyPath);
    logs.info() << "Timeseries loaded";

    data.scenarioGroupRepository = loadScenarioGroupRepository(studyPath);
    measure.tick();
    logs.info() << "Scenario groups loaded";

    logs.info() << "Modeler loaded in " << measure.toStringInSeconds();

    Checks::checkLocations(data);
    logs.info() << "Locations validity OK";

    checkIntegerStrategies(data);

    return data;
}

} // namespace Antares::Solver::LoadFiles
