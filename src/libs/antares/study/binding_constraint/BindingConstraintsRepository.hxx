//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>

namespace Antares::Data {

inline uint BindingConstraintsRepository::size() const {
    return (uint) constraints_.size();
}

inline bool BindingConstraintsRepository::empty() const {
    return constraints_.empty();
}

template<class PredicateT>
inline void BindingConstraintsRepository::each(const PredicateT &predicate) {
    for (auto bc : constraints_)
        predicate(*bc);
}

template<class PredicateT>
inline void BindingConstraintsRepository::each(const PredicateT &predicate) const {
    for (auto bc : constraints_)
        predicate(*bc);
}
}
