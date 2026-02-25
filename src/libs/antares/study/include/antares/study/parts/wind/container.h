// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_WIND_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_WIND_CONTAINER_H__

#include <antares/series/series.h>

#include "prepro.h"

namespace Antares::Data::Wind
{
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

    //! Data for the pre-processor
    std::unique_ptr<Data::Wind::Prepro> prepro;

    TimeSeriesNumbers tsNumbers;

    /*! Data for time-series */
    TimeSeries series;

}; // class Container

} // namespace Antares::Data::Wind

#endif // __ANTARES_LIBS_STUDY_PARTS_WIND_CONTAINER_H__
