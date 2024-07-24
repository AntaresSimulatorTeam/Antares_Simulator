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

/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_LIBS_CONSTANTS_H__
#define __ANTARES_LIBS_CONSTANTS_H__

#include <array>

const unsigned int HOURS_PER_DAY = 24;

const unsigned int MONTHS_PER_YEAR = 12;
const unsigned int WEEKS_PER_YEAR = 53;
const unsigned int DAYS_PER_YEAR = 365;
const unsigned int HOURS_PER_YEAR = 8760;

namespace Antares::Constants
{
extern const std::array<unsigned int, MONTHS_PER_YEAR> daysPerMonth;
extern const unsigned int nbHoursInAWeek;
} // namespace Antares::Constants

enum AntaresConstants
{
    ant_k_area_name_max_length = 128,
    ant_k_cluster_name_max_length = 128,
    ant_k_constraint_name_max_length = 128,
};

#endif /* __ANTARES_LIBS_CONSTANTS_H__ */
