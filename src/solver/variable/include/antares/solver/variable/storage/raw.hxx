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
#ifndef __SOLVER_VARIABLE_STORAGE_RAW_HXX__
#define __SOLVER_VARIABLE_STORAGE_RAW_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace R
{
namespace AllYears
{
template<class NextT, int FileFilter>
inline void Raw<NextT, FileFilter>::initializeFromStudy(Antares::Data::Study& study)
{
    rawdata.initializeFromStudy(study);
    // Next
    NextType::initializeFromStudy(study);
}

} // namespace AllYears
} // namespace R
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_RAW_HXX__
