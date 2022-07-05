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
#ifndef __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
#define __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>
#include <antares/mersenne-twister/mersenne-twister.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
class State;
}
} // namespace Solver

class HydroManagement final
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit HydroManagement(Data::Study& study);
    //! Destructor
    ~HydroManagement();
    //@}

    //! Get an initial reservoir level
    double randomReservoirLevel(double min, double avg, double max);

    //! Perform the hydro ventilation
    void operator()(double* randomReservoirLevel,
                    Solver::Variable::State& state,
                    uint y,
                    uint numSpace);

public:
    //! Random number generator
    MersenneTwister random;

    // forward declaration
    struct PerArea;

private:
    //! \name Steps
    //@{
    //! Prepare inflows scaling for each area
    void prepareInflowsScaling(uint numSpace);
    //! Prepare the net demand for each area
    template<enum Data::StudyMode ModeT>
    void prepareNetDemand(uint numSpace);
    //! Prepare the effective demand for each area
    void prepareEffectiveDemand(uint numSpace);
    //! Monthly Optimal generations
    void prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y, uint numSpace);

    //! Monthly target generations
    // note: inflows may have two different types, if in swap mode or not
    // \return The total inflow for the whole year
    double prepareMonthlyTargetGenerations(Data::Area& area, PerArea& data);

    void prepareDailyOptimalGenerations(Solver::Variable::State& state, uint y, uint numSpace);
    void prepareDailyOptimalGenerations(Solver::Variable::State& state,
                                        Data::Area& area,
                                        uint y,
                                        uint numSpace);
    //@}

    //! \name Utilities
    //@{
    //! Beta variable
    double BetaVariable(double a, double b);
    //! Gamma variable
    double GammaVariable(double a);
    //@}

    enum
    {
        //! The maximum number of days in a year
        dayYearCount = 366
    };


public:
    //! Reference to the study
    Data::Study& study;
    //! General data
    Data::Parameters& parameters;

    //! Temporary data
    struct PerArea
    {
        //! Monthly local effective demand
        double MLE[12];
        //! Monthly optimal generation
        double MOG[12];
        //! Monthly optimal level
        double MOL[12];
        //! Monthly target generations
        double MTG[12];
        //! inflows
        double inflows[12];

        //! Net demand, for each day of the year, for each area
        double DLN[dayYearCount];
        //! Daily local effective load
        double DLE[dayYearCount];
        //! Daily optimized Generation
        double DOG[dayYearCount];

    }; // struct PerArea

    //! Temporary data per area
    PerArea** pAreas;

}; // class HydroManagement

} // namespace Antares

#endif // __ANTARES_SOLVER_HYDRO_MANAGEMENT_MANAGEMENT_H__
