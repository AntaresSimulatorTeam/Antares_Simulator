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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__

#include <functional>

#include "scBuilderDataInterface.h"

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
/*!
** \brief Rules for hydro levels, for all years and areas
*/
class hydroLevelsData final: public dataInterface
{
public:
    //! Matrix
    using MatrixType = Matrix<double>;

public:
    // Constructor

    hydroLevelsData(const std::string& iniFilePrefix,
                    std::function<void(Study&, MatrixType&)> applyToTarget);

    //! \name Data manupulation
    //@{
    /*!
    ** \brief Reset data from the study
    */
    bool reset(const Study& study);

    /*!
    ** \brief Export the data into a mere INI file
    */
    void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const override;

    /*!
    ** \brief Assign a single value
    **
    ** \param index An area index
    ** \param year  A year
    ** \param value The new hydro level
    */
    void setTSnumber(uint index, uint year, double value);
    //@}

    uint width() const;

    uint height() const;

    double get_value(uint x, uint y) const;

    void set_value(uint x, uint y, double value);

    bool apply(Study& study) override;

private:
    //! Hydro levels overlay (0 if auto)
    MatrixType pHydroLevelsRules;
    // prefix to be added when calling saveToINIFileHydroLevel
    const std::string addToPrefix_;

    std::function<void(Study&, MatrixType&)> applyToTarget_;

}; // class hydroLevelsData

// class hydroLevelsData : inline functions

inline void hydroLevelsData::setTSnumber(uint areaindex, uint year, double value)
{
    assert(areaindex < pHydroLevelsRules.width);
    if (year < pHydroLevelsRules.height)
    {
        pHydroLevelsRules[areaindex][year] = value;
    }
}

inline uint hydroLevelsData::width() const
{
    return pHydroLevelsRules.width;
}

inline uint hydroLevelsData::height() const
{
    return pHydroLevelsRules.height;
}

inline double hydroLevelsData::get_value(uint x, uint y) const
{
    return pHydroLevelsRules.entry[y][x];
}

inline void initLevelApply(Study& study, Matrix<double>& matrix)
{
    study.scenarioInitialHydroLevels.copyFrom(matrix);
}

inline void finalLevelApply(Study& study, Matrix<double>& matrix)
{
    study.scenarioFinalHydroLevels.copyFrom(matrix);
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__
