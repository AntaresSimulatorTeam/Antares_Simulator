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
#ifndef __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__
#define __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__

#include "prepro.h"
#include "series.h"
#include <yuni/core/noncopyable.h>

namespace Antares
{
namespace Data
{
namespace Load
{
class Container final : private Yuni::NonCopyable<Container>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Container();
    //! Destructor
    ~Container();
    //@}

    /*!
    ** \brief Reset to default values
    */
    void resetToDefault();

    /*!
    ** \brief Make sure that all data are loaded
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark the load data as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Get the amount of memory currently used by the class
    */
    Yuni::uint64 memoryUsage() const;

public:
    //! Data for the pre-processor
    Data::Load::Prepro* prepro;
    /*! Data for time-series */
    DataSeriesLoad* series;

}; // class Container

} // namespace Load
} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_PARTS_LOAD_CONTAINER_H__
