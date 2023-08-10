//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>
#include "../../utils.h"

namespace Antares::Data {

inline uint BindingConstraintsRepository::size() const {
    return (uint) constraints_.size();
}

inline bool BindingConstraintsRepository::empty() const {
    return constraints_.empty();
}

template<class PredicateT>
inline void BindingConstraintsRepository::each(const PredicateT &predicate) {
    uint count = (uint) constraints_.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(constraints_[i]));
}

template<class PredicateT>
inline void BindingConstraintsRepository::each(const PredicateT &predicate) const {
    uint count = (uint) constraints_.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(constraints_[i].get()));
}

template<class PredicateT>
inline void BindingConstraintsRepository::eachActive(const PredicateT &predicate) const {
    uint count = (uint) constraints_.size();
    for (uint i = 0; i != count; ++i) {
        auto &constraint = *(constraints_[i].get());
        if (constraint.isActive())
            predicate(constraint);
    }
}

}