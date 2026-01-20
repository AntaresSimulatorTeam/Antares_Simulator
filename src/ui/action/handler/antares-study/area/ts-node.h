// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TS_NODE_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TS_NODE_H__

#include <yuni/yuni.h>
#include <action/action.h>
#include <antares/study/fwd.h>

namespace Antares::Action::AntaresStudy::Area
{
class NodeTimeseries: public IAction
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
    NodeTimeseries(Data::TimeSeriesType ts);
    //! Destructor
    virtual ~NodeTimeseries();
    //@}

    virtual bool allowUpdate() const;

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! Type of the timeseries
    const Data::TimeSeriesType pType;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Area

#include "ts-node.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TS_NODE_H__
