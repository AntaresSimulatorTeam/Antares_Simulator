// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_CREATE_H__
#define __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_CREATE_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::ThermalCluster
{
class Create: public IAction
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
    Create(const AnyString& areaname, const AnyString& clustername);
    //! Destructor
    virtual ~Create();
    //@}

    virtual bool allowUpdate() const;

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! The original area name
    YString pOriginalAreaName;
    //! The original cluster name
    YString pOriginalPlantName;
    //!
    YString pTargetPlantName;
    //! The futur cluster name
    YString pFuturPlantName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::ThermalCluster

#include "create.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_THERMAL_PLANT_CREATE_H__
