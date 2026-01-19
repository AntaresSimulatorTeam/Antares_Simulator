// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__

#include <yuni/yuni.h>
#include "../action.h"

namespace Antares::Action::Settings
{
/*!
** \brief A dummy action, for displaying some text
*/
class DecalAreaPosition: public IAction
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
    explicit DecalAreaPosition(bool enabled = true);
    //! Destructor
    virtual ~DecalAreaPosition();
    //@}

    virtual bool autoExpand() const;

    virtual bool shouldPrepareRootNode() const;

    virtual void behaviorToText(Behavior behavior, Yuni::String& out);

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);
    virtual void prepareSkipWL(Context& ctx);

private:
    Yuni::String pValueX;
    Yuni::String pValueY;

}; // class DecalAreaPosition

} // namespace Antares::Action::Settings

#include "decal-area-position.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_H__
