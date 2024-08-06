
#pragma once

#include <string>

#include <yuni/core/getopt.h>

namespace Antares::TSGenerator
{
struct Settings
{
    std::string studyFolder;

    /// generate TS for all clusters if activated
    bool allThermal = false;
    /// generate TS for a list "area.cluster;area2.cluster2;"
    std::string thermalListToGen = "";

    /// generate TS for all links if activated
    bool allLinks = false;
    /// generate TS for a list "area.link;area2.link2;"
    std::string linksListToGen;
};

bool parseOptions(int, char*[], Settings&);
std::unique_ptr<Yuni::GetOpt::Parser> createTsGeneratorParser(Settings&);

bool checkOptions(Settings& options);
bool linkTSrequired(Settings& options);
bool thermalTSrequired(Settings& options);
} // namespace Antares::TSGenerator
