// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_UNLOAD_DATA_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_UNLOAD_DATA_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Link
{
class UnloadData: public IAction
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
    UnloadData(const AnyString& fromarea, const AnyString& toarea);

    //! Destructor
    virtual ~UnloadData();
    //@}

    virtual bool visible() const;

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

#include "unload-data.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_AREA_UNLOAD_DATA_H__
