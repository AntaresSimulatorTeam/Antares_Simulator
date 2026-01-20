// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TIMESERIES_H__

#include <yuni/yuni.h>
#include <action/action.h>
#include <antares/study/fwd.h>

namespace Antares::Action::AntaresStudy::Area
{
class DataTimeseries: public IAction
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
    DataTimeseries(Data::TimeSeriesType ts, const AnyString& areaname);

    /*!
    ** \brief Default constructor
    */
    DataTimeseries(Data::TimeSeriesType ts,
                   const AnyString& areaname,
                   const AnyString& clustername);

    //! Destructor
    virtual ~DataTimeseries();
    //@}

    virtual void datagridCaption(YString& title);

    virtual void behaviorToText(Behavior behavior, YString& out);

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);
    virtual void registerViewsWL(Context& ctx);

private:
    Data::TimeSeriesType pType;
    Data::AreaName pOriginalAreaName;
    std::string pOriginalPlantName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Area

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_TIMESERIES_H__
