// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "renewable-cluster-item.h"

using namespace Yuni;

namespace Antares::Component::HTMLListbox::Item
{
RenewableClusterItem::RenewableClusterItem(Antares::Data::RenewableCluster* a):
    ClusterItem(a),
    pRenewableCluster(a)
{
    preloadImages();
}

RenewableClusterItem::RenewableClusterItem(Antares::Data::RenewableCluster* a,
                                           const wxString& additional):
    ClusterItem(a, additional),
    pRenewableCluster(a)
{
    preloadImages();
}

RenewableClusterItem::~RenewableClusterItem()
{
}

wxString RenewableClusterItem::htmlContentTail()
{
    // Return nothing
    return wxString();
}

Antares::Data::RenewableCluster* RenewableClusterItem::renewableAggregate() const
{
    return pRenewableCluster;
}

void RenewableClusterItem::addAdditionalIcons(wxString& /* out */) const
{
    // No additional icons should be added for renewable clusters
}

} // namespace Antares::Component::HTMLListbox::Item
