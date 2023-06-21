//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>
#include "../../utils.h"

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

//Following code is there to handle both BindingConstraintRTI and BindingConstraintList while we remove one or the other
template<class T>
std::string GroupPredicate(const T& bc) {
    return bc.group;
}

template<class T>
std::string GroupPredicate(const std::shared_ptr<T>& bc) {
    return bc->group();
}

template<class T>
unsigned TimeSeriesWidthPredicate(const T& bc) {
    return bc.RHSTimeSeries().width;
}

template<class T>
unsigned TimeSeriesWidthPredicate(const std::shared_ptr<T>& bc) {
    return bc->RHSTimeSeries().width;
}

template<class ListBindingConstraints>
unsigned int BindingConstraintsList::NumberOfTimeseries(const ListBindingConstraints &list, const std::string& group_name) {
    //Assume all BC in a group have the same width
    const auto& binding_constraint = std::find_if(list.begin(), list.end(), [&group_name](auto& bc) {
        return GroupPredicate(bc) == group_name;
    });
    if (binding_constraint == list.end())
        return 0;
    return TimeSeriesWidthPredicate(*binding_constraint);
}
}