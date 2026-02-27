// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data::ScenarioBuilder
{
/*!
 ** \brief Interface for scenario builder data (time series, hydro levels, ...)
 */
class dataInterface
{
public:
    virtual ~dataInterface() = default;

    //! \name Data manupulation
    //@{
    /*!
     ** \brief Reset data from the study
     */
    virtual bool reset(const Study& study) = 0;

    [[nodiscard]] virtual uint width() const = 0;

    [[nodiscard]] virtual uint height() const = 0;

    /*!
     ** \brief Apply the changes to the study corresponding data (time series, hydro levels, ...)
     **
     ** This method is only useful when launched from the solver.
     */
    virtual bool apply(Study& study) = 0;

}; // class dataInterface

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__
