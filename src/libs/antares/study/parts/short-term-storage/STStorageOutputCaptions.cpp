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

#include <vector>
#include "antares/study/parts/short-term-storage/STStorageOutputCaptions.h"

namespace Antares::Data::ShortTermStorage
{
static const std::vector<std::string> groups = {"PSP_open",
                                                "PSP_closed",
                                                "Pondage",
                                                "Battery",
                                                "Other1",
                                                "Other2",
                                                "Other3",
                                                "Other4",
                                                "Other5"};

static const std::vector<std::string> variables = {"injection", "withdrawal", "level"};

const unsigned int nb_groups = groups.size();
const unsigned int nb_variables = variables.size();

std::string getVariableCaptionFromColumnIndex(unsigned int col_index)
{
    if (col_index < nb_groups * nb_variables)
    {
        unsigned int group_index = col_index / nb_variables;
        unsigned int variable_index = col_index % nb_variables;
        return groups[group_index] + "_" + variables[variable_index];
    }
    else
        return "<unknown>";
}

} // End namespace Antares::Data::ShortTermStorage
