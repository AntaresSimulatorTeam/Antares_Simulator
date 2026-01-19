// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_INPUT_H__
#define __ANTARES_TOOLBOX_INPUT_H__

#include <ui/common/component/panel.h>

namespace Antares::Toolbox::InputSelector
{
class AInput: public Antares::Component::Panel
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    ** \param parent the parent window
    */
    AInput(wxWindow* parent);
    //! Destructor
    virtual ~AInput();
    //@}

    /*!
    ** \brief Clear then Update the input
    */
    virtual void update() = 0;

    /*!
    ** \brief Only update the content and not the list itself
    */
    virtual void updateRowContent();

    /*!
    ** \brief Update each item available in the input
    **
    ** \param searchString The string to highlight
    */
    virtual void updateInnerData(const wxString& searchString);

    /*!
    ** \brief Get the recommended size for the control
    */
    virtual wxPoint recommendedSize() const
    {
        return wxPoint(50, 50);
    }

protected:
    virtual void internalBuildSubControls() = 0;

}; // class IInput

} // namespace Antares::Toolbox::InputSelector

#endif // __ANTARES_TOOLBOX_INPUT_H__
