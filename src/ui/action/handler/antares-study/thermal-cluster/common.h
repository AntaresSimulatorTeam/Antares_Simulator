// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_COMMON_H__
#define __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_COMMON_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::ThermalCluster
{
class CommonData: public IAction
{
public:
    //! The most suitable smart ptr for the class
    using Ptr = IAction::Ptr;
    //! The threading policy
    using ThreadingPolicy = IAction::ThreadingPolicy;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    CommonData();
    //! Destructor
    virtual ~CommonData();
    //@}

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

}; // class IAction

} // namespace Antares::Action::AntaresStudy::ThermalCluster

#endif // __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_COMMON_H__
