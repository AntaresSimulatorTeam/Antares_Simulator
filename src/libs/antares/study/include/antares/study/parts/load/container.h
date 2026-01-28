// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__

#include <yuni/core/noncopyable.h>

#include <antares/series/series.h>

namespace Antares::Data::Load
{

class Prepro;

class Container final: private Yuni::NonCopyable<Container>
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
    ** \brief Mark the load data as modified
    */
    void markAsModified() const;

    //! Data for the pre-processor
    std::unique_ptr<Data::Load::Prepro> prepro;

    TimeSeriesNumbers tsNumbers;

    /*! Data for time-series */
    TimeSeries series;

}; // class Container

} // namespace Antares::Data::Load

#endif // __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__
