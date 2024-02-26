#include <memory>
#include <string>

#include <antares/exception/LoadingError.hpp>
#include <antares/study/header.h>
#include <antares/study/study.h>
#include <antares/logs/logs.h>
#include <yuni/core/getopt.h>

#include <antares/timeseries-generation/timeseries-generation.h>

using namespace Antares;

std::unique_ptr<Yuni::GetOpt::Parser> createTsGeneratorParser(TsGeneratorSettings& settings)
{
    auto parser = std::unique_ptr<Yuni::GetOpt::Parser>(new Yuni::GetOpt::Parser());
    parser->addParagraph("Antares Time Series generator\n");

    parser->addFlag(settings.allThermal, ' ', "all-thermal", "Generate TS for all thermal clusters");

    parser->addFlag(settings.allThermal, 't', "thermal", "Generate TS for a list of thermal clusters");

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

    for (auto& [name, area] : study->areas)
        for (auto& c : area->thermal.list.all())
            logs.notice() << c->name();

    return 0;
}

