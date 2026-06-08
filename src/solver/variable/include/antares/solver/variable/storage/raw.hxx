// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RAW_HXX__
#define __SOLVER_VARIABLE_STORAGE_RAW_HXX__

namespace Antares::Solver::Variable::R::AllYears
{
inline void Raw::initializeFromStudy(Antares::Data::Study& study)
{
    rawdata.initializeFromStudy(study);
}

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_RAW_HXX__
