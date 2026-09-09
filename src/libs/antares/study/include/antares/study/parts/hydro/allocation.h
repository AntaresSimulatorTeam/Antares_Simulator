// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
#define __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>

#include "../../fwd.h"

namespace Antares::Data
{
class HydroAllocation final
{
public:
    //! Coefficients
    using Coefficients = std::map<AreaName, double>;

public:
    /*!
    ** \brief Clear all coefficients
    */
    void clear();

    /*!
    ** \brief Set the coefficient for a specific area index
    */
    void fromArea(const AreaName& areaid, double value);

    /*!
    ** \brief Load allocation coefficients from a file
    */
    bool loadFromFile(const AreaName& referencearea, const std::filesystem::path& filename);

    //! \name Solver
    //@{
    /*!
    ** \brief Iterate through all non-null coeff
    **
    ** \code
    ** area.hydroAllocation.eachNonNull([&] (unsigned int areaindex, double value)
    ** {
    ** 	std::cout << "area: " << areaindex << " = " << value << std::endl;
    ** });
    ** \endcode
    **
    ** \warning The internal map pValuesFromAreaID must be initialized
    **   prepareForSolver must be called.
    */
    template<class CallbackT>
    void eachNonNull(const CallbackT& callback) const;

    /*!
    ** \brief Prepare coefficients ordered by the area ids
    */
    void prepareForSolver(const AreaList& list);
    //@}

    //! Get all coefficients
    const Coefficients& coefficients() const;

private:
    //! Coefficients for other areas
    Coefficients pValues;
    //! Coefficients for other areas, ordered by the area id
    std::unordered_map<unsigned int, double> pValuesFromAreaID;
}; // class HydroAllocation

} // namespace Antares::Data

#include "allocation.hxx"

#endif // __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
