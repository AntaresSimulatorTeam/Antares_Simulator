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
#ifndef __SOLVER_SIMULATION_SOLVER_DATA_H__
#define __SOLVER_SIMULATION_SOLVER_DATA_H__

#include <antares/study/fwd.h>

namespace Antares::Solver::Private::Simulation
{
class CacheData
{
public:
    /*!
    ** \brief Re-Initialize the inner variables
    */
    void initialize(const Antares::Data::Parameters& parameters);

public:
    //! True if the prepro `solar` must be used
    bool haveToRefreshTSSolar;
    //! True if the prepro `wind` must be used
    bool haveToRefreshTSWind;
    //! True if the prepro `hydro` must be used
    bool haveToRefreshTSHydro;
    //! True if the prepro `thermal` must be used
    bool haveToRefreshTSThermal;

    //! Refresh time interval for Solar
    unsigned int refreshIntervalSolar;
    //! Refresh time interval for Hydro
    unsigned int refreshIntervalHydro;
    //! Refresh time interval for wind
    unsigned int refreshIntervalWind;
    //! Refresh time interval for thermal
    unsigned int refreshIntervalThermal;

}; // class CacheData

} // namespace Antares::Solver::Private::Simulation

#endif // __SOLVER_SIMULATION_SOLVER_H__
