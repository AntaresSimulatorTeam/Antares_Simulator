/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __SOLVER_SIMULATION_SOLVER_DATA_H__
#define __SOLVER_SIMULATION_SOLVER_DATA_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Solver
{
namespace Private
{
namespace Simulation
{
class CacheData
{
public:
    /*!
    ** \brief Re-Initialize the inner variables
    */
    void initialize(const Antares::Data::Parameters& parameters);

public:
    //! True if the prepro `load` must be used
    bool haveToRefreshTSLoad;
    //! True if the prepro `solar` must be used
    bool haveToRefreshTSSolar;
    //! True if the prepro `wind` must be used
    bool haveToRefreshTSWind;
    //! True if the prepro `hydro` must be used
    bool haveToRefreshTSHydro;
    //! True if the prepro `thermal` must be used
    bool haveToRefreshTSThermal;

    //! Refresh time interval for Load
    unsigned int refreshIntervalLoad;
    //! Refresh time interval for Solar
    unsigned int refreshIntervalSolar;
    //! Refresh time interval for Hydro
    unsigned int refreshIntervalHydro;
    //! Refresh time interval for wind
    unsigned int refreshIntervalWind;
    //! Refresh time interval for thermal
    unsigned int refreshIntervalThermal;

}; // class CacheData

} // namespace Simulation
} // namespace Private
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_SIMULATION_SOLVER_H__
