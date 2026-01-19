// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/area/area.h>
#include "antares/date/date.h"

namespace Antares
{

//! Prepare minimum generation scaling for each area
class MinGenerationScaling final
{
public:
    MinGenerationScaling(Data::AreaList& areas, const Date::Calendar& calendar);
    void Run(uint year);

private:
    Data::AreaList& areas_;
    const Date::Calendar& calendar_;
};

} // namespace Antares
