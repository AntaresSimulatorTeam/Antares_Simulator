// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_PLANT_ROOT_NODE_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_PLANT_ROOT_NODE_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::ThermalCluster
{
class RootNode: public IAction
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
    explicit RootNode(const AnyString& areaname);
    //! Destructor
    virtual ~RootNode();
    //@}

    virtual bool allowUpdate() const;

protected:
    virtual void prepareSkipWL(Context& ctx);
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    Data::AreaName pOriginalAreaName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::ThermalCluster

#include "root-node.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_PLANT_ROOT_NODE_H__
