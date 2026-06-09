// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <antares/study/area/area.h>
#include "antares/date/date.h"

namespace Antares
{

//! Prepare inflows scaling for each area
class PrepareInflows
{
public:
    PrepareInflows(Data::AreaList& areas, const Date::Calendar& calendar);
    void loadInflows(uint year);
    //! prepare data for Final reservoir level
    void changeInflowsToAccommodateFinalLevels(uint year);

private:
    Data::AreaList& areas_;
    const Date::Calendar& calendar_;
};

} // namespace Antares
