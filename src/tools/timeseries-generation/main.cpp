/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <memory>
#include <string>

#include <antares/solver/ts-generator/generator.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/study/header.h>
#include <antares/study/study.h>
#include <antares/logs/logs.h>
#include <yuni/core/getopt.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/writer/writer_factory.h>
#include <antares/writer/result_format.h>

#include <antares/timeseries-generation/timeseries-generation.h>

using namespace Antares;

std::unique_ptr<Yuni::GetOpt::Parser> createTsGeneratorParser(TsGeneratorSettings& settings)
{
    auto parser = std::make_unique<Yuni::GetOpt::Parser>();
    parser->addParagraph("Antares Time Series generator\n");

    parser->addFlag(settings.allThermal, ' ', "all-thermal", "Generate TS for all thermal clusters");

    parser->addFlag(settings.thermalListToGen, 't', "thermal", "Generate TS for a list of thermal clusters");

    parser->remainingArguments(settings.studyFolder);


    return parser;
}

int main(int argc, char *argv[])
{
    TsGeneratorSettings settings;

    auto parser = createTsGeneratorParser(settings);
    switch (auto ret = parser->operator()(argc, argv); ret)
    {
        using namespace Yuni::GetOpt;
    case ReturnCode::error:
        throw Error::CommandLineArguments(parser->errors());
    case ReturnCode::help:
        // End the program
        return 0;
    default:
        break;
    }

    auto study = std::make_shared<Data::Study>(true);
    Data::StudyLoadOptions studyOptions;

    if (!study->loadFromFolder(settings.studyFolder, studyOptions))
    {
        if (settings.studyFolder.empty())
            logs.error() << "No study given to the generator";
        return 1;
    }

    Benchmarking::NullDurationCollector nullDurationCollector;

    study->parameters.noOutput = false;
    auto resultWriter = Solver::resultWriterFactory(
            Data::ResultFormat::legacyFilesDirectories, study->folderOutput, nullptr, nullDurationCollector);

    auto clusters = TSGenerator::getClustersToGen(study->areas, settings.thermalListToGen, true, true);

    for (auto& c : clusters)
        logs.notice() << c->id();

    return TSGenerator::GenerateThermalTimeSeries(*study, clusters, 0, *resultWriter);
}

