// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FILTER_OPERATION_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATION_H__

#include <wx/sizer.h>
#include "filter.h"
#include "parameter/parameter.h"

namespace Antares::Toolbox::Filter
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
} // namespace Antares::Toolbox::Filter

#endif // __ANTARES_TOOLBOX_FILTER_OPERATION_H__
