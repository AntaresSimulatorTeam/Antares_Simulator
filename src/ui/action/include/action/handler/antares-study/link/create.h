// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Link
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
    Create(const Data::AreaName& fromarea, const Data::AreaName& toarea);

    Create(const Data::AreaName& fromarea,
           const Data::AreaName& targetfromarea,
           const Data::AreaName& toarea,
           const Data::AreaName& targettoarea);

    //! Destructor
    virtual ~Create();
    //@}

    virtual bool allowUpdate() const;

    /*!
    ** \brief Create all standard actions for copy/pasting an area
    */
    void createActionsForAStandardLinkCopy(Context& ctx);

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! From
    Data::AreaName pOriginalFromAreaName;
    Data::AreaName pTargetFromAreaName;
    //! To
    Data::AreaName pOriginalToAreaName;
    Data::AreaName pTargetToAreaName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Link

#include "create.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_LINK_CREATE_H__
