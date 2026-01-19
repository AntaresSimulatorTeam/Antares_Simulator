// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_ALLOCATION_HYDRO_POST_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_ALLOCATION_HYDRO_POST_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Area
{
class AllocationHydroPost: public IAction
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
    explicit AllocationHydroPost(IAction* parent, const AnyString& areaname);
    //! Destructor
    virtual ~AllocationHydroPost();
    //@}

    virtual bool visible() const;

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! The attached action
    IAction::Ptr pAction;
    Data::AreaName pOriginalAreaName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Area

#include "allocation-hydro-post.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_ALLOCATION_HYDRO_POST_H__
