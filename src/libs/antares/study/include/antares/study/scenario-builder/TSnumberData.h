// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__

#include "scBuilderDataInterface.h"

namespace Antares::Data::ScenarioBuilder
{
/*!
** \brief Rules for TS numbers, for all years and a single timeseries
*/
class TSNumberData: public dataInterface
{
public:
    //! Matrix
    using MatrixType = Matrix<uint32_t>;

public:
    // We use default constructor and destructor

    //! \name Data manupulation
    //@{
    /*!
    ** \brief Reset data from the study
    */
    bool reset(const Study& study) override;

    /*!
    ** \brief Assign a single value
    **
    ** \param index An area index or a thermal cluster index
    ** \param year  A year
    ** \param value The new TS number
    */
    void setTSnumber(unsigned int index, unsigned int year, unsigned int value);
    //@}

    unsigned int width() const override;
    unsigned int height() const override;

    double get_value(unsigned int x, unsigned int y) const;
    void set_value(unsigned int x, unsigned int y, unsigned int value) const;

protected:
    virtual std::string get_prefix() const = 0;

    virtual unsigned int get_tsGenCount(const Study& study) const = 0;

protected:
    //! All TS number overlay (0 if auto)
    MatrixType pTSNumberRules;

}; // class TSNumberData

// class TSNumberData : inline functions

inline unsigned int TSNumberData::width() const
{
    return pTSNumberRules.width;
}

inline unsigned int TSNumberData::height() const
{
    return pTSNumberRules.height;
}

inline double TSNumberData::get_value(unsigned int x, unsigned int y) const
{
    return pTSNumberRules.entry[y][x];
}

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_TS_NUMBER_H__
