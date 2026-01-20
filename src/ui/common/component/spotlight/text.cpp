// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "spotlight.h"

using namespace Yuni;

namespace Antares::Component
{
Spotlight::Text::Text()
{
    // This item should not be taken into consideration in the result set
    pCountedAsResult = false;
}

Spotlight::Text::~Text()
{
    // destructor
}

} // namespace Antares::Component
