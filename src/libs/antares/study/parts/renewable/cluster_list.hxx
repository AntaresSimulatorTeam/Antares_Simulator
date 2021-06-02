namespace Antares
{
namespace Data
{
inline void RenewableClusterList::flush()
{
#ifndef ANTARES_SWAP_SUPPORT
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        RenewableCluster& it = *(i->second);
        it.flush();
    }
#endif
}

inline uint RenewableClusterList::size() const
{
    return (uint)cluster.size();
}

inline bool RenewableClusterList::empty() const
{
    return cluster.empty();
}

inline RenewableClusterList::iterator RenewableClusterList::begin()
{
    return cluster.begin();
}

inline RenewableClusterList::const_iterator RenewableClusterList::begin() const
{
    return cluster.begin();
}

inline RenewableClusterList::iterator RenewableClusterList::end()
{
    return cluster.end();
}

inline RenewableClusterList::const_iterator RenewableClusterList::end() const
{
    return cluster.end();
}

inline const RenewableCluster* RenewableClusterList::find(const Data::ClusterName& id) const
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

inline RenewableCluster* RenewableClusterList::find(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

template<class PredicateT>
void RenewableClusterList::each(const PredicateT& predicate) const
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
    {
        const RenewableCluster& it = *(i->second);
        predicate(it);
    }
}

template<class PredicateT>
void RenewableClusterList::each(const PredicateT& predicate)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        RenewableCluster& it = *(i->second);
        predicate(it);
    }
}

} // namespace Data
} // namespace Antares
