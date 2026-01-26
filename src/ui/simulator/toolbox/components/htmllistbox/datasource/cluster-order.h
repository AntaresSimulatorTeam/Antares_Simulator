// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include "datasource.h"
#include <yuni/core/event.h>
#include <antares/study/study.h>
#include "../item/cluster-item.h"

using namespace Antares::Component::HTMLListbox::Item;

namespace Antares::Component::HTMLListbox::Datasource
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

class ClustersByOrder: public Yuni::IEventObserver<ClustersByOrder>, public IDatasource
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

    virtual void refresh(const wxString& search = wxEmptyString);

    void onAreaChanged(Data::Area* area);
    void onInvalidateAllAreas();

    void hasGroupChanged(bool b);
    bool hasGroupChanged() const;

private:
    virtual void reorderItemsList(const wxString& search) = 0;
    virtual void rebuildItemsList(const wxString& search) = 0;

protected:
    Data::Area* pArea;
    bool hasGroupJustChanged_;
    std::map<wxString, IItem::Ptr> groups_to_items_;
    std::map<Data::Cluster*, ClusterItem::Ptr> pClustersToItems;

}; // class ClustersByOrder

} // namespace Antares::Component::HTMLListbox::Datasource
