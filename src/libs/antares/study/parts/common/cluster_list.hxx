namespace Antares
{
namespace Data
{
inline void ClusterList::flush()
{
#ifndef ANTARES_SWAP_SUPPORT
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        Cluster& it = *(i->second);
        it.flush();
    }
#endif
}

inline uint ClusterList::size() const
{
    return (uint)cluster.size();
}

inline bool ClusterList::empty() const
{
    return cluster.empty();
}

inline ClusterList::iterator ClusterList::begin()
{
    return cluster.begin();
}

inline ClusterList::const_iterator ClusterList::begin() const
{
    return cluster.begin();
}

inline ClusterList::iterator ClusterList::end()
{
    return cluster.end();
}

inline ClusterList::const_iterator ClusterList::end() const
{
    return cluster.end();
}

inline const Cluster* ClusterList::find(const Data::ClusterName& id) const
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

inline Cluster* ClusterList::find(const Data::ClusterName& id)
{
    auto i = cluster.find(id);
    return (i != cluster.end()) ? i->second : nullptr;
}

template<class PredicateT>
void ClusterList::each(const PredicateT& predicate) const
{
    auto end = cluster.cend();
    for (auto i = cluster.cbegin(); i != end; ++i)
    {
        const Cluster& it = *(i->second);
        predicate(it);
    }
}

template<class PredicateT>
void ClusterList::each(const PredicateT& predicate)
{
    auto end = cluster.end();
    for (auto i = cluster.begin(); i != end; ++i)
    {
        Cluster& it = *(i->second);
        predicate(it);
    }
}

} // namespace Data
} // namespace Antares
