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

#pragma once

#include "datasource.h"
#include <yuni/core/event.h>
#include <antares/study.h>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

struct SortAlphaOrder
{
    inline bool operator()(const Data::Cluster* a, const Data::Cluster* b)
    {
        return a->name() < b->name();
    }
};

struct SortAlphaReverseOrder
{
    inline bool operator()(const Data::Cluster* a, const Data::Cluster* b)
    {
        return a->name() > b->name();
    }
};

wxString groupNameToGroupTitle(Data::Area* area, wxString& groupName);

class ClustersByOrder : public Yuni::IEventObserver<ClustersByOrder>, public IDatasource
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    ClustersByOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~ClustersByOrder();
    //@}

    virtual wxString name() const = 0;
 
    virtual const char* icon() const = 0;

    virtual void refresh(const wxString& search = wxEmptyString) = 0;

    void onAreaChanged(Data::Area* area);
    void onInvalidateAllAreas();

    void hasGroupChanged(bool b);
    bool hasGroupChanged();

protected:
    Data::Area* pArea;
    bool hasGroupJustChanged_;

}; // class ClustersByOrder


} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares