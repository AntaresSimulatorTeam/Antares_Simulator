// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/locale/locale.h"

#include <clocale>
#include <iostream>

#include <yuni/yuni.h>

namespace Antares
{
void InitializeDefaultLocale()
{
#ifdef YUNI_OS_WINDOWS
    if (!std::setlocale(LC_ALL, "English"))
    {
        std::cerr << "impossible to set locale to English" << std::endl;
    }

#else
    if (!std::setlocale(LC_ALL, "en_US.utf8"))
    {
        std::cerr << "impossible to set locale to en_US.utf8" << std::endl;
    }
#endif
}
} // namespace Antares
