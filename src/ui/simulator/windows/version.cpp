// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "version.h"
#include <antares/config/config.h>

namespace Antares
{
wxString VersionToWxString()
{
    return wxString(wxT("Antares v")) << wxT(ANTARES_VERSION_STR);
}

} // namespace Antares
