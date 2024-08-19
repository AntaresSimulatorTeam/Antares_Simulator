#include "antares/tools/ts-generator/tsGenerationOptions.h"

#include <antares/logs/logs.h>

namespace Antares::TSGenerator
{

std::unique_ptr<Yuni::GetOpt::Parser> createTsGeneratorParser(Settings& settings)
{
    auto parser = std::make_unique<Yuni::GetOpt::Parser>();
    parser->addParagraph("Antares Time Series generator\n");

    parser->addFlag(settings.allThermal,
                    ' ',
                    "all-thermal",
                    "Generate TS for all thermal clusters");
    parser->addFlag(settings.thermalListToGen,
                    ' ',
                    "thermal",
                    "Generate TS for a list of area IDs and thermal clusters IDs, "
                    "\nusage: --thermal=\"areaID.clusterID;area2ID.clusterID\"");

    parser->addFlag(settings.allLinks, ' ', "all-links", "Generate TS capacities for all links");
    parser->addFlag(settings.linksListToGen,
                    ' ',
                    "links",
                    "Generate TS capacities for a list of 2 area IDs, "
                    "usage: --links=\"areaID.area2ID;area3ID.area1ID\"");

    parser->remainingArguments(settings.studyFolder);

    return parser;
}

bool parseOptions(int argc, const char* argv[], Settings& options)
{
    auto parser = createTsGeneratorParser(options);
    switch (auto ret = parser->operator()(argc, argv); ret)
    {
        using namespace Yuni::GetOpt;
    case ReturnCode::error:
        logs.error() << "Unknown arguments, aborting";
        return false;
    case ReturnCode::help:
        return false;
    default:
        break;
    }
    return true;
}

bool checkOptions(Settings& options)
{
    if (options.allThermal && !options.thermalListToGen.empty())
    {
        logs.error() << "Conflicting options, either choose all thermal clusters or a list";
        return false;
    }

    if (options.allLinks && !options.linksListToGen.empty())
    {
        logs.error() << "Conflicting options, either choose all links or a list";
        return false;
    }
    return true;
}

bool linkTSrequired(Settings& options)
{
    return options.allLinks || !options.linksListToGen.empty();
}

bool thermalTSrequired(Settings& options)
{
    return options.allThermal || !options.thermalListToGen.empty();
}
} // namespace Antares::TSGenerator
