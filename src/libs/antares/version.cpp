// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/antares/version.h"

#include <iostream>

#include "antares/config/config.h"

namespace Antares
{
void PrintVersionToStdCout()
{
#ifdef GIT_SHA1_SHORT_STRING
    std::cout << ANTARES_VERSION_STR << " (revision " << GIT_SHA1_SHORT_STRING << ")" << std::endl;
#else
    std::cout << ANTARES_VERSION_STR << std::endl;
#endif
}

const char* VersionToCString()
{
    return ANTARES_VERSION_PUB_STR;
}

} // namespace Antares
