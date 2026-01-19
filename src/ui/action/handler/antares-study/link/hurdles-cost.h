// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREAHURDLES_COST_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREAHURDLES_COST_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Link
{
class HurdlesCost: public IAction
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
    HurdlesCost(const AnyString& fromarea, const AnyString& toarea);

    //! Destructor
    virtual ~HurdlesCost();
    //@}

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! From
    Data::AreaName pOriginalFromAreaName;
    //! To
    Data::AreaName pOriginalToAreaName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Link

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREAHURDLES_COST_H__
