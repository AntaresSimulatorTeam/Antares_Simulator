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
#ifndef __ANTARES_TOOLBOX_FILTER_COMPONENT_H__
#define __ANTARES_TOOLBOX_FILTER_COMPONENT_H__

#include <wx/panel.h>
#include <vector>
#include <list>
#include "input.h"
#include "../gridhelper.h"
#include "filter.h"
#include <antares/date/date.h>
#include "component/panel.h"

namespace Antares
{
namespace Toolbox
{
namespace Filter
{
class Component : public Antares::Component::Panel
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    ** \param parent The parent window
    */
    Component(wxWindow* parent, Date::Precision precision = Date::stepNone);
    //! Destructor
    virtual ~Component();
    //@}

    Date::Precision precision() const
    {
        return pPrecision;
    }
    void precision(const Date::Precision p)
    {
        pPrecision = p;
    }

    /*!
    ** \brief Create and add an input
    */
    Input* add();

    /*!
    ** \brief Remove an input
    */
    bool remove(Input* in);
    void remove(int id);

    /*!
    ** \brief Remove all inputs
    */
    void clear();

    void updateSearchResults();

    void refresh();

    Yuni::Event<void()> onUpdateSearchResults;

    //! \name Grid
    //@{
    wxGrid* grid() const
    {
        return pGrid;
    }
    void grid(wxGrid* g)
    {
        pGrid = g;
    }

    Antares::Component::Datagrid::VGridHelper* gridHelper() const
    {
        return pGridHelper;
    }
    void gridHelper(Antares::Component::Datagrid::VGridHelper* g)
    {
        pGridHelper = g;
    }
    //@}

    /*!
    ** \brief Set the precision required by the datagrid
    */
    void dataGridPrecision(Date::Precision p)
    {
        pDataGridPrecision = p;
    }

private:
    //! Input List
    using InputList = std::list<Input*>;
    using InputVector = std::vector<Input*>;

    void evtRefreshGrid();

    void classifyFilters(InputVector& onRows, InputVector& onCols, InputVector& onCells);

private:
    //! All inputs
    InputList pInputs;
    //! Attached grid
    wxGrid* pGrid;
    //! Grid Helper
    Antares::Component::Datagrid::VGridHelper* pGridHelper;

    //! Concurrent refresh count
    int pRefreshBatchCount;

    //! Precision
    Date::Precision pPrecision;
    //!
    Date::Precision pDataGridPrecision;

}; // class Component

} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_COMPONENT_H__
