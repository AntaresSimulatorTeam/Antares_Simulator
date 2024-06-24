
#pragma once

#include "tsGenerationOptions.h"
#include <antares/solver/ts-generator/generator.h>

namespace fs = std::filesystem;

namespace Antares::TSGenerator {

class LinksTSgenerator
{
public:
    LinksTSgenerator(Settings&);
    void extractData();
    bool generate();

private:
    LinkPairs extractLinkNamesFromStudy();
    LinkPairs extractLinkNamesFromCmdLine(const LinkPairs&);
    StudyParamsForLinkTS readGeneralParamsForLinksTS();
    void extractLinksSpecificTSparameters();

    std::string linksFromCmdLineOptions_;
    fs::path studyFolder_;
    bool generateTSforAllLinks_ = false;
    std::vector<LinkTSgenerationParams> linkList_;
    StudyParamsForLinkTS generalParams_;
};

} // End Antares::TSGenerator
