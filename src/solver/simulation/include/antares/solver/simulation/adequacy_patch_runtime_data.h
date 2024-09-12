/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once
#include <set>
#include <vector>

#include <antares/study/fwd.h>
#include <antares/study/study.h>

using adqPatchParamsMode = Antares::Data::AdequacyPatch::AdequacyPatchMode;

class AdequacyPatchRuntimeData
{
private:
    std::vector<std::set<int>> csrTriggeredHoursPerArea_;

public:
    explicit AdequacyPatchRuntimeData() = default;
    AdequacyPatchRuntimeData(const Antares::Data::AreaList& areas,
                             const std::vector<Antares::Data::AreaLink*>& links);

    std::vector<adqPatchParamsMode> areaMode;
    std::vector<adqPatchParamsMode> originAreaMode;
    std::vector<adqPatchParamsMode> extremityAreaMode;
    std::vector<double> hurdleCostCoefficients;

    bool wasCSRTriggeredAtAreaHour(int area, int hour) const;
    void addCSRTriggeredAtAreaHour(int area, int hour);
};
