
#pragma once

#include "tsGenerationOptions.h"

namespace Antares::TSGenerator {

class LinksTSgenerator
{
public:
    LinksTSgenerator(Settings&);
    void extractData();
    void generate();
private:
    Settings settings_;
};

} // End Antares::TSGenerator
