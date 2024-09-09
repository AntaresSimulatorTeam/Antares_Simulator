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
#ifndef __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
#define __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__

#include <filesystem>
#include <map>
#include <unordered_map>

#include "../../fwd.h"

namespace Antares
{
namespace Data
{
class HydroAllocation final
{
public:
    //! Coefficients
    using Coefficients = std::map<AreaName, double>;

public:
    //! \id Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    HydroAllocation();
    //! Destructor
    ~HydroAllocation();
    //@}

    /*!
    ** \brief Clear all coefficients
    */
    void clear();

    /*!
    ** \brief Remove coefficients related to a given area index
    */
    void remove(const AreaName& areaid);

    void rename(const AreaName& oldid, const AreaName& newid);

    /*!
    ** \brief Get the coefficient for a specific area
    */
    double fromArea(const Area& area) const;
    /*!
    ** \brief Get the coefficient for a specific area
    */
    double fromArea(const Area* area) const;
    /*!
    ** \brief Get the coefficient for a specific area index
    */
    double fromArea(const AreaName& areaid) const;

    /*!
    ** \brief Set the coefficient for a specific area (reference)
    */
    void fromArea(const Area& area, double value);
    /*!
    ** \brief Set the coefficient for a specific area (weak pointer)
    */
    void fromArea(const Area* area, double value);
    /*!
    ** \brief Set the coefficient for a specific area index
    */
    void fromArea(const AreaName& areaid, double value);

    //! \name Load / Save
    //@{
    /*!
    ** \brief Load allocation coefficients from a file
    */
    bool loadFromFile(const AreaName& referencearea, const std::filesystem::path& filename);

    /*!
    ** \brief Load allocation coefficients from a file
    */
    bool saveToFile(const AnyString& filename) const;

    /*!
    ** \brief Copy correlation coefficients from another correlation grid
    **
    ** \param source The correlation matrix source
    ** \param studySource The associated study for the given correlation grid
    ** \param areaSource Name of the area to import
    ** \param mapping Area's name Mapping from the source to the target
    ** \param study Study for the local correlation grid
    */
    void copyFrom(const HydroAllocation& source,
                  const Study& studySource,
                  const AreaNameMapping& mapping,
                  const Study& study);
    //@}

    //! \name Solver
    //@{
    /*!
    ** \brief Iterate through all non-null coeff
    **
    ** \code
    ** area.hydroAllocation.eachNonNull([&] (uint areaindex, double value)
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

    //! \name Operators
    //@{
    //! \see fromArea()
    double operator[](const AreaName& areaid) const;
    //! \see fromArea()
    double operator[](const Area& area) const;

    //! Get all coefficients
    const Coefficients& coefficients() const;
    //@}

private:
    //! Coefficients for other areas
    Coefficients pValues;
    //! Coefficients for other areas, ordered by the area id
    std::unordered_map<uint, double> pValuesFromAreaID;
#ifndef NDEBUG
    //! Flag to prevent misuse of the bad container
    // TODO Find a more reliable way...
    bool pMustUseValuesFromAreaID;
#endif

}; // class HydroAllocation

} // namespace Data
} // namespace Antares

#include "allocation.hxx"

#endif // __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
