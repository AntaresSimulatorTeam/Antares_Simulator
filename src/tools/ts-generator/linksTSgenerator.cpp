
#include "antares/tools/ts-generator/linksTSgenerator.h"

#include "antares/utils/utils.h"

namespace Antares::TSGenerator
{
// ==================
// Free functions
// ==================
std::vector<std::string> extractTargetAreas(const fs::path& sourceLinkDir)
{
    std::vector<std::string> to_return;
    fs::path pathToIni = sourceLinkDir / "properties.ini";
    IniFile ini;
    ini.open(pathToIni); // gp : we should handle reading issues
    for (auto s = ini.firstSection; s; s = s->next)
    {
        to_return.push_back(transformNameIntoID(s->name));
    }
    return to_return;
}

bool pairs_match(const LinkPair& p1, const LinkPair& p2)
{
    return (p1.first == p2.first && p1.second == p2.second)
           || (p1.first == p2.second && p1.second == p2.first);
}

const LinkPair* getMatchingPairInCollection(const LinkPair& pair, const LinkPairs& collection)
{
    for (const auto& p: collection)
    {
        if (pairs_match(pair, p))
        {
            return &p;
        }
    }
    return nullptr;
}

bool readLinkGeneralProperty(StudyParamsForLinkTS& params,
                             const Yuni::String& key,
                             const Yuni::String& value)
{
    if (key == "derated")
    {
        return value.to<bool>(params.derated);
    }
    if (key == "nbtimeserieslinks")
    {
        return value.to<unsigned int>(params.nbLinkTStoGenerate);
    }
    if (key == "seed-tsgen-links")
    {
        unsigned int seed{0};
        if (!value.to<unsigned int>(seed))
        {
            return false;
        }
        params.random.reset(seed);
        return true;
    }
    return true;
}

std::vector<LinkTSgenerationParams> CreateLinkList(const LinkPairs& linksFromCmdLine)
{
    std::vector<LinkTSgenerationParams> to_return;
    std::for_each(linksFromCmdLine.begin(),
                  linksFromCmdLine.end(),
                  [&to_return](const auto& link_pair)
                  {
                      LinkTSgenerationParams link;
                      link.namesPair = link_pair;
                      to_return.push_back(std::move(link));
                  });
    return to_return;
}

LinkTSgenerationParams* findLinkInList(const LinkPair& link_to_find,
                                       std::vector<LinkTSgenerationParams>& linkList)
{
    for (auto& link: linkList)
    {
        if (link.namesPair == link_to_find)
        {
            return &link;
        }
    }
    return nullptr;
}

bool readLinkIniProperty(LinkTSgenerationParams* link,
                         const Yuni::String& key,
                         const Yuni::String& value)
{
    if (key == "unitcount")
    {
        return value.to<uint>(link->unitCount);
    }

    if (key == "nominalcapacity")
    {
        return value.to<double>(link->nominalCapacity);
    }

    if (key == "law.planned")
    {
        return value.to(link->plannedLaw);
    }

    if (key == "law.forced")
    {
        return value.to(link->forcedLaw);
    }

    if (key == "volatility.planned")
    {
        return value.to(link->plannedVolatility);
    }

    if (key == "volatility.forced")
    {
        return value.to(link->forcedVolatility);
    }

    if (key == "force-no-generation")
    {
        return value.to<bool>(link->forceNoGeneration);
    }
    return true;
}

void readLinkIniProperties(LinkTSgenerationParams* link, IniFile::Section* section)
{
    for (const IniFile::Property* p = section->firstProperty; p; p = p->next)
    {
        if (!readLinkIniProperty(link, p->key, p->value))
        {
            std::string linkName = link->namesPair.first + "." + link->namesPair.second;
            logs.warning() << "Link '" << linkName << "' : reading value of '" << p->key
                           << "' went wrong";
            link->hasValidData = false;
        }
    }
}

void readSourceAreaIniFile(fs::path pathToIni,
                           std::string sourceAreaName,
                           std::vector<LinkTSgenerationParams>& linkList)
{
    IniFile ini;
    ini.open(pathToIni); // gp : we should handle reading problems here
    for (auto* section = ini.firstSection; section; section = section->next)
    {
        std::string targetAreaName = transformNameIntoID(section->name);
        const LinkPair processedLink = std::make_pair(sourceAreaName, targetAreaName);
        if (auto* foundLink = findLinkInList(processedLink, linkList); foundLink)
        {
            readLinkIniProperties(foundLink, section);
        }
    }
}

void readIniProperties(std::vector<LinkTSgenerationParams>& linkList, fs::path toLinksDir)
{
    for (auto& link: linkList)
    {
        std::string sourceAreaName = link.namesPair.first;
        fs::path pathToIni = toLinksDir / sourceAreaName / "properties.ini";
        readSourceAreaIniFile(pathToIni, sourceAreaName, linkList);
    }
}

fs::path makePreproFile(const fs::path& preproFilePath, const std::string& changingEnd)
{
    auto to_return = preproFilePath;
    to_return += changingEnd + ".txt";
    return to_return;
}

bool readLinkPreproTimeSeries(LinkTSgenerationParams& link, fs::path sourceAreaDir)
{
    bool to_return = true;
    const auto preproId = link.namesPair.first + "/" + link.namesPair.second;
    link.prepro = std::make_unique<Data::PreproAvailability>(preproId, link.unitCount);

    auto preproFileRoot = sourceAreaDir / "prepro" / link.namesPair.second;

    // Testing files existence
    auto preproFile = makePreproFile(preproFileRoot, "");
    auto modulationDirectFile = makePreproFile(preproFileRoot, "_mod_direct");
    auto modulationIndirectFile = makePreproFile(preproFileRoot, "_mod_indirect");
    std::vector<fs::path> paths{preproFile, modulationDirectFile, modulationIndirectFile};
    if (std::any_of(paths.begin(), paths.end(), [](auto& path) { return !fs::exists(path); }))
    {
        link.hasValidData = false;
        return false;
    }

    // Files loading
    to_return = link.prepro->data.loadFromCSVFile(preproFile.string(),
                                                  Data::PreproAvailability::preproAvailabilityMax,
                                                  DAYS_PER_YEAR)
                && link.prepro->validate() && to_return;

    to_return = link.modulationCapacityDirect.loadFromCSVFile(modulationDirectFile.string(),
                                                              1,
                                                              HOURS_PER_YEAR)
                && to_return;

    to_return = link.modulationCapacityIndirect.loadFromCSVFile(modulationIndirectFile.string(),
                                                                1,
                                                                HOURS_PER_YEAR)
                && to_return;

    link.hasValidData = link.hasValidData && to_return;
    return to_return;
}

void readPreproTimeSeries(std::vector<LinkTSgenerationParams>& linkList, fs::path toLinksDir)
{
    for (auto& link: linkList)
    {
        std::string sourceAreaName = link.namesPair.first;
        fs::path sourceAreaDir = toLinksDir / sourceAreaName;
        if (!readLinkPreproTimeSeries(link, sourceAreaDir))
        {
            logs.warning() << "Could not load all prepro/modulation data for link '"
                           << link.namesPair.first << "." << link.namesPair.second << "'";
        }
    }
}

// ==================
// Class methods
// ==================
LinksTSgenerator::LinksTSgenerator(Settings& settings):
    linksFromCmdLineOptions_(settings.linksListToGen),
    studyFolder_(settings.studyFolder),
    generateTSforAllLinks_(settings.allLinks)
{
}

void LinksTSgenerator::extractData()
{
    auto allLinksPairs = extractLinkNamesFromStudy();

    LinkPairs namesLinksToGenerate;
    if (generateTSforAllLinks_)
    {
        namesLinksToGenerate = allLinksPairs;
    }
    else
    {
        namesLinksToGenerate = extractLinkNamesFromCmdLine(allLinksPairs);
    }

    linkList_ = CreateLinkList(namesLinksToGenerate);
    extractLinksSpecificTSparameters();

    generalParams_ = readGeneralParamsForLinksTS();
}

LinkPairs LinksTSgenerator::extractLinkNamesFromStudy()
{
    LinkPairs to_return;
    fs::path linksDir = studyFolder_ / "input" / "links";
    for (const auto& item: fs::directory_iterator{linksDir})
    {
        if (item.is_directory())
        {
            std::string sourceAreaName = item.path().filename().generic_string();
            auto targetAreas = extractTargetAreas(item);
            for (auto& targetAreaName: targetAreas)
            {
                auto linkPair = std::make_pair(sourceAreaName, targetAreaName);
                to_return.push_back(linkPair);
            }
        }
    }
    return to_return;
}

LinkPairs LinksTSgenerator::extractLinkNamesFromCmdLine(const LinkPairs& allLinks)
{
    LinkPairs to_return;
    LinkPairs pairsFromCmdLine = splitStringIntoPairs(linksFromCmdLineOptions_, ';', '.');
    for (auto& p: pairsFromCmdLine)
    {
        if (const auto* found_pair = getMatchingPairInCollection(p, allLinks); found_pair)
        {
            to_return.push_back(*found_pair);
        }
        else
        {
            logs.error() << "Link '" << p.first << "." << p.second << "' not found";
        }
    }
    return to_return;
}

StudyParamsForLinkTS LinksTSgenerator::readGeneralParamsForLinksTS()
{
    StudyParamsForLinkTS to_return;
    fs::path pathToGeneraldata = studyFolder_ / "settings" / "generaldata.ini";
    IniFile ini;
    ini.open(pathToGeneraldata); // gp : we should handle reading issues
    for (auto* section = ini.firstSection; section; section = section->next)
    {
        // Skipping sections useless in the current context
        Yuni::String sectionName = section->name;
        if (sectionName != "general" && sectionName != "seeds - Mersenne Twister")
        {
            continue;
        }

        for (const IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            if (!readLinkGeneralProperty(to_return, p->key, p->value))
            {
                logs.warning() << ini.filename() << ": reading value of '" << p->key
                               << "' went wrong";
            }
        }
    }
    return to_return;
}

void LinksTSgenerator::extractLinksSpecificTSparameters()
{
    fs::path toLinksDir = studyFolder_ / "input" / "links";
    readIniProperties(linkList_, toLinksDir);
    readPreproTimeSeries(linkList_, toLinksDir);
}

bool LinksTSgenerator::generate()
{
    auto saveTSpath = fs::path(studyFolder_) / "output" / formatTime(getCurrentTime(), "%Y%m%d-%H%M");
    saveTSpath /= "ts-generator";
    saveTSpath /= "links";

    return generateLinkTimeSeries(linkList_, generalParams_, saveTSpath);
}

} // namespace Antares::TSGenerator
