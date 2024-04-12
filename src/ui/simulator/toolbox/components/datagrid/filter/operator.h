/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATION_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATION_H__

#include <wx/sizer.h>
#include "filter.h"
#include "parameter/parameter.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
// Forward declaration
class AFilterBase;

namespace Operator
{
class AOperator
{
public:
    //! namespace Constructor & Destructor
    //@{
    //! Constructor
    AOperator(AFilterBase* parent, const wxChar* name, const wxChar* caption);
    //! Destructor
    virtual ~AOperator();
    //@}

    //! \name Name
    //@{
    //! Get the name of the operator
    const wxString& name() const
    {
        return pName;
    }
    //@}

    //! \name Caption
    //@{
    //! Get the caption of the filter
    const wxString& caption() const
    {
        return pCaption;
    }
    //! Set the caption of the filter
    void caption(const wxString& v);
    //@}

    virtual bool compute(const int a) const = 0;
    virtual bool compute(const double a) const = 0;
    virtual bool compute(const wxString& a) const = 0;

    //! Operator ()
    template<typename U>
    bool operator()(const U& a, const U& b) const
    {
        return compute(a, b);
    }

    /*!
    ** \brief Get the sizer with all controls to interact with parameters
    */
    wxSizer* sizer(wxWindow* parent);

    void refreshAttachedGrid();

public:
    //! Additional parameters
    Parameter::List parameters;

private:
    AFilterBase* pParentFilter;
    //! Name of the operator
    const wxString pName;
    //! Caption of the operator
    wxString pCaption;
    //! Sizer
    wxSizer* pSizer;

}; // class AOperator

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_OPERATION_H__
