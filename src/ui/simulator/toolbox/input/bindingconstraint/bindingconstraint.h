// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__
#define __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__

#include <yuni/core/event.h>
#include <antares/study/constraint.h>
#include "../input.h"

namespace Antares::Toolbox::InputSelector
{
class BindingConstraint: public AInput, public Yuni::IEventObserver<BindingConstraint>
{
public:
    BindingConstraint(wxWindow* parent);
    virtual ~BindingConstraint();

    virtual void update();
    virtual void updateRowContent();
    virtual void updateInnerData(const wxString& searchString);

    virtual wxPoint recommendedSize() const
    {
        return wxPoint(150, 200);
    }

public:
    //! A binding constraint has been selected
    Yuni::Event<void(Data::BindingConstraint*)> onBindingConstraintChanged;
    //! A binding constraint has been double clicked
    Yuni::Event<void(Data::BindingConstraint*)> onBindingConstraintDblClick;

protected:
    virtual void internalBuildSubControls();

private:
    //! Event: The application is about to leave
    void onApplicationOnQuit();

    void onUpdate();
    void onStudyAreaUpdate(Data::Area*);
    void onStudyLinkUpdate(Data::AreaLink*);

    /*!
    ** \brief Clear the control and broadcast a change in the current selected area
    */
    void clear();

}; // class Area

} // namespace Antares::Toolbox::InputSelector

#endif // __ANTARES_TOOLBOX_INPUT_BINDING_CONSTRAINT_BINDING_CONSTRAINT_H__
