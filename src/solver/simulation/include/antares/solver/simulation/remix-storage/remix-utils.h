// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <span>
#include <vector>

namespace Antares::Solver::Simulation
{
inline double min_on_subrange(std::vector<double>&& v, unsigned h, unsigned H)
{
    if (!v.size())
    {
        throw std::invalid_argument("call min_on_subrange on an empty vector");
    }

    if (H >= v.size() || h >= v.size())
    {
        throw std::invalid_argument("call of min_on_subrange : hour out of bound");
    }

    if (h >= H)
    {
        throw std::invalid_argument("call min_on_subrange with inconsistant hours");
    }

    std::span<double> subset(v.begin() + h, v.begin() + H);
    return *std::ranges::min_element(subset);
}

template<typename T>
class CyclicIterator final
{
public:
    CyclicIterator(std::vector<T>& v);
    CyclicIterator& operator++(int);
    T& operator*();
    bool operator==(const std::vector<T>::iterator& other) const;

private:
    template<typename U>
    friend CyclicIterator<U>& delete_current(CyclicIterator<U>& it);

    void back_to_begin();

    std::vector<T>& v_;
    std::vector<T>::iterator it_;
};

template<typename T>
CyclicIterator<T>::CyclicIterator(std::vector<T>& v):
    v_(v),
    it_(v.begin())
{
}

template<typename T>
CyclicIterator<T>& CyclicIterator<T>::operator++(int)
{
    it_++;
    back_to_begin();
    return *this;
}

template<typename T>
T& CyclicIterator<T>::operator*()
{
    return *it_;
}

template<typename T>
bool CyclicIterator<T>::operator==(const typename std::vector<T>::iterator& other) const
{
    return it_ == other;
}

template<typename T>
void CyclicIterator<T>::back_to_begin()
{
    if (it_ == v_.end())
    {
        it_ = v_.begin();
    }
}

template<typename T>
CyclicIterator<T>& delete_current(CyclicIterator<T>& cyclicIt)
{
    cyclicIt.it_ = cyclicIt.v_.erase(cyclicIt.it_);
    cyclicIt.back_to_begin();
    return cyclicIt;
}

}; // namespace Antares::Solver::Simulation
