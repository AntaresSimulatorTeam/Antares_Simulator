//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include "../../utils.h"
#include "BindingConstraintsList.h"

namespace Antares::Data {

inline uint BindingConstraintsList::size() const {
    return (uint) pList.size();
}

inline bool BindingConstraintsList::empty() const {
    return pList.empty();
}

template<class PredicateT>
inline void BindingConstraintsList::each(const PredicateT &predicate) {
    uint count = (uint) pList.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(pList[i]));
}

template<class PredicateT>
inline void BindingConstraintsList::each(const PredicateT &predicate) const {
    uint count = (uint) pList.size();
    for (uint i = 0; i != count; ++i)
        predicate(*(pList[i].get()));
}

template<class PredicateT>
inline void BindingConstraintsList::eachEnabled(const PredicateT &predicate) const {
    uint count = (uint) pList.size();
    for (uint i = 0; i != count; ++i) {
        auto &constraint = *(pList[i].get());
        if (constraint.enabled() && !constraint.skipped())
            predicate(constraint);
    }
}
}