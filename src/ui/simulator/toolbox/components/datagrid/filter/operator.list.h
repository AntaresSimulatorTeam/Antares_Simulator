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
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__
#define __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__

#include <wx/string.h>
#include <yuni/yuni.h>
#include <vector>

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
// Forward declaration
class AOperator;

/*!
** \brief Operator list
*/
class List
{
public:
    //! \name Constructor && Destructor
    //@{
    //! Constructor
    List(AFilterBase* parent);
    //! Destructor
    ~List();
    //@}

    /*!
    ** \brief Clear the container
    */
    void clear();

    /*!
    ** \brief Add an operator from its given name
    **
    ** \param name Name of the operator
    ** \return True if an operator has been added, false if not found
    */
    bool add(const wxString& name);

    void addStdArithmetic(bool withModulo = true);
    void addStdWeekday(bool withModulo = false);
    void addStdMonth(bool withModulo = false);

    uint size() const
    {
        return (uint)pItems.size();
    }
    uint count() const
    {
        return (uint)pItems.size();
    }

    //! Get if the list is empty
    bool empty() const
    {
        return pItems.empty();
    }

    //! Get the parent filter
    AFilterBase* parentFilter() const
    {
        return pParentFilter;
    }

    /*!
    ** \brief Get the operator at a given index
    */
    AOperator* at(const int indx) const throw()
    {
        return pItems[indx];
    }

    //! Operator []
    AOperator* operator[](const int indx) const throw()
    {
        return pItems[indx];
    }

private:
    bool internalAdd(const wxString& name);

private:
    //! Parent filter
    AFilterBase* pParentFilter;
    //! Operator list (typedef)
    using OperatorList = std::vector<AOperator*>;
    //! Operator list
    OperatorList pItems;

}; // class List

} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#include "operator.h"

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__
