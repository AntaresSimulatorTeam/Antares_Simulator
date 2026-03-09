// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__

#include <antares/array/matrix.h>
#include <antares/series/series.h>

#include "../../fwd.h"
#include "defines.h"

namespace Antares::Data
{
/*!
** \brief Thermal
*/
class EconomicInputData final
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit EconomicInputData();
    //@}

    /*!
    ** \brief Reset all values to their default ones
    */
    void reset();

    //! Copy data from another struct
    void copyFrom(const EconomicInputData& rhs);

    /*!
    ** \brief Load settings for the thermal prepro from a folder
    **
    ** \param folder The source folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& study, const std::filesystem::path& folder);

    /*!
    ** \brief Save settings used by the thermal prepro to a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AnyString& folder) const;

    //! All {FO,PO}{Duration,Rate} annual values
    // max x DAYS_PER_YEAR
    TimeSeries::TS fuelcost;
    TimeSeries::TS co2cost;

}; // class EconomicInputData

} // namespace Antares::Data
#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__
