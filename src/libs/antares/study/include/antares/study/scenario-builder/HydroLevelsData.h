// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_DATA_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_DATA_H__

#include "scBuilderDataInterface.h"

namespace Antares::Data::ScenarioBuilder
{
/*!
** \brief Abstract base for hydro levels rules (initial or final), for all years and areas
*/
class HydroLevelsData: public dataInterface
{
public:
    //! Matrix
    using MatrixType = Matrix<double>;

    //! \name Data manipulation
    //@{
    /*!
    ** \brief Reset data from the study
    */
    bool reset(const Study& study) override;

    /*!
    ** \brief Assign a single value
    **
    ** \param index An area index
    ** \param year  A year
    ** \param value The new hydro level
    */
    void setTSnumber(uint index, uint year, double value);
    //@}

    uint width() const override;

    uint height() const override;

    double get_value(uint x, uint y) const;

    void set_value(uint x, uint y, double value);

    bool apply(Study& study) override = 0;

protected:
    //! Hydro levels overlay (0 if auto)
    MatrixType pHydroLevelsRules;

}; // class HydroLevelsData

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_DATA_H__
