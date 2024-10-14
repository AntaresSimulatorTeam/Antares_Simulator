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
#ifndef __ANTARES_LIBS_STUDY_PARTS_SOLAR_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_SOLAR_CONTAINER_H__

#include <antares/series/series.h>

namespace Antares
{
namespace Data
{
namespace Solar
{
class Prepro;

class Container
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Container();
    //! Destructor
    ~Container() = default;
    //@}

    /*!
    ** \brief Reset to default values
    */
    void resetToDefault();

    /*!
    ** \brief Make sure that all data are loaded
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark all data as modified
    */
    void markAsModified() const;

    //! Data for the pre-processor
    std::unique_ptr<Data::Solar::Prepro> prepro;

    TimeSeriesNumbers tsNumbers;

    /*! Data for time-series */
    TimeSeries series;

}; // class Container

} // namespace Solar
} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_PARTS_SOLAR_CONTAINER_H__
