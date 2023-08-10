/*
** Copyright 2007-2023 RTE
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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__

#include "scBuilderDataInterface.h"

namespace Antares::Data::ScenarioBuilder
{
/*!
** \brief Rules for TS numbers, for all years and a single timeseries
*/
class TSNumberData : public dataInterface
{
public:
    //! Matrix
    using MatrixType = Matrix<Yuni::uint32>;

public:
    // We use default constructor and destructor

    //! \name Data manupulation
    //@{
    /*!
    ** \brief Reset data from the study
    */
    bool reset(const Study& study) override;

    /*!
    ** \brief Export the data into a mere INI file
    */
    void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const override;

    /*!
    ** \brief Assign a single value
    **
    ** \param index An area index or a thermal cluster index
    ** \param year  A year
    ** \param value The new TS number
    */
    void set(uint index, uint year, uint value);
    //@}

    uint width() const;
    uint height() const;

    double get_value(uint x, uint y) const;
    void set_value(uint x, uint y, uint value);

    /*
    ** Give the study an access to TS numbers scenarii
    */
    virtual bool apply(Study& study) = 0;

protected:
    virtual CString<512, false> get_prefix() const = 0;

    virtual uint get_tsGenCount(const Study& study) const = 0;

protected:
    //! All TS number overlay (0 if auto)
    MatrixType pTSNumberRules;

}; // class TSNumberData

// class TSNumberData : inline functions

inline uint TSNumberData::width() const
{
    return pTSNumberRules.width;
}

inline uint TSNumberData::height() const
{
    return pTSNumberRules.height;
}

inline double TSNumberData::get_value(uint x, uint y) const
{
    return pTSNumberRules.entry[y][x];
}

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__
