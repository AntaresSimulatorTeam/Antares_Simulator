/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_MEMORY_MEMORY_HXX__
#define __ANTARES_LIBS_MEMORY_MEMORY_HXX__

namespace Antares
{
inline uint64_t Memory::processID() const
{
    return pProcessID;
}

template<class T>
Memory::Array<T>::Array(const Yuni::NullPtr&)
{
}

template<class T>
Memory::Array<T>::Array(const Memory::Array<T>&)
{
}

template<class T>
template<class U>
Memory::Array<T>::Array(const Memory::Array<U>&)
{
}

template<class T>
inline Memory::Array<T>::Array(size_t size)
{
    allocate(size);
}

template<class T>
inline Memory::Array<T>::~Array()
{
    delete[] pPointer;
    pPointer = nullptr;
}

template<class T>
void Memory::Array<T>::allocate(size_t size)
{
    delete[] pPointer;
    pPointer = new T[size];
}

template<class T>
T& Memory::Array<T>::operator[](uint i)
{
    return (T&)pPointer[i];
}

template<class T>
const T& Memory::Array<T>::operator[](uint i) const
{
    return (const T&)pPointer[i];
}

template<class T>
inline void Memory::Release(T*& pointer)
{
    delete[] pointer;
    pointer = nullptr;
}

template<class T>
inline void Memory::Allocate(T*& out, size_t size)
{
    assert(size > 0);
    out = new T[size];
}

template<class T>
inline bool Memory::Null(const T* out)
{
    return !out;
}

template<class T>
inline bool Memory::StrictNull(const T* out)
{
    return !out;
}

template<class U>
inline U* Memory::RawPointer(U* array)
{
    return array;
}

template<class U>
inline void Memory::Zero(uint count, U* array)
{
    (void)::memset(array, 0, sizeof(U) * count);
}

template<class U>
inline void Memory::Assign(uint count, U* array, const U& value)
{
    for (uint i = 0; i != count; ++i)
        array[i] = value;
}

} // namespace Antares

#endif // __ANTARES_LIBS_MEMORY_MEMORY_HXX__
