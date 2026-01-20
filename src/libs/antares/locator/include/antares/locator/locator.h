// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_SOLVER_H__
#define __ANTARES_LIBS_SOLVER_H__

#include <yuni/yuni.h>

#include "antares/study/version.h"

namespace Antares::Solver
{
enum Feature
{
    //! The standard solver
    standard = 0,
    //! The solver with years computed in parallel
    parallel,
};

/*!
** \brief Find the location of the solver
** \return A non-empty string if the solver has been found, empty otherwise
*/
bool FindLocation(Yuni::String& location);

/*!
** \brief Find the location of the constraints builder
** \return A non-empty string if the solver has been found, empty otherwise
*/
bool FindConstraintsBuilderLocation(Yuni::String& location);

/*!
** \brief Try to find the location of `antares-ybyaggregator`
*/
bool FindYearByYearAggregator(Yuni::String& filename);

} // namespace Antares::Solver

#endif // __ANTARES_LIBS_SOLVER_H__
