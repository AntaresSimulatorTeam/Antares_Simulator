/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_MEMORY_MEMORY_HXX__
#define __ANTARES_LIBS_MEMORY_MEMORY_HXX__

#define ANTARES_SWAP_ACQUIRE_PTR

namespace Antares
{
inline Yuni::uint64 Memory::processID() const
{
    return pProcessID;
}

template<class T>
inline Memory::Handle Memory::allocate(size_t count)
{
    return internalAllocate(count * sizeof(T));
}

template<class T>
inline Memory::Array<T>::Array() : pPointer(nullptr)
{
}

template<class T>
Memory::Array<T>::Array(const Yuni::NullPtr&) : pPointer(nullptr)
{
}

template<class T>
Memory::Array<T>::Array(const Memory::Array<T>&) : pPointer(nullptr)
{
}

template<class T>
template<class U>
Memory::Array<T>::Array(const Memory::Array<U>&) : pPointer(nullptr)
{
}

template<class T>
inline Memory::Array<T>::Array(size_t size) : pPointer(nullptr)
{
    allocate(size);
}

template<class T>
inline Memory::Array<T>::~Array()
{
    release();
}

template<class T>
void Memory::Array<T>::allocate(size_t size)
{
    delete[] pPointer;
    pPointer = new T[size];
}

template<class T>
inline bool Memory::Array<T>::needFlush() const
{
    return false;
}

template<class T>
inline void Memory::Array<T>::flush() const
{
    // gp : to be removed
}

template<class T>
void Memory::Array<T>::release()
{
    delete[] pPointer;
    pPointer = nullptr;
}

template<class T>
void Memory::Array<T>::acquire()
{
    ANTARES_SWAP_ACQUIRE_PTR;
}

template<class T>
void Memory::Array<T>::assign(uint count)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    memset((void*)pPointer, 0, sizeof(T) * count);
}

template<class T>
void Memory::Array<T>::assign(uint count, const T& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] = value;
}

template<class T>
void Memory::Array<T>::copy(uint count, const T* value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    memcpy((void*)pPointer, value, sizeof(T) * count);
}

template<class T>
void Memory::Array<T>::copy(uint count, const Memory::Array<T>& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    (void)memcpy((void*)pPointer, (void*)value.pPointer, sizeof(T) * count);
}

template<class T>
void Memory::Array<T>::increment(uint count, const T& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] += value;
}

template<class T>
void Memory::Array<T>::increment(uint count, const T* value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] += value[i];
}

template<class T>
void Memory::Array<T>::increment(uint count, const Memory::Array<T>& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] += ((T*)value.pPointer)[i];
}

template<class T>
void Memory::Array<T>::multiply(uint count, const T& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] *= value;
}

template<class T>
void Memory::Array<T>::multiply(uint count, const T* value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] *= value[i];
}

template<class T>
void Memory::Array<T>::multiply(uint count, const Memory::Array<T>& value)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    for (uint i = 0; i != count; ++i)
        ((T*)pPointer)[i] *= ((T*)value.pPointer)[i];
}

template<class T>
inline const T* Memory::Array<T>::rawptr() const
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (const T*)pPointer;
}

template<class T>
inline T* Memory::Array<T>::rawptr()
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (T*)pPointer;
}

template<class T>
inline Memory::Array<T>::operator void*()
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (void*)pPointer;
}

template<class T>
inline Memory::Array<T>::operator const void*() const
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (const void*)pPointer;
}

template<class T>
inline T* Memory::Array<T>::operator->()
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (T*)pPointer;
}

template<class T>
inline const T* Memory::Array<T>::operator->() const
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (const T*)pPointer;
}

template<class T>
inline const T& Memory::Array<T>::operator*() const
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (const T&)*pPointer;
}

template<class T>
inline T& Memory::Array<T>::operator*()
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (T&)*pPointer;
}

template<class T>
T& Memory::Array<T>::operator[](uint i)
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (T&)pPointer[i];
}

template<class T>
const T& Memory::Array<T>::operator[](uint i) const
{
    ANTARES_SWAP_ACQUIRE_PTR;
    return (const T&)pPointer[i];
}

template<class T>
inline Memory::Array<T>& Memory::Array<T>::operator=(const Memory::Array<T>&)
{
    return *this;
}

template<class T>
inline Memory::Array<T>& Memory::Array<T>::operator=(const Yuni::NullPtr&)
{
    release();
    return *this;
}

template<class T>
inline bool Memory::Array<T>::operator!() const
{
    return not pPointer;
}

template<class T>
inline bool Memory::Array<T>::valid() const
{
    return pPointer;
}

template<class T>
inline bool Memory::Array<T>::strictNull() const
{
    return not pPointer;
}

template<class T>
inline void Memory::Release(T*& pointer)
{
    delete[] pointer;
    pointer = nullptr;
}

template<class T>
inline void Memory::Release(Array<T>& pointer)
{
    pointer.release();
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
inline bool Memory::StrictNull(const T*& out)
{
    return !out;
}

template<class T>
inline void Memory::Flush(T*&)
{
    // do nothing
}

template<class T>
inline void Memory::Acquire(T*&)
{
    // do nothing
}

template<class U>
inline U* Memory::RawPointer(U* array)
{
    return array;
}

template<class U>
inline U* Memory::RawPointer(Array<U>& array)
{
    return array.rawptr();
}

template<class U>
inline const U* Memory::RawPointer(const Array<U>& array)
{
    return array.rawptr();
}

template<class U>
inline void Memory::Zero(uint count, U* array)
{
    (void)::memset(array, 0, sizeof(U) * count);
}

template<class U>
inline void Memory::Zero(uint count, Array<U>& array)
{
    array.assign(count);
}

template<class U>
inline void Memory::Assign(uint count, U* array)
{
    (void)::memset(array, 0, sizeof(U) * count);
}

template<class U>
inline void Memory::Assign(uint count, Array<U>& array)
{
    array.assign(count);
}

template<class U>
inline void Memory::Assign(uint count, U* array, const U& value)
{
    for (uint i = 0; i != count; ++i)
        array[i] = value;
}

template<class U>
inline void Memory::Assign(uint count, Array<U>& array, const U& value)
{
    array.assign(count, value);
}

} // namespace Antares

#endif // __ANTARES_LIBS_MEMORY_MEMORY_HXX__
