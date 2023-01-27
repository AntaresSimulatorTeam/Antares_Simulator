/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_TOOLBOX_FILTER_FILTER_H__
#define __ANTARES_TOOLBOX_FILTER_FILTER_H__

#include <antares/wx-wrapper.h>
#include <wx/panel.h>
#include "operator.h"
#include "operator.list.h"
#include <antares/date.h>

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
// Forward declaration
class Input;

/*!
** \brief Abstract Filter
**
** A filter is a single test/condition to know if a value should be
** displayed and/or managed according the user-settings (for example
** `hour in the year > 42` is a filter).
** A filter is mainly used by datagrid.
**
** A filter is composed into three parts : Its name/caption, its operator
** (equals, less than...) and the parameters (a single one most of the time).
** Graphically, it will be the same. All components created by the filter
** only handle data specific to the filter itself.
*/
class AFilterBase : public wxEvtHandler
{
public:
    /*!
    ** \brief Precision of the filter
    **
    ** All filters with a precision below than required will not
    ** be displayed.
    */
    static Date::Precision Precision()
    {
        return Date::stepNone;
    }

public:
    //! \name Name
    //@{
    //! Get the name of the filter
    static const wxChar* Name()
    {
        return wxT("(null)");
    }
    //@}

    //! \name Caption
    //@{
    //! Get the caption of the filter
    static const wxChar* Caption()
    {
        return wxT("(null)");
    }
    //@}

    /*!
    ** \brief Get the caption of a filter from its name
    */
    static const wxChar* CaptionFromName(const wxString& name, Date::Precision precision);

    /*!
    ** \brief Create a filter from its name
    */
    static AFilterBase* FactoryCreate(Input* parent, const wxString& name);

public:
    //! \name Constructor && Destructor
    //@{
    //! Default constructor
    AFilterBase(Input* parent);
    //! Destructor
    virtual ~AFilterBase();
    //@}

    //! \name Precision of the filter
    //@{
    virtual Date::Precision precision() const;
    //@}

    //! \name What kind of item should be tested
    //@{
    virtual bool checkOnRowsLabels() const
    {
        return false;
    }
    virtual bool checkOnColsLabels() const
    {
        return false;
    }
    virtual bool checkOnCells() const
    {
        return false;
    }
    //@}

    //! Get if the filter is about the raw values
    virtual bool checkOnNumericValues() const
    {
        return true;
    }

    //! Get the name of the filter
    virtual const wxChar* name() const = 0;

    //! Get the caption of the filter
    virtual const wxChar* caption() const = 0;

    //! \name Filter
    //@{
    virtual bool rowIsValid(int row) const;
    virtual bool colIsValid(int col) const;
    virtual bool cellIsValid(const double v) const;
    virtual bool cellIsValid(const wxString& v) const;
    //@}

    //! \name GUI
    //@{
    /*!
    ** \brief Create all GUI components for the filter
    **
    ** Actually all components are not created for the filter, but only
    **
    */
    void recreateGUI(wxWindow* parent);

    /*!
    ** \brief Update the GUI according the current selected operator
    */
    void refreshGUIOperator();

    /*!
    ** \brief Update the GUI according the given operator
    **
    ** The operator is not forced to be the selected one.
    */
    void refreshGUIOperator(Operator::AOperator* op);

    /*!
    ** \brief Delete all components associated to the GUI
    */
    void deleteGUI();

    //! Get the sizer
    wxSizer* sizer() const
    {
        return pMainSizer;
    }

    void refreshAttachedGrid();
    //@}

    //! Get the parent input
    Input* parentInput() const
    {
        return pParentInput;
    }

    /*!
    ** \brief Precision required by the datagrid
    */
    void dataGridPrecision(Date::Precision p);

public:
    //! List of all possible operations for the filter
    Operator::List operators;
    //! The current selected operator for the filter
    Operator::AOperator* currentOperator;

protected:
    //! Precision
    Date::Precision pDataGridPrecision;

private:
    //! The operator has been changed
    void onOperatorChanged(wxCommandEvent& evt);

private:
    //! Parent input
    Input* pParentInput;
    //! The main sizer (controls for the operator + parameters)
    wxSizer* pMainSizer;
    //! The sizer for parameters (all controls after the operator)
    wxSizer* pSizerParameters;
    //! The last parent used for creating controls
    wxWindow* pLastParent;

}; // class AFilterBase

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_FILTER_H__
